// Questão 3
#include <pthread.h>

#define N 5
#define M 3
#define TAM_ARRAY 10

int arr[TAM_ARRAY];
int ids_leitor[N], ids_escritor[M];
int leitores_ativos = 0, escritores_ativos = 0, escritores_esperando = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_leitores = PTHREAD_COND_INITIALIZER, cond_escritores = PTHREAD_COND_INITIALIZER;

void* Ler(void* threadid){
    while(1){
        pthread_mutex_lock(&mutex);
        while(escritores_ativos > 0 || escritores_esperando > 0){
            pthread_cond_wait(&cond_leitores, &mutex);
        }
        leitores_ativos++;
        
        pthread_mutex_unlock(&mutex);

        int i = rand() % TAM_ARRAY;
        printf("Thread L%d leu arr[%d] = %d\n", *(int*)threadid, i, arr[i]);
        printf("=> leitores=%d | escritores=%d\n", leitores_ativos, escritores_ativos);

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
    while(1){
        pthread_mutex_lock(&mutex);
        escritores_esperando++;
        while(leitores_ativos > 0 || escritores_ativos > 0){
            pthread_cond_wait(&cond_escritores, &mutex);
        }
        escritores_esperando--;
        escritores_ativos++;
        pthread_mutex_unlock(&mutex);

        int i = rand() % TAM_ARRAY;
        int value = rand() % 100;
        arr[i] = value;
        printf("Thread E%d escreveu arr[%d] = %d\n", *(int*)threadid, i, arr[i]);
        printf("=> leitores=%d | escritores=%d\n", leitores_ativos, escritores_ativos);

        pthread_mutex_lock(&mutex);
        escritores_ativos--;
        pthread_cond_broadcast(&cond_leitores);
        pthread_cond_broadcast(&cond_escritores);
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

int main(){
    pthread_t threads_leitor[N];
    pthread_t threads_escritor[M];

    for(int i = 0; i < N; i++){
        ids_leitor[i] = i + 1;
        int rc = pthread_create(&threads_leitor[i], NULL, Ler, &(ids_leitor[i]));
        if(rc){
            printf("ERRO; código de retorno é %d\n", rc);
            exit(1);
        }
    }
    for(int i = 0; i < M; i++){
        ids_escritor[i] = i + 1;
        int rc = pthread_create(&threads_escritor[i], NULL, Escrever, &(ids_escritor[i]));
        if(rc){
            printf("ERRO; código de retorno é %d\n", rc);
            exit(1);
        }
    }

    pthread_exit(NULL);
}