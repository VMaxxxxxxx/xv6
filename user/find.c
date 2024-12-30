#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

void find(char *path, const char *filename)
{
    char buf[512], *p; // 用于存放读取目录项之后拼接出来的完整路径
    int fd;
    struct dirent de; // 存储目录项的信息，包括inum：该目录项对应的文件的inode编号，name：该目录项的文件名
    struct stat st; // 存储文件或目录的状态信息，包含dev、ino、type、nlink、size等
    
    if((fd = open(path, 0)) < 0)
    {
        // 打开当前传入的路径的目录或文件
        fprintf(2, "find: cannot open %s\n", path);
        return ;
    }
    
    if(fstat(fd, &st) < 0)
    {
        // 获取刚才打开的文件的状态信息，保存到结构体st中
        fprintf(2, "find: cannot fstat %s\n", path);
        close(fd);
        return ;
    }
    
    if(st.type != T_DIR)
    {
        // 参数错误，find的第一个参数必须是目录
        fprintf(2, "usage: find <directory> <filename>\n");
        return ;
    }

    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
    {
        // 将要拼接的完整路径名的长度，已经超出了buf的长度
        fprintf(2, "find: path too long\n");
        return ;
    }

    strcpy(buf, path); // 先将上一级目录复制到buf里等待拼接当前路径下的目录名
    p = buf + strlen(buf); // 移动p指向上一级目录字符数组的末尾
    *p++ = '/'; // 写入一个路径分隔符，后续就是当前路径下的目录名
    while(read(fd, &de, sizeof de) == sizeof de)  // 挨个读取当前路径下的目录项，存放目录项信息到de中
    {
        if(de.inum == 0)  // 空项，跳过
        {
            continue;
        }
        memmove(p, de.name, DIRSIZ); // 从buf的p位置开始写入读取的有效目录项的名字，最长写入长度为DIRSIZ
        p[DIRSIZ] = 0; // 因为p是个指针，所以，利用指针和数组的联系，这里最后写入一个结束符
        if(stat(buf, &st) < 0)
        {
            // 获取当前buf保存的字符串路径的状态信息
            fprintf(2, "find cannot stat %s\n", buf);
            continue;
        }
        // 当buf中字符串路径是个目录时，并且不属于.和..，说明需要递归调用来进行查找深层目录
        if(st.type == T_DIR && strcmp(p, ".") != 0 && strcmp(p, "..") != 0)
        {
            find(buf, filename);
        }
        else if(strcmp(filename, p) == 0) // 如果当前查找文件名和加入的文件名相同，找到了，打印
        {
            printf("%s\n", buf);
        }
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    // 判断参数个数
    if(argc != 3)
    {
        fprintf(2, "usage: find <directory> <filename>\n");
        exit(1);
    }
    // 调用find方法，参数1：查找目录，参数2：查找文件名
    find(argv[1], argv[2]);
    exit(0);
}