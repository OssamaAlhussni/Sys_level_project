#include "monitor.h"
#include <unistd.h>

// = 1 when program is running
volatile sig_atomic_t g_running = 1;

void handle_sigint(int sig)
{
    (void)sig;//to avoid a warning from compiler
    g_running = 0;

    //this message appears when the user presses ctrl+c
    const char msg[] = "\nCtrl+c pressed, stopping program.\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

void setup_signal_handlers(void)
{
    //when recieving ctr+c we direct it to our function
    signal(SIGINT, handle_sigint);
}
