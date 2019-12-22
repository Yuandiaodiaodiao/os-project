//
// Created by qq295 on 2019/12/18.
//

#ifndef PROJECT6_DP_H
#define PROJECT6_DP_H


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>

using namespace std;
/*信号灯控制用的共同体*/
typedef union semuns {
    int val;
} Sem_uns;

//管程中使用的信号量
class Sema {
public:
    Sema(int id);

    ~Sema();

    int down(); //信号量加 1

    int up(); //信号量减 1

private:
    int sem_id; //信号量标识符
};

//管程中使用的锁
class Lock {
public:
    Lock(Sema *lock);

    ~Lock();

    void close_lock();

    void open_lock();

private:
    Sema *sema; //锁使用的信号量
};

class Condition {
public:
    Condition(Sema *sema1, Sema *sema2);

    ~Condition();

    void Wait(Lock *conditionLock, int direct); //过路条件不足时阻塞
    int Signal(int direc);
//唤醒相反方向阻塞车辆
private:
    Sema *sema0; // 一个方向阻塞队列
    Sema *sema1; // 另一方向阻塞队列
    Lock *lock; // 进入管程时获取的锁
};

class OneWay {
public:
    OneWay(int maxall, int maxcur);

    ~OneWay();

    void Arrive(int direc);

// 车辆准备上单行道,direc 为行车方向
    void Cross(int direc);

// 车辆正在单行道上
    void Quit(int direc);

// 车辆通过了单行道
    int *eastCount;
    int *westCount;
    int *eastWait;
    int *westWait;
    int *sumPassedCars;//已经通过的车辆总数
private:
//建立或获取 ipc 信号量的一组函数的原型说明
    int get_ipc_id(char *proc_file, key_t key);

    int set_sem(key_t sem_key, int sem_val, int sem_flag);

//创建共享内存
    char *set_shm(key_t shm_key, int shm_num, int shm_flag);

    int rate; //车速
    int *maxCars;//最大同向车数
    int *numCars; //当前正在通过的车辆数
    int *currentDire;//当前通过的车辆的方向
    Condition *condition; //通过单行道的条件变量
    Lock *lock;//单行道管程锁
};


using namespace std;

Sema::Sema(int id) {
    sem_id = id;
}

Sema::~Sema() {

}

/*
* 信号灯上的 down/up 操作
* semid:信号灯数组标识符
* semnum:信号灯数组下标
* buf:操作信号灯的结构
*/
int Sema::down() {
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

int Sema::up() {
    Sem_uns arg;
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

/*
* 用于单行道管程的互斥执行
*/
Lock::Lock(Sema *s) {
    sema = s;
}

Lock::~Lock() {

}

//上锁
void Lock::close_lock() {
    sema->down();
}

//开锁
void Lock::open_lock() {
    sema->up();
}

int OneWay::get_ipc_id(char *proc_file, key_t key) {
#define BUFSZ 256
    FILE *pf;
    int i, j;
    char line[BUFSZ], colum[BUFSZ];
    if ((pf = fopen(proc_file, "r")) == NULL) {
        perror("Proc file not open");
        exit(EXIT_FAILURE);
    }
    fgets(line, BUFSZ, pf);
    while (!feof(pf)) {
        i = j = 0;
        fgets(line, BUFSZ, pf);
        while (line[i] == ' ')
            i++;
        while (line[i] != ' ')
            colum[j++] = line[i++];
        colum[j] = '\0';
        if (atoi(colum) != key)
            continue;
        j = 0;
        while (line[i] == ' ')
            i++;
        while (line[i] != ' ')
            colum[j++] = line[i++];
        colum[j] = '\0';
        i = atoi(colum);
        fclose(pf);
        return i;
    }
    fclose(pf);
    return -1;
}

/*
*
set_shm 函数建立一个具有 n 个字节 的共享内存区
*
如果建立成功,返回 一个指向该内存区首地址的指针 shm_buf
*
输入参数:
*
shm_key 共享内存的键值
*
shm_val 共享内存字节的长度
*
shm_flag 共享内存的存取权限
*/
char *OneWay::set_shm(key_t shm_key, int shm_num, int shm_flg) {
    int i, shm_id;
    char *shm_buf;
//测试由 shm_key 标识的共享内存区是否已经建立
    if ((shm_id = get_ipc_id("/proc/sysvipc/shm", shm_key)) < 0) {
//shmget 新建 一个长度为 shm_num 字节的共享内存
        if ((shm_id = shmget(shm_key, shm_num, shm_flg)) < 0) {
            perror("shareMemory set error");
            exit(EXIT_FAILURE);
        }
//shmat 将由 shm_id 标识的共享内存附加给指针 shm_buf
        if ((shm_buf = (char *) shmat(shm_id, 0, 0)) < (char *) 0) {
            perror("get shareMemory error");

            exit(EXIT_FAILURE);
        }
        for (i = 0; i < shm_num; i++)
            shm_buf[i] = 0; //初始为 0
    }
//共享内存区已经建立,将由 shm_id 标识的共享内存附加给指针 shm_buf
    if ((shm_buf = (char *) shmat(shm_id, 0, 0)) < (char *) 0) {

        perror("get shareMemory error");

        exit(EXIT_FAILURE);
    }
    return shm_buf;
}

/*
*
set_sem 函数建立一个具有 n 个信号灯的信号量
*
如果建立成功,返回 一个信号量的标识符 sem_id
*
输入参数:
*
sem_key 信号量的键值
*
sem_val 信号量中信号灯的个数
*
sem_flag 信号量的存取权限
*/
int OneWay::set_sem(key_t sem_key, int sem_val, int sem_flg) {
    int sem_id;
    Sem_uns sem_arg;
//测试由 sem_key 标识的信号量是否已经建立
    if ((sem_id = get_ipc_id("/proc/sysvipc/sem", sem_key)) < 0) {
//semget 新建一个信号灯,其标号返回到 sem_id
        if ((sem_id = semget(sem_key, 1, sem_flg)) < 0) {

            perror("semaphore create error");
            exit(EXIT_FAILURE);
        }
    }
//设置信号量的初值
    sem_arg.val = sem_val;
    if (semctl(sem_id, 0, SETVAL, sem_arg) < 0) {

        perror("semaphore set error");

        exit(EXIT_FAILURE);
    }
    return sem_id;
}

Condition::Condition(Sema *semax1, Sema *semax2) {
    sema0 = semax1;
    sema1 = semax2;
}

/**
* 看看是否能通过
*/
void Condition::Wait(Lock *lock, int direc) {
    if (direc == 0) {
        cout << getpid() << " wait ->" << "\n";
        lock->open_lock();
        sema0->down();
        lock->close_lock();
    } else if (direc == 1) {
        cout << getpid() << " wait <-" << "\n";
        lock->open_lock();
        sema1->down();
        lock->close_lock();
    }
}

int Condition::Signal(int direc) {
    int i;
    if (direc == 0)   //唤醒一个方向
    {
        i = sema0->up();
    } else if (direc == 1) {
        i = sema1->up();
    }
    return i;
}

/*
* get_ipc_id() 从/proc/sysvipc/文件系统中获取 IPC 的 id 号
* pfile: 对应/proc/sysvipc/目录中的 IPC 文件分别为
*
msg-消息队列,sem-信号量,shm-共享内存
* key: 对应要获取的 IPC 的 id 号的键值
*/
Condition::~Condition() {

};

/*
*
set_shm 函数建立一个具有 n 个字节 的共享内存区
*
如果建立成功,返回 一个指向该内存区首地址的指针 shm_buf
*
输入参数:
*
shm_key 共享内存的键值
*
shm_val 共享内存字节的长度
*
shm_flag 共享内存的存取权限
*/
OneWay::OneWay(int maxall, int maxcur) {
    Sema *sema0;
    Sema *sema1;
    Sema *semaLock;
    int ipc_flg = IPC_CREAT | 0644;
    maxCars = (int *) set_shm(100, 1, ipc_flg); //最大单向
    numCars = (int *) set_shm(200, 1, ipc_flg); //当前方向上通过的总的车辆数
    currentDire = (int *) set_shm(300, 1, ipc_flg); //当前方向 0 东 1 西
    eastCount = (int *) set_shm(501, 1, ipc_flg);
    westCount = (int *) set_shm(502, 1, ipc_flg);
    sumPassedCars = (int *) set_shm(700, 1, ipc_flg);
    eastWait = (int *) set_shm(801, 1, ipc_flg);
    westWait = (int *) set_shm(802, 1, ipc_flg);
    int sema0_id = set_sem(401, 0, ipc_flg);
    int sema1_id = set_sem(402, 0, ipc_flg);
    int semaLock_id = set_sem(601, maxcur, ipc_flg);
//初始化所有变量
    *maxCars = maxcur;
    *numCars = 0;
    *currentDire = 0;
    *eastCount = 0;
    *westCount = 0;
    *sumPassedCars = 0;
    *eastWait = 0;
    *westWait = 0;
    sema0 = new Sema(sema0_id);
    sema1 = new Sema(sema1_id);
    semaLock = new Sema(semaLock_id);
    lock = new Lock(semaLock);
    condition = new Condition(sema0, sema1);
}

void OneWay::Arrive(int direc) {
    cout << "car born " << getpid() << (direc == 0 ? "  >>" : " <<") << "🚗  " << endl;

    lock->close_lock();

    //堆积车辆过多 换向

    if ((*currentDire != direc || *numCars >= *maxCars)||(*sumPassedCars >= 3&&(*eastWait>3 || *westWait>3) && *numCars > 0)) {
        *sumPassedCars=*sumPassedCars+1;
        cout<<"sum="<<*sumPassedCars<<endl;
        cout<<"wait= "<<*westWait <<" "<<*eastWait<<endl;
        if (*sumPassedCars == 3) {
            cout << getpid() << "start stop all=================" << endl;
        }
        if (direc == 0) {
            *eastWait += 1;
        } else if (direc == 1) {
            *westWait += 1;
        }
        condition->Wait(lock, direc);
       if( *sumPassedCars>3 )*sumPassedCars= 0;
    }

    cout << " solve car " << getpid() << (direc == 0 ? "  >>" : " <<") << "🚗  " << endl;

    if (direc == 0)   //东 +1
    {
        *eastWait -= 1;
        *eastCount = *eastCount + 1;
    } else if (direc == 1)     //西 +1
    {
        *westCount = *westCount + 1;
        *westWait -= 1;
    }
    *numCars = *numCars + 1;
    *currentDire = direc;
    lock->open_lock();
}

void OneWay::Cross(int direc) {

    lock->close_lock();
    sleep(1);
    cout << getpid() << (direc == 0 ? "  -->>" : " <<--") << " road cars:";
    for (int i = 1; i <= *numCars; ++i) {
        cout << (direc == 0 ? "  -->>" : " <<--") << " 🚗  ";
    }
    cout << endl;
    lock->open_lock();
    sleep(10);

}

void OneWay::Quit(int direc) {
    lock->close_lock();
    *numCars -= 1;
    if (direc == 0) {
        cout << getpid() << " left " << "\n";
    } else if (direc == 1) {
        cout << getpid() << " left " << "\n";
    }
    if (*numCars == 0) {
        if (direc == 0) {
            if (*westWait > 0) {
                condition->Signal(1);
            } else if (*eastWait > 0) {
                condition->Signal(0);
            }
        } else if (direc == 1) {
            if (*eastWait > 0) {
                condition->Signal(0);
            } else if (*westWait > 0) {
                condition->Signal(1);
            }
        }
    }
    lock->open_lock();
}

OneWay::~OneWay() {
    delete condition;
}

#endif //PROJECT6_DP_H
