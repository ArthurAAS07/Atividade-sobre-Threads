// Questão 3
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define N 5
#define M 3
#define TAM_ARRAY 10

int arr[TAM_ARRAY];
bool trava = false;
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

void* Ler(void* threadid){
    if(!trava) {
        for(int i = 0; i < TAM_ARRAY; i++){
            printf("%d ", arr[i]);
        }
        printf("\n");
    } else {
        printf("Aguardando a escrita...\n");
    }
    return NULL;
}

void* Escrever(void* threadid){
    trava = true;
    pthread_mutex_lock(&mymutex);
    for(int i = 0; i < TAM_ARRAY; i++){
        arr[i] = rand() % 100;
    }
    pthread_mutex_unlock(&mymutex);
    trava = false;
    return NULL;
}

int main(int argc, char *argv[]){
    pthread_t threads_leitor[N];
    pthread_t threads_escritor[M];

    for(int i = 0; i < N; i++){
        int rc = pthread_create(&threads_leitor[i], NULL, Ler, NULL);
        if(rc){
            printf("ERRO; código de retorno é %d\n", rc);
            exit(1);
        }
    }
    for(int i = 0; i < M; i++){
        int rc = pthread_create(&threads_escritor[i], NULL, Escrever, NULL);
        if(rc){
            printf("ERRO; código de retorno é %d\n", rc);
            exit(1);
        }
    }
    for(int i = 0; i < N; i++){
        pthread_join(threads_leitor[i], NULL);
    }
    for(int i = 0; i < M; i++){
        pthread_join(threads_escritor[i], NULL);
    }
    return 0;
}