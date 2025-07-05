#include <stdio.h>
#include <string.h>
#include "huffman.h"
#include "colors.h"
#include "help.h"
#include "header.h"

int main(int argc, char* argv[]) {
    if (argc < 2 || strcmp(argv[1], "help") == 0) {
        printUsage(argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "compress") == 0) {
        print_header("compress");

        if (argc != 3) {
            printUsage(argv[0]);
            return 1;
        }

        printf(GREEN "Iniciando compactação...\n" RESET);

        compressEntry(argv[2]);

        printf(GREEN "Arquivo compactado com sucesso!\n" RESET);

    } else if (strcmp(argv[1], "decompress") == 0) {
        print_header("decompress");

        if (argc != 3) {
            printUsage(argv[0]);
            return 1;
        }

        printf(GREEN "Iniciando descompactação...\n" RESET);

        if (isDirectory(argv[2])) {
            // nunca ocorre: arquivo .adr é sempre arquivo
            printf(RED "Você deve fornecer o caminho para o arquivo .adr, não uma pasta.\n" RESET);
            return 1;
        } else {
            // nova funcao genérica que lida com .adr e define saida automaticamente
            decompressEntry(argv[2]);

            printf(GREEN "Arquivo descompactado com sucesso!\n" RESET);
        }
    } else {
        printf(RED "Comando inválido: %s\n" RESET, argv[1]);
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}

