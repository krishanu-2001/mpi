#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int randInt()
{
  return rand() % 1000;
}

  /********** Merge Function **********/
void merge(int *a, int *b, int l, int m, int r)
{

  // printf("][][] array - ");
  // for(int i=l;i<=r;i++){
  //   printf("%d ", a[i]);
  // }
  // printf("\n");
  int h, i, j, k;
  h = l;
  i = l;
  j = m + 1;

  while ((h <= m) && (j <= r))
  {

    if (a[h] <= a[j])
    {

      b[i] = a[h];
      h++;
    }

    else
    {

      b[i] = a[j];
      j++;
    }

    i++;
  }

  if (m < h)
  {

    for (k = j; k <= r; k++)
    {

      b[i] = a[k];
      i++;
    }
  }

  else
  {

    for (k = h; k <= m; k++)
    {

      b[i] = a[k];
      i++;
    }
  }

  for (k = l; k <= r; k++)
  {

    a[k] = b[k];
  }
}

/********** Recursive Merge Function **********/
void mergeSort(int *a, int *b, int l, int r)
{

  int m;

  if (l < r)
  {

    m = (l + r) / 2;

    mergeSort(a, b, l, m);
    mergeSort(a, b, (m + 1), r);
    merge(a, b, l, m, r);
  }
}


int main(int argc, char *argv[])
{
  srand(time(NULL));
  // srand(0);

  int world_rank, world_size;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  /*---------- Take Input ----------*/
  int n;
  int *arr;
  if (world_rank == 0)
  {
    printf("Input array size(n): ");
    scanf("%d", &n);
    arr = (int *)calloc(n, sizeof(int));
    printf("Array: ");
    for (int i = 0; i < n; i++)
    {
      arr[i] = randInt();
      printf("%d ", arr[i]);
    }
    printf("\n");
  }

  /*---------- Broadcast ----------*/
  MPI_Bcast(&n, sizeof(int), MPI_INT, 0, MPI_COMM_WORLD);

  /*---------- Scatter ----------*/
  if (n % world_size != 0)
  {
    printf("Use correct number of processors\n");
    exit(0);
  }

  int r = n / world_size;
  int *subarray = (int *)calloc(r, sizeof(int));
  int *sorted = (int *)calloc(r, sizeof(int));

  int sizeSorted = r;
  
  MPI_Scatter(arr, r, MPI_INT, subarray, r, MPI_INT, 0, MPI_COMM_WORLD);

  printf("Processor %d: subarray: ", world_rank);
  for (int i = 0; i < r; i++)
  {
    printf("%d ", subarray[i]);
    sorted[i] = subarray[i];
  }
  printf("\n");

  mergeSort(subarray, sorted, 0, r-1);

  /*---------- tree based merging -----------*/
  /*
      n n n n n n n n
      |/  |/  |/  |/   
      n   n   n   n
      |  /    |  /
      n       n
      | _____/
      n
  */
  
  int cur_world_size = world_size;
  int np = 2;
  while(cur_world_size > 1){
    if(world_rank % (np/2) != 0) {
      break;
    }
    int r;
    int *other_sorted = (int* )malloc(sizeSorted*sizeof(int));
    if(world_rank % np == (np/2)){
      r = sizeSorted;
      for(int i=0;i<r;i++){
        other_sorted[i] = sorted[i];
      }
      MPI_Send(&r, 1, MPI_INT, world_rank-np/2, 5, MPI_COMM_WORLD);
      MPI_Send(other_sorted, r, MPI_INT, world_rank-np/2, 11, MPI_COMM_WORLD);
    } else if(world_rank/(np/2) != cur_world_size-1){ // last one excluded if cur_world_size odd
      MPI_Recv(&r, 1, MPI_INT, world_rank+np/2, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv(other_sorted, r, MPI_INT, world_rank+np/2, 11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      /*---------- merging 2 sorted arrays -----------*/
      int r1 = sizeSorted;
      sorted = (int* )realloc(sorted, (r+r1)*sizeof(int));
      sizeSorted = r+r1;
      for(int i=0;i<r;i++){
        sorted[r1+i] = other_sorted[i];
      }
      int* newSorted = (int*)malloc(sizeSorted*sizeof(int));
      merge(sorted, newSorted, 0, r1-1, r+r1-1);
      sorted = newSorted;

    }
    // MPI_Barrier(MPI_COMM_WORLD);
    np *= 2;
    cur_world_size = (cur_world_size+1) / 2;
  }

  if(world_rank == 0){
    printf("Sorted array - ");
    int r = sizeSorted;
    for(int i=0;i<r;i++){
      printf("%d ", sorted[i]);
    }
    printf("\n");
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;
}
