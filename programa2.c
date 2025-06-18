#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_SENSORES 8000

typedef struct {
    long long timestamp;
    char valor;
}Leitura_t;

long long converter_para_timestamp(int ano, int mes, int dia, int hora, int min, int seg) {
    struct tm data = {0};
    data.tm_year = ano - 1900;
    data.tm_mon = mes - 1;
    data.tm_mday = dia;
    data.tm_hour = hora;
    data.tm_min = min;
    data.tm_sec = seg;
    return (long long)mktime(&data);
}

int carregar_dados(const char *arquivo, Leitura_t * dados){
    FILE *arquivo_dados = fopen(arquivo, "r");
    if (!arquivo_dados){
        perror("Erro ao abrir arquivo.\n");
        return 0;
    }
    int i=0;
    while(fscanf(arquivo_dados, "%lld; %f\n", dados[i].timestamp, dados[i].valor ==2)){
        i++;
        if (i >= MAX_SENSORES) break;
    }
    fclose(arquivo_dados);
    return i;
}

int busca_binaria_mais_proxima(Leitura_t *dados, int size, long long alvo){
    int inicio = 0, fim = size -1;
    int melhor_indice= -1;
    long long menor_diferença = 100000000000000;

    while(inicio <= fim){
        int meio = (inicio + fim) / 2;
        long long diff = llabs(dados[meio].timestamp - alvo);

        if(diff < menor_diferença){
            menor_diferença = diff;
            melhor_indice = meio;
        }
        if(dados[meio].timestamp < alvo){
            inicio = meio + 1;
        }else if (dados[meio].timestamp > alvo){
            fim = meio -1;
        }else { return meio; }
    }
    return melhor_indice;
}

int main(){
    char nome_sensor[20];
    int dia, mes, ano, hora, minuto, segundo;

    printf("Infomre o nome do sensor a ser pesquisado:");
    scanf("%s", nome_sensor);

    printf("Informe a data e hora (DD MM AAAA HH MM SS):");
    scanf("%d, %d, %d, %d, %d, %d", &dia, &mes, &ano, &hora, &minuto, &segundo);

    long long timestamp_alvo = converter_para_timestamp(ano, mes, dia, hora, minuto, segundo);

    char nome_arquivo[20];
    snprintf(nome_arquivo, sizeof(nome_arquivo), "%stxt", nome_sensor);

    Leitura_t dados[MAX_SENSORES];
    int total = carregar_dados(nome_arquivo, dados);

    if(total == 0){
        printf("Nenhum dado carragado.\n");
        return 1;
    }

    int indice = busca_binaria_mais_proxima(dados, total, timestamp_alvo);
    if(indice != -1){
        printf("\nLeitura mais próxima encontrada:\n");
        printf("Sensor: %s\nTimestamp: %lld\nValor: %s", nome_sensor, dados[indice].timestamp, dados[indice].valor);
    }else {
        printf("Nenhuma leitura encontrada.\n");
    } return 0;
}