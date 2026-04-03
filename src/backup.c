#include "monitor.h"
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

static int ensure_backup_dir(void)
{
    //creates backup folder
    //returns 0 if successful
    //returns -1 if error occurs 
    //and the error is not due to the dir existing

    if (mkdir("backup", 0755) == -1 && errno != EEXIST) {
        perror("mkdir");
        return -1;
    }
    return 0;
}

void backup_file(const char *filepath)
{
    //if backup folders not created dont proceed
    if (ensure_backup_dir() != 0) {
        return;
    }

    FILE *src = fopen(filepath, "r");
    if (src == NULL) {
        perror("fopen source");
        return;
    }

    //we look for the first backslash from the end of the path
    //then we move the pointer by 1 to make it point to the first letter
    //of the file
    const char *name = strrchr(filepath, '/');
    if (name != NULL) {
        name++;
    } else {
        name = filepath;
    }//this runs when the filepath is already the name

    char backup_path[MAX_PATH];
    //attach "backup/" to the name of the file
    snprintf(backup_path, MAX_PATH, "backup/%s", name);

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
