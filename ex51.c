#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <string.h>

#define SYS_CALL_ERROR "Error in system call\n"
#define ERROR_SIZE strlen(SYS_CALL_ERROR)


char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return (buf);
}

void Error() {
    write(STDERR_FILENO, SYS_CALL_ERROR, ERROR_SIZE);
}

void Run() {
    int pipedes[2];
    if (pipe(pipedes) < 0) {
        Error();
    }
    int terminated = 0;
    int pid;
    switch ((pid = fork())) {
        case -1:
            Error();
        case 0:
            if (dup2(pipedes[0], STDIN_FILENO) < 0) {
                Error();
            }
            char *args[] = {"./draw.out", NULL};
            if (execvp(args[0], args) < 0) {
                Error();
            }
        default:
            while (!terminated) {
                char input = getch();
                if (input == 'q' || input == 'w' || input == 'a' || input == 's' || input == 'd') {
                    if (write(pipedes[1], &input, 1) < 0) {
                        Error();
                    }
                    if (kill(pid, SIGUSR2) < 0) {
                        Error();
                    }
                    if (input == 'q') {
                        terminated = 1;
                    }
                }
            }
    }
}

int main() {
    Run();
    return 0;
}