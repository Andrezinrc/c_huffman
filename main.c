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
        compress(argv[2], argv[3]);
        printCompressionStats(argv[2], argv[3]);
    } else if (strcmp(argv[1], "decompress") == 0) {
        decompress(argv[2], argv[3]);
    } else {
        printf("Comando inválido. Use 'compress' ou 'decompress'.\n");
        return 1;
    }

    return 0;
}
