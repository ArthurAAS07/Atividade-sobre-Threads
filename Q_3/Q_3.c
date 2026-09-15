// Questão 3
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define N 5
#define M 3
#define TAM_ARRAY 10

typedef int semaphore;

int arr[TAM_ARRAY];
semaphore reader_sem = 1, writer_sem = 1;
int reader_count = 0;

void* Ler(void* threadid){
    while(true){
        down(&reader_sem);
        reader_count++;
        if(reader_count == 1){down(&writer_sem);}
        up(&reader_sem);
        int i = rand() % TAM_ARRAY;
        printf("Thread L%ld leu arr[%d] = %d\n", threadid, i, arr[i]);
        down(&reader_sem);
        reader_count--;
        if(reader_count == 0){up(&writer_sem);}
        up(&reader_sem);
    }
        
    pthread_exit(NULL);
}

void* Escrever(void* threadid){
    while(true){
        down(&writer_sem);
        int i = rand() % TAM_ARRAY;
        int value = rand() % 100;
        arr[i] = value;
        printf("Thread E%ld escreveu arr[%d] = %d\n", threadid, i, arr[i]);
        up(&writer_sem);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    pthread_t threads_leitor[N];
    pthread_t threads_escritor[M];

    for(int i = 0; i < N; i++){
        int rc = pthread_create(&threads_leitor[i], NULL, Ler, (void*)(i+1));
        if(rc){
            printf("ERRO; código de retorno é %d\n", rc);
            exit(1);
        }
    }
    for(int i = 0; i < M; i++){
        int rc = pthread_create(&threads_escritor[i], NULL, Escrever, (void*)(i+1));
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
    
    pthread_exit(NULL);
}