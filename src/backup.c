#include "monitor.h"
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

//creates backup folder if it does not exist
int ensure_directory(const char *path)
{
    struct stat st;
    //check if path has any objects
    if (stat(path,&st) == 0){
    	//checks if the directory already exists
    	// If it is a directory, return 0
    	if (S_ISDIR(st.st_mode)) {
        	return 0;
    	}
    	// If it exists but is not a directory
    	//return -1
    	else {
		printf("Cannot continue, %s is a file not a dir.",path);
        	return -1;
     	}
    }

    //creates the directory if it does not exist
    mkdir(path, 0755);

    return 0;
}

void backup_file(const char *filepath)
{
    //if backup folders not created dont proceed
    if (ensure_directory(BACKUP_DIR) != 0) {
        return;
    }

    FILE *src = fopen(filepath, "r");
    if (src == NULL) {
        perror("fopen source");
        return;
    }

    //we look for the last slash in the path
    //then we move the pointer by 1 to make it point to the first letter
    //of the file name
    const char *name = strrchr(filepath, '/');
    if (name != NULL) {
        name++;
    } else {
        name = filepath;
    }//this runs when the filepath is already the file name

    char backup_path[MAX_PATH];
    //attach "backup/" to the name of the file
    snprintf(backup_path, MAX_PATH, "%s/%s", BACKUP_DIR, name);

    FILE *dest = fopen(backup_path, "w");
    if (dest == NULL) {
        perror("fopen backup");
        fclose(src);
        return;
    }

    char buffer[1024];
    //write the text in the buffer to the destination file
    while (fgets(buffer, sizeof(buffer), src) != NULL) {
        fputs(buffer, dest);
    }

    printf("Backup created: %s\n", backup_path);

    fclose(src);
    fclose(dest);
}
