#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <unistd.h>
#include"ipc.h"


int main(int argc, char *argv[]) {
    int type =0;
    if(argc>=2){
        type=atoi(argv[1]) - 1;
    }
    int combine1 = 0;
    printf("producer %d\n",type+1);

    mutexkey = 101;
    smoker1key = 102;
    smoker2key = 103;
    smoker3key = 104;
    finish1k = 201;
    finish2k = 202;
    finish3k = 203;

    buff_key = 301;
    buff_num = 2;


    sem_flg = IPC_CREAT | 0644;

    buff_ptr = (char *)set_shm(buff_key,buff_num,sem_flg);

    sem_val = 1;

    mutex = set_sem(mutexkey, sem_val, sem_flg);

    sem_val = 0;
    smoker1 = set_sem(smoker1key, sem_val, sem_flg);
    smoker2 = set_sem(smoker2key, sem_val, sem_flg);
    smoker3 = set_sem(smoker3key, sem_val, sem_flg);
    finish1 = set_sem(finish1k, sem_val, sem_flg);
    finish2 = set_sem(finish2k, sem_val, sem_flg);
    finish3 = set_sem(finish3k, sem_val, sem_flg);

    int cnt=0;
    int finish[3]={finish1,finish2,finish3};
    int smoker[3]={smoker1,smoker2,smoker3};

    while (cnt<1000) {
        wait(mutex);
        buff_ptr[type]='A'+(cnt+type)%3;
        signal(mutex);
        printf("producer %d for somker %d\n",type+1,cnt+1);
        //拉起吸烟者
        signal(smoker[cnt]);
        //等待吸烟结束
        wait(finish[cnt]);
        cnt++;
        cnt%=3;
    }
    return EXIT_SUCCESS;
}
