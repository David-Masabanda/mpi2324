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

int sumar(int *vector, const int n){
    int suma = 0;
    for (int i = 0; i < n; i++){
        suma += vector[i];
    }
    return suma;
}


int main( int argc, char *argv[]){

    int rank, nprocs;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int block_size;
    int real_size;
    int padding = 0;

    std::vector<int> A;
    std::vector<int> A_local;

    if(rank==0){
        A = read_file();
        int max_items = A.size();

        real_size=max_items;
        block_size=max_items/nprocs;

        if (max_items%nprocs!=0){
            real_size=std::ceil((double)max_items/nprocs)*nprocs;
            block_size=real_size/nprocs;
            padding=real_size-max_items;
        }

        std::printf("RANK_%d, Dimension: %d, real_size: %d, block_size: %d, padding: %d\n",
                    rank, max_items, real_size, block_size, padding);
    }

    MPI_Bcast(&block_size , 1 , MPI_INT , 0 , MPI_COMM_WORLD);

    if (rank==0){
        MPI_Send(&padding , 1 , MPI_INT, nprocs-1 , 0, MPI_COMM_WORLD);
    }else if(rank==nprocs-1){
        MPI_Recv(&padding , 1 , MPI_INT, 0 , MPI_ANY_TAG , MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    //reserva de memoria para cada rank
    A_local.resize(block_size);

    //ENVIAR DATOS
    MPI_Scatter( A.data(), block_size , MPI_INT , 
                A_local.data(), block_size , MPI_INT, 0 , MPI_COMM_WORLD);

    //Procesar
    int new_size = block_size;
    if (rank==nprocs-1){
        new_size=block_size-padding;
    }

    std::printf("RANK_%d caculando promedio, block_size= %d\n", rank, new_size);

    int promedio_local=sumar(A_local.data(), new_size);


    //RECIBIR DATOS 
    int promedio_total;

    MPI_Reduce(&promedio_local, &promedio_total, 1 , MPI_INT , MPI_SUM , 0, MPI_COMM_WORLD);

   if(rank == 0){
        double promedio=(double)promedio_total/real_size;
        std::printf("Promedio %f\n", promedio);
    }

    MPI_Finalize();
    
    return 0;
}