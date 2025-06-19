#include <stdio.h>
#include <stdlib.h> 
#include <time.h> 
#include <string.h>
#include <stdbool.h>

#define MAX_SENSORES 2000

const int qtd_sensores;
typedef enum { CONJ_Z, CONJ_Q, TEXTO, BINARIO } TipoDado;

typedef struct {
    char ID_SENSOR[50];
    TipoDado tipo;
} Sensor;

typedef struct {
    int dia;
    int mes;
    int ano;
    int hora;
    int min;
} Data;

time_t converterData(Data data){
    struct tm t;
    t.tm_year = data.ano -1900;
    t.tm_mon = data.mes - 1;
    t.tm_mday = data.dia;
    t.tm_hour = data.hora;
    t.tm_min = data.min;
    return mktime(&t);
}

time_t gerarTimestampAleatorio(time_t inicio, time_t fim) {
    return inicio + rand() % (fim - inicio + 1);
}


void gerarValor(TipoDado tipo, char *saida) {
    const char *opcoes[] = {"ON", "OFF", "STAND BY", "OK"};
    switch (tipo) {
        case CONJ_Z:
            sprintf(saida, "%d", rand() % 1000);
            break;
        case CONJ_Q:
            sprintf(saida, "%.2f", (float)(rand() % 10000) / 100);
            break;
        case BINARIO:
            sprintf(saida, "%d", rand() % 2);
            break;
        case TEXTO:
            sprintf(saida, "%s", opcoes[rand() % qtd_sensores]);
            break;
    }
}

TipoDado escolhertipo(){
    int opcao;
    printf("Escolha o tipo de dado:\n");
    printf("1 - Valor com tipos de números inteiros(CONJ_Z).\n");
    printf("2 - Valor com tipos de números flutuantes(CONJ_Q).\n");
    printf("3 - Valor com tipos de texto(TEXTO).\n");
    printf("4 - Valor com tipos booleanos(BINARIO)");
    scanf("%d", &opcao);

    switch (opcao){
        case 1: return CONJ_Z;
        case 2: return CONJ_Q;
        case 3: return TEXTO;
        case 4: return BINARIO;
        default:
            printf("Tipo inválido. Tente novamente.");

    }

}


int main(){

    int qtd_leitura_sensores = 2000;
    Sensor sensores[MAX_SENSORES];
    srand(time(NULL));

    printf("\n--------- Programa 03 ---------\n");
    printf("Descrição: Programa responsável por gerar amostras de sensores.\n\n");

    Data inicioData, fimData;

    printf("Digite o início do intervalo (DD MM AAAA HH MM): ");
    scanf("%d %d %d %d %d", &inicioData.dia, &inicioData.mes, &inicioData.ano, &inicioData.hora, &inicioData.min);
    printf("Digite o fim do intervalo (DD MM AAAA HH MM): ");
    scanf("%d %d %d %d %d", &fimData.dia, &fimData.mes, &fimData.ano, &fimData.hora, &fimData.min);

    time_t inicio = converterData(inicioData);
    time_t fim = converterData(fimData);

    if (fim <= inicio) {
        printf("Erro: a data final deve ser maior que a inicial.\n");
        return 1;
    }

    printf("Quantos sensores você deseja gerar amostras?");
    scanf("%d", &qtd_sensores);
    getchar();

    for(int i = 0; i < qtd_sensores; i++){
        printf("\nSensor Nº%d:\n", i + 1);
        printf("Digite o ID do sensor(nome): ");
        scanf("%s", sensores[i].ID_SENSOR);
        sensores[i].tipo = escolhertipo();
    }

    FILE *arquivo = fopen("dados_gerados.txt", "w");
    if(arquivo == NULL){
    perror("Erro ao criar o arquivo.");
    return 1;
    }

    char valor[100];
    for(int i = 0; i < qtd_sensores; i++)
    {
        for(int j = 0; j < qtd_leitura_sensores; j++)
        {
            time_t timestamp = gerarTimestampAleatorio(inicio, fim);
            gerarValor(sensores[i].tipo, valor);
            fprintf(arquivo, "%lld %s %s\n", (long long)timestamp, sensores[i].ID_SENSOR[qtd_sensores], valor);
        }
    }
    fclose(arquivo);
    printf("\nArquivo 'dados_gerados.txt' gerado com sucesso.\n");
    return 0;
}