#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N 3          // Constante: Quantidade de núcleos (máximo de threads simultâneas)
#define MAX_FILA 100 // Tamanho da lista de prontos

// Variáveis Globais
int lista_pronto[MAX_FILA];
int inicio = 0;
int fim = 0;
int total_itens_fila = 0;

pthread_mutex_t mutex_fila;
pthread_cond_t cond_fila_nao_vazia; // Controla quando o escalonador dorme/acorda por falta de tarefas

int nucleos_livres = N;

pthread_mutex_t mutex_nucleos;
pthread_cond_t cond_nucleos_livres; // Controla o limite de N threads em execução simultânea

void* executar_tarefa(void* arg) {
    int id_tarefa = *(int*)arg;
    free(arg); // liberar a memória alocada pelo escalonador

    printf(" Nucleo ocupado thread da tarefa %d iniciou a execucao.\n", id_tarefa);
    
    // simula o tempo de execução 
    sleep(3); 
    
    printf("  Nucleo livre thread da tarefa %d concluiu.\n", id_tarefa);
    
    pthread_mutex_lock(&mutex_nucleos);
    nucleos_livres++;
    // Acorda o escalonador
    pthread_cond_signal(&cond_nucleos_livres); 
    pthread_mutex_unlock(&mutex_nucleos);
    
    pthread_exit(NULL);
}

//thread escalonador escondida
void* escalonador(void* arg) {
    printf("Escalonador iniciado. Gerenciando %d nucleos simultaneos.\n", N);

    while (1) {
         //Dorme se a lista_pronto estiver vazia
        pthread_mutex_lock(&mutex_fila);
        while (total_itens_fila == 0) {
            pthread_cond_wait(&cond_fila_nao_vazia, &mutex_fila);
        }
        
        // Pega a thread da lista_pronto 
        int id_tarefa = lista_pronto[inicio];
        inicio = (inicio + 1) % MAX_FILA;
        total_itens_fila--;
        pthread_mutex_unlock(&mutex_fila);

        // Se a fila tem item, aguarda liberar um núcleo 
        pthread_mutex_lock(&mutex_nucleos);
        while (nucleos_livres == 0) {
            pthread_cond_wait(&cond_nucleos_livres, &mutex_nucleos);
        }
        nucleos_livres--;
        pthread_mutex_unlock(&mutex_nucleos);

        printf("Escalonador acordou! Despachando tarefa %d para um nucleo.\n", id_tarefa);

        // Cria a thread e coloca pra executar no núcleo
        pthread_t thread_trabalhadora;
        int* arg_id = malloc(sizeof(int));
        *arg_id = id_tarefa;


        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        
        pthread_create(&thread_trabalhadora, &attr, executar_tarefa, arg_id);
        pthread_attr_destroy(&attr);
    }
    return NULL;
}

//Funcao auxiliar
void adicionar_na_lista_pronto(int id_tarefa) {
    pthread_mutex_lock(&mutex_fila);
    lista_pronto[fim] = id_tarefa;
    fim = (fim + 1) % MAX_FILA;
    total_itens_fila++;
    pthread_mutex_unlock(&mutex_fila);

    printf("Sistema nova tarefa %d adicionada na lista_pronto.\n", id_tarefa);
    
    // Acorda o escalonador
    pthread_cond_signal(&cond_fila_nao_vazia);
}

//main 
int main() {
   //iniciar
    pthread_mutex_init(&mutex_fila, NULL);
    pthread_cond_init(&cond_fila_nao_vazia, NULL);
    
    pthread_mutex_init(&mutex_nucleos, NULL);
    pthread_cond_init(&cond_nucleos_livres, NULL);

    //cirar a thread escalonador
    pthread_t thread_escalonador;
    pthread_create(&thread_escalonador, NULL, escalonador, NULL);

    // simulando as tarefas chegando simultaneamente
    for (int i = 1; i <= 6; i++) {
        adicionar_na_lista_pronto(i);
        sleep(1); // Da 1 segundo de diferença entre a chegada de cada uma
    }

    // Espera tempo suficiente para todas as 6 tarefas terminarem 
    sleep(10);
    printf("Sistema desligando...\n");

    return 0;
}