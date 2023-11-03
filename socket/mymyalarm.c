#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#define BUFSIZE 256
#define TIMEOUT 10


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
/*while((ed-st)/1000>sec)
{
    ed=clock();
}
*/
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
exit(0);
}

int main()
{
char buf[BUFSIZE];
if(signal(SIGALRM,timeout) == SIG_ERR) {
perror("signal failed.");
exit(1);
}
myalarm(TIMEOUT);
while (fgets(buf, BUFSIZE, stdin) != NULL) {
printf("echo: %s",buf);
myalarm(TIMEOUT);
}
}
