
/**
 *  An MPI multiprocessing program that counts the number of primes
 *  between 2 and some upper limit.  The upper limit can be given as
 *  a command-line parameter.  If no limit is specified, or if the 
 *  specified value is illegal, then a 10000000 is used.
 *
 *  In this version of the program, processes communicate using
 *  simple MPI_Send and MPI_Recv commands.
 */

#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

int DEFAULT_RANGE = 10000000;  // Default upper limit.

int process_count;  // This variable, available throughtout the program,
                    // is the number of processes in the virtual machine.
                    // Each process has a numerical "rank" in the
                    // range 0 to process_count - 1.  Every process
                    // runs its own copy of this program.

int my_rank;  // This variable, available throughtout the program, is the
              // rank of the process that is running this copy of the
              // program.
              
/**
 *  Try to convert the first command line argument to an
 *  integer that is at least equal to the 2 times the 
 *  number of processes.  If it works, that integer is returned.
 *  If not, then DEFAULT_RANGE is returned.
 */
int get_range(int argc, char **argv) {
   if (argc < 2 || !argv[1])
      return DEFAULT_RANGE;
   int range = atoi(argv[1]);
   if (range < 2*process_count)
      return DEFAULT_RANGE;
   return range;
}

/**
 * Determine whether an integer is prime.  This assumes that
 * the parameter n is greater than or equal to 2.
 */
int is_prime(int n) {
   int top,d;
   top = (int)(sqrt((double)n)+0.0001);
   for (d = 2; d <= top; d++)
      if ( n % d == 0 )
         return 0;
   return 1;
}

/**
 * Count the number of primes in the block of values assigned to this
 * process.  Processes other than process 0 send their counts to
 * process 0, which computes the total of all the counts and outputs
 * the result.
 */
void do_work(int argc, char **argv) {

   int range = get_range(argc,argv);  // Counting primes between 2 and range.
   double delta = (double)range / process_count;
   int min;  // Start of block of values for this process.
   int max;  // End of block of values for this process.
   min = (my_rank == 0)? 2 : (int)(my_rank*delta) + 1;
   max = (my_rank == process_count-1)? range : (int)((my_rank+1)*delta);

   int count = 0;  // Number of primes counted.
   
   int i;

   if ( my_rank != 0) {
          // Send count from this process to process 0.
      for (i = min; i <= max; i++)    // Count the primes.
         if (is_prime(i))
            count++;
      int data_to_send[3];
      data_to_send[0] = count;
      data_to_send[1] = min;
      data_to_send[2] = max;
      MPI_Send( data_to_send, 3, MPI_INT, 0, 0, MPI_COMM_WORLD);
   }
   else {
         // Process 0 receives the counts from all processes and adds
         // them to its own count, giving the total number of primes
         // in all blocks.
      double start = MPI_Wtime();
      for (i = min; i <= max; i++)    // Count the primes.
         if (is_prime(i))
            count++;
      printf("Process 0 reports %d primes between %d and %d.\n", count, min, max);
      int ct;
      for (ct = 1; ct < process_count; ct++) {
         int data_received[3];
         MPI_Status status;
         MPI_Recv( data_received, 3, MPI_INT, MPI_ANY_SOURCE,
                        MPI_ANY_TAG, MPI_COMM_WORLD, &status );
         count += data_received[0];
         printf("Process %d reports %d primes between %d and %d.\n", 
              status.MPI_SOURCE, data_received[0], data_received[1], data_received[2]);
      }
      printf("\nTotal number of primes between 2 and %d:  %d.\n", range, count);
      double elapsed = MPI_Wtime() - start;
      printf("\nElapsed time:  %1.3f seconds.\n", elapsed);
   }
}


int main(int argc, char **argv) {

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
   MPI_Comm_size(MPI_COMM_WORLD, &process_count);
   
   do_work(argc, argv);
   
   MPI_Finalize();

}
