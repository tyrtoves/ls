#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

char *getUid( uid_t uid ) {
	struct passwd *getpwuid(), *usr_ptr;
	static char numstr[10];
	if ( ( usr_ptr = getpwuid( uid )) == NULL ) {
		sprintf( numstr,"% d", uid );
		return numstr;
	}
	else
		return usr_ptr->pw_name;
}

char *getGid( gid_t gid ) {
	struct group *getgrgid(), *grp_ptr;
	static char numstr[10];
	if ((grp_ptr = getgrgid( gid )) == NULL) {
		sprintf(numstr,"% d", gid);
		return numstr;
	}
	else
		return grp_ptr->gr_name;
}

void printMode( int mode, char printmod[]) {
	strcpy( printmod, "--------" );
	if ( S_ISDIR(mode)) printmod[0] = 'd';
	if ( S_ISCHR(mode)) printmod[0] = 'c';
	if ( S_ISBLK(mode))  printmod[0] = 'b';
	if ( mode & S_IRUSR) printmod[1] = 'r';
	if ( mode & S_IWUSR) printmod[2] = 'w';
	if ( mode & S_IXUSR) printmod[3] = 'x';
	if ( mode & S_IRGRP) printmod[4] = 'r';
	if ( mode & S_IWGRP) printmod[5] = 'w';
	if ( mode & S_IXGRP) printmod[6] = 'x';
	if ( mode & S_IROTH) printmod[7] = 'r';
	if ( mode & S_IWOTH) printmod[8] = 'w';
	if ( mode & S_IXOTH) printmod[9] = 'x';
}


void ls(char *path, char full_info) {
	
	int i = 0;
	DIR *dir;
	struct dirent *dir_str;
	struct stat info;
	dir = opendir(path);
	
	if (!dir) {
		perror("DIR");
		return;
	}

	while ( (dir_str = readdir(dir)) != NULL) {
		
		if ( dir_str->d_name[0] == '.' ) 
			continue;
		
		char newPath[ PATH_MAX ];
       	 
		strncpy( newPath, path, PATH_MAX );
        //strncat( newPath, "/", PATH_MAX );
      	strncat( newPath, dir_str->d_name, PATH_MAX );

		if ( stat( newPath, &info ) < 0 ) {
 			perror("STAT123");
 			return;
        }
        
		char printmod[8];
		if ( full_info ) {	
			printMode( info.st_mode, printmod );
        	printf( "%s %4d %-10s %-10s %10d %.10s %s \n", printmod, (int)info.st_nlink, getUid( info.st_uid ), getGid( info.st_gid ), info.st_size, ctime( &info.st_ctime), dir_str->d_name );
		}
		else { 	
			printf( "%s \n", dir_str->d_name );
		}
	}
	closedir(dir);	
}


void ls_r( char *path, char full_info ) {

	DIR *dir = opendir(path);
	
	struct stat stat_str;
	struct dirent *dir_str;
	
	printf( "%s \n", path);
	ls( path, full_info );
	
	char newPath[PATH_MAX];
	while( ( dir_str = readdir(dir) ) != NULL ) { 	
	
		if ( dir_str->d_name[0] == '.' ) 
			continue;
			
		if ( strcmp( path, "." ) == 0 )
			strncpy( newPath, dir_str->d_name, PATH_MAX ); 			
		else {
			strncpy( newPath, path, PATH_MAX );
			
			strncat( newPath, dir_str->d_name, PATH_MAX );
		}
		
		if( stat( newPath, &stat_str ) < 0 ) {
			perror("STAT1");
			return;
		}
		
		if ( stat_str.st_mode & S_IFDIR ) {
			strncat( newPath, "/", PATH_MAX );
			ls_r( newPath, full_info );
		}

	}
}

int main(int argc, char** argv) {

	if (argc < 2) {
		char* msg = "Error: wrong number of arguments";
		write(2, msg, strlen(msg));
		return;
	}

	
	int i;
	char recurs = 0;
	char full_info = 0;
	
	for (i = 0 ; i < argc; ++i) {
		if (strcmp("-l", argv[i]) == 0) {
			full_info = i;
		}
		if (strcmp("-r", argv[i]) == 0) {
			recurs = i;
		}
	}	
	for (i = 1; i < argc; ++i) {
		if (i == recurs || i == full_info)
			continue;
		else {
			if (recurs)
				ls_r(argv[i], full_info);
			else
				ls(argv[i], full_info);
		}
	}			
	return(0);
}
