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
#include <iostream>
#include <cstdio>
#include <mpi.h> // API MPI
#include <gsl/gsl_vector.h> //gsl

using namespace std;

int world_size; // número total de processos
int world_rank; // ID (rank) do processo
int my_n = 3;   // O tamanho dos vetores  emissores

gsl_vector *my_v; // Os vetores emissores
gsl_vector *v;    // O vetor v no receptor
gsl_vector *my_soma;
gsl_vector *soma;
size_t n; // O tamanho do vetor v no receptor
double num;
double SomaTotal;
/*
 * Retorna um número aleatorio inteiro entre minimo e o maximo(inclusive os extremos)
 */
int numeroAleatorio(int minimo, int maximo) {
    return minimo + rand() % (maximo - minimo + 1);
}

int main() {

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
    my_soma = gsl_vector_alloc(1); //cada processo tem o seu my_soma
    printf("Vetor my_v(%d) = (", world_rank);
    srand((unsigned) (world_rank + time(0))); //para gerar números aleatórios reais.
    gsl_vector_set(my_soma,0,0.0);
    double SomaParcial[1]={0.0};
    for (int i = 0; i < my_n; i++) {
        num = (double) numeroAleatorio(0, 10) +
              (double) numeroAleatorio(0, 0) / 100;
        gsl_vector_set(my_v, i, num);
        gsl_vector_set(my_soma,0,gsl_vector_get(my_soma,0)+num);
        SomaParcial[0] += num;
        printf("%6.1f", gsl_vector_get(my_v, i));
    }
    printf(")\n");
    printf("A soma do vetor %d eh %6.1f\n",world_rank,gsl_vector_get(my_soma,0));

    n = world_size * my_n; // total de números(espaços alocados) em todos os processos
    v = gsl_vector_alloc(0);
    soma = gsl_vector_alloc(0);
    if (world_rank == 0) {
        v = gsl_vector_alloc(n); // aloca o vetor v no processo 0 com n espaços
        soma = gsl_vector_alloc(world_size);
    }

    /*
     * O vetor v do processo 0 recebe "my_n" números dos vetores my_v de cada processo emissor
     * inclusive de si próprio e imprime o vetor v final.
     */
    MPI_Gather(my_v->data, my_n, MPI_DOUBLE, v->data, my_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if (world_rank == 0) {
        printf("v = ");
        for (size_t i = 0; i < n; i++)
            printf("%4.1f ", gsl_vector_get(v, i));
        printf("\n");
    }

    /*
     * O vetor v do processo 0 recebe 5 números dos vetores my_v de cada processo emissor
     * inclusive de si próprio e imprime o vetor v final.
     */
    MPI_Gather(my_soma->data, 1, MPI_DOUBLE, soma->data, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if (world_rank == 0) {
        SomaTotal=0.0;
        for (int i = 0; i < world_size; i++)
            SomaTotal += gsl_vector_get(soma, i);
        printf("A soma total eh %7.1f\n",SomaTotal);
    }

    /*
     * O vetor v do processo 0 recebe 5 números dos vetores my_v de cada processo emissor
     * inclusive de si próprio e imprime o vetor v final.
     */

    double RecebeSomaParcial[world_size]; // Pequeno erro, todos os processos criam RecebeSomaParcial
    MPI_Gather(SomaParcial, 1, MPI_DOUBLE, RecebeSomaParcial, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if (world_rank == 0) {
        SomaTotal=0.0;
        for (int i = 0; i < world_size; i++)
            SomaTotal += RecebeSomaParcial[i];
        printf("A soma total eh %7.1f e a media aritmetica eh %7.1f\n",SomaTotal,SomaTotal/n);
    }


    // Finaliza o MPI
    MPI_Finalize();

    return 0;
}
/*
 * No Linux ou no WSL Rode no terminal
 * $ mpic++ 334gather.cpp -lgsl -lgslcblas -o 334gather
 * $ mpirun -np 6 -oversubscribe 334gather
 * Processo 0 soma = 15.000000
 *
 * No Windows
 * $ mpic++ E331Media.cpp -o E331Media.exe -lgsl -lgslcblas
 * $ mpiexec -n 5 E331Media.exe
 * */
