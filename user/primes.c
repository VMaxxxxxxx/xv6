#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

const uint INT_LEN = sizeof(int);

int lpipe_first_data(int lpipe[2], int *dst)
{
    // 对传入的管道，读取第一个数，写入dst中并返回
    if(read(lpipe[RD], dst, sizeof(int)) == sizeof(int))
    {
        printf("prime %d\n", *dst);
        return 0;
    }
    // 没有读取到数字
    return -1;
}

void transmit_data(int lpipe[2], int rpipe[2], int first)
{
    int data;
    // 将左侧管道内数据读取出来，若不整除首元素，写入右侧管道
    while(read(lpipe[RD], &data, sizeof(int)) == sizeof(int))
    {
        if(data % first)
        {
            write(rpipe[WR], &data, sizeof(int));
        }
    }
    close(lpipe[RD]);
    close(rpipe[WR]);
}

void primes(int lpipes[2])
{
    close(lpipes[WR]);
    int first;
    // 从左侧管道传入的数据，计算出第一个元素，然后对其余元素，如果不能整除该首元素，则传入右侧管道
    if(lpipe_first_data(lpipes, &first) == 0)
    {
        int p[2];
        pipe(p);
        // 左右管道的交替
        transmit_data(lpipes, p, first);
        // 创建子进程，继续向右处理，父进程等待处理返回
        if(fork() == 0)
        {
            primes(p);
        }
        else
        {
            close(p[RD]);
            wait(0);
        }
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    int p[2];
    pipe(p);
    // 创建原始管道，里面存放了2-35的数
    for(int i = 2; i <= 35; ++i)
    {
        write(p[WR], &i, INT_LEN);
    }

    // 子进程里来找出素数，父进程负责等待找出所有素数后关闭程序
    if(fork() == 0)
    {
        primes(p);
    }
    else
    {
        close(p[WR]);
        close(p[RD]);
        wait(0);
    }

    exit(0);
}