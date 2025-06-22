#ifndef STATS_H
#define STATS_H

// retorna o tamanho do arquivo em bytes
long getFileSize(const char* filePath);

// imprime estatisticas de compressão: tamanho original, tamanho compactado e economia percentual
void printCompressionStats(const char* originalPath, const char* compressedPath);

#endif
