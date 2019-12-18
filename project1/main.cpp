#include<unistd.h>
#include <stdio.h>
#include<sys/wait.h>
#include<stdlib.h>

pid_t pidarray[2];
int pslock = 0;

typedef void (*sighandler_t)(int);

void sigcat() {
//    printf("%d Process continue\n", getpid());
}

int main() {
    signal(SIGINT, (sighandler_t) sigcat);
    while (true) {
        pidarray[0] = fork();
        if (pidarray[0] > 0) { //main thread
            pidarray[1] = fork(); //subthread2
        }

        if (pidarray[0] == 0) {//subthread1

            pause();
            system("ls");
            break;
        } else if (pidarray[1] == 0) { //subthread2
            system("ps");
            break;
        } else {//mainthread
            int status;
            waitpid(pidarray[1], &status, 0);

            kill(pidarray[0], SIGINT);
            waitpid(pidarray[0], &status, 0);

        }
        sleep(3);
    }


    return 0;
}