#include "ipc.h"

int main(int argc, char const *argv[])
{
	int rate;
	//指定睡眠秒数，从命令行读入，调节进程速度。
	if(argv[1] != NULL) rate = atoi(argv[1]);
	else rate = 3; //不指定为 3 秒

	//共享内存使用的变量
	buff_key = 101;//缓冲区任给的键值
	buff_num = 1;//缓冲区任给的长度
	pput_key = 102;//生产者放产品指针的键值
	pput_num = 1; //指针数
	shm_flg = IPC_CREAT | 0644;//共享内存读写权限
	//获取缓冲区使用的共享内存， buff_ptr 指向缓冲区首地址
	buff_ptr = (char *)set_shm(buff_key,buff_num,shm_flg);
	//获取生产者放产品位置指针 pput_ptr
	pput_ptr = (int *)set_shm(pput_key,pput_num,shm_flg);

	//信号量使用的变量
	prod_key = 201;//生产者同步信号灯键值 empty
	pmtx_key = 202;//生产者互斥信号灯键值 mutex1
	glue_key = 305;
	tobacco_key = 400;
	paper_key = 410;
	cmtx_key = 302;//消费者互斥信号灯键值 mutex2

	sem_flg = IPC_CREAT | 0644;
	//生产者同步信号灯初值设为缓冲区最大可用量
	sem_val = buff_num;
	//获取生产者同步信号灯，引用标识存 prod_sem
	prod_sem = set_sem(prod_key,sem_val,sem_flg);
	//消费者初始无产品可取，同步信号灯初值设为 0
	sem_val = 0;
	//获取消费者同步信号灯，引用标识存 cons_sem
	glue_sem = set_sem(glue_key,sem_val,sem_flg);
	tobacco_sem = set_sem(tobacco_key,sem_val,sem_flg);
	paper_sem = set_sem(paper_key,sem_val,sem_flg);
	//生产者互斥信号灯初值为 1
	sem_val = 1;
	//获取生产者互斥信号灯，引用标识存 pmtx_sem
	pmtx_sem = set_sem(pmtx_key,sem_val,sem_flg);

	//循环执行模拟生产者不断放产品
	while(1)
	{
		//如果缓冲区满则生产者阻塞
		down(prod_sem);//p
		//如果另一生产者正在放产品，本生产者阻塞
		down(pmtx_sem);//p
		//用写一字符的形式模拟生产者放产品，报告本进程号和放入的字符及存放的位置
		printf("qaq\n");
		sleep(rate);
		int r = rand() % 3;
		printf("r:%d\n",r);
		if(r == 0)
		{
			printf("%d producer provide: glue \n",getpid());
			//唤醒阻塞的生产者
			up(pmtx_sem);//v
			//唤醒阻塞的消费者
			up(glue_sem);//v
		}
		else if(r == 1)
		{
			printf("%d producer provide: tobacco \n",getpid());
			//唤醒阻塞的生产者
			up(pmtx_sem);//v
			//唤醒阻塞的消费者
			up(tobacco_sem);//v
		}
		else if(r == 2)
		{
			printf("%d producer provide: paper \n",getpid());
			//唤醒阻塞的生产者
			up(pmtx_sem);//v
			//唤醒阻塞的消费者
			up(paper_sem);//v
		}
	}
	return EXIT_SUCCESS;
}
