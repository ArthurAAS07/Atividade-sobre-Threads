// Questão 4
#include <pthread.h>

// Definições de constantes: Quantidade de soluções, número de iterações e número de threads
#define QTD_SOL 4
#define ITERACOES 10
#define N_THREADS 4

// Matriz A do sistema Ax=b
double A[QTD_SOL][QTD_SOL] = {
    {10, -1,  2,  0},
    {-1, 11, -1,  3},
    {2, -1, 10, -1},
    {0, 3, -1, 8}
};

// Vetor b do sistema Ax=b
double b[QTD_SOL] = {
    6,
    25,
    -11,
    15
};

// Vetor das soluções
double x[QTD_SOL];

// Vetor auxiliar para armazenar x(k+1)
double novo_x[QTD_SOL];

// Barreira para sincronizar as threads
pthread_barrier_t barreira;

// Estrutura para passar dados para as threads
typedef struct {
    int inicio;
    int fim;
    int id;
} dados_thread;

void *jacobi(void *arg) {

    dados_thread *dados = (dados_thread *)arg;

    for(int k = 0; k < ITERACOES; k++) {

        // Cada thread calcula uma parte das incógnitas.
        for(int i = dados->inicio; i < dados->fim; i++){

            double soma = 0;

            for(int j = 0; j < QTD_SOL; j++) {
                if(i != j) {
                    soma += A[i][j] * x[j];
                }
            }

            novo_x[i] = (b[i] - soma) / A[i][i];
        }

        // Espera todas as threads terminarem a iteração atual.
        pthread_barrier_wait(&barreira);

        // Atualiza os valores para a próxima iteração.
        for(int i = dados->inicio; i < dados->fim; i++) {
            x[i] = novo_x[i];
        }

        // Garante que todas atualizaram x antes de continuar.
        pthread_barrier_wait(&barreira);
    }

    pthread_exit(NULL);
}

int main(){

    pthread_t threads[N_THREADS];
    dados_thread dados[N_THREADS];

    // Valor inicial x(0)=1
    for(int i = 0; i < QTD_SOL; i++){
        x[i] = 1;
    }

    //    Inicializa a barreira.
    pthread_barrier_init(&barreira, NULL, N_THREADS);

    int quantidade = QTD_SOL / N_THREADS;
    int resto = QTD_SOL % N_THREADS;
    int inicio = 0;

    for(int i = 0; i < N_THREADS; i++) {

        dados[i].inicio = inicio;
        dados[i].fim = inicio + quantidade;

        // Distribui possíveis sobras
        if(resto > 0) {
            dados[i].fim++;
            resto--;
        }

        dados[i].id = i;
        inicio = dados[i].fim;

        // Cria a thread
        pthread_create(&threads[i], NULL, jacobi, &dados[i]);
    }

    // Espera todas as threads terminarem.
    for(int i = 0; i < N_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    // Finaliza a barreira
    pthread_barrier_destroy(&barreira);

    return 0;
}