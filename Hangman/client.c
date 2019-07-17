#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
 
#define BUF_SIZE 1024
 
void error_handling(char *message);
void *send_msg(void* arg);
void *recv_msg(void* arg);
 
int main(int argc, char *argv[])
{
    int sock;
    char message[BUF_SIZE];
    int str_len, recv_len, recv_cnt;
    struct sockaddr_in serv_addr;
    pthread_t send_thread, recv_thread;
    void *thread_return;
 
    if(argc!=3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }
    
    //socket(int domain, int type, int protocol)
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);   
    if(sock==-1)
        error_handling("socket() error!");
    
    //bind()
    //구조체 초기화 (gabage값 비우기)
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));
    
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connect() error!");
    else
        puts("Connected...........");
    
    //pthread_create() send와 recv용 둘다만들어줌
    pthread_create(&send_thread, NULL, send_msg, (void *)&sock);
    pthread_create(&recv_thread, NULL, recv_msg, (void *)&sock);
 
    //두 쓰레드가 종료될때까지 기다림
    pthread_join(send_thread, &thread_return);    //send가 먼저 종료됨
    pthread_join(recv_thread, &thread_return);
 
    close(sock);
    return 0;
}
 
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
 
void *send_msg(void* arg)
{
    int sock = *((int*)arg);
    int str_len;
    char message[BUF_SIZE];
 
    while(1) 
    {
        fgets(message, BUF_SIZE, stdin);
        str_len=write(sock, message, strlen(message));
    }
}
 
void *recv_msg(void* arg)
{
    int sock = *((int*)arg);
    int recv_cnt;
    char message[BUF_SIZE];
 
    while(1)
    {
        recv_cnt=read(sock, &message, BUF_SIZE-1);
        if(recv_cnt==-1)
            error_handling("read() error!");
 
        message[recv_cnt]=0;
        if(!strcmp(message, "fail"))    //게임인원이 다찼거나 게임진행시 종료 (server에서 fail메세지전송)
            error_handling("Connect Fail-game already start");
 
        printf("%s", message);
    }
}

