
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <math.h>

void mergeSort(int *array, int start, int end);
void merge(int *array, int start, int middle, int end);
int bruteSort(int array[], int len);

void *threadMergeSort(void *args);
void *threadMerge(void *args);

typedef struct info
{
  int *array;
  int start;
  int end;
}Info;

int main(int argc, char *argv[])
{
  int numprocessors = sysconf(_SC_NPROCESSORS_ONLN);

  int length = 50000000;

  int *array = malloc(sizeof(int) * length);

  for(int i = 0; i < length; i ++)
  {
    array[i] = rand() % length;
    // printf("%d,", array[i]);
  }
  // printf("%s\n", "");

  struct timeb start;
  ftime(&start);

  {
    pthread_t tid[numprocessors];
    Info info[numprocessors];

    for(int i = 0; i < numprocessors; i ++)
    {
      info[i].array = array;
      info[i].start = (float)length/(float)numprocessors*(float)i;
      info[i].end = (float)length/(float)numprocessors*(float)(i+1);
        pthread_create(tid+i, NULL, &threadMergeSort, (void*)&(info[i]));
    }

    for(int i = 0; i < numprocessors; i ++)
    {
      pthread_join(tid[i], NULL);
    }
  }

  while((numprocessors/=2) > 0)
  {
    pthread_t tid[numprocessors];
    Info info[numprocessors];

    for(int i = 0; i < numprocessors; i ++)
    {
      info[i].array = array;
      info[i].start = (float)length/(float)numprocessors*(float)i;
      info[i].end = (float)length/(float)numprocessors*(float)(i+1);
      pthread_create(tid+i, NULL, &threadMerge, (void*)&(info[i]));
    }

    for(int i = 0; i < numprocessors; i ++)
    {
      pthread_join(tid[i], NULL);
    }
  }

  struct timeb end;
  ftime(&end);
  printf("%d\n", (int)(1000 * (end.time - start.time) + (end.millitm - start.millitm)));

  // for(int i = 0; i < length; i ++)
  // {
  //     printf("%d,", array[i]);
  // };
  //
  // printf("%s\n", "");

  free(array);

  return 0;
}

void *threadMerge(void *args)
{
  Info *info = (Info *)args;

  int middle = (info->end-info->start)/2;
  // printf("middle%d\n", middle);

  merge(info->array, info->start, middle+info->start-1, info->end-1);

  pthread_exit(NULL);
}

void *threadMergeSort(void *args)
{
  Info *info = (Info *)args;

  mergeSort(info->array, info->start, info->end-1);

  pthread_exit(NULL);
}

void mergeSort(int *array, int start, int end)
{
  if(start < end)
  {
    int middle = (end-start)/2;

    mergeSort(array, start, middle+start);
    mergeSort(array, middle+start+1, end);

    merge(array, start, middle+start, end);
  }
}

void merge(int *array, int start, int middle, int end)
{
  int llen = middle-start+1;
  int rlen = end-middle;

  int *left = malloc(sizeof(int) * (llen));
  int *right = malloc(sizeof(int) * (rlen));

  {
    int i;
    int z;
    for (i = 0, z = 0; i < llen || z < rlen; i ++, z++)
    {
      if(i < llen)
      {
        left[i] = array[start+i];
      }
      if(z < rlen)
      {
        right[z] = array[i+middle+1];
      }
    }
  }

  {
    int l;
    int r;
    int i;
    for(i = start, l = 0, r = 0; i < end+1; i ++)
    {
      if(r == rlen)
      {
        array[i] = left[l];
        l++;
      }
      else if(l == llen)
      {
        array[i] = right[r];
        r++;
      }
      else if(r == rlen || left[l] < right[r])
      {
        array[i] = left[l];
        l++;
      }
      else
      {
        array[i] = right[r];
        r++;
      }
    }
  }
  free(left);
  free(right);
}

int bruteSort(int array[], int len)
{
  int inversions = 0;

  {
    int i;
    for(i = 0; i < len-1; i ++)
    {
      {
        int z;
        for(z = i+1; z < len; z ++)
        {
          if(array[i] > array[z])
          {
            inversions ++;
          }
        }
      }
    }
    printf("\n");
  }

  return inversions;
}
