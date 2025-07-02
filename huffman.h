#ifndef HUFFMAN_H
#define HUFFMAN_H

// estrutura do nó da árvore de huffman
typedef struct Node {
    unsigned char character;  // caractere armazenado
    int frequency;            // frequência do caractere
    struct Node *left;        // filho esquerdo
    struct Node *right;       // filho direito
} Node;

// terminal candy: simple progress bar
// le um arquivo e conta a frequência de cada byte
// cria e inicializa um novo nó da árvore de Huffman com caractere e frequência
// funcao de comparcao usada para ordenar os nós por frequência
// gera uma lista de nós a partir do array de frequências
// constrói a árvore de Huffman a partir de uma lista de nós com caracteres e suas frequência
// percorre a árvore de Huffman e gera os códigos binários para cada caractere folha
// compacta o arquivo original usando huffman e grava o resultado no arquivo de saida
// descompacta o arquivo comprimido com Huffman e grava o resultado no arquivo de saida

void showProgressBar(int percent);
int* CountFrequency(const char fileName[]);
Node* createNode(unsigned char character, int frequency);
int compareNode(const void* a, const void* b);
int generateNodeList(int* frequency, Node* nodeList[]);
Node* buildHuffmanTree(Node* nodes[], int count);
void generateCodes(Node* root, char* path, int depth, char* codes[256]);
void compress(const char* filePath, const char* outputPath);
void decompress(const char* filePath, const char* outputPath);

#endif
