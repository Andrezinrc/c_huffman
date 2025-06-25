#include <stdio.h>
#include <string.h>
#include "huffman.h"
#include "stats.h"
#include "colors.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf(RED "Uso: %s <compress|decompress> <arquivo_entrada> <arquivo_saida>\n" RESET, argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "compress") == 0) {
        printf(BLUE "======================================\n" RESET);
        printf(BLUE "             HUFFPACK 1.0\n" RESET);
        printf(BLUE "    Huffman-Based Folder Compressor\n" RESET);
        printf(BLUE "======================================\n" RESET);
        printf(BLUE "   Desenvolvido por Andrecode ©2025\n\n" RESET);
        printf(GREEN "Iniciando compactação...\n" RESET);
        compress(argv[2], argv[3]);
        printf(GREEN "Arquivo compactado com sucesso!\n" RESET);
        printCompressionStats(argv[2], argv[3]);

    } else if (strcmp(argv[1], "decompress") == 0) {
        printf(GREEN "Iniciando descompactação...\n" RESET);
        decompress(argv[2], argv[3]);
        printf(GREEN "Arquivo descompactado com sucesso!\n" RESET);
        printCompressionStats(argv[2], argv[3]);
    }

    return 0;
}
