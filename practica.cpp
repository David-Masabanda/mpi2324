#include <iostream>
#include <mpi.h>


int sumar(int *tmp, int n) {
    int suma = 0;
    for (int i = 0; i < n; i++) {
        suma = suma + tmp[i];
    }
    return suma;
}

int main (int argc, char** argv){
    MPI_Init(&argc, &argv);
    int rank, nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    //std::printf("Rank %d of %d procs\n",rank,nprocs);

    int datos[100];

    if (rank==0)
    {
        //Inicializar
        for (int i = 0; i < 100; i++) {
            datos[i] = i;
        }

        //ENVIAR
        for (int id_rank = 1; id_rank < nprocs; id_rank++){
            int inicio=id_rank*25;
            MPI_Send( &datos[inicio] , 25, MPI_INT, id_rank , 0 , MPI_COMM_WORLD);
        }

        //Procesar
        int suma_total [4];
        suma_total[0]=sumar(datos,25);

        //RECIBIR
        for (int id_rank = 1; id_rank < nprocs; id_rank++){
            MPI_Recv( &suma_total[id_rank] , 1 , MPI_INT , id_rank , MPI_ANY_TAG , MPI_COMM_WORLD , MPI_STATUS_IGNORE);
        }

        //Agrupar
        int total=sumar(suma_total,4);
        std::printf("SUMA TOTAL: %d\n", total);
    }
    else
    {
        //RECIBIR
        MPI_Recv(datos , 25 , MPI_INT , 0 , 0 , MPI_COMM_WORLD , MPI_STATUS_IGNORE);

        //Procesar
        int suma_rank=sumar(datos,25);
        std::printf("RANK_%d Suma parcial:%d\n", rank, suma_rank);

        //ENVIAR
        MPI_Send( &suma_rank , 1 , MPI_INT , 0 , 0 , MPI_COMM_WORLD);
    }
    

    MPI_Finalize();
    return 0;
}