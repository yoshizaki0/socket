#define MAXCLIENTS 5
#include <sys/types.h>
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
int clen,nbytes=1,reuse;
int state=1;
int i,j,flag=0;
int maxsock;
int  socknum[MAXCLIENTS];
char username[MAXCLIENTS][1024];
char namebox[1024];
char remove[1024];
int max=MAXCLIENTS;
int cln_num=0;
int numbers=0;
char copyname[1048];
char rbuf[1024];
char message[1024];
char readmes[1024];
bzero(&readmes,sizeof(readmes));
if(state==1){
    cln_num=0;
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

bzero(&svr,sizeof(svr));
svr.sin_family=AF_INET;
svr.sin_addr.s_addr=htonl(INADDR_ANY); /* 受付側の IP アドレスは任意 */
svr.sin_port=htons(10140); /* ポート番号 10140 を介して受け付ける */
/* ソケットにソケットアドレスを割り当てる */
if(bind(sock,(struct sockaddr *)&svr,sizeof(svr))<0) {
perror("bind");
exit(1);
}
/* 待ち受けクライアント数の設定 */
if (listen(sock,max)<0) { /* 待ち受け数に 5 を指定 */
perror("listen");
exit(1);
}
    state=2;
}
    if(state==2)
    { 
        do{ 
        flag=0; 
        FD_ZERO(&rfds); /* rfds を空集合に初期化 */
        for(i=0;i<max;i++){
        FD_SET(socknum[i],&rfds); /* 標準入力 */
        }
        FD_SET(sock,&rfds); /* クライアントを受け付けたソケット */
        maxsock=socknum[0];
        for(i=0;i<cln_num;i++){
        if(maxsock<socknum[i])
        {
          maxsock=socknum[i];
        }
        }
        if(maxsock<sock)
        {
            maxsock=sock;
        }
            /* 監視する待ち時間を 1 秒に設定 */
        tv.tv_sec = 1;
        tv.tv_usec = 0;
/* 標準入力とソケットからの受信を同時に監視する */
        if(select(maxsock+1,&rfds,NULL,NULL,&tv)>0) {
        if(FD_ISSET(sock,&rfds)) { /*state=4*//* 受付要求があったら */
        clen = sizeof(clt);
        if ( ( csock = accept(sock,(struct sockaddr *)&clt,&clen) ) <0 ) {
            perror("accept");
            exit(1);
        }
        if(cln_num>max-1){
            write(csock,"REQUEST REJECTED\n",17);
            close(csock);

        }
        else
        {    
            
            write(csock,"REQUEST ACCEPTED\n",17);
            /*state=5*/
            bzero(&remove,sizeof(remove));
            if ( ( nbytes = read(csock,remove,sizeof(remove)) ) < 0) {
                    perror("read");
                }
                bzero(&namebox,sizeof(namebox));
                memcpy(namebox,remove, nbytes-1);
            
            
            for(j=0;j<cln_num;j++)
            {  
                if(strncmp(namebox,username[j],sizeof(namebox)+1)==0)
                {
                   write(csock,"USERNAME REJECTED\n",17);
                   printf("reject username of < %s >\n",namebox);
                   close(csock);
                   flag=1;
                }
        
            }
        if(flag==0)
        {   
            strcpy(username[cln_num],namebox);
            write(csock,"USERNAME REGISTERED\n",20);
            printf("regist username of < %s >\n",namebox);
            socknum[cln_num]=csock;
            cln_num++;

        }

        
        }
        }
        for(i=0;i<cln_num;i++){
        if(FD_ISSET(socknum[i],&rfds)) { /* ソケットから受信したなら *//*srate=6*/
            bzero(&readmes,sizeof(readmes));
                        numbers=i;

                    
                
            if ( ( nbytes = read(socknum[i],readmes,sizeof(readmes)) ) ==0) {
                printf("exit username of < %s >\n",username[i]);
                close(socknum[i]);
                for(j=numbers;j<cln_num-1;j++)
                {
                    socknum[j]=socknum[j+1];
                    strcpy(username[j],username[j+1]);
                }
                cln_num=cln_num-1;


                    
                }
            
        else{
            
            for(j=0;j<cln_num;j++)
            {
            bzero(&copyname,sizeof(copyname));
            strcpy(copyname,username[numbers]);
            strcat(copyname,">");
            strcat(copyname,readmes);
            write(socknum[j],copyname,sizeof(copyname));
            }
            }   

        }
        }
        }

} while(1);

}
}

