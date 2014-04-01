#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


char file_type(mode_t mode);
int dir_traversal(char* input);
int stat_call(char* file_name, struct dirent* dirptr);
char* file_perms(mode_t mode);


/*
  Purpose: Traverses file system from specified directory or current directory 
  if input is null
  input: Start dir
  function: Creates a file that holds stat info for each file on file system
  output: success or fail

*/
int main(int c, char* argv[]){
   int ret,
       size;
   char* buf;

   buf = (char*) calloc(64, sizeof(char));

   if(argv[1] != NULL){
     size = strlen(argv[1]);
     strncpy(buf, argv[1], size);
     buf[size] = '\0';
     printf("Buf = %s\n", buf);  
   }
   else{
      strcpy(buf, "..");
   }

   /* Call recursive directory traversal on main directory */
   ret = dir_traversal(buf);

/*
   if((basedir = opendir(buf)) == NULL){
       perror("Failed to open home dir");
   }

   while((dirptr = readdir(basedir)) != NULL){
       printf("file -> %s\n", dirptr->d_name);
   }
*/
   

   // Open file
//   ret = open("testfile.txt", O_APPEND|O_CREAT);


   
   // print to file 

   // Close file

   free(buf);

   return 0;
}



/*
   Purpose: Traverses the file system, calls itself recursively
   input: Buffer holding dir path
   output: fail or success
   Author: Erik Steggall
*/
int dir_traversal(char* buf){
   DIR* basedir;
   struct dirent* dirptr;
   struct stat stat_buf;
   char* name_buf;
   int size,
       ret;

   name_buf = (char*) calloc(64, sizeof(char));

   dirptr = malloc(sizeof(struct dirent));
   
   if((basedir = opendir(buf)) == NULL){
       perror("Failed to open home dir");
   }

   /* cycle through directory entries */
   while((dirptr = readdir(basedir)) != NULL){
       /* If it's parent or self do nothing */
       if( strcmp(dirptr->d_name, ".") == 0 || 
           strcmp(dirptr->d_name, "..") == 0) continue;
       
       /* Do stuff here for preorder */
//       printf("file -> %s\n", dirptr->d_name);

       /* copy path into name buffer */
       size = strlen(buf);  
       strncpy(name_buf, buf, size);
       name_buf[size] = '\0';
       strcat(name_buf, "/");
       strcat(name_buf, dirptr->d_name);
       ret = stat_call(name_buf, dirptr);

       /* Open up stat buffer for dir entry */
       if(stat(name_buf, &stat_buf) < 0){
          perror("Stat failed\n");
       }

       /* If it is a directory recurse */
       if(S_ISDIR(stat_buf.st_mode)){
//          printf("########dirptr->d_name = %s\n", dirptr->d_name);
          dir_traversal(name_buf);
       } 
   }
   closedir(basedir);
   
   free(dirptr);
   free(name_buf);

   return 0 ;

}


/*
   Purpose: Get stats on file
   input: File path, Directory entry
   output: Success or fail
   Author: Bithin Alangot
*/
int stat_call(char* name, struct dirent* dirptr){
    struct stat stat_buf;
    char time_buf[300];
    struct tm* time;
    struct passwd* user;
    struct group* group;
//    printf("fname = %s\n", name);
    if (lstat(name, &stat_buf) < 0){
        perror("lstat() failed\n");
    }
//    user = getpwuid(stat_buf.st_uid);
//    group = getgrgid(stat_buf.st_gid);
//    time = localtime(&stat_buf.st_mtime);
//    strftime(time_buf, sizeof(time_buf), "%d.%m.%Y %H:%M:%S", time);
//    printf("%s %d %s %s %s %s\n",file_perms(stat_buf.st_mode), (int)stat_buf.st_nlink, user->pw_name, group->gr_name, time_buf, dirptr->d_name);
    printf("name = %s\n", dirptr->d_name);
    
}


/* 
   Purpose: Get type of file
   input: mode
   output: file type (char)
   Author: Bithin Alangot
*/
char file_type(mode_t mode) {
    char m;
    switch (mode & S_IFMT) {
        case S_IFREG:
            m = '-';
            break;
        case S_IFDIR:
            m = 'd';
            break;
        case S_IFCHR:
            m = 'c';
            break;
        case S_IFBLK:
            m = 'b';
            break;
        case S_IFLNK:
            m = 's';
            break;
        case S_IFIFO:
            m = 'p';
            break;
        case S_IFSOCK:
            m = 's';
            break;
        default:
            m = '?';

    }
    return m;
}

/* 
   Purpose: Get file permissions
   input: file mode
   output: permission string
   Author: Bithin Alangot
*/
char* file_perms(mode_t mode) { 
    char *ptr[] = {"---", "--x","-w-","-wx", "r--","r-x", "rw-","rwx"};
    static char perms[11];
    perms[0] = file_type(mode);
    strcpy(&perms[1], ptr[(mode >> 6)& 7]);
    strcpy(&perms[4], ptr[(mode >> 3)& 7]);
    strcpy(&perms[7], ptr[(mode)& 7]);

    if ((mode & S_ISUID) != 0)
        perms[3] = 's';

    if ((mode & S_ISGID) != 0)
        perms[6] = 's';

    if ((mode & S_ISVTX) != 0)
        perms[9] = 't';

    return perms;
}
