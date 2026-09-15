// Questão 3
#include <pthread.h>

#define N 5
#define M 5
#define TAM_ARRAY 10
#define POS_ARRAY_LEITURA 9
#define POS_ARRAY_ESCRITA 9
#define ESCRITOR_VALUE 10

int arr[TAM_ARRAY];
int ids_leitor[N], ids_escritor[M];
int leitores_ativos = 0, escritor_ativo = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_leitores = PTHREAD_COND_INITIALIZER, cond_escritores = PTHREAD_COND_INITIALIZER;

void* Ler(void* threadid){
    int leitor;
    while(1){
        pthread_mutex_lock(&mutex);
        while(escritor_ativo){
            pthread_cond_wait(&cond_leitores, &mutex);
        }
        leitores_ativos++;
        
        pthread_mutex_unlock(&mutex);

        leitor = arr[POS_ARRAY_LEITURA];
        
        pthread_mutex_lock(&mutex);
        leitores_ativos--;
        if(leitores_ativos == 0){
            pthread_cond_signal(&cond_escritores);
        }
        pthread_mutex_unlock(&mutex);
    }
        
    pthread_exit(NULL);
}

void* Escrever(void* threadid){
    int escritor = ESCRITOR_VALUE;
    while(1){
        pthread_mutex_lock(&mutex);
        
        while(leitores_ativos > 0 || escritor_ativo){
            pthread_cond_wait(&cond_escritores, &mutex);
        }
        escritor_ativo = 1;

        arr[POS_ARRAY_ESCRITA] = escritor;

        escritor_ativo=0;
        
        pthread_cond_broadcast(&cond_leitores);
        pthread_cond_signal(&cond_escritores);

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

int main(){
    pthread_t threads_leitor[N];
    pthread_t threads_escritor[M];

    for(int i = 0; i < M; i++){
        ids_escritor[i] = i + 1;
        pthread_create(&threads_escritor[i], NULL, Escrever, &(ids_escritor[i]));
    }

    for(int i = 0; i < N; i++){
        ids_leitor[i] = i + 1;
        pthread_create(&threads_leitor[i], NULL, Ler, &(ids_leitor[i]));
    }

    pthread_exit(NULL);
}