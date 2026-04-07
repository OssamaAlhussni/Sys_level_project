#include "monitor.h"
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Error: Missing directory argument.\n");
        printf("Usage: %s <directory_path>\n", argv[0]);
        printf("Example: %s home/project\n", argv[0]);
        return 1;
    }

    //sets up ctrl+c handling
    setup_signal_handlers();

    //makes sure logs folder exists
    if (ensure_directory(LOG_DIR) != 0) {
        printf("Failed to create logs directory.\n");
        return 1;
    }

    //makes sure backup folder exists
    if (ensure_directory(BACKUP_DIR) != 0) {
        printf("Failed to create backup directory.\n");
        return 1;
    }
    //pipe
    //link[0] is the write
    //link[1] is the read
    int link[2];

    //creates the pipe for communication between parent and child
    //checks for any errors
    if (pipe(link) == -1) {
        perror("pipe");
        return 1;
    }

    //creates a new process
    pid_t pid = fork();

    //if process cant be created
    if (pid < 0) {
        perror("fork");
        close(link[0]);
        close(link[1]);
        return 1;
    }

    if (pid == 0) {
        //child process
        close(link[1]); //child does not write to pipe

        //child stops if ctrl+c is pressed
        setup_signal_handlers();

        //reads file data and writes it to log file
        int result = child_log_loop(link[0], LOG_FILE);

        close(link[0]);
	return result;

    }

    //parent process
    close(link[0]); //parent does not read from pipe

    FileList list;
    init_list(&list);

    //parent scans the directory
    scan_directory(argv[1], &list);

    printf("\nTotal txt files: %d\n", list.count);

    //backs up files and sends file info to child
    for (int i = 0; i < list.count && g_running; i++) {
        backup_file(list.items[i].path);

        if (write_file_info(link[1], &list.items[i]) != 0) {
            perror("write to pipe");
            break;
        }
    }

    //close write end so child knows there is no more data
    close(link[1]);

    //free the dynamic memory
    free_list(&list);

    //wait for child to finish
    int status;

    //wait for the child
    int wait_result = waitpid(pid, &status, 0);

    //If it failed because of Ctrl+C, wait until the child finishes
    while (wait_result == -1 && errno == EINTR) {
    	wait_result = waitpid(pid, &status, 0);
    }

    //check if the child succeeded or if there was a real error
    if (wait_result == -1) {
    	perror("Error with child process");
    }
    else {
    	printf("Child process finished successfully.\n");
    }


    if (!g_running) {
        printf("Program stopped because ctrl+c was pressed.\n");
    }

    return 0;
}
