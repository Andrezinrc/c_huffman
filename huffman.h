#ifndef HUFFMAN_H
#define HUFFMAN_H

// estrutura do nó da árvore de huffman
typedef struct Node {
    unsigned char character;  // caractere armazenado
    int frequency;            // frequência do caractere
    struct Node *left;        // filho esquerdo
    struct Node *right;       // filho direito
} Node;

#include <stdio.h>

// verifica se um caminho é um diretorio
// libera a memória da arvore de huffman
// cria diretorios ausentes para um caminho de arquivo

int isDirectory(const char* path);
void freeTree(Node* root);
void createDirsForFile(const char* filePath);

// retorna o tamanho do arquivo em bytes
// terminal candy: simple progress bar
// le um arquivo e conta a frequência de cada byte
// cria e inicializa um novo nó da árvore de Huffman com caractere e frequência
// funcao de comparcao usada para ordenar os nós por frequência
// gera uma lista de nós a partir do array de frequências
// constrói a árvore de Huffman a partir de uma lista de nós com caracteres e suas frequência
// percorre a árvore de Huffman e gera os códigos binários para cada caractere folha
// retorna o tamanho do arquivo em bytes
// calcula o tamanho total de uma pasta recursivamente
// exibe um valor de tamanho em formato legivel (KB, MB, GB, etc)

long getFileSize(const char* path);
void showProgressBar(int percent);
int* CountFrequency(const char fileName[]);
Node* createNode(unsigned char character, int frequency);
int compareNode(const void* a, const void* b);
int generateNodeList(int* frequency, Node* nodeList[]);
Node* buildHuffmanTree(Node* nodes[], int count);
void generateCodes(Node* root, char* path, int depth, char* codes[256]);
long getFolderSize(const char* path);
void printHumanSize(const char* label, long bytes);

// verifica se o caminho é um arquivo regular
int is_file(const char* path);

// verifica se o caminho é um diretorio
int is_folder(const char* path);

// funcoes principais para compressao
void compressSingleFileToStream(const char* filePath, const char* relativePath, FILE* output);
void walkAndCompress(const char* basePath, const char* currentPath, FILE* output);
void compressEntry(const char* inputPath);

// funcao para descompressao
void decompressFolderFromHuff(const char* huffPath, char* outputDir);
void decompressEntry(const char* inputPath);

#endif
