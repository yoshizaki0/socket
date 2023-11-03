BUFSIZE=100;

int main(int argc, char *argv[])
{
char buf[BUFSIZE];
char fbuf[BUFSIZE];
int fd[2];
int two[2];
int pid, msglen,msglens, status;
if (argc != 3) {
printf("bad argument.\n");
exit(1);
}
if (pipe(fd) == -1) {
perror("pipe failed.");
exit(1);
}
if (pipe(two) == -1) {
perror("pipe2 failed.");
exit(1);
}
if ((pid=fork())== -1) {
perror("fork failed.");
exit(1);
}
if (pid == 0) { /* Child process */
close(fd[0]);
close(two[1]);
msglen = strlen(argv[1]) + 1;
if (write(fd[1], argv[1], msglen) == -1) {
perror("pipe write.");
exit(1);
}
if (read(two[0], fbuf, BUFSIZE) == -1) {
perror("pipe read.");
exit(1);
}
printf("Message from oya process: \n");
printf("\t%s\n",fbuf);
exit(0);
} else { /* Parent process */
close(fd[1]);
close(two[0]);
msglens = strlen(argv[2]) + 1;
if (write(two[1], argv[2], msglens) == -1) {
perror("pipe write.");
exit(1);
}
if (read(fd[0], buf, BUFSIZE) == -1) {
perror("pipe read.");
exit(1);
}


wait(&status);
printf("Message from child process: \n");
printf("\t%s\n",buf);
}
}