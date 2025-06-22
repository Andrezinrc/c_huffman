#include <stdio.h>
#include <string.h>
#include "huffman.h"
#include "stats.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Uso: %s <compress|decompress> <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "compress") == 0) {
        printf("Iniciando compactação...\n");
        compress(argv[2], argv[3]);
        printf("Arquivo compactado com sucesso!\n");
        printCompressionStats(argv[2], argv[3]);

    } else if (strcmp(argv[1], "decompress") == 0) {
        printf("Iniciando descompactação...\n");
        decompress(argv[2], argv[3]);
        printf("Arquivo descompactado com sucesso!\n");

        // exibe tamanho antes e depois da descompactacao 
        long compressed = getFileSize(argv[2]);
        long restored = getFileSize(argv[3]);

        printf("\n--- Estatísticas de Descompactação ---\n");
        printf("Tamanho do arquivo .huff: %.2f MB\n", compressed / (1024.0 * 1024));
        printf("Tamanho restaurado: %.2f MB\n", restored / (1024.0 * 1024));

    } else {
        printf("Comando inválido. Use 'compress' ou 'decompress'.\n");
        return 1;
    }

    return 0;
}
