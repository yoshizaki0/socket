#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc,char **argv) {
int asock,bsock,csock;
struct sockaddr_in svr;
struct sockaddr_in clt;
struct hostent *cp;
int clen,nbytes,reuse;
char rbuf[1024];
/* ソケットの生成 */
if ((asock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0) {
perror("socket");
exit(1);
}
/* ソケットアドレス再利用の指定 */
reuse=1;
if(setsockopt(asock,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))<0) {
perror("setsockopt");
exit(1);
}
/* client 受付用ソケットの情報設定 */
bzero(&svr,sizeof(svr));
svr.sin_family=AF_INET;
svr.sin_addr.s_addr=htonl(INADDR_ANY); /* 受付側の IP アドレスは任意 */
svr.sin_port=htons(10120); /* ポート番号 10120 を介して受け付ける */
/* ソケットにソケットアドレスを割り当てる */
if(bind(asock,(struct sockaddr *)&svr,sizeof(svr))<0) {
perror("bind");
exit(1);
}
/* 待ち受けクライアント数の設定 */
if (listen(asock,5)<0) { /* 待ち受け数に 5 を指定 */
perror("listen");
exit(1);
}
do {
/* クライアントの受付 */
clen = sizeof(clt);
if ( ( csock = accept(asock,(struct sockaddr *)&clt,&clen) ) <0 ) {
perror("accept");
exit(2);
}
/* クライアントのホスト情報の取得 */
cp = gethostbyaddr((char *)&clt.sin_addr,sizeof(struct in_addr),AF_INET);
printf("[%s]\n",cp->h_name);

if ( ( bsock = accept(asock,(struct sockaddr *)&clt,&clen) ) <0 ) {
perror("accept");
exit(2);
}
/* クライアントのホスト情報の取得 */
cp = gethostbyaddr((char *)&clt.sin_addr,sizeof(struct in_addr),AF_INET);
printf("[%s]\n",cp->h_name);
do {
/* クライアントからのメッセージ受信 */
if ( ( nbytes = read(csock,rbuf,sizeof(rbuf)) ) < 0) {
perror("read");
} else {
write(bsock,rbuf,nbytes);
/* 受信文字列をそのままクライアントへ返す (echo) */
}
} while ( nbytes != 0 ); /* read() が 0 を返すまで (=End-Of-File) 繰り返す */
close(csock);
close(bsock);
printf("closed\n");
} while(1); /* 次の接続要求を繰り返し受け付ける */
}
