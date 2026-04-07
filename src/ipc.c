#include "monitor.h"
#include <unistd.h>
#include <errno.h>

//this function keeps reading until all bytes are received
ssize_t read_full(int fd, void *buf, ssize_t count)
{
    //total bytes read
    ssize_t total = 0;
    char *ptr = (char *)buf;

    while (total < count) {
	//reads data from fd and places it in the ptr buffer
	//n holds the bytes read
        ssize_t n = read(fd, ptr + total, count - total);

        if (n > 0) {
            total += n;
        } else if (n == 0) {
            //end of pipe
            return total;
        } else {
            return -1;
        }
    }

    return total;
}

//this function keeps writing until all bytes are sent
ssize_t write_full(int fd, void *buf, ssize_t count)
{
    ssize_t total = 0;
    char *ptr = (char *)buf;

    while (total < count) {
        ssize_t n = write(fd, ptr + total, count - total);

        if (n > 0) {
            total += n;
        } else if (n < 0) {
            return -1;
        }
    }

    return total;
}

int write_file_info(int fd, FileInfo *info)
{
    //send the whole struct through the pipe
    ssize_t result = write_full(fd, info, sizeof(FileInfo));

    //if amount sent matches the size of the struct, success
    if (result == (ssize_t)sizeof(FileInfo)) {
        return 0;
    }
    else {
        return -1;
    }
}

int child_log_loop(int read_fd, char *log_path)
{
    //opens the log file in append mode
    FILE *log = fopen(log_path, "a");
    if (log == NULL) {
        perror("fopen log");
        return -1;
    }

    //log title
    fprintf(log, "\n===== File Monitoring Report =====\n");
    fflush(log);

    FileInfo info;

    //child keeps reading file data from the pipe
    while (g_running) {
        ssize_t n = read_full(read_fd, &info, sizeof(FileInfo));

        if (n == 0) {
            //no more data from parent
            break;
        }

        if (n < 0) {
            if (!g_running) {
                break;
            }
            perror("read");
            fclose(log);
            return -1;
        }

        if (n != (ssize_t)sizeof(FileInfo)) {
            printf("Incomplete data received from pipe, (%ld).\n",n);
            break;
        }

        //write file details into the log
        fprintf(log, "File: %s\n", info.path);
        fprintf(log, "Size: %ld bytes\n", info.size);
        fprintf(log, "----------------------------------\n");
        fflush(log);
    }

    if (!g_running) {
        fprintf(log, "Shutdown due to ctrl+c.\n");
        fflush(log);
    }

    fclose(log);
    return 0;
}
