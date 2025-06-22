#include <stdio.h>
#include "stats.h"

// retorna o tamanho do arquivo em bytes
long getFileSize(const char* filePath) {
    FILE* file = fopen(filePath, "rb");
    if (!file) return -1;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    return size;
}

// imprime estatisticas de compressão: tamanho original, tamanho compactado e economia percentual
void printCompressionStats(const char* originalPath, const char* compressedPath) {
    long originalSize = getFileSize(originalPath);
    long compressedSize = getFileSize(compressedPath);

    if (originalSize <= 0 || compressedSize <= 0) {
        printf("Erro ao obter tamanho dos arquivos.\n");
        return;
    }

    double ratio = 100.0 * (1.0 - (double)compressedSize / originalSize);

    const char* units[] = {"B", "KB", "MB", "GB"};
    int u1 = 0, u2 = 0;
    double oSize = originalSize, cSize = compressedSize;

    while (oSize >= 1024 && u1 < 3) { oSize /= 1024.0; u1++; }
    while (cSize >= 1024 && u2 < 3) { cSize /= 1024.0; u2++; }

    printf("\n--- Estatísticas de Compressão ---\n");
    printf("Tamanho original: %.2f %s\n", oSize, units[u1]);
    printf("Tamanho compactado: %.2f %s\n", cSize, units[u2]);
    printf("Economia: %.2f%%\n", ratio);
}
