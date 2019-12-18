#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include<iostream>
using namespace std;

int pipFxin[2];
int pipFxout[2];
int pipFyin[2];
int pipFyout[2];
int pipFxyin[2];
int pipFxyout[2];

int fy(int y)
{
    if(y == 1 || y == 2)
        return 1;
    return fy(y-1) + fy(y-2);
}
int pid1 = -1;
int pid2 = -1;
int main(int argc, char *arg[]) {
    pipe(pipFxin);
    pipe(pipFxout);
    pipe(pipFyin);
    pipe(pipFyout);
    pipe(pipFxyin);
    pipe(pipFxyout);
    pid1 = fork();
    if(pid1 == 0)
    {
        close(pipFxin[1]);
        close(pipFxout[0]);
        int x = 0;
        int ans = 1;
        read(pipFxin[0],&x,sizeof(int));
        for(int i = 1 ; i <= x ; i ++)
            ans *= i;
        x = ans;
        printf("fx %d\n",x);
        write(pipFxout[1],&x,sizeof(int));
    }
    else
    {
        pid2 = fork();
        if(pid2 == 0)
        {
            close(pipFyin[1]);
            close(pipFyout[0]);
            int y = 0;
            read(pipFyin[0],&y,sizeof(int));
            y = fy(y);
            printf("fy %d\n",y);
            write(pipFyout[1],&y,sizeof(int));
        }
        else
        {
            int ans = 0;
            int x,y;
            scanf("%d%d",&x,&y);
            close(pipFxin[0]);
            close(pipFxout[1]);
            close(pipFyin[0]);
            close(pipFyout[1]);
            write(pipFxin[1],&x,sizeof(int));
            read(pipFxout[0],&x,sizeof(int));
            ans += x;
            write(pipFyin[1],&y,sizeof(int));
            read(pipFyout[0],&y,sizeof(int));
            ans += y;
            printf("ans: %d\n",ans);
        }
    }
    exit(EXIT_SUCCESS);
}


