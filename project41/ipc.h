//
// Created by Yuan on 18/12/2019.
//

#ifndef PROJECT3_IPC_H
#define PROJECT3_IPC_H
#define BUFSZ 256
key_t buff_key;
int buff_num;
char *buff_ptr;
key_t pput_key;
int pput_num;
int *pput_ptr;

key_t mutexkey;
key_t smoker1key;
key_t smoker2key;
key_t smoker3key;
key_t finish1k;
key_t finish2k;
key_t finish3k;

int mutex;
int mutexForSeller;
int smoker1;
int smoker2;
int smoker3;
int finish1;
int finish2;
int finish3;



int sem_val;
int sem_flg;


int get_ipc_id(char *proc_file, key_t key);
typedef union semuns {
    int val;
} Sem_uns;

int set_sem(key_t sem_key, int sem_val, int sem_flg) {
    int sem_id;
    Sem_uns sem_arg;
    if ((sem_id = semget(sem_key, 1, sem_flg)) < 0) {
        perror("semaphore create error");
        exit(EXIT_FAILURE);
    }
    sem_arg.val = sem_val;
    if (semctl(sem_id, 0, SETVAL, sem_arg) < 0) {
        perror("semaphore set error");
        exit(EXIT_FAILURE);
    }
    return sem_id;
}
int wait(int sem_id) {
    struct sembuf buf;
    buf.sem_op = -1;
    buf.sem_num = 0;
    buf.sem_flg = SEM_UNDO;
    if ((semop(sem_id, &buf, 1)) < 0) {
        perror("down error ");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

int signal(int sem_id) {
    struct sembuf buf;
    buf.sem_op = 1;
    buf.sem_num = 0;
    buf.sem_flg = SEM_UNDO;
    if ((semop(sem_id, &buf, 1)) < 0) {
        perror("up error ");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

char * set_shm(key_t shm_key,int shm_num,int shm_flg)
{
    /*
    创建具有n字节的共享内存,成功返回首地址指针。
    key指定key,val指定长度，flag指定权限
    */
    int i,shm_id;
    char * shm_buf;
    //测试由 shm_key 标识的共享内存区是否已经建立
    if((shm_id = get_ipc_id("/proc/sysvipc/shm",shm_key)) < 0 )
    {
        //shmget 新建 一个长度为 shm_num 字节的共享内存,其标号返回到 shm_id
        if((shm_id = shmget(shm_key,shm_num,shm_flg)) <0)
        {
            perror("shareMemory set error");
            exit(EXIT_FAILURE);
        }
        //shmat 将由 shm_id 标识的共享内存附加给指针 shm_buf
        if((shm_buf = (char *)shmat(shm_id,0,0)) < (char *)0)
        {
            perror("get shareMemory error");
            exit(EXIT_FAILURE);
        }
        for(i=0; i<shm_num; i++) shm_buf[i] = 0; //初始为 0
    }
    //shm_key 标识的共享内存区已经建立,将由 shm_id 标识的共享内存附加给指针 shm_buf
    if((shm_buf = (char *)shmat(shm_id,0,0)) < (char *)0)
    {
        perror("get shareMemory error");
        exit(EXIT_FAILURE);
    }
    return shm_buf;
}
int get_ipc_id(char *proc_file,key_t key)
{
    /*
    key是要获取IPC的id号。
    PROC_FILE对应三个文件，msg,sem,shm
    原理就是循环取key进行判断，如果存在返回id，否则返回-1.
    */
    FILE *pf;
    int i,j;
    char line[BUFSZ],colum[BUFSZ];
    if((pf = fopen(proc_file,"r")) == NULL)
    {
        perror("Proc file not open");
        exit(EXIT_FAILURE);
    }
    fgets(line, BUFSZ,pf);
    while(!feof(pf))
    {
        i = j = 0;
        fgets(line, BUFSZ,pf);
        while(line[i] == ' ') i++;
        while(line[i] !=' ') colum[j++] = line[i++];
        colum[j] = '\0';
        if(atoi(colum) != key) continue;
        j=0;
        while(line[i] == ' ') i++;
        while(line[i] !=' ') colum[j++] = line[i++];
        colum[j] = '\0';
        i = atoi(colum);
        fclose(pf);
        return i;
    }
    fclose(pf);
    return -1;
}
#endif //PROJECT3_IPC_H
