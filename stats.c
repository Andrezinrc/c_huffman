#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "stats.h"
#include "colors.h"

// retorna o tamanho do arquivo em bytes
long getFileSize(const char* filePath) {
    FILE* file = fopen(filePath, "rb");
    if (!file) return -1;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    return size;
}

// retorna o tamanho total em bytes de todos os arquivos dentro da pasta, recursivamente
long getFolderSize(const char* path) {
    long totalSize = 0;

    DIR* dir = opendir(path);
    if (!dir) return -1;

    struct dirent* entry;
    char fullPath[2048];

    while ((entry = readdir(dir)) != NULL) {
        // Ignora "." e ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

        struct stat pathStat;
        if (stat(fullPath, &pathStat) == -1)
            continue;

        if (S_ISDIR(pathStat.st_mode)) {
            // Soma tamanho recursivamente se for diretório
            long subFolderSize = getFolderSize(fullPath);
            if (subFolderSize >= 0)
                totalSize += subFolderSize;
        } else if (S_ISREG(pathStat.st_mode)) {
            // Soma tamanho do arquivo
            totalSize += pathStat.st_size;
        }
    }

    closedir(dir);
    return totalSize;
}

// imprime estatisticas de compressão: tamanho original, tamanho compactado e economia percentual
void printCompressionStats(const char* originalPath, const char* compressedPath) {
    long originalSize = getFileSize(originalPath); // tamanho antes da compressao
    long compressedSize = getFileSize(compressedPath); // tamanho após compressao

    if (originalSize <= 0 || compressedSize <= 0) {
        printf(RED "Erro ao obter tamanho dos arquivos.\n" RESET);
        return;
    }

    // calcula percentual de economia
    double ratio = 100.0 * ((double)originalSize - compressedSize) / originalSize;

    const char* units[] = {"B", "KB", "MB", "GB"};
    int u1 = 0, u2 = 0;
    double oSize = originalSize, cSize = compressedSize;

    // ajusta unidades (B, KB, MB...) para melhor leitura
    while (oSize >= 1024 && u1 < 3) { oSize /= 1024.0; u1++; }
    while (cSize >= 1024 && u2 < 3) { cSize /= 1024.0; u2++; }

    // exibe os resultados formatados
    printf(BLUE "\n--- Estatísticas de Compressão ---\n" RESET);
    printf(BLUE "Tamanho original: %.2f %s\n" RESET, oSize, units[u1]);
    printf(BLUE "Tamanho compactado: %.2f %s\n" RESET, cSize, units[u2]);

    if (ratio >= 0)
        printf("Economia: %.2f%%\n", ratio);
    else
        printf("Aumento de tamanho: %.2f%%\n", -ratio);
}

// imprime estatisticas de descompactacao: tamanhos e recuperacao percentual
void printDecompressionStats(const char* compressedPath, const char* outputPath) {
    // obtem tamanho do arquivo .huff
    long compressedSize = getFileSize(compressedPath);
    // obtem tamanho total da pasta descompactada
    long extractedSize = getFolderSize(outputPath);

    // verifica se houve erro ao obter os tamanhos
    if (compressedSize <= 0 || extractedSize <= 0) {
        printf(RED "Erro ao obter tamanho dos arquivos.\n" RESET);
        return;
    }

    // calcula percentual de recuperacao
    double ratio = 100.0 * ((double)extractedSize - compressedSize) / compressedSize;
    
    // converte tamanhos para unidades legíveis (KB, MB, ...)
    const char* units[] = {"B", "KB", "MB", "GB"};
    int u1 = 0, u2 = 0;
    double cSize = compressedSize, eSize = extractedSize;

    while (cSize >= 1024 && u1 < 3) { cSize /= 1024.0; u1++; }
    while (eSize >= 1024 && u2 < 3) { eSize /= 1024.0; u2++; }

    // imprime as estatisticas
    printf(BLUE "\n--- Estatísticas de Descompactação ---\n" RESET);
    printf(BLUE "Tamanho compactado: %.2f %s\n" RESET, cSize, units[u1]);
    printf(BLUE "Tamanho descompactado: %.2f %s\n" RESET, eSize, units[u2]);

    // imprime percentual de recuperação ou reducao anormal
    if (ratio >= 0)
        printf("Recuperação: %.2f%%\n", ratio);
    else
        printf("Redução negativa (?): %.2f%%\n", -ratio);
}
