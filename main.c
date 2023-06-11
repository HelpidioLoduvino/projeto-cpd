#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "Docs-serial.h"
#include "Docs-serial.c"

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Ler o arquivo de entrada
    Documento *documento = NULL;
    if (rank == 0) {
        documento = ler_file();
    }

    // Transmitir o número de documentos e armários para todos os processos
    int num_documento, num_armario;
    if (rank == 0) {
        num_documento = documento->num_documento;
        num_armario = documento->num_armario;
    }
    MPI_Bcast(&num_documento, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&num_armario, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Dividir a carga de trabalho entre os processos
    int local_start = rank * (num_documento / size);
    int local_end = (rank == size - 1) ? num_documento : (rank + 1) * (num_documento / size);

    // Alocar memória para os documentos locais
    int local_num_documento = local_end - local_start;
    Documento *local_documento = (Documento *)malloc(local_num_documento * sizeof(Documento));

    // Distribuir os documentos para todos os processos
    MPI_Scatter(documento, local_num_documento * sizeof(Documento), MPI_BYTE,
                local_documento, local_num_documento * sizeof(Documento), MPI_BYTE,
                0, MPI_COMM_WORLD);

    // Realizar os cálculos locais
    Armario *local_armario = armario_inicial(NULL, local_documento);
    local_armario = calcular_media_assunto(local_armario, local_documento);
    local_armario = calcular_distancia(local_armario, local_documento);

    // Juntar os armários locais de todos os processos
    Armario *armarios = NULL;
    if (rank == 0) {
        armarios = (Armario *)malloc(num_documento * sizeof(Armario));
    }
    MPI_Gather(local_armario, local_num_documento * sizeof(Armario), MPI_BYTE,
               armarios, local_num_documento * sizeof(Armario), MPI_BYTE,
               0, MPI_COMM_WORLD);

    // Ordenar os armários no processo raiz
    if (rank == 0) {
        armarios = ordenar_armario(armarios);

        // Escrever o arquivo de saída
        FILE *file = fopen("docs.out.txt", "w");
        if (file != NULL) {
            for (int i = 0; i < num_documento; i++) {
                fprintf(file, "%d %d\n", documentos[i].indice_documento, documentos[i].posicao_armario);
            }
            fclose(file);
        } else {
            printf("Erro ao abrir o arquivo de saida!\n");
        }

        free(armarios);
        free(documento);
    }

    // Limpar a memória alocada
    free(local_documento);
    free(local_armario);
    MPI_Finalize();
    return 0;
}
