#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include<math.h>

void merge(int *a, int *b, int lb, int m, int ub);

void mergeSort(int *a, int *b, int lb, int ub);

int main(int argc, char *argv[])
{
  MPI_Init(&argc, &argv);
  srand(time(NULL));
  int rank;
  int npes;
  MPI_Comm_size(MPI_COMM_WORLD, &npes); MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  


  int n;
  if (rank == 0){
    printf("Input n: \n");
    scanf("%d", &n);    
  }


  MPI_Bcast(&n, sizeof(int), MPI_INT, 0, MPI_COMM_WORLD);

  int size1 = n / npes;
  int *subarray = malloc(size1*sizeof(int));
  int *sorted = malloc(size1*sizeof(int));

  int size2 = size1;
  
  printf("Processor %d: \n ", rank);
  for (int i = 0; i < size1; i++)
  {
    subarray[i] = rand();
    printf("%d ", subarray[i]);
    sorted[i] = subarray[i];
  }
  printf("\n");

  mergeSort(subarray, sorted, 0, size1-1);
  
  int cur_npes = npes;
  int some = 1;
  while(cur_npes > 1){
    if(rank % (some) != 0) {
      break;
    }
    int size1;
    int *other = (int* )malloc(size2*sizeof(int));
    if(rank % 2*some == (some)){
      size1 = size2;
      MPI_Send(&size1, 1, MPI_INT, rank-some, 100, MPI_COMM_WORLD);
      for(int i=0;i<size1;i++){
        other[i] = sorted[i];
      }
      MPI_Send(other, size1, MPI_INT, rank-some, 110, MPI_COMM_WORLD);
    } else if(rank/(some) != cur_npes-1){
      int r1 = size2;
      MPI_Recv(&size1, 1, MPI_INT, rank+some, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      sorted = (int* )realloc(sorted, (size1+r1)*sizeof(int));
      size2 = size1+r1;
      MPI_Recv(other, size1, MPI_INT, rank+some, 110, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      for(int i=0;i<size1;i++){
        sorted[r1+i] = other[i];
      }
      int* newSorted = (int*)malloc(size2*sizeof(int));
      merge(sorted, newSorted, 0, r1-1, size1+r1-1);
      sorted = newSorted;

    }
    some *= 2;
    cur_npes = (cur_npes+1) / 2;
  }

  if(rank == 0){
    for(int i=0;i<size2;i++){
      printf("%d ", sorted[i]);
    }
    printf("\n");
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;
}

void merge(int *a, int *b, int lb, int m, int ub)
{
  int h, i, j, k;
  h = lb;
  i = lb;
  j = m + 1;

  while ((h <= m) && (j <= ub))
  {

    if (a[h] <= a[j])
    {
      b[i++] = a[h++];
    }

    else
    {

      b[i++] = a[j++];
    }
  }

  if (m < h)
  {

    for (k = j; k <= ub; k++)
    {
      b[i++] = a[k];
    }
  }

  else
  {

    for (k = h; k <= m; k++)
    {
      b[i++] = a[k];
    }
  }

  for (k = lb; k <= ub; k++)
  {

    a[k] = b[k];
  }
}

void mergeSort(int *a, int *b, int lb, int ub)
{

  int m;

  if (lb < ub)
  {

    m = (lb + ub) / 2;

    mergeSort(a, b, lb, m);
    mergeSort(a, b, (m + 1), ub);
    merge(a, b, lb, m, ub);
  }
}
