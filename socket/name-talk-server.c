#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc,char **argv) {
fd_set rfds; /* select() で用いるファイル記述子集合 */
struct timeval tv; /* select() が返ってくるまでの待ち時間を指定する変数 */
int sock,csock;
struct sockaddr_in svr;
struct sockaddr_in clt;
struct hostent *cp;
int clen,nbytes,reuse;
char rbuf[1024];
char message[1024];
char readmes[1024];
char  rname[1024];
char  tname[1024];
/* ソケットの生成 */
if ((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0) {
perror("socket");
exit(1);
}
/* ソケットアドレス再利用の指定 */
reuse=1;
if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))<0) {
perror("setsockopt");
exit(1);
}
/* client 受付用ソケットの情報設定 */
bzero(&svr,sizeof(svr));
svr.sin_family=AF_INET;
svr.sin_addr.s_addr=htonl(INADDR_ANY); /* 受付側の IP アドレスは任意 */
svr.sin_port=htons(10120); /* ポート番号 10120 を介して受け付ける */
/* ソケットにソケットアドレスを割り当てる */
if(bind(sock,(struct sockaddr *)&svr,sizeof(svr))<0) {
perror("bind");
exit(1);
}
/* 待ち受けクライアント数の設定 */
if (listen(sock,5)<0) { /* 待ち受け数に 5 を指定 */
perror("listen");
exit(1);
}
do {
/* クライアントの受付 */
clen = sizeof(clt);
if ( ( csock = accept(sock,(struct sockaddr *)&clt,&clen) ) <0 ) {
perror("accept");
exit(2);
}
/* クライアントのホスト情報の取得 */
cp = gethostbyaddr((char *)&clt.sin_addr,sizeof(struct in_addr),AF_INET);
printf("[%s]\n",cp->h_name);
printf("plese your name");
scanf("%s",tname);
fgets(readmes,1024,stdin);
write(csock,tname,sizeof(message));
read(csock,rname,sizeof(rname));

do{
/* 入力を監視するファイル記述子の集合を変数 rfds にセットする */
FD_ZERO(&rfds); /* rfds を空集合に初期化 */
FD_SET(0,&rfds); /* 標準入力 */
FD_SET(csock,&rfds); /* クライアントを受け付けたソケット */
/* 監視する待ち時間を 1 秒に設定 */
tv.tv_sec = 1;
tv.tv_usec = 0;
/* 標準入力とソケットからの受信を同時に監視する */
if(select(csock+1,&rfds,NULL,NULL,&tv)>0) {
if(FD_ISSET(0,&rfds)) { /* 標準入力から入力があったなら */
/* 標準入力から読み込みクライアントに送信 */
bzero(&message,sizeof(message));
if(read(0,message,sizeof(message))<0)
{
    close(csock);
    break;
}

write(csock,message,sizeof(message));
printf("%s:%s",tname,message);
}
if(FD_ISSET(csock,&rfds)) { /* ソケットから受信したなら */
/* ソケットから読み込み端末に出力 */
bzero(&readmes,sizeof(readmes));
read(csock,readmes,sizeof(readmes));
printf("%s:",rname);
printf("%s",readmes);
}
}
} while(1);
close(csock);
printf("closed\n");
} while(1); /* 次の接続要求を繰り返し受け付ける */
}
