#include <iostream>
#include <mpi.h>
#include <vector>
#include <cmath>

#define MAX_ITEMS 25

int sumar(int* tmp,int n){
    int suma=0;
    for(int i=0;i<n;i++){
        suma +=tmp[i];
    }
    return suma;
}

//Comunicacion Colectiva
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, nprocs;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    //-------------------------------------------
    int block_size;
    int real_size;
    int padding = 0;

    int suma_parcial;
    int suma_total=0;

    if(MAX_ITEMS%nprocs != 0){
        real_size = std::ceil((double)MAX_ITEMS/nprocs) * nprocs;
        block_size = real_size/nprocs;
        padding= real_size - MAX_ITEMS;
    }


    if(rank==0){

        //Inicializar
        std::vector<int> data;
        data.resize(real_size);

        //Imprimir informacion
        std::printf("Dimension: %d, real_size: %d, block_size: %d, padding: %d\n",
                    MAX_ITEMS,real_size,block_size,padding);

        for(int i=0;i<MAX_ITEMS+1;i++){
            data[i]=i;
        }

    //El scatter es para separar a todos(incluso el 0, para evitar esto se usa in place)
    MPI_Scatter( data.data() , block_size , MPI_INT , 
                MPI_IN_PLACE , 0 , MPI_INT , 0 , MPI_COMM_WORLD);

    
    //Procesar datos 
    suma_parcial = sumar(data.data(),block_size);
    std::printf("RANK_%d: suma parcial= %d\n",rank,suma_parcial);

    }else{

        std::vector<int> data_local(block_size);

        //Recibir datos
        MPI_Scatter(nullptr, 0, MPI_INT,
                    data_local.data(), block_size, MPI_INT,
                    0, MPI_COMM_WORLD);


        int block_size_tmp = block_size;
        if(rank==nprocs-1){
            block_size_tmp=block_size-padding;
        }

        //Procesar datos
        suma_parcial = sumar(data_local.data(), block_size_tmp);
        std::printf("RANK_%d: suma parcial = %d\n", rank, suma_parcial);


    }

            //Enviar la suma parcial al rank 0
        MPI_Reduce(&suma_parcial,&suma_total, 1, MPI_INT,
               MPI_SUM, 0, MPI_COMM_WORLD);

    //Imprimir el resultado;
    if(rank==0){
        std::printf("Suma Total = %d\n",suma_total);
    }

    MPI_Finalize();
    return 0;
}