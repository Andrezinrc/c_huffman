#ifndef STATS_H
#define STATS_H

// retorna o tamanho do arquivo em bytes
long getFileSize(const char* filePath);

// retorna o tamanho total em bytes de todos os arquivos dentro da pasta, recursivamente
long getFolderSize(const char* path);

// imprime estatisticas de compressão: tamanho original, tamanho compactado e economia percentual
void printCompressionStats(const char* originalPath);

// imprime estatisticas de descompactacao: tamanhos e recuperacao percentual
void printDecompressionStats(const char* compressedPath);
#endif
