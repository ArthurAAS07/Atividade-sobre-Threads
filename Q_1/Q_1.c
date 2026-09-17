// Questão 3
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TAM_VETOR 10
#define N_THREADS 4

// Vetor que será somado
int vetor[TAM_VETOR] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; // Inicializando com alguns valores para teste

// Vetor para armazenar resultados parciais
long long int soma_parcial[N_THREADS];

// Estrutura para passar informações para as threads
typedef struct {
    int inicio;
    int fim;
    int id;
} dados_thread;

// Função executada pelas threads
void *somar(void *arg) {

    dados_thread *dados = (dados_thread *) arg;
    long long int soma = 0;
        
    // Cada thread soma somente sua parte do vetor.
    for(int i = dados->inicio; i < dados->fim; i++) {
        soma += vetor[i];
    }

    // Cada thread escreve em uma posição diferente, então não existe disputa.
    soma_parcial[dados->id] = soma;

    pthread_exit(NULL);
}

int main() {

    pthread_t threads[N_THREADS];
    dados_thread dados[N_THREADS];

    int tamanho_bloco = TAM_VETOR / N_THREADS;

    // Criação das threads
    for(int i = 0; i < N_THREADS; i++) {

        dados[i].inicio = i * tamanho_bloco;
        
        // A última thread pega possíveis elementos restantes.
        if(i == N_THREADS-1) {
            dados[i].fim = TAM_VETOR;
        } else {
            dados[i].fim = (i+1) * tamanho_bloco;
        }

        dados[i].id = i;
        pthread_create(&threads[i], NULL, somar, &dados[i]);
    }

    // Thread principal espera todas terminarem.
    for(int i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    //Soma dos resultados parciais
    long long int soma_total = 0;

    for(int i = 0; i < N_THREADS; i++) {
        soma_total += soma_parcial[i];
    }

    printf("\nSoma final: %lld\n", soma_total);

    pthread_exit(NULL);
}