#ifndef MONITOR_H
//includes the file only once
#define MONITOR_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define MAX_PATH 1024
//limits file path length

#define INITIAL_CAPACITY 10
//assume we will find 10 files

#define BACKUP_DIR "backup"
#define LOG_DIR "logs"
#define LOG_FILE "logs/report.txt"

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


//external variable which value must be always checked in current time
//sig atomic makes it accessible in 1 step
extern volatile sig_atomic_t g_running;

void setup_signal_handlers(void);
void init_list(FileList *list);
void free_list(FileList *list);
void scan_directory(const char *dir, FileList *list);
void backup_file(const char *filepath);
int ensure_directory(const char *path);
int write_file_info(int fd,  FileInfo *info);
int child_log_loop(int read_fd, char *log_path);

//const is used to not allow modification of the path
#endif
