#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define NUM_CONSULTORIOS 7
#define TEMPO_ATENDIMENTO 5
#define TEMPO_ATT 2

pthread_mutex_t mutex_linhas[NUM_CONSULTORIOS];
pthread_mutex_t mutex_tela;

char pacientes_atuais[NUM_CONSULTORIOS][50] = {
    "Gabro", "Feldspato", "Cherte",
    "Riebeck", "Esker", "Gneiss", "Corneana"
};

const char *cores_bg[NUM_CONSULTORIOS] = {
    "\033[40m", "\033[41m", "\033[42m",
    "\033[43m", "\033[44m", "\033[45m",
    "\033[46m"
};

typedef struct {
    char arquivo[100];
} DadosThread;

// Atualiza a linha de um consultório no terminal
void atualizar_tela_linha(int consultorio, const char *nome) {

    // mutex_tela é usado para evitar que múltiplas threads escrevam no terminal ao mesmo tempo
    pthread_mutex_lock(&mutex_tela);

    printf("\033[%d;1H", consultorio);
    printf("%s\033[37;1m\033[KPaciente %-20s Consultório %d\033[0m\n", cores_bg[consultorio - 1], nome, consultorio);

    fflush(stdout);

    pthread_mutex_unlock(&mutex_tela);
}

// Função executada pelas threads
void *processar_arquivo(void *arg) {

    DadosThread *dados = (DadosThread *)arg;
    FILE *arquivo = fopen(dados->arquivo, "r");

    // Se arquivo tiver vazio cancela a thread
    if (arquivo == NULL) {
        pthread_exit(NULL);
    }

    char nome[50];
    int consultorio;

    while (fscanf(arquivo, "%s %d", nome, &consultorio) == 2) {

        // Ignora consultórios fora dos limites
        if (consultorio < 1 || consultorio > NUM_CONSULTORIOS) continue;

        int indice = consultorio - 1;

        // Bloqueia o mutex do consultório correspondente
        pthread_mutex_lock(&mutex_linhas[indice]);

        // Atualiza o paciente atual do consultório
        strcpy(pacientes_atuais[indice], nome);

        // Atualiza a linha do consultório no terminal
        atualizar_tela_linha(consultorio, nome);

        // Mantém a linha bloqueada por alguns segundos, simulando o tempo de atendimento
        sleep(TEMPO_ATENDIMENTO);

        pthread_mutex_unlock(&mutex_linhas[indice]);
    }

    fclose(arquivo);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    // Se não houver arquivos passados como argumento, encerra o programa
    if (argc < 2) {
        return 1;
    }

    int quantidade_arquivos = argc - 1;

    pthread_t threads[quantidade_arquivos];
    DadosThread dados[quantidade_arquivos];
    
    // Inicia mutex para cada consultório
    for (int i = 0; i < NUM_CONSULTORIOS; i++) {
        pthread_mutex_init(&mutex_linhas[i], NULL);
    }

    // Inicia mutex de alterar a tela
    pthread_mutex_init(&mutex_tela, NULL);

    printf("\033[2J");

    // Mostra a tela
    for (int i = 1; i <= NUM_CONSULTORIOS; i++) {
        atualizar_tela_linha(i, pacientes_atuais[i - 1]);
    }

    // Espera a tela atualizar
    sleep(TEMPO_ATT);

    // Cada arquivo externo é processado por uma thread
    for (int i = 0; i < quantidade_arquivos; i++) {

        strcpy(dados[i].arquivo, argv[i + 1]);

        pthread_create(&threads[i], NULL, processar_arquivo, &dados[i]);
    }

    // Espera todas as threads terminarem
    for (int i = 0; i < quantidade_arquivos; i++) {
        pthread_join(threads[i], NULL);
    }

    // Finaliza mutexes
    for (int i = 0; i < NUM_CONSULTORIOS; i++) {
        pthread_mutex_destroy(&mutex_linhas[i]);
    }
    pthread_mutex_destroy(&mutex_tela);

    printf("\033[%d;1H\033[0m", NUM_CONSULTORIOS + 2);

    return 0;
}
