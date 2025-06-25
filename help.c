#include <stdio.h>
#include "colors.h"
#include "help.h"

// imprime os comandos disponiveis
void printUsage(const char *programName) {
    printf(BLUE "Uso:\n" RESET);
    printf("  %s compress <entrada> <saida.huff>\n", programName);
    printf("  %s decompress <entrada.huff> <saida>\n", programName);
    printf("  %s help\n\n", programName);

    printf(BLUE "Exemplos:\n" RESET);
    printf("  %s compress arquivo.bin arquivo.huff\n", programName);
    printf("  %s decompress arquivo.huff arquivo_restaurado.bin\n", programName);
}
