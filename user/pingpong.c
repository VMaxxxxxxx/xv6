#include <kernel/types.h>
#include <user/user.h>
#define RD 0 // 管道的读端
#define WR 1 // 管道的写端

int main(int argc, char const *argv[])
{
    char buf = 'p'; // 父子进程传输的字节内容
    int fd_c2p[2]; //子进程 -> 父进程 0端是父进程读取子进程发送数据的端口，1端是子进程向父进程发送数据的端口
    int fd_p2c[2]; //父进程 -> 子进程 0端是子进程读取父进程发送数据的端口，1端是父进程向子进程发送数据的端口
    
    pipe(fd_c2p); //创建管道
    pipe(fd_p2c);

    int pid = fork();
    int exit_status = 0;
    if(pid < 0)
    {
        // 没有fork成功，关闭两个管道的所有读写端，退出
        fprintf(2, "fork() error!\n");
        close(fd_c2p[RD]);
        close(fd_c2p[WR]);
        close(fd_p2c[RD]);;
        close(fd_p2c[WR]);
        exit(1);
    }
    else if(pid == 0)
    {
        // pid为0代表此处为子程序执行的代码段
        // 子进程中，父进程不能读不能写
        close(fd_c2p[RD]); //关闭父进程读端
        close(fd_p2c[WR]); //关闭父进程写端
        if(read(fd_p2c[RD], &buf, sizeof(char)) != sizeof(char))
        {
            // step2： 子进程读取父进程发来的数据
            // 出错
            fprintf(2, "child read() error!\n");
            // 退出状态：1：错误退出
            exit_status = 1;
        }
        else
        {
            // 正确
            fprintf(1, "%d: received ping\n", getpid());
        }
        if(write(fd_c2p[WR], &buf, sizeof(char)) != sizeof(char))
        {
            // step3：子进程向父进程写数据
            // 写错误
            fprintf(2, "child write() error!\n");
            exit_status = 1;
        }
        // 使用完了子进程的读写端口，关闭
        close(fd_c2p[WR]);
        close(fd_p2c[RD]);
        exit(exit_status); // 异常退出的状态已经记录
    }
    else
    {
        // 关闭子进程的读写端
        close(fd_c2p[WR]);
        close(fd_p2c[RD]);

        if(write(fd_p2c[WR], &buf, sizeof(char)) != sizeof(char))
        {
            // step1：父进程向子进程写数据
            // 错误
            fprintf(2, "parent write() error!\n");
            exit_status = 1;
        }
        if(read(fd_c2p[RD], &buf, sizeof(char)) != sizeof(char))
        {
            // step4：读取来自子进程的数据
            // 错误
            fprintf(2, "parent read() error!\n");
            exit_status = 1;
        }
        else
        {
            // 正确
            fprintf(1, "%d: received pong\n", getpid());
        }
        // 关闭父进程用到的腹泻端口，退出
        close(fd_c2p[RD]);
        close(fd_p2c[WR]);
        exit(exit_status);
    }
}