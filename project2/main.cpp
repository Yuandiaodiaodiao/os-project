#include <stdio.h>
#include<sched.h>
#include <unistd.h>
#include <stdlib.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<iostream>
#include <signal.h>
#include <sys/types.h>

using namespace std;
int pid;

typedef void (*sighandler_t)(int);
struct sched_param p;
int prio=5;

void sigint() {
   if(pid>0){
       prio++;
       setpriority(PRIO_PROCESS, pid, prio);
   }
}
void sigtstp() {
    if(pid>0){
        prio--;
        setpriority(PRIO_PROCESS, pid, prio);
    }
}
int main(int argc, char *argv[]) {

    int i, j, status;
    if ((pid = fork()) > 0) {

        //主进程`
        p.sched_priority = 10;
        sched_setscheduler(pid,
                           SCHED_OTHER,
                           &p);
        setpriority(PRIO_PROCESS, pid, prio);
    }
    signal(SIGINT, (sighandler_t) sigint);
    signal(SIGTSTP , (sighandler_t) sigtstp);

    sleep(1);
    for (j = 0; j < 20; ++j) {
        cout << " pid=" << getpid() << " pri=" << getpriority(PRIO_PROCESS, 0) << endl;
        sleep(1);
    }


    return EXIT_SUCCESS;
}


