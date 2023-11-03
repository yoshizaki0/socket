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
if((connect(sock,(struct sockaddr_in *)&svr,sizeof(svr)))>0)
{
    printf("Server connect/n");
    close(sock);
    exit(1);
}

while(1){
if(fgets(message,1024,stdin)==NULL)
{
    close(sock);
    break;
}
write(sock,message,1024);
read(sock,readmes,1024);
printf("%s",readmes);
}
}

