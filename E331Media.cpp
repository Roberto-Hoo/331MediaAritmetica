/* E 3.3.1.
Faça um código MPI para computar a média aritmética simples de
números randômicos em ponto flutuante.
 */

/*
 3.3.4 Recebimento coletivo de dados distribuídos
A rotina MPI_Gather, permite que um processo receba simultaneamente dados que estão distribuídos
 entre os demais processos. Sua sintaxe é a seguinte

int MPI_Gather(
  const void *sendbuf,
  int sendcount,
  MPI_Datatype sendtype,
  void *recvbuf,
  int recvcount,
  MPI_Datatype recvtype,
  int root,
  MPI_Comm comm)

Sua sintaxe é parecida com a da rotina MPI_Scatter. Veja lá!
 Aqui, root é o identificador rank do processo receptor.
*/

#include <stdio.h>
#include <mpi.h> // API MPI
#include <gsl/gsl_vector.h> //gsl

int world_size; // número total de processos
int world_rank; // ID (rank) do processo
const size_t my_n = 5; // O tamanho dos vetores  emissores
gsl_vector *my_v; // Os vetores emissores
gsl_vector *v; // O vetor v no receptor
size_t n; // O tamanho do vetor v no receptor
double num;

/*
 * Retorna um número aleatorio inteiro entre minimo e o maximo(inclusive os extremos)
 */
int numeroAleatorio(int minimo, int maximo) {
    return minimo + rand() % (maximo - minimo + 1);
}

int main(void) {


    MPI_Init(NULL, NULL); // Inicializa o MPI
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    /*
     *  Aloca em cada processo o vetor my_v com 5 espaços
     *  e os define:
     *  Processo 0: my_v=my_v(0)=(1,2,3,4,5),
     *  Processo 1: my_v=my_v(1)=(6,7,8,9,10),..., etc,...
     *
    */
    my_v = gsl_vector_alloc(my_n); //cada processo tem o seu my_v
    printf("Vetor my_v(%d) = (", world_rank);
    for (size_t i = 0; i < my_n; i++) {
        num = (double) numeroAleatorio(0, 99) +
              (double) numeroAleatorio(0, 99) / 100;
        gsl_vector_set(my_v, i, 5 * world_rank + i + 1);
        printf("%6.1f", gsl_vector_get(my_v, i));
    }
    printf(")\n");

    n = world_size * my_n; // total de números(espaços alocados) em todos os processos
    v = gsl_vector_alloc(0);
    if (world_rank == 0) {
        v = gsl_vector_alloc(n); // aloca o vetor v no processo 0 com n espaços
    }

    /*
     * O vetor v do processo 0 recebe 5 números dos vetores my_v de cada processo emissor
     * inclusive de si próprio e imprime o vetor v final.
     */
    MPI_Gather(my_v->data, my_n, MPI_DOUBLE, v->data, my_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if (world_rank == 0) {
        printf("v = ");
        for (size_t i = 0; i < n; i++)
            printf("%4.1f ", gsl_vector_get(v, i));
        printf("\n");
    }

    // Finaliza o MPI
    MPI_Finalize();

    return 0;
}
/*
 * Rode no terminal
 * $ mpic++ 334gather.cpp -lgsl -lgslcblas -o 334gather
 * $ mpirun -np 6 -oversubscribe 334gather
 * Processo 0 soma = 15.000000
 *
 * */
