
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#define NUM_ITEMS 10

void mergeSort(int numbers[], int temp[], int array_size);
void m_sort(int numbers[], int temp[], int left, int right);
void merge(int numbers[], int temp[], int left, int mid, int right);

int numbers[NUM_ITEMS];
int temp[NUM_ITEMS];


int main()
{
  int i,p1,p2;
  int min,max;
  int status;
  int fd[2];
int two[2];
if (pipe(fd) == -1) {
perror("pipe failed.");
exit(1);
}
if (pipe(two) == -1) {
perror("pipe2 failed.");
exit(1);
}
  //seed random number generator
  srand(getpid());

  //fill array with random integers
  for (i = 0; i < NUM_ITEMS; i++)
    numbers[i] = rand();

min=(NUM_ITEMS-1)/2;
max=NUM_ITEMS-1;

if ((p1=fork())== -1) {
perror("fork failed.");
exit(1);
}
if ((p2=fork())== -1) {
perror("fork failed.");
exit(1);
}

  if (p1 == 0) { /* Child1 process */
m_sort(numbers,temp,0,min);
close(fd[0]);
close(two[0]);
close(two[1]);
if (write(fd[1], numbers, sizeof(numbers)) == -1) {
perror("pipe write.");
exit(1);
}
exit(0);
  }
  if (p2 == 0) { /* Child2 process */
m_sort(numbers,temp,min+1,max);
close(two[0]);
close(fd[0]);
close(fd[1]);
if (write(two[1], numbers, sizeof(numbers)) == -1) {
perror("pipe write.");
exit(1);
}
exit(0);

  }
  else{/* Parent process */
  close(fd[1]);
  close(two[1]);
  if (read(fd[0], numbers, sizeof(numbers)) == -1) {
       perror("pipe read.");
       exit(1);
       }
      for(i=0;i<NUM_ITEMS;i++)
      temp[i]=numbers[i];

  if (read(two[0], numbers, sizeof(numbers)) == -1) {
      perror("pipe read.");
      exit(1);
     }
     for(i=0;i<min+1;i++)
     numbers[i]=temp[i];
  wait(&status);


  merge(numbers, temp, 0, min+1, max);
  //perform merge sort on array

  printf("Done with sort.\n");

  for (i = 0; i < NUM_ITEMS; i++)
    printf("%i\n", numbers[i]);
  }
  return 0;
}


void mergeSort(int numbers[], int temp[], int array_size)
{
  m_sort(numbers, temp, 0, array_size - 1);
}



void m_sort(int numbers[], int temp[], int left, int right)
{
  int mid;

  if (right > left)
  {
    mid = (right + left) / 2;
    m_sort(numbers, temp, left, mid);
    m_sort(numbers, temp, mid+1, right);

    merge(numbers, temp, left, mid+1, right);
  }
}


void merge(int numbers[], int temp[], int left, int mid, int right)
{
  int i, left_end, num_elements, tmp_pos;

  left_end = mid - 1;
  tmp_pos = left;
  num_elements = right - left + 1;

  while ((left <= left_end) && (mid <= right))
  {
    if (numbers[left] <= numbers[mid])
    {
      temp[tmp_pos] = numbers[left];
      tmp_pos = tmp_pos + 1;
      left = left +1;
    }
    else
    {
      temp[tmp_pos] = numbers[mid];
      tmp_pos = tmp_pos + 1;
      mid = mid + 1;
    }
  }

  while (left <= left_end)
  {
    temp[tmp_pos] = numbers[left];
    left = left + 1;
    tmp_pos = tmp_pos + 1;
  }
  while (mid <= right)
  {
    temp[tmp_pos] = numbers[mid];
    mid = mid + 1;
    tmp_pos = tmp_pos + 1;
  }

  for (i=0; i <= num_elements; i++)
  {
    numbers[right] = temp[right];
    right = right - 1;
  }
}
