#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#define TIMEOUT 10
void send_input_data(int socktnum);
void myalarm(int sec);
void timeout(void);
int flag=0;
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
if(signal(SIGALRM,timeout) == SIG_ERR) {/*シグナルの登録*/
perror("signal failed.");
exit(1);
}
/* ソケットの生成 */
if ((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0) {
perror("socket");
exit(1);
}

/* host(ソケットの接続先) の情報設定 */
bzero((char *)&svr,sizeof(svr));
svr.sin_family=AF_INET;
svr.sin_port=htons(10130);
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
myalarm(TIMEOUT);
do{
    if(flag==1)
{     
    close(sock);
    close(csock);
   break;
}
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
if((fgets(message,1024,stdin))==NULL)
{  
    break;
}

write(sock,message,sizeof(message));
myalarm(TIMEOUT);
}
if(FD_ISSET(sock,&rfds)) { /* ソケットから受信したなら */
/* ソケットから読み込み端末に出力 */
bzero(&readmes,sizeof(readmes));
if((read(sock,readmes,sizeof(readmes)))==0)
{
break;
}

printf("%s",readmes);
myalarm(TIMEOUT);
}
}
} while(1);
close(sock);
  close(csock);
printf("closed\n");
exit(0);
}
void myalarm(int sec) {
static int pid=-1;
int c_pid,num;
int i;
struct sigaction sa;
   sa.sa_handler = SIG_IGN;
   sa.sa_flags = SA_NOCLDWAIT;
   if (sigaction(SIGCHLD, &sa, NULL) == -1) {
       exit(1);
   }
c_pid = getpid();
if(pid!=-1)
{
if (kill(pid,SIGTERM) == -1) {
perror("kill failed.");
exit(1);
}

}


if ((pid=fork())== -1) {
perror("fork failed.");
exit(1);
}
if (pid == 0) { /* Child process */
for(i=0;i<10;i++)
{
sleep(sec/10);
if(c_pid!=getppid())
{
 exit(0);
}
}
if( kill(c_pid,SIGALRM)== -1) {
perror("signal failed.");
exit(1);
}

exit(0);
}
else{/* Parent process */

}
}
void timeout()
{
printf("This program is timeout.\n");
flag=1;
}
