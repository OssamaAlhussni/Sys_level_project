#include "monitor.h"
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
int is_txt(const char *name)
{
    int len = strlen(name);

    //if the file name is less than 4 it means it doesnt have .txt
    if (len < 4) {
        return 0;
    }
    //by adding the length of the string to itself
    //makes it point to the /0 character
    //then we go back 4 characters
    //if it ends with .txt it will return 0
    return strcmp(name + len - 4, ".txt") == 0;
}

void init_list(FileList *list)
{
    list->count = 0;
    list->capacity = INITIAL_CAPACITY;
    list->items = malloc(list->capacity * sizeof(FileInfo));
    // allocating memory for the items array
    // based on the capacity of the list

    //if no space in memory is found, program exits
    // and prints the error message
    if (list->items == NULL) {
        perror("malloc");
        exit(1);
    }
}

void free_list(FileList *list)
{
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

static void add_file(FileList *list, const char *path, long size)
{
    //double capacity if array is full
    if (list->count == list->capacity) {
        list->capacity *= 2;

	//reallocate memory of items to a new address with bigger size
        FileInfo *temp = realloc(list->items,
                                 list->capacity * sizeof(FileInfo));

        if (temp == NULL) {
            perror("realloc");
            exit(1);
        }

        list->items = temp;
    }
    //copies path of the file into the path variable in the fileinfo struct
    // limits size to MAX_PATH
    snprintf(list->items[list->count].path, MAX_PATH, "%s", path);

    //updates the size of the file
    list->items[list->count].size = size;
    list->count++;
}

static void scan_recursive(const char *dir, FileList *list)
{
    //open the dir at the path given and assign dp to its address
    DIR *dp = opendir(dir);
    //if doesnt exist or cant open it it exits the function call
    if (dp == NULL) {
        perror("opendir");
        return;
    }
    struct dirent *entry;
    //entry holds the name and type of the current folder/file
    while (entry = readdir(dp)) {
        //skips the current and parent folder to avoid forever loop
        if (strcmp(entry->d_name, ".") == 0 ||
                      strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char fullpath[MAX_PATH];
	//copies the full folder name and subfolder/filename into fullpath
        snprintf(fullpath, MAX_PATH, "%s/%s", dir, entry->d_name);

        struct stat st;
	//assigns the properties of the folder/file at the path to st
	//error checking
        if (stat(fullpath, &st) == -1) {
            perror("stat");
            continue;
        }
	//check if the path is a dir
	// if it is then call the function to go through the subfolders
	//if it points to a file and of type .txt
	//then add it to the list
        if (S_ISDIR(st.st_mode)) {
            scan_recursive(fullpath, list);
        } else if (S_ISREG(st.st_mode) && is_txt(entry->d_name)) {
            add_file(list, fullpath, st.st_size);
            printf("Found file: %s (%ld B)\n", fullpath, st.st_size);
        }
    }

    closedir(dp);
}

void scan_directory(const char *dir, FileList *list)
{
    if (dir == NULL || list == NULL) return;
    scan_recursive(dir, list);
}
