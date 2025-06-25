#include <stdio.h>
#include <string.h>
#include "huffman.h"
#include "stats.h"
#include "colors.h"
#include "help.h"

int main(int argc, char* argv[]) {
    printf("\033[2J\033[H");
    printf(BLUE "======================================\n" RESET);
    printf(BLUE "             HUFFPACK 1.0\n" RESET);
    printf(BLUE "    Huffman-Based Folder Compressor\n" RESET);
    printf(BLUE "======================================\n" RESET);
    printf("   Desenvolvido por Andrecode ©2025\n\n");

    if (argc < 2 || strcmp(argv[1], "help") == 0) {
        printUsage(argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "compress") == 0) {
        if (argc != 4) {
            printUsage(argv[0]);
            return 1;
        }
        printf(GREEN "Iniciando compactação...\n" RESET);
        compress(argv[2], argv[3]);
        printf(GREEN "Arquivo compactado com sucesso!\n" RESET);
        printCompressionStats(argv[2], argv[3]);

    } else if (strcmp(argv[1], "decompress") == 0) {
        if (argc != 4) {
            printUsage(argv[0]);
            return 1;
        }
        printf(GREEN "Iniciando descompactação...\n" RESET);
        decompress(argv[2], argv[3]);
        printf(GREEN "Arquivo descompactado com sucesso!\n" RESET);
        printCompressionStats(argv[2], argv[3]);

    } else {
        printf(RED "Comando inválido: %s\n" RESET, argv[1]);
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}
