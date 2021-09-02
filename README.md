# mpi
Parallel Programming Install Mpich2 on Linux or use WSL

### Steps to Compile
mpicc file.c -o file

### Steps to Execute
mpirun -n <# of processors> ./file

      eg. $mpirun -n 2 ./sum_array 4 1 2 3 4 
