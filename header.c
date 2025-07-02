#include <stdio.h>
#include "colors.h"

// imprime o cabecalho decorativo
void print_header(const char* role) {
    printf("\033[2J\033[H");
    printf(BLUE "======================================\n" RESET);
    printf(BLUE "             HUFFPACK 1.0\n" RESET);
    printf(BLUE "    Huffman-Based Folder Compressor\n" RESET);
    printf(BLUE "======================================\n" RESET);
    printf("   Desenvolvido por Andrecode ©2025\n\n");
    printf("Modo: %s\n\n", role);
}
