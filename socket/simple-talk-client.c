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
int clen,nbytes,reuse;
char rbuf[1024];
char message[1024];
char readmes[1024];
/* ソケットの生成 */
if ((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0) {
perror("socket");
exit(1);
}

/* host(ソケットの接続先) の情報設定 */
bzero((char *)&svr,sizeof(svr));
svr.sin_family=AF_INET;
svr.sin_port=htons(10120);
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
bzero(&message,sizeof(message));
if(read(0,message,sizeof(message))<0)
{
    close(sock);
    break;
}

write(sock,message,sizeof(message));
}
if(FD_ISSET(sock,&rfds)) { /* ソケットから受信したなら */
/* ソケットから読み込み端末に出力 */
bzero(&readmes,sizeof(readmes));
read(sock,readmes,sizeof(readmes));
printf("%s",readmes);
}
}
} while(1);
}
