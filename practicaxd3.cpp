#include <iostream>
#include <mpi.h>
#include <vector>
#include <chrono>
#include <functional>
#include <fstream>
#include <math.h>

std::vector<int> read_file() {
    std::fstream fs("./datos.txt", std::ios::in);
    std::string line;
    std::vector<int> ret;
    while (std::getline(fs, line)) {
        ret.push_back(std::stoi(line));
    }
    fs.close();
    return ret;
}
int max(int *vector, const int n)
{
    int max = 0;
    for (int i = 0; i < n; i++)
    {
        if (vector[i] > max)
            max = vector[i];
    }
    return max;
}

int min(int *vector, const int n)
{
    int min = 0;
    for (int i = 0; i < n; i++)
    {
        if (vector[i] < min)
            min = vector[i];
    }
    return min;
}

int main( int argc, char *argv[]){

    int rank, nprocs;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int block_size;
    int real_size;
    int padding=0;

    std::vector<int> A;
    std::vector<int> A_local;

    if(rank==0){
        A=read_file();
        int max_items=A.size();

        real_size=max_items;
        block_size=real_size/nprocs;

        if (max_items%nprocs!=0){
            real_size=std::ceil((double)max_items/nprocs)*nprocs;
            block_size=real_size/nprocs;
            padding=real_size-max_items;
        }
    }

    MPI_Bcast( &block_size , 1 , MPI_INT , 0, MPI_COMM_WORLD);

    if (rank==0){
        MPI_Send( &padding , 1 , MPI_INT , nprocs-1 , 0, MPI_COMM_WORLD);
    }else if (rank==nprocs-1){
        MPI_Recv( &padding , 1 , MPI_INT , 0 , MPI_ANY_TAG , MPI_COMM_WORLD , MPI_STATUS_IGNORE);
    }
    
    A_local.resize(block_size);

    MPI_Scatter(A.data() , block_size , MPI_INT , 
                A_local.data() , block_size , MPI_INT , 
                0 , MPI_COMM_WORLD);
    
    int new_size=block_size;
    if(rank==nprocs-1){
        new_size=block_size-padding;
    }

    int min_local=min(A_local.data(), new_size);
    int max_local=max(A_local.data(), new_size);

    int min_general;
    int max_general;

    MPI_Reduce( &min_local , &min_general , 1 , MPI_INT , MPI_MIN , 0 , MPI_COMM_WORLD);
    MPI_Reduce( &max_local , &max_general , 1 , MPI_INT , MPI_MAX , 0 , MPI_COMM_WORLD);

   if(rank == 0){
        std::printf("Minimo %d, Maximo %d\n", min_general, max_general);
    }

    MPI_Finalize();
    
    return 0;
}