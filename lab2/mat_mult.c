#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N_t 3

int main(int argc, char *argv[])
{
  int world_rank, world_size, i = 0, j = 0, k = 0;
  int column = N_t;
  int row = N_t;
  int count = N_t * N_t;

  char ch;
  double *A, *B, *C, a = 0, b = 0, c = 0, n;
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  if (world_rank == 0)
  {
    if (count != world_size)
    {
      printf("No of processes must be equal to %d\n", count);
      exit(0);
    }
    A = (double *)malloc(sizeof(double) * row * column); // matrix in row major form
    B = (double *)malloc(sizeof(double) * row * column);
    k = 0;

    printf("A matrix:\n");
    for (i = 0; i < row; i++)
    {
      for (j = 0; j < column; j++)
      {
        A[k] = (i + j);
        printf("%6.2f\t", A[k]);
        k++;
      }
      printf("\n");
    }

    k = 0;
    printf("\nB matrix:\n");
    for (i = 0; i < row; i++)
    {
      for (j = 0; j < column; j++)
      {
        B[k] = i + j;
        printf("%6.2f\t", B[k]);
        k++;
      }
      printf("\n");
    }
  }

  MPI_Bcast(&row, 1, MPI_INT, 0, MPI_COMM_WORLD);
  int periods[] = {1, 1}; //both vertical and horizontal movement;
  int dims[] = {row, row};
  int coords[2];                             /* 2 Dimension topology so 2 coordinates */
  int right = 0, left = 0, down = 0, up = 0; // neighbor ranks
  MPI_Comm cart_comm;
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &cart_comm);
  MPI_Scatter(A, 1, MPI_DOUBLE, &a, 1, MPI_DOUBLE, 0, cart_comm);
  MPI_Scatter(B, 1, MPI_DOUBLE, &b, 1, MPI_DOUBLE, 0, cart_comm);
  MPI_Comm_rank(cart_comm, &world_rank);
  MPI_Cart_coords(cart_comm, world_rank, 2, coords);
  MPI_Cart_shift(cart_comm, 1, coords[0], &left, &right);
  MPI_Cart_shift(cart_comm, 0, coords[1], &up, &down);
  MPI_Sendrecv_replace(&a, 1, MPI_DOUBLE, left, 11, right, 11, cart_comm, MPI_STATUS_IGNORE);
  MPI_Sendrecv_replace(&b, 1, MPI_DOUBLE, up, 11, down, 11, cart_comm, MPI_STATUS_IGNORE);
  c = c + a * b;
  for (i = 1; i < row; i++)
  {
    MPI_Cart_shift(cart_comm, 1, 1, &left, &right);
    MPI_Cart_shift(cart_comm, 0, 1, &up, &down);
    MPI_Sendrecv_replace(&a, 1, MPI_DOUBLE, left, 11, right, 11, cart_comm, MPI_STATUS_IGNORE);
    MPI_Sendrecv_replace(&b, 1, MPI_DOUBLE, up, 11, down, 11, cart_comm, MPI_STATUS_IGNORE);
    c = c + a * b;
  }
  C = (double *)calloc(sizeof(double), row * row);
  MPI_Gather(&c, 1, MPI_DOUBLE, C, 1, MPI_DOUBLE, 0, cart_comm);
  if (world_rank == 0)
  {
    k = 0;
    printf("\nA * B:\n");
    for (i = 0; i < row; i++)
    {
      for (j = 0; j < row; j++)
      {
        printf("%6.2f\t", C[k]);
        k++;
      }
      printf("\n");
    }
  }
  MPI_Finalize();
  return 0;
}