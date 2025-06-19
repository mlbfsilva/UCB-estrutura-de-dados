#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SENSORES 8000
#define MAX_VALOR 64
#define MAX_ID_SENSOR 64

typedef struct {
    long timestamp;
    char valor[MAX_VALOR];
} Leitura_t;

typedef struct {
    char id_sensor[MAX_ID_SENSOR];
    Leitura_t *leituras;
    int total;
    int capacidade;
} Sensor_t;

Sensor_t sensores[MAX_SENSORES];
int total_sensores = 0;

int contar_linhas_arquivo(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir arquivo para contagem de linhas");
        return -1;
    }
    int linhas = 0;
    char buffer[512];
    while (fgets(buffer, sizeof(buffer), arquivo)) {
        linhas++;
    }
    fclose(arquivo);
    return linhas;
}

Sensor_t* encontrar_sensor_ou_criar(const char* id_sensor) {
    for (int i = 0; i < total_sensores; i++) {
        if (strcmp(sensores[i].id_sensor, id_sensor) == 0) {
            return &sensores[i];
        }
    }
    if (total_sensores >= MAX_SENSORES) {
        fprintf(stderr, "Erro: número máximo de sensores atingido.\n");
        exit(1);
    }
    Sensor_t* novo = &sensores[total_sensores++];
    strcpy(novo->id_sensor, id_sensor);
    novo->leituras = NULL;
    novo->total = 0;
    novo->capacidade = 0;
    return novo;
}

void adicionar_leitura(Sensor_t* sensor, long timestamp, const char* valor) {
    if (sensor->total >= sensor->capacidade) {
        sensor->capacidade = (sensor->capacidade == 0) ? 10 : sensor->capacidade * 2;
        sensor->leituras = realloc(sensor->leituras, sensor->capacidade * sizeof(Leitura_t));
        if (!sensor->leituras) {
            perror("Erro ao alocar memória para leituras");
            exit(1);
        }
    }
    sensor->leituras[sensor->total].timestamp = timestamp;
    strncpy(sensor->leituras[sensor->total].valor, valor, MAX_VALOR);
    sensor->leituras[sensor->total].valor[MAX_VALOR - 1] = '\0';
    sensor->total++;
}

void abrir_arquivo(const char *arq_nome, int n_linhas, Sensor_t *sensor_array) {
    FILE *arquivo_entrada = fopen(arq_nome, "r");
    if (arquivo_entrada == NULL) {
        printf("Erro ao abrir arquivo.\n");
        return;
    }

    char linha[256];
    int linhas_lidas = 0;
    int numero_linha = 1;

    while (fgets(linha, sizeof(linha), arquivo_entrada) && linhas_lidas < n_linhas) {
        long timestamp;
        char id_sensor[MAX_ID_SENSOR];
        char valor[MAX_VALOR];

        int lida = sscanf(linha, "%ld;%63[^;];%63[^\n]", &timestamp, id_sensor, valor);
        if (lida != 3) {
            fprintf(stderr, "Linha inválida [%d]: %s", numero_linha, linha);
            numero_linha++;
            continue;
        }

        Sensor_t *sensor = encontrar_sensor_ou_criar(id_sensor);
        adicionar_leitura(sensor, timestamp, valor);
        linhas_lidas++;
        numero_linha++;
    }

    fclose(arquivo_entrada);
}


void ordenar_leituras_por_timestamp_decrescente(Sensor_t *sensor) {
    for (int i = 0; i < sensor->total - 1; i++) {
        for (int j = i + 1; j < sensor->total; j++) {
            if (sensor->leituras[i].timestamp < sensor->leituras[j].timestamp) {
                Leitura_t temp = sensor->leituras[i];
                sensor->leituras[i] = sensor->leituras[j];
                sensor->leituras[j] = temp;
            }
        }
    }
}

void criar_arquivos_por_sensor() {
    for (int i = 0; i < total_sensores; i++) {
        Sensor_t *s = &sensores[i];
        ordenar_leituras_por_timestamp_decrescente(s);
        char nome_arquivo[128];
        snprintf(nome_arquivo, sizeof(nome_arquivo), "%s.txt", s->id_sensor);
        FILE *arquivo = fopen(nome_arquivo, "w");
        if (!arquivo) {
            perror("Erro ao criar arquivo para sensor");
            continue;
        }
        for (int j = 0; j < s->total; j++) {
            fprintf(arquivo, "%ld;%s\n", s->leituras[j].timestamp, s->leituras[j].valor);
        }
        fclose(arquivo);
    }
}

int main() {
    char nome_arquivo[128];
    printf("Digite o nome do arquivo a ser lido: ");
    scanf("%127s", nome_arquivo);
    int numero_linhas = contar_linhas_arquivo(nome_arquivo);
    if (numero_linhas <= 0) {
        return 1;
    }
    abrir_arquivo(nome_arquivo, numero_linhas, sensores);
    printf("Sensores lidos: %d\n", total_sensores);
    for (int i = 0; i < total_sensores; i++) {
        printf("Sensor %s: %d leituras\n", sensores[i].id_sensor, sensores[i].total);
    }
    criar_arquivos_por_sensor();
    return 0;
}
