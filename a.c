//write 

#include "t.h"
#include <fcntl.h>
int len=110;
int runs=1;
void stop(){
    runs=0;
}
int main(int argc, char** argv)
{
    int semid, shmid;
    char *shmaddr;
    char write_str[SHM_SIZE],write_buf[SHM_SIZE];//缓冲区字符数组
    char *ret;
    signal(SIGINT,stop);
    FILE *fp;//文件指针

    while(runs)
    {
        if((fp=fopen("./text.txt","w"))<0){
            printf("打开命名管道失败\n");
            exit(1);
        }
        printf("请输入： ");
        fgets(write_buf,SHM_SIZE,stdin);
        if(strncmp(write_buf, "end", 3) == 0)
        {
            printf("接受到结束信号,命名管道通信结束 \n");
            runs = 0;
        }
        if(fputs(write_buf,fp)==EOF){
            printf("写入命名管道失败\n");
            exit(1);
        }
         fclose(fp);
    }
    runs=1;
    while(runs);
    runs=1;
    printf("接收到SIGINT信号，接下来开始共享内存和信号量通信");
    printf("-----------------------------------------------\n");
    if((shmid = creatshm("./test.txt", 57, SHM_SIZE)) == -1) //创建或者获取共享内存
        return -1;
/*建立进程和共享内存连接*/
    if((shmaddr = shmat(shmid, (char*)0, 0)) == (char *)-1){
        perror("attch shared memory error!\n");
        exit(1);
    }    
    if((semid = creatsem("./test.txt", 39, 1, 1)) == -1)//创建信号量
        return -1;
    while(1){
        wait_sem(semid, 0);//等待信号量可以被获取
        sem_p(semid, 0);  //获取信号量
/***************写共享内存***************************************************/
        printf("write : ");
        ret = fgets(write_str, 1024,stdin);
        if(write_str[0] == '#') // '#'结束写进程
            break;
        int len = strlen(write_str);
        write_str[len] = '\0';
        strcpy(shmaddr, write_str);
/****************************************************************************/
        sem_v(semid, 0); //释放信号量
        usleep(3000);  //本进程睡眠.
    }
    sem_delete(semid); //把semid指定的信号集从系统中删除
    deleteshm(shmid);   //从系统中删除shmid标识的共享内存
    return 0;
}
