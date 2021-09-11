#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

int sumup(int *a, int n){
  int sum = 0;
  for(int i=0;i<n;i++){
    sum += a[i];
  }
  return sum;
}

int main(int argc, char** argv){
  int world_rank, world_size;
  MPI_Init(&argc, &argv);
  int n = atoi(argv[1]);
  int *arr = (int*)malloc(n*sizeof(int));
  printf("The original array\n");
  for(int i=0;i<n;i++){
    arr[i] = atoi(argv[i+2]);
    printf("%d ", arr[i]);
  }
  printf("\n");

  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  // split items to processors
  int size = n / world_size;

  /********** Send each subarray to each process **********/
  int *sub_array = malloc(size * sizeof(int));
  MPI_Scatter(arr, size, MPI_INT, sub_array, size, MPI_INT, 0, MPI_COMM_WORLD);

  int sub_sum = sumup(sub_array, size);

  printf("Processor %d gives sum %d\n", world_rank, sub_sum);

  int *sub_sums = NULL; // buffer to store result of all
  if(world_rank == 0){
    sub_sums = malloc(sizeof(int)*world_size);
  }

  MPI_Gather(&sub_sum, 1, MPI_INT, sub_sums, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // Compute sum of all subsums 
  if (world_rank == 0){
    int sum = sumup(sub_sums, n);
    printf("Total sum = %d\n", sum);
  }

  MPI_Finalize();
  return 0;
}
