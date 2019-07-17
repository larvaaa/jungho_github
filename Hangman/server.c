#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/time.h>
#include <sys/select.h>
 
#define BUF_SIZE 1024
 
#define HINT 3
 
void error_handling(char *message);
char* drawHangman(int num);
 
int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    char message[BUF_SIZE];
    int str_len;
    
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;
 
    fd_set reads, copy_reads;
    struct timeval timeout;
    int fd_max, fd_num, i;
 
    int user_cnt=0;     //유저 수
    int user[3]={0};    //유저정보=[소켓]
    int game_flag=0;    //게임진행여부
    int challenger1[2]={0};    //도전자1
    int challenger2[2]={0};    //도전자2
    int examiner[2]={0};    //출제자
    char word[BUF_SIZE];    //제시어
    char question[BUF_SIZE];    //현재답안
    int hint_cnt, hang_cnt, j, iscorrect, iscontinue;
    
    if(argc!=2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    
    //socket(int domain, int type, int protocol)
    serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(serv_sock==-1)
        error_handling("socket() error!");
    
    //bind()
    memset(&serv_addr, 0, sizeof(serv_addr));
    //구조체 serv_addr 값채우기
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));
 
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("bind() error!");
    
    if(listen(serv_sock, 5)==-1)
        error_handling("listen() error!");
    
    clnt_addr_size=sizeof(clnt_addr);
 
    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);
    fd_max=serv_sock;
 
    while(1)
    {
        copy_reads=reads;    //원본훼손 방지, copy해서넘김
        timeout.tv_sec=5;
        timeout.tv_usec=5000;    //microsec
        fd_num=select(fd_max+1, &copy_reads, 0, 0, &timeout); //0은 리턴받지않는다
 
        if(fd_num==-1) break;    //오류시종료
        if(fd_num==0) continue;    //소켓이 없을경우
 
        for(i=0; i<fd_max+1; i++)
        {
            if(FD_ISSET(i, &copy_reads))    //1인지아닌지확인-1이면처리
            {
                if(i==serv_sock)    //listening소켓인지확인
                {    //accept
                    clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
                    if(clnt_sock==-1)
                        error_handling("accept() error!");
                    FD_SET(clnt_sock, &reads);
                    if(clnt_sock > fd_max) 
                        fd_max=clnt_sock;
                    printf("connected client: %d\n", clnt_sock);
                    user_cnt++;
                    if(user_cnt<=3){
                        user[user_cnt-1]=clnt_sock;
                        strcpy(message, "############ Welcome to Hangman Game !! ############");
                        if(user_cnt<3){
                            strcat(message, "\n상대방이 접속하면 게임이 시작됩니다.\n");
                        }
                        else if(FD_ISSET(user[0], &reads)&&FD_ISSET(user[1], &reads)&&user_cnt==3){
                            strcat(message, "\n잠시 후 게임이 시작됩니다.\n");
                            send(user[0], "잠시 후 게임이 시작됩니다.\n", strlen("잠시 후 게임이 시작됩니다.\n"), 0);
                            send(user[1], "잠시 후 게임이 시작됩니다.\n", strlen("잠시 후 게임이 시작됩니다.\n"), 0);
                            game_flag=1;
                        }    
                        send(clnt_sock, message, strlen(message), 0);
                    }
                    else{    //3명정원이 다차면
                        send(clnt_sock, "fail", strlen("fail"), 0);
                    }
 
                    if(FD_ISSET(user[0], &reads)&&FD_ISSET(user[1], &reads)&&FD_ISSET(user[2], &reads)&&game_flag==1&&user_cnt==3){    //게임시작
                        hint_cnt=HINT; hang_cnt=0;
                        strcpy(message, "이번 게임의 출제자는...\n");
                        send(user[0], message, strlen(message), 0);
                        send(user[1], message, strlen(message), 0);
                        send(user[2], message, strlen(message), 0);
                        //난수생성 
                        srand((unsigned)time(NULL));
                        examiner[0]=user[rand()%3];
                        if(examiner[0]==user[0]){
                            challenger1[0]=user[1];
                            challenger2[0]=user[2];
                        }
                        else if(examiner[0]==user[1]){
                            challenger1[0]=user[0];
                            challenger2[0]=user[2];
                        }
                        else{
                            challenger1[0]=user[0];
                            challenger2[0]=user[1];
                        }
                        strcpy(message, "상대방입니다!\n");
                        send(challenger1[0], message, strlen(message), 0);
                        strcpy(message, "출제자가 문제를 낼동안 잠시만 기다려주세요!\n");
                        send(challenger1[0], message, strlen(message), 0);
                        strcpy(message, "상대방입니다!\n");
                        send(challenger2[0], message, strlen(message), 0);
                        strcpy(message, "출제자가 문제를 낼동안 잠시만 기다려주세요!\n");
                        send(challenger2[0], message, strlen(message), 0);
                        strcpy(message, "당신입니다!\n");
                        send(examiner[0], message, strlen(message), 0);
                        strcpy(message, "출제할 영어단어를 소문자 알파벳으로 입력하세요 : ");
                        send(examiner[0], message, strlen(message), 0);
                        examiner[1]=1;
                    }
                }
                else
                {    //client소켓이므로 receive
                    str_len=recv(i, message, BUF_SIZE, 0);
                    
                    if(str_len==0){    //client가 게임을 종료한 경우
                        FD_CLR(i, &reads);
                        close(i);
                        printf("closed socket: %d\n", i);
                        user_cnt--;
                        if(user_cnt<3 && game_flag==1){ //사용자 퇴장 시 게임상태 변경, 게임 종료알림
                            strcpy(message, "\n사용자가 퇴장하여 게임이 중지되었습니다.\n상대방이 접속하면 게임이 다시 시작됩니다.\n");
                            challenger1[1]=0;
                            challenger2[1]=0;
                            examiner[1]=0;
                            if(user[0]==i){
                                user[0]=user[1];
                                user[1]=user[2];
                            }
                            else if(user[1]==i){
                                user[1]=user[2];
                            }
                            send(user[0], message, strlen(message), 0);
                            send(user[1], message, strlen(message), 0);                     
                            game_flag=0;
                        }
                    }
                    else if(i==challenger1[0]&&FD_ISSET(challenger2[0], &reads)&&FD_ISSET(examiner[0], &reads)&&game_flag==1)
                    {    
                        message[str_len-1]=0;
                        if(strcmp(message, ""))
                        switch(challenger1[1]){
                            case 1:
                                if(!strcmp(message, "hint")){    //힌트요청시
                                    if(hint_cnt==0){    //사용가능힌트가없으면
                                        strcpy(message, "힌트를 모두 소진하였습니다.\n");
                                        strcat(message, "소문자 알파벳을 입력하세요 : ");
                                        send(i, message, strlen(message), 0);
                                    }
                                    else{
                                        strcpy(message, "1번 도전자가 힌트를 요청하였습니다.\n");
                                        strcat(message, "힌트를 입력하세요 : ");
                                        send(examiner[0], message, strlen(message), 0);
                                        examiner[1]=3;
                                        challenger1[1]=0;
                                        challenger2[1]=0;
                                    }
                                    break;
                                }
                                iscontinue=0; iscorrect=0;
                                if(strlen(message)==1){//알파벳 답확인 
                                    for(j=0;  j<strlen(word); j++){
                                        if(word[j]==message[0]){
                                            iscorrect=1;
                                            question[j]=word[j];
                                        }
                                        else if(question[j]=='*')
                                            iscontinue=1;
                                    }
                                    strcat(message, "\n");
                                    send(examiner[0], "1번 도전자의 답 입력 : ", strlen("1번 도전자의 답 입력 : "), 0);
                                    send(examiner[0], message, strlen(message), 0);
                                    send(challenger2[0], "1번 도전자의 답 입력 : ", strlen("1번 도전자의 답 입력 : "), 0);
                                    send(challenger2[0], message, strlen(message), 0);
                                    if(iscorrect==1)    //맞췄을때
                                        strcpy(message, "맞췄습니다!\n");
                                    else{            //틀렸을때
                                        strcpy(message, "틀렸습니다!\n");    
                                        hang_cnt++;
                                    }    //결과알림
                                    strcat(message, "- 문제 : ");
                                    strcat(message, question); strcat(message, "\n");
                                    strcat(message, drawHangman(hang_cnt));
                                    send(examiner[0], message, strlen(message), 0);
                                    send(challenger1[0], message, strlen(message), 0);
                                    send(challenger2[0], message, strlen(message), 0);
                                    if(hang_cnt==6){    //마지막기회일때 
                                        strcpy(message, "마지막 기회입니다.\n");
                                        send(examiner[0], message, strlen(message), 0);
                                        send(challenger1[0], message, strlen(message), 0);
                                        send(challenger2[0], message, strlen(message), 0);
                                    }    
                                    if(iscontinue==0){    //다맞췄을때 
                                        strcpy(message, "도전자의 승리!\n");
                                    }
                                    else if(hang_cnt==7){    //기회를모두소진했을때 
                                        strcpy(message, "출제자의 승리!\n");
                                    }
                                    else{    //그렇지않았을때->다시답을입력받음 
                                        strcpy(message, "2번 도전자가 답을 입력중입니다.\n");
                                        send(examiner[0], message, strlen(message), 0);
                                        strcpy(message, "* 'hint'입력시 힌트사용가능.\n 2번 도전자가 답을 입력중입니다. ");
                                        send(challenger1[0], message, strlen(message), 0);
                                        strcpy(message, "당신의 차례입니다!\n소문자 알파벳을 입력하세요 : ");
                                        send(challenger2[0], message, strlen(message), 0);
                                        break;
                                    }    //게임의종료 (클라이언트가 나가지않으면 게임은 계속됨)
                                    strcat(message, "게임이 종료되었습니다.\n");
                                    strcat(message, "다음 게임에서는 역할이 바뀝니다.\n이번 게임의 출제자는~?\n");
                                    send(examiner[0], message, strlen(message), 0);
                                    send(challenger1[0], message, strlen(message), 0);
                                    send(challenger2[0], message, strlen(message), 0);
                                    //게임재시작
                                    hint_cnt=HINT; hang_cnt=0;
                                    srand((unsigned)time(NULL));
                                    examiner[0]=user[rand()%3];
                                    if(examiner[0]==user[0]){
                                        challenger1[0]=user[1];
                                        challenger2[0]=user[2];
                                    }
                                    else if(examiner[0]==user[1]){
                                        challenger1[0]=user[0];
                                        challenger2[0]=user[2];
                                    }
                                    else{
                                        challenger1[0]=user[0];
                                        challenger2[0]=user[1];
                                    }
                                    strcpy(message, "상대방입니다!\n");
                                    strcat(message, "출제자가 문제를 낼동안 잠시만 기다려주세요!\n");
                                    send(challenger1[0], message, strlen(message), 0);
                                    strcpy(message, "상대방입니다!\n");
                                    strcat(message, "출제자가 문제를 낼동안 잠시만 기다려주세요!\n");
                                    send(challenger2[0], message, strlen(message), 0);
                                    strcpy(message, "당신입니다!\n");
                                    strcat(message, "출제할 영어단어를 소문자 알파벳으로 입력하세요 : ");
                                    send(examiner[0], message, strlen(message), 0);
                                    examiner[1]=1;
                                    challenger1[1]=0;
                                    challenger2[1]=0;
                                    break;
                                }
                            default:
                            break;
                        }
                    }

                    else if(i==challenger2[0]&&FD_ISSET(challenger1[0], &reads)&&FD_ISSET(examiner[0], &reads)&&game_flag==1)
                    {    
                        message[str_len-1]=0;
                        if(strcmp(message, ""))
                        switch(challenger2[1]){
                            case 1:
                                if(!strcmp(message, "hint")){    //힌트요청시
                                    if(hint_cnt==0){    //사용가능힌트가없으면
                                        strcpy(message, "힌트를 모두 소진하였습니다.\n");
                                        strcat(message, "소문자 알파벳을 입력하세요 : ");
                                        send(i, message, strlen(message), 0);
                                    }
                                    else{
                                        strcpy(message, "2번 도전자가 힌트를 요청하였습니다.\n");
                                        strcat(message, "힌트를 입력하세요 : ");
                                        send(examiner[0], message, strlen(message), 0);
                                        examiner[1]=4;
                                        challenger1[1]=0;
                                        challenger2[1]=0;
                                    }
                                    break;
                                }
                                iscontinue=0; iscorrect=0;
                                if(strlen(message)==1){//알파벳 답확인 
                                    for(j=0;  j<strlen(word); j++){
                                        if(word[j]==message[0]){
                                            iscorrect=1;
                                            question[j]=word[j];
                                        }
                                        else if(question[j]=='*')
                                            iscontinue=1;
                                    }
                                    strcat(message, "\n");
                                    send(examiner[0], "2번 도전자의 답 입력 : ", strlen("2번 도전자의 답 입력 : "), 0);
                                    send(examiner[0], message, strlen(message), 0);
                                    send(challenger1[0], "2번 도전자의 답 입력 : ", strlen("2번 도전자의 답 입력 : "), 0);
                                    send(challenger1[0], message, strlen(message), 0);
                                    if(iscorrect==1)    //맞췄을때
                                        strcpy(message, "맞췄습니다!\n");
                                    else{            //틀렸을때
                                        strcpy(message, "틀렸습니다!\n");    
                                        hang_cnt++;
                                    }    //결과알림
                                    strcat(message, "- 문제 : ");
                                    strcat(message, question); strcat(message, "\n");
                                    strcat(message, drawHangman(hang_cnt));
                                    send(examiner[0], message, strlen(message), 0);
                                    send(challenger1[0], message, strlen(message), 0);
                                    send(challenger2[0], message, strlen(message), 0);
                                    if(hang_cnt==6){    //마지막기회일때 
                                        strcpy(message, "마지막 기회입니다.\n");
                                        send(examiner[0], message, strlen(message), 0);
                                        send(challenger1[0], message, strlen(message), 0);
                                        send(challenger2[0], message, strlen(message), 0);
                                    }    
                                    if(iscontinue==0){    //다맞췄을때 
                                        strcpy(message, "2번 도전자의 승리!\n");
                                    }
                                    else if(hang_cnt==7){    //기회를모두소진했을때 
                                        strcpy(message, "출제자의 승리!\n");
                                    }
                                    else{    //그렇지않았을때->다시답을입력받음 
                                        strcpy(message, "1번 도전자가 답을 입력중입니다.\n");
                                        send(examiner[0], message, strlen(message), 0);
                                        strcpy(message, "당신의 차례! \n소문자 알파벳을 입력하세요 : ");
                                        send(challenger1[0], message, strlen(message), 0);
                                        strcpy(message, "* 'hint'입력시 힌트사용가능.\n2번 도전자가 답을 입력중입니다.");
                                        send(challenger2[0], message, strlen(message), 0);
                                        break;
                                    }    //게임의종료 (클라이언트가 나가지않으면 게임은 계속됨)
                                    strcat(message, "게임이 종료되었습니다.\n");
                                    strcat(message, "다음 게임에서는 역할이 바뀝니다.\n이번 게임의 출제자는...\n");
                                    send(examiner[0], message, strlen(message), 0);
                                    send(challenger1[0], message, strlen(message), 0);
                                    send(challenger2[0], message, strlen(message), 0);
                                    //게임재시작
                                    hint_cnt=HINT; hang_cnt=0;
                                    srand((unsigned)time(NULL));
                                    examiner[0]=user[rand()%3];
                                    if(examiner[0]==user[0]){
                                        challenger1[0]=user[1];
                                        challenger2[0]=user[2];
                                    }
                                    else if(examiner[0]==user[1]){
                                        challenger1[0]=user[0];
                                        challenger2[0]=user[2];
                                    }
                                    else{
                                        challenger1[0]=user[0];
                                        challenger2[0]=user[1];
                                    }
                                    strcpy(message, "상대방입니다!\n");
                                    strcat(message, "출제자가 문제를 낼동안 잠시만 기다려주세요!\n");
                                    send(challenger1[0], message, strlen(message), 0);
                                    strcpy(message, "상대방입니다!\n");
                                    strcat(message, "출제자가 문제를 낼동안 잠시만 기다려주세요!\n");
                                    send(challenger2[0], message, strlen(message), 0);
                                    strcpy(message, "당신입니다!\n");
                                    strcat(message, "출제할 영어단어를 소문자 알파벳으로 입력하세요 : ");
                                    send(examiner[0], message, strlen(message), 0);
                                    examiner[1]=1;
                                    challenger1[1]=0;
                                    challenger2[1]=0;
                                    break;
                                }
                            default:
                            break;
                        }
                    }

                    else if(i==examiner[0]&&FD_ISSET(challenger1[0], &reads)&&FD_ISSET(challenger2[0], &reads)&&game_flag==1)
                    {    
                        message[str_len-1]=0;
                        if(strcmp(message, ""))
                        switch(examiner[1]){
                            case 1:
                                strcpy(word, message);
                                printf("%s\n", word);
                                strcpy(message, "**출제단어 : ");
                                strcat(message, word);
                                strcat(message, "\n맞으면 'y' / 틀리면 'n' 입력 : ");
                                send(i, message, strlen(message), 0);
                                examiner[1]=2;
                                break;
                            case 2:    //출제단어확인작업 
                                if(!strcmp(message, "y")){
                                    examiner[1]=0;
                                    challenger1[1]=1;
                                    challenger2[1]=1;
                                    for(j=0;  j<strlen(word); j++)
                                        question[j]='*';
                                    question[j]='\0';
                                    
                                    strcpy(message, "- 문제 : ");
                                    strcat(message, question); strcat(message, "\n");
                                    send(examiner[0], message, strlen(message), 0);
                                    send(challenger1[0], message, strlen(message), 0);
                                    send(challenger2[0], message, strlen(message), 0);

                                    strcpy(message, drawHangman(0));
                                    send(examiner[0], message, strlen(message), 0);
                                    send(challenger1[0], message, strlen(message), 0);
                                    send(challenger2[0], message, strlen(message), 0);

                                    strcpy(message, "도전자가 답을 입력중입니다.\n");
                                    send(examiner[0], message, strlen(message), 0);
                                    strcpy(message, "소문자 알파벳을 입력하세요 : ");
                                    send(challenger1[0], message, strlen(message), 0);
                                    strcpy(message, "1번 도전자가 입력중입니다. 잠시만 기다려주세요.");
                                    send(challenger2[0], message, strlen(message), 0);
                                }
                                else if(!strcmp(message, "n")){
                                    strcpy(message, "출제할 영어단어를 소문자 알파벳으로 입력하세요 : ");
                                    send(examiner[0], message, strlen(message), 0);
                                    examiner[1]=1;
                                }
                                else{
                                    strcpy(message, "다시입력하세요.\n맞으면 'y' / 틀리면 'n' 입력 : ");
                                    send(examiner[0], message, strlen(message), 0);
                                }
                                break;
                            case 3:    //1번 도전자가 힌트요청
                                strcat(message, "\n");
                                send(challenger1[0], message, strlen(message), 0);
                                send(challenger2[0], message, strlen(message), 0);
                                hint_cnt--;
                                strcpy(message, "1번 도전자가 답을 입력중입니다.\n");
                                send(examiner[0], message, strlen(message), 0);
                                strcpy(message, "소문자 알파벳을 입력하세요 : ");
                                send(challenger1[0], message, strlen(message), 0);
                                strcpy(message, "1번 도전자가 입력중입니다. 잠시만 기다려주세요.");
                                send(challenger2[0], message, strlen(message), 0);
                                examiner[1]=0;
                                challenger1[1]=1;
                                break;
                            case 4:    //2번 도전자가 힌트요청
                                strcat(message, "\n");
                                send(challenger1[0], message, strlen(message), 0);
                                send(challenger2[0], message, strlen(message), 0);
                                hint_cnt--;
                                strcpy(message, "2번 도전자가 답을 입력중입니다.\n");
                                send(examiner[0], message, strlen(message), 0);
                                strcpy(message, "2번 도전자가 입력중입니다. 잠시만 기다려주세요.");
                                send(challenger1[0], message, strlen(message), 0);
                                strcpy(message, "소문자 알파벳을 입력하세요 : ");
                                send(challenger2[0], message, strlen(message), 0);
                                examiner[1]=0;
                                challenger2[1]=1;
                                break;
                            default:
                            break;
                        }
                    }
                }
            }
        }
    }
 
    close(serv_sock);
    return 0;
}
 
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
 
char* drawHangman(int num){
    char* hangman={ 0 };
    switch (num)
    {
        case 0:
        hangman = "┌───┐\n│\n│\n│\n│\n└──────\n";
        break;
        case 1:
        hangman = "┌───┐\n│　○\n│\n│\n│\n└──────\n";
        break;
        case 2:
        hangman = "┌───┐\n│　○\n│　 |\n│\n│\n└──────\n";
        break;
        case 3:
        hangman = "┌───┐\n│　○\n│　/|\n│\n│\n└──────\n";
        break;
        case 4:
        hangman = "┌───┐\n│　○\n│　/|＼\n│　\n│\n└──────\n";
        break;
        case 5:
        hangman = "┌───┐\n│　○\n│　/|＼\n│　/\n│\n└──────\n";
        break;
        case 6:
        hangman = "┌───┐\n│　○\n│　/|＼\n│　/＼\n│\n└──────\n";
        break;
        case 7:
        hangman = "┌───┐\n│　○\n│　 X\n│　/|＼\n│　/＼\n└──────\n";
        break;
        default:
        hangman = "drawing error\n";
        break;
    }
    return hangman;
}

