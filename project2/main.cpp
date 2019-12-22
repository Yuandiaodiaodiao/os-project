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

void *taskFx(void *arg) {
    while (true) {
        int x, ans = 1;
        read(pipFxin[0], &x, sizeof(int));
        if (x == 0)break;
        for (int a = 1; a <= x; ++a) {
            ans *= a;
        }
        write(pipFxout[1], &ans, sizeof(int));
    }
    close(pipFxin[0]);
    close(pipFxout[1]);

}

void *taskFy(void *arg) {
    while (true) {
        int x;
        read(pipFyin[0], &x, sizeof(int));
        if (x == 0)break;
        int *array = new int[x + 2];
        array[1] = 1;
        array[2] = 1;
        for (int a = 3; a <= x; ++a) {
            array[a] = array[a - 1] + array[a - 2];
        }
        write(pipFyout[1], &array[x], sizeof(int));
        delete[] array;
    }
    close(pipFyin[0]);
    close(pipFyout[1]);


}
void *taskFxy(void *arg) {

    while (true) {
        int xy[2];
        int z;
        cout<<"wait read xy"<<endl;
        read(pipFxyin[0], xy, 2*sizeof(int));
        cout<<"readout x="<<xy[0]<<" y="<<xy[1]<<endl;
        if (xy[0] == 0 || xy[1]==0){
            int zero=0;
            write(pipFxin[1], &zero, sizeof(int));
            write(pipFyin[1], &zero, sizeof(int));
            break;
        }
        write(pipFxin[1], &xy[0], sizeof(int));
        write(pipFyin[1], &xy[1], sizeof(int));
        int x,y;
        read(pipFxout[0], &x, sizeof(int));
        read(pipFyout[0], &y, sizeof(int));


        int ans=x+y;
        write(pipFxyout[1], &ans, sizeof(int));
    }
//    close(pipFyout[0]);
//    close(pipFxyout[1]);
//    close(pipFxout[0]);
//    close(pipFyin[1]);
//    close(pipFxin[1]);
//    close(pipFxyin[0]);
//



}

int main(int argc, char *arg[]) {
    int ret;


    pipe(pipFxin);
    pipe(pipFxout);
    pipe(pipFyin);
    pipe(pipFyout);
    pipe(pipFxyin);
    pipe(pipFxyout);
    pthread_t threads[3];
    pthread_create(&threads[0], NULL, taskFx, (void *) &ret);
    pthread_create(&threads[1], NULL, taskFy, (void *) &ret);
    pthread_create(&threads[2], NULL, taskFxy, (void *) &ret);


    cout<<"main pipFxyin"<<endl;
    int xy[2];
    xy[0]=3;
    xy[1]=2;
    write(pipFxyin[1], xy, 2*sizeof(int));
    int ans;
    read(pipFxyout[0], &ans, sizeof(int));




    cout<<"ans="<<ans<<endl;
    xy[0]=0;
    write(pipFxyin[1], xy, 2*sizeof(int));



    exit(EXIT_SUCCESS);
}


