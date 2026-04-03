#ifndef MONITOR_H
//includes the file only once
#define MONITOR_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_PATH 1024
//limits file path length

#define INITIAL_CAPACITY 10
//assume we will find 10 files



typedef struct {
    char path[MAX_PATH];
    long size;
} FileInfo;

typedef struct {
    FileInfo *items; //points to where each file is stored
    int count; //num of files
    int capacity;
} FileList;

// here we create two datatypes Fileinfo and filelist
// then we describe the attributes that they have




void init_list(FileList *list);
void free_list(FileList *list);
void scan_directory(const char *dir, FileList *list);
void backup_file(const char *filepath);
//const is used to not allow modification of the path
#endif
