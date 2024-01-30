//Librerias
#include <iostream>
#include <vector>
#include <mpi.h>
#include <cmath>

//Metodos adicionales (Calculo, lectura, etc...)
int xd (int *tmp, int n) {

}


int main (int argc, char** argv){

    MPI_Init(&argc, &argv);
    int rank, nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    //std::printf("Rank %d of %d procs\n",rank,nprocs);
    
    //Valores generales para todos los rank
    int xd [100];
    /* *** */

    if (rank==0)
    {
        //Inicializar
        /*     */

        //ENVIAR
        for (int id_rank = 1; id_rank < nprocs; id_rank++){
            MPI_Send( & , , MPI_, id_rank , 0 , MPI_COMM_WORLD);
        }

        //Procesar
  

        //RECIBIR
        for (int id_rank = 1; id_rank < nprocs; id_rank++){
            MPI_Recv( & ,  , MPI_ , id_rank , 0 , MPI_COMM_WORLD , MPI_STATUS_IGNORE);
        }

        //Agrupar
        /*     */
        std::printf("SUMA TOTAL: %d\n", total);
    }
    else
    {
        //RECIBIR
        MPI_Recv(  ,  , MPI_ , 0 , 0 , MPI_COMM_WORLD , MPI_STATUS_IGNORE);

        //Procesar
 

        //ENVIAR
        MPI_Send( & ,  , MPI_ , 0 , 0 , MPI_COMM_WORLD);
    }
    

    MPI_Finalize();
    return 0;
}