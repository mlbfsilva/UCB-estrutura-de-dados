#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SENSORES 2000

typedef enum { CONJ_Z, CONJ_Q, TEXTO, BINARIO, TIPO_INVALIDO } TipoDado;

typedef struct {
    char ID_SENSOR[50];
    TipoDado tipo;
} Sensor;

typedef struct {
    int dia, mes, ano, hora, min;
} Data;

typedef struct {
    time_t timestamp;
    char id_sensor[50];
    char valor[100];
} Amostra;

time_t converterData(Data data) {
    struct tm t = {0};
    t.tm_year = data.ano - 1900;
    t.tm_mon = data.mes - 1;
    t.tm_mday = data.dia;
    t.tm_hour = data.hora;
    t.tm_min = data.min;
    t.tm_sec = 0;
    t.tm_isdst = -1;
    return mktime(&t);
}

bool validarData(Data data) {
    return (data.ano >= 1970 && data.mes >= 1 && data.mes <= 12 &&
            data.dia >= 1 && data.dia <= 31 &&
            data.hora >= 0 && data.hora <= 23 &&
            data.min >= 0 && data.min <= 59);
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
            sprintf(saida, "%s", opcoes[rand() % 4]);
            break;
        default:
            strcpy(saida, "INVALIDO");
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
            return escolhertipo();
    }
}

int quantidadeSensores(){
    printf("Quantos sensores deseja gerar? ");
    int qtd_sensores;
    if (scanf("%d", &qtd_sensores) != 1 || qtd_sensores <= 0 || qtd_sensores > MAX_SENSORES) {
        printf("Quantidade de sensores inválida.\n");
        return quantidadeSensores();
    }
    return qtd_sensores;
}

int main() {
    int qtd_leitura_sensores = 2000;
    int qtd_sensores;
    Sensor sensores[MAX_SENSORES];
    srand(time(NULL));

    printf("\n--------- Programa 03 ---------\n");

    Data inicioData, fimData;
    char nome_arquivo[100];

    printf("Digite o início do intervalo (DD MM AAAA HH MM): ");
    if (scanf("%d %d %d %d %d", &inicioData.dia, &inicioData.mes, &inicioData.ano,
              &inicioData.hora, &inicioData.min) != 5 || !validarData(inicioData)) {
        printf("Data inicial inválida.\n");
        return 1;
    }

    printf("Digite o fim do intervalo (DD MM AAAA HH MM): ");
    if (scanf("%d %d %d %d %d", &fimData.dia, &fimData.mes, &fimData.ano,
              &fimData.hora, &fimData.min) != 5 || !validarData(fimData)) {
        printf("Data final inválida.\n");
        return 1;
    }

    time_t inicio = converterData(inicioData);
    time_t fim = converterData(fimData);
    if (fim <= inicio) {
        printf("Erro: a data final deve ser maior que a inicial.\n");
        return 1;
    }
    quantidadeSensores();
    
    getchar(); // Limpar o buffer

    for (int i = 0; i < qtd_sensores; i++) {
        printf("\nSensor Nº%d:\n", i + 1);
        printf("Digite o ID do sensor: ");
        scanf("%s", sensores[i].ID_SENSOR);
        sensores[i].tipo = escolhertipo();
        if (sensores[i].tipo == TIPO_INVALIDO) return 1;
    }

    printf("Digite o nome do arquivo para salvar os dados (ex: dados.txt): ");
    scanf("%s", nome_arquivo);

    FILE *arquivo = fopen(nome_arquivo, "w");
    if (!arquivo) {
        perror("Erro ao criar o arquivo");
        return 1;
    }

    Amostra *amostras = malloc(sizeof(Amostra) * qtd_sensores * qtd_leitura_sensores);
    if (!amostras) {
        perror("Erro ao alocar memória");
        fclose(arquivo);
        return 1;
    }

    int total = 0;
    for (int i = 0; i < qtd_sensores; i++) {
        for (int j = 0; j < qtd_leitura_sensores; j++) {
            amostras[total].timestamp = gerarTimestampAleatorio(inicio, fim);
            strcpy(amostras[total].id_sensor, sensores[i].ID_SENSOR);
            gerarValor(sensores[i].tipo, amostras[total].valor);
            total++;
        }
    }

    for (int i = total - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Amostra temp = amostras[i];
        amostras[i] = amostras[j];
        amostras[j] = temp;
    }

    for (int i = 0; i < total; i++) {
        fprintf(arquivo, "%lld;%s;%s\n", (long long)amostras[i].timestamp,
                amostras[i].id_sensor, amostras[i].valor);
    }

    free(amostras);
    fclose(arquivo);
    printf("\nArquivo '%s' gerado com sucesso.\n", nome_arquivo);
    return 0;
}
