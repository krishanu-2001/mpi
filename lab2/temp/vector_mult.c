#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

int main(){
  MPI_Init(NULL, NULL);
  int world_rank, world_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // input 100k size vector
  int n = 100000;
  int *A = (int*)malloc(n*sizeof(int));
  int *B = (int*)malloc(n*sizeof(int));

  // fill data in arr
  for(int i=0;i<n;i++){
    A[i] = (i % 100);
    B[i] = (i % 50);
  }

  // printing
  if(world_rank == 0){
    for(int i=0;i<n;i++){
      printf("%d ", A[i]);
    }
    printf("\n");

    for(int i=0;i<n;i++){
      printf("%d ", B[i]);
    }
    printf("\n");
  }

  int size = n / world_size;

  int *subarray_A = (int*)malloc(size*(sizeof(int)));
  int *subarray_B = (int*)malloc(size*(sizeof(int)));

  MPI_Scatter(A, size, MPI_INT, subarray_A, size, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Scatter(B, size, MPI_INT, subarray_B, size, MPI_INT, 0, MPI_COMM_WORLD);

  // dot product here
  int *resultBuf = NULL;
  if(world_rank == 0){
    int *resultBuf = (int*)malloc(world_size*(sizeof(int))); 
  }

  int sub_sum = 0;
  for(int i=0;i<size;i++){
    sub_sum += subarray_A[i]*subarray_B[i];
  }

  printf("Processor %d gives sum %d\n", world_rank, sub_sum);
  MPI_Barrier(MPI_COMM_WORLD);

  int ans = 0;
  MPI_Reduce(&sub_sum, &ans, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if(world_rank == 0){
    // gather here
    printf("Vector multiplication gives %d\n", ans);
  }

  MPI_Finalize();
  return 0;
}