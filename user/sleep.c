#include "kernel/types.h"
#include "user/user.h"
int main(int argc, char const *argv[])
{
    if(argc != 2)
    {
        fprintf(2, "usage: sleep <time>\n");
        exit(1);
    }
    // atoi是把main参数列表的字符串转换成数字，其声明在user.h中
    sleep(atoi(argv[1]));
    exit(0);
}