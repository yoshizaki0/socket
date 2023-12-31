#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/wait.h>
#define NUMPROCS 4
char filename[]="counter";
int count1()
{

FILE *ct;
int count;

if ((ct=fopen(filename, "r"))==NULL) exit(1);
fscanf(ct, "%d\n", &count);
count++;
fclose(ct);
if ((ct=fopen(filename, "w"))==NULL) exit(1);
fprintf(ct, "%d\n", count);
fclose(ct);
return count;
}
int main()
{
int i, sid;
key_t key;
struct sembuf {
unsigned short sem_num; /* セマフォ番号 */
short sem_op; /* セマフォ操作 */
short sem_flg; /* 操作フラグ */
};
struct sembuf sb;
int count, pid, status;
FILE *ct;
sb.sem_num=0;
sb.sem_flg=0;
setbuf(stdout, NULL); /* set stdout to be unbufferd */
if ((key = ftok(".", 1)) == -1){
fprintf(stderr,"ftok path does not exist.\n");
exit(1);
}
if ((sid=semget(key, 1, 0666 | IPC_CREAT)) == -1) {
perror("semget error.");
exit(1);
}
semctl(sid, 0, SETVAL, 1);
setbuf(stdout, NULL); /* set stdout to be unbufferd */
count = 0;
if ((ct=fopen(filename, "w"))==NULL) exit(1);
fprintf(ct, "%d\n", count);
fclose(ct);
for (i=0; i<NUMPROCS; i++) {
if ((pid=fork())== -1) {
perror("fork failed.");
exit(1);
}
if (pid == 0) { /* Child process */
sb.sem_op=-1;
if (semop(sid, &sb, 1) == -1) {
perror("sem_wait semop error.");
exit(1);
}
count = count1();
sb.sem_op=1;
if (semop(sid, &sb, 1) == -1) {
perror("sem_wait semop error.");
exit(1);
}
printf("count = %d\n", count);

exit(0);
}
}
for (i=0; i<NUMPROCS; i++) {
wait(&status);
}
semctl(sid, 0, IPC_RMID);
exit(0);
}