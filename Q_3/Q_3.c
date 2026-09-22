// Questão 3
#include <pthread.h>

// Definições de macros
#define N 5
#define M 5
#define TAM_ARRAY 10
#define POS_ARRAY_LEITURA 9
#define POS_ARRAY_ESCRITA 9
#define ESCRITOR_VALUE 10

// array compartilhado e variáveis de controle
int arr[TAM_ARRAY];
int ids_leitor[N], ids_escritor[M];
int leitores_ativos = 0, escritor_ativo = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_leitores = PTHREAD_COND_INITIALIZER, cond_escritores = PTHREAD_COND_INITIALIZER;

void* Ler(void* threadid){
    int leitor;
    while(1){
        // Bloqueia a seção crítica para verificar se há escritores ativos
        pthread_mutex_lock(&mutex);

        while(escritor_ativo){
            pthread_cond_wait(&cond_leitores, &mutex);
        }
        leitores_ativos++;

        // Libera a seção crítica para permitir que outros leitores acessem
        pthread_mutex_unlock(&mutex);

        // Leitura do array compartilhado
        leitor = arr[POS_ARRAY_LEITURA];
        
        // Bloqueia a seção crítica para atualizar o contador de leitores ativos
        pthread_mutex_lock(&mutex);

        leitores_ativos--;

        // Se não houver mais leitores ativos, sinaliza para os escritores que podem prosseguir
        if(leitores_ativos == 0){
            pthread_cond_signal(&cond_escritores);
        }

        // Libera a seção crítica
        pthread_mutex_unlock(&mutex);
    }
        
    pthread_exit(NULL);
}

void* Escrever(void* threadid){
    // Valor a ser escrito no array compartilhado
    int escritor = ESCRITOR_VALUE;
    while(1){

        // Bloqueia a seção crítica para verificar se há leitores ou escritores ativos
        pthread_mutex_lock(&mutex);
        
        // Espera até que não haja leitores ou escritores ativos
        while(leitores_ativos > 0 || escritor_ativo){
            pthread_cond_wait(&cond_escritores, &mutex);
        }

        // Indica que há um escritor ativo
        escritor_ativo = 1;

        // Escrita no array compartilhado
        arr[POS_ARRAY_ESCRITA] = escritor;

        // Indica que o escritor terminou e libera a seção crítica
        escritor_ativo=0;
        
        // Sinaliza para os leitores e escritores que podem prosseguir
        pthread_cond_broadcast(&cond_leitores);
        pthread_cond_signal(&cond_escritores);

        // Libera a seção crítica
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

int main(){
    // Inicializa N threads leitores e M threads escritores
    pthread_t threads_leitor[N];
    pthread_t threads_escritor[M];

    // Criação das threads escritoras
    for(int i = 0; i < M; i++){
        ids_escritor[i] = i + 1;
        pthread_create(&threads_escritor[i], NULL, Escrever, &(ids_escritor[i]));
    }

    // Criação das threads leitoras
    for(int i = 0; i < N; i++){
        ids_leitor[i] = i + 1; // Atribui um ID único para cada thread leitora diferente de 0
        pthread_create(&threads_leitor[i], NULL, Ler, &(ids_leitor[i]));
    }

    pthread_exit(NULL);
}