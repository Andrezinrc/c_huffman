#ifndef HUFFMAN_H
#define HUFFMAN_H

// estrutura do nó da árvore de huffman
typedef struct Node {
    unsigned char character;  // caractere armazenado
    int frequency;            // frequência do caractere
    struct Node *left;        // filho esquerdo
    struct Node *right;       // filho direito
} Node;

#endif