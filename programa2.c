#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_SENSORES 8000
#define MAX_VALOR 64

typedef struct {
    long long timestamp;
    char valor[MAX_VALOR];
} Leitura_t;

long long converter_para_timestamp(int ano, int mes, int dia, int hora, int min) {
    struct tm data = {0};
    data.tm_year = ano - 1900;
    data.tm_mon = mes - 1;
    data.tm_mday = dia;
    data.tm_hour = hora;
    data.tm_min = min;
    data.tm_sec = 0;
    return (long long)mktime(&data);
}

int carregar_dados(const char *arquivo, Leitura_t *dados) {
    FILE *arquivo_dados = fopen(arquivo, "r");
    if (!arquivo_dados) {
        perror("Erro ao abrir arquivo");
        return 0;
    }

    int i = 0;
    while (fscanf(arquivo_dados, "%lld;%63[^\n]\n", &dados[i].timestamp, dados[i].valor) == 2) {
        i++;
        if (i >= MAX_SENSORES) break;
    }

    fclose(arquivo_dados);
    return i;
}

int busca_binaria_mais_proxima(Leitura_t *dados, int size, long long alvo) {
    int inicio = 0, fim = size - 1;
    int melhor_indice = -1;
    long long menor_diferenca = 100000000000000;

    while (inicio <= fim) {
        int meio = (inicio + fim) / 2;
        long long diff = llabs(dados[meio].timestamp - alvo);

        if (diff < menor_diferenca) {
            menor_diferenca = diff;
            melhor_indice = meio;
        }

        if (dados[meio].timestamp < alvo) {
            inicio = meio + 1;
        } else if (dados[meio].timestamp > alvo) {
            fim = meio - 1;
        } else {
            return meio;
        }
    }

    return melhor_indice;
}

int main() {
    char nome_sensor[64];
    int dia, mes, ano, hora, minuto;

    printf("Informe o nome do sensor a ser pesquisado: ");
    scanf("%63s", nome_sensor);

    printf("Informe a data e hora (DD MM AAAA HH MM): ");
    scanf("%d %d %d %d %d", &dia, &mes, &ano, &hora, &minuto);

    long long timestamp_alvo = converter_para_timestamp(ano, mes, dia, hora, minuto);

    char nome_arquivo[128];
    snprintf(nome_arquivo, sizeof(nome_arquivo), "%s.txt", nome_sensor);

    Leitura_t dados[MAX_SENSORES];
    int total = carregar_dados(nome_arquivo, dados);

    if (total == 0) {
        printf("Nenhum dado carregado.\n");
        return 1;
    }

    int indice = busca_binaria_mais_proxima(dados, total, timestamp_alvo);
    if (indice != -1) {
        printf("\nLeitura mais próxima encontrada:\n");
        printf("Sensor: %s\nTimestamp: %lld\nValor: %s\n", nome_sensor, dados[indice].timestamp, dados[indice].valor);
    } else {
        printf("Nenhuma leitura encontrada.\n");
    }

    return 0;
}
