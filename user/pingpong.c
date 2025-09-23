#include "kernel/types.h"
#include "user.h"

int main(int argc, char* argv[])
{
    int c2f[2], f2c[2];  // 父子进程双向通信管道
    pipe(c2f);
    pipe(f2c);

    int pid = fork();
    if (pid < 0) {
        printf("fork error\n");
        exit(-1);
    }
    
    if (pid == 0) {  // 子进程
        close(f2c[1]);  // 关闭父->子写端
        close(c2f[0]);  // 关闭子->父读端
        
        int father_pid, child_pid = getpid();
        read(f2c[0], &father_pid, sizeof(int));
        printf("%d: received ping from pid %d\n", child_pid, father_pid);
        
        write(c2f[1], &child_pid, sizeof(int));
        
        close(f2c[0]);
        close(c2f[1]);
        exit(0);
    } 
    else {  // 父进程
        close(f2c[0]);  // 关闭父->子读端
        close(c2f[1]);  // 关闭子->父写端
        
        int father_pid = getpid(), child_pid;
        write(f2c[1], &father_pid, sizeof(int));
        read(c2f[0], &child_pid, sizeof(int));
        printf("%d: received pong from pid %d\n", father_pid, child_pid);
        
        close(f2c[1]);
        close(c2f[0]);
        wait(0);
        exit(0);
    }
}