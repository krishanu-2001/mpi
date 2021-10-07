#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include<stdbool.h>

/*
 *  Algorithm - Split [1,n] into p parts, get count of primes and sum them
 *  Time Complexity 
 *  O(n/p.sqrt(n))
 *  Space Complexity
 *  O(1)
 */

bool check_prime(int x){
  /*----- O(sqrt(n)) -----*/
  for(int i=2;i*i<=x;i++){
    if(x % i == 0) return false;
  }
  return true;
}

int main()
{

  MPI_Init(NULL, NULL);
  int world_rank, world_size;
  double time1, time2, duration;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  int n;
  if (world_rank == 0)
  {
    printf("Please enter N: ");
    scanf("%d", &n);
  }

  time1 = MPI_Wtime();

  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

  int r = n / world_size;
  int min = world_rank * r + 1; // [1, 4], [5, n]
  int max = (world_rank + 1) * r;
  if (world_rank == world_size - 1)
    max = n;

  int count = 0;
  for (int i = min; i <= max; i++)
  {
    if(i < 2) continue;
    if(check_prime(i)) count += 1;
  }
  printf("Process %d - primes in [%d, %d] is %d\n", world_rank, min, max, count);
  int total_count = 0;
  MPI_Reduce(&count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  if (world_rank == 0)
  {
    printf("\nNo. of primes between [1, N] is %d", total_count);
    
    time2 = MPI_Wtime();
    duration = time2 - time1;
    printf("\n");
    printf("Time taken by program(us) = %0.3f\n", duration*1e6);
  }
  MPI_Finalize();
  return 0;
}