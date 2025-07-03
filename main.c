#include <stdio.h>
#include <string.h>
#include "huffman.h"
#include "stats.h"
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

        if (isDirectory(argv[2])) {
            compressFolder(argv[2]);
        } else {
            compress(argv[2]);
        }

        printf(GREEN "Arquivo compactado com sucesso!\n" RESET);
        printCompressionStats(argv[2]);

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
            char outputDir[1024];
            decompressFolderFromHuff(argv[2], outputDir); // preenche outputDir
            printf(GREEN "Arquivo descompactado com sucesso!\n" RESET);
            printDecompressionStats(argv[2]); // usa .adr para deduzir pasta
        }
    }else {
        printf(RED "Comando inválido: %s\n" RESET, argv[1]);
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}
