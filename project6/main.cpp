#include <iostream>
#include "dp.h"
int main() {
    int maxCars=1000;
    int maxSingelDirect=10;

    OneWay *oneWay = new OneWay(maxCars, maxSingelDirect);
//建立管程,判断可不可进、决定方向,进入单行道
    int i;
    int pid[maxCars];
    for (i = 0; i < maxCars; i++)   //创建子进程
    {
        sleep(1);
        pid[i] = fork();
        if (pid[i] == 0)
        {
            srand(time(NULL));
            int direct = int(rand())%2; //决定东西方向
            oneWay->Arrive(direct); //进入
            oneWay->Cross(direct); //通过
            oneWay->Quit(direct); //离开
            exit(EXIT_SUCCESS);
        }
    }
    for (i = 0; i < maxCars; i++)
    {
        waitpid(pid[i], NULL, 0);
    }

    delete oneWay;
    return EXIT_SUCCESS;


}