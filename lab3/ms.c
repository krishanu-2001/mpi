#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include<ctype.h>
#include <time.h>
#include<math.h>

void md(int *a, int *b, int lb, int m, int ub);

void mS(int *a, int *b, int lb, int ub);

int main(int argc, char *argv[])
{
  MPI_Init(&argc, &argv);
  srand(time(NULL));
  int rank;
  int npes;
  MPI_Comm_size(MPI_COMM_WORLD, &npes); MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  

  int n;
  if (rank == 0){
    printf("n: \n");
    scanf("%d", &n);    
  }


  MPI_Bcast(&n, sizeof(int), MPI_INT, 0, MPI_COMM_WORLD);

  int size1 = n / npes;
  int *subd = malloc(size1*sizeof(int));

  int size2 = size1;
  int *srtd = malloc(size1*sizeof(int));
  printf("Processor %d: \n ", rank);
  for (int i = 0; i < size1; i++)
  {
    subd[i] = rand();
    printf("%d ", subd[i]);
    srtd[i] = subd[i];
  }
  printf("\n");

  mS(subd, srtd, 0, size1-1);
  
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
        other[i] = srtd[i];
      }
      if(n % npes == 0) {
        printf("Case 1 correct input divisible by no of processors\n");
      } else if(n % npes == 1) {
        printf("Case 2 incorrect input\n");
      }
      MPI_Send(other, size1, MPI_INT, rank-some, 110, MPI_COMM_WORLD);
    } else if(rank/(some) != cur_npes-1){
      int r1 = size2;
      if(n % npes == 1) {
        printf("Case 1\n");
      } else if(n % npes == 4) {
        printf("Case 2\n");
      }
      MPI_Recv(&size1, 1, MPI_INT, rank+some, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      srtd = (int* )realloc(srtd, (size1+r1)*sizeof(int));
      size2 = size1+r1;
      MPI_Recv(other, size1, MPI_INT, rank+some, 110, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      for(int i=0;i<size1;i++){
        srtd[r1+i] = other[i];
      }
      int* newsrtd = (int*)malloc(size2*sizeof(int));
      md(srtd, newsrtd, 0, r1-1, size1+r1-1);
      srtd = newsrtd;

    }
    some *= 2;
    cur_npes = (cur_npes+1) / 2;
  }
  if(rank == 0){
  printf("\n\nSorted array: ");
    for(int i=0;i<size2;i++){
      printf("%d ", srtd[i]);
    }
    printf("\n\n\n");
  }

  MPI_Finalize();
  return 0;
}

void md(int *a, int *b, int lb, int m, int ub)
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

void mS(int *a, int *b, int lb, int ub)
{

  int m;

  if (lb < ub)
  {

    m = (lb + ub) / 2;

    mS(a, b, lb, m);
    mS(a, b, (m + 1), ub);
    md(a, b, lb, m, ub);
  }
}
