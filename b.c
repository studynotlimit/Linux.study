//read 

#include "t.h"
int len=110;
int runs=1;
void stop(){
    runs=0;
}
int main(int argc, char** argv)
{
    int semid, shmid;
    char *shmaddr,read_buf[SHM_SIZE];
    FILE *fp;//文件指针
    signal(SIGINT,&stop);
    if((mkfifo("./text.txt", 00666)) < 0)
    {
        perror("创建命名管道失败!\n");
        exit(1);
    }
    else printf("你成功创建了一个命名管道 \n");
     while(runs)
    {
        if((fp=fopen("./text.txt","r"))<0){
            printf("打开命名管道失败\n");
            exit(1);
        }
        
        if(fgets(read_buf,SHM_SIZE,fp)!=NULL){
            printf("读取字符串：%s",read_buf);
            fclose(fp);
        }else{
			if(ferror(fp))
			{
				printf("读取命名管道失败 \n");
				exit(1);
			}
		}
        if(strncmp(read_buf, "end", 3) == 0){
            printf("接受到结束信号,命名管道通信结束 \n");
            runs = 0;
        }
    }
    runs=1;
    while(runs);
    runs=1;
    printf("接收到SIGINT信号，接下来开始共享内存和信号量通信");
    printf("-----------------------------------------------\n");
    if((shmid = creatshm("./test.txt", 57, SHM_SIZE)) == -1)
        return -1;
    if((shmaddr = shmat(shmid, (char*)0, 0)) == (char *)-1){
        perror("attch shared memory error!\n");
        exit(1);
    }
    if((semid = opensem("./test.txt", 39)) == -1)
        return -1;
    printf("read start....................\n");        
    while(1){
        printf("read : ");
        wait_sem(semid, 0);  //等待信号量可以获取
        if(sem_p(semid, 0) == -1) //获取信号量失败退出。当server写入'#'时引发
            break;
        printf("%s", shmaddr);

        sem_v(semid, 0);
        usleep(1000);
    }    
    return 0;
}
