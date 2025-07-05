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

        // calcula tamanho original (arquivo ou pasta)
        long originalSize = isDirectory(argv[2]) ? getFolderSize(argv[2]) : getFileSize(argv[2]);

        // monta nome do .adr
        char adrPath[1024];
        snprintf(adrPath, sizeof(adrPath), "%s.adr", argv[2]);

        long compressedSize = getFileSize(adrPath);

        // exibe resultados
        if (originalSize != -1 && compressedSize != -1) {
            printHumanSize("\nTamanho original", originalSize);
            printHumanSize("Tamanho compactado", compressedSize);
        }

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
            // calcula tamanho compactado antes da descompressão
            long compressedSize = getFileSize(argv[2]);

            // nova funcao genérica que lida com .adr e define saida automaticamente
            decompressEntry(argv[2]);

            // monta saída sem .adr
            char outputName[1024];
            strncpy(outputName, argv[2], sizeof(outputName));
            outputName[sizeof(outputName) - 1] = '\0';
            char* dot = strrchr(outputName, '.');
            if (dot && strcmp(dot, ".adr") == 0) *dot = '\0';

            // calcula tamanho final (arquivo ou pasta)
            long finalSize = isDirectory(outputName) ? getFolderSize(outputName) : getFileSize(outputName);

            // exibe resultados
            if (compressedSize != -1 && finalSize != -1) {
                printHumanSize("\nTamanho compactado", compressedSize);
                printHumanSize("Tamanho descompactado", finalSize);
            }

            printf(GREEN "Arquivo descompactado com sucesso!\n" RESET);
        }
    } else {
        printf(RED "Comando inválido: %s\n" RESET, argv[1]);
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}
