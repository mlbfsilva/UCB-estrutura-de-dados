#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SENSORES 8000
#define MAX_VALOR 64
#define MAX_LINHAS 2000
#define MAX_ID_SENSOR 64

typedef enum {TIPO_INT, TIPO_FLOAT, TIPO_BOOL, TIPO_STRING, TIPO_INVALIDO} TipoDado;

typedef struct {
    long timestamp;
    char valor;
}Leitura_t;

typedef struct {
    char id_sensor;
    Leitura_t * leituras;
    int total;
    int capacidade;
}Sensor_t;

Sensor_t sensores[MAX_SENSORES];
int total_sensores = 0;

TipoDado detectar_tipo(const char * valor){
    if(strcmp(valor, "true") ==0 || strcmp(valor, "false")==0)
        return TIPO_BOOL;
    
    int ponto = 0;
    int i = (valor[0] == '-')? 1: 0;
    for(; valor[i]; i++){
        if(valor[i] == '.'){
            if(ponto++) return TIPO_STRING;
        }else if(!isdigit(valor[i])){
            return(strlen(valor)<=16) ? TIPO_STRING : TIPO_INVALIDO;
        }
    }
    return (ponto == 1) ? TIPO_FLOAT : TIPO_INT;
}

const char* tipo(TipoDado tipo){
    switch (tipo){
        case TIPO_INT: return "int";
        case TIPO_FLOAT: return "float";
        case TIPO_BOOL: return "bool";
        case TIPO_STRING: return "string";
        default: return "desconhecido";
    }

}

Sensor_t* encontrar_sensores_ou_cria(const char * id_sensor){
    for(int i = 0; i < total_sensores; i++){
        if(strcmp(sensores[i].id_sensor, id_sensor) == 0){
            return &sensores[i];
        }
    }
    if (total_sensores > MAX_SENSORES){
        fprintf(stderr, "Limite de sensores ultrapassado.\n");
        exit(1);
    }
    Sensor_t* novo = &sensores[total_sensores++];
    strcpy(novo->id_sensor, id_sensor);
    novo->leituras = NULL;
    novo->total = 0;
    novo->capacidade =0;
    return novo;
}

void adicionar_leitura(Sensor_t* sensor, long timestamp, const char* valor){
    sensor->capacidade = (sensor->capacidade == 0)? 10 : sensor->capacidade * 2;
    sensor->leituras = realloc(sensor->leituras, sensor->capacidade * sizeof(Leitura_t));
    if(!sensor->leituras){
        perror("Erro ao reservar memória");
        exit(1);
    }
    sensor->leituras[sensor->total].timestamp = timestamp;
    strncpy(sensor->leituras[sensor->total].valor, valor, MAX_VALOR);
    sensor->total++;
}

void ordenar_sensores_timestamp(Sensor_t *sensor, int size){
    for (int i=0; i < size -1; i++){
        for(int j = i+1; j<size; j++){
            if(sensor[i].leituras->timestamp > sensor[j].leituras->timestamp){
                Sensor_t temp = sensor[i];
                sensor[i] = sensor[j];
                sensor[j] = temp;
            }
        }
    }
}

void salvar_arquivos(){
    for(int i =0; i < total_sensores; i++){
        Sensor_t* s = &sensores[i];
        qsort(s->leituras, s->total, sizeof(Leitura_t), ordenar_sensores_timestamp);

        char nome_arquivo[20];
        snprintf(nome_arquivo, sizeof(nome_arquivo), "%s.txt", s->id_sensor);

        FILE * arquivo = fopen(nome_arquivo, "w");
        if(!arquivo){
            perror("Erro ao tentar criar arquivo.");
            continue;
        }
        for(int j = 0; j< s->total; j++){
            fprintf(arquivo, s->leituras[j].timestamp, s->id_sensor, s->leituras[j].valor);
        }
        fclose(arquivo);
    }
}

int main(int argc, char* argv[]){
    if(argc !=2){
        fprintf(stderr, "Arquivo de entrada: %s\n", argv[0]);
        return 1;
    }
    FILE* arquivo_entrada = fopen(argv[1], "r");
    if(!arquivo_entrada){
        perror("Erro ao abrir arquivo.\n");
        return 1;
    }

    char linha[MAX_LINHAS];
    while(fgets(linha, sizeof(linha), arquivo_entrada)){
        long timestamp;
        char id_sensor[MAX_ID_SENSOR];
        char valor[MAX_VALOR];

        if(sscanf(linha, "%ld, %s, %s", &timestamp, id_sensor, valor) !=3){
            fprintf(stderr, "Linha inválida: %s", linha);
            continue;
        }
        TipoDado tipo = detectar_tipo(valor);
        if(tipo == TIPO_INVALIDO){
            fprintf(stderr, "Valor inválido: %s\n", valor);
            continue;
        }
        Sensor_t *s = encontrar_sensores_ou_cria(id_sensor);
        adicionar_leitura(s, timestamp, valor);
    }

    fclose(arquivo_entrada);
    salvar_arquivos;
    printf("Processamento concluído. Arquivos gerados por sensor.\n");
    return 0;
}