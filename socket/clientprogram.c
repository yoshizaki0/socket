#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
void send_input_data(int socktnum);
int main(int argc,char **argv) {
fd_set rfds; /* select() で用いるファイル記述子集合 */
struct timeval tv; /* select() が返ってくるまでの待ち時間を指定する変数 */
int sock,csock;
struct sockaddr_in svr;
struct sockaddr_in clt;
struct hostent *hp;
int clen,nbytes=1,reuse;
int state=1;
int msglen;
char rbuf[1024];
char message[1024];
char readmes[1024];
/*state1*/
if(state==1){
/* ソケットの生成 */
    if ((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0) {
        perror("socket");
        exit(1);
    }

/* host(ソケットの接続先) の情報設定 */
    bzero((char *)&svr,sizeof(svr));
    svr.sin_family=AF_INET;
    svr.sin_port=htons(10140);
    if ( ( hp = gethostbyname(argv[1]) ) == NULL ) {
        fprintf(stderr,"unknown host %s\n",argv[1]);
        exit(1);
    }
    bcopy(hp->h_addr,&svr.sin_addr,hp->h_length);
    if((connect(sock,(struct sockaddr *)&svr,sizeof(svr)))>0)
    {
        printf("Server connect/n");
        close(sock);
        exit(1);
    }
    state=2;
}
/*state2*/
if(state==2){
    read(sock,readmes,sizeof(readmes));
    /*mes= &readmes[0];*/
/*接続許可の確認*/
    if(strncmp(readmes,"REQUEST ACCEPTED\n",17)==0)
    {
        printf("ACCCEPTED\n");
     state=3;
    }
    else{
        
        state=6;
    }
}
/*state3*/
if(state==3){     
    strcpy(rbuf,argv[2]);
    int len = strlen(rbuf);
    rbuf[len++]='\n';
    rbuf[len]='\0';
    msglen = strlen(rbuf);
  
    write(sock,rbuf,msglen);
    read(sock,readmes,sizeof(readmes));
    printf("go home\n");
    if(strncmp(readmes,"USERNAME REGISTERED\n",20)==0)
    {
        printf("REGISTERED\n");
     state=4;
    }
    else{
    printf("go state6\n");
        state=6;
    }

}
if(state==4){
    do{
/* 入力を監視するファイル記述子の集合を変数 rfds にセットする */
        FD_ZERO(&rfds); /* rfds を空集合に初期化 */
        FD_SET(0,&rfds); /* 標準入力 */
        FD_SET(sock,&rfds); /* クライアントを受け付けたソケット */
/* 監視する待ち時間を 1 秒に設定 */
        tv.tv_sec = 1;
        tv.tv_usec = 0;
/* 標準入力とソケットからの受信を同時に監視する */
        if(select(sock+1,&rfds,NULL,NULL,&tv)>0) {
            if(FD_ISSET(0,&rfds)) { /* 標準入力から入力があったなら */
/* 標準入力から読み込みクライアントに送信 */
                if ( ( nbytes = read(0,rbuf,sizeof(rbuf)) ) < 0) {
                    perror("read");
                } 
                else {
                        write(sock,rbuf,nbytes);
/* 受信文字列をそのままクライアントへ返す (echo) */
                }
/* 受信文字列をそのままクライアントへ返す (echo) */
}
                

            }
            if(FD_ISSET(sock,&rfds)) { /* ソケットから受信したなら */
/* ソケットから読み込み端末に出力 */
                if ( ( nbytes = read(sock,rbuf,sizeof(rbuf)) ) < 0) {
                    perror("read");
                } 
                else {
                        write(1,rbuf,nbytes);
/* 受信文字列をそのままクライアントへ返す (echo) */
                }
            
            }
        }while(nbytes!=0);
    state=5;
}
if(state==5){
    printf("closed\n");
    close(sock);
    exit(0);
}
if(state==6){
    printf("closed\n");
    close(sock);
    exit(1);
}
}




