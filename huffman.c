#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"


// le um arquivo e conta a frequência de cada byte
int* CountFrequency(const char fileName[]) {
    // aloca memoria para 256 inteiros e inicializa com zero
    int *frequency = calloc(256, sizeof(int));
    
    if (!frequency) {
        perror("Erro ao alocar memória para frequências");
        return NULL;
    }

    // abre o arquivo em modo binário de leitura
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("Erro ao abrir arquivo");
        free(frequency);
        return NULL;
    }

    unsigned char byte;
    // le o arquivo byte a byte
    while (fread(&byte, 1, 1, file) == 1) {
        frequency[byte]++;
    }

    fclose(file);
    return frequency;
}

// cria e inicializa um novo nó da árvore de Huffman com caractere e frequência
Node* createNode(unsigned char character, int frequency) {
    // aloca memória para um novo nó
    Node *node = (Node*)malloc(sizeof(Node));
    
    if (!node) {
        return NULL;
    }

    // inicializa os campos do nó
    node->character = character;
    node->frequency = frequency;
    node->left = NULL;
    node->right = NULL;

    return node;
}

int main() {
    
    return 0;
}