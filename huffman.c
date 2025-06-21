#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"
#define MAX_NODES 256

// le um arquivo e conta a frequência de cada byte
// cria e inicializa um novo nó da árvore de Huffman com caractere e frequência
// funcao de comparcao usada para ordenar os nós por frequência
// gera uma lista de nós a partir do array de frequências
// constrói a árvore de Huffman a partir de uma lista de nós com caracteres e suas frequência

int* CountFrequency(const char fileName[]);
Node* createNode(unsigned char character, int frequency);
int compareNode(const void* a, const void* b);
int generateNodeList(int* frequency, Node* nodeList[]);
Node* buildHuffmanTree(Node* nodes[], int count);


int main() {
    
    return 0;
}


// le um arquivo e conta a frequência de cada byte
int* CountFrequency(const char fileName[]){
    // aloca memoria para 256 inteiros e inicializa com zero
    int *frequency = calloc(256, sizeof(int));
    
    if (!frequency) {
        perror("Erro ao alocar memória para frequências");
        return NULL;
    }

    // abre o arquivo em modo binário de leitura
    FILE *file = fopen(fileName, "rb");
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
    file = NULL;

    
    return frequency;
}

// cria e inicializa um novo nó da árvore de Huffman com caractere e frequência
Node* createNode(unsigned char character, int frequency){
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


// funcao de comparcao usada para ordenar os nós por frequência
int compareNode(const void* a, const void* b){
    Node* nodeA = *(Node**)a; // converte o ponteiro genérico para ponteiro de Node
    Node* nodeB = *(Node**)b; // idem
    return nodeA->frequency - nodeB->frequency; // ordena pela frequência
}

// gera uma lista de nós a partir do array de frequências
int generateNodeList(int* frequency, Node* nodeList[]){
    int count = 0;

    for(int i = 0; i < 256; i++){
        if(frequency[i] > 0){
            // cria um nó com o caractere e sua frequência
            nodeList[count++] = createNode((unsigned char)i, frequency[i]);
        }
    }

    return count; // quantidade de nós criados
}

// constrói a árvore de Huffman a partir de uma lista de nós com caracteres e suas frequências
Node* buildHuffmanTree(Node* nodes[], int count){
    // ordena a lista de nós por frequência
    while(count > 1){

        //pega os dois nós de menor frequência
        Node *left = nodes[0];
        Node *right = nodes[1];

        //cria um novo nó combinando os dois
        Node *newNode = createNode('\0', left->frequency + right->frequency);
        newNode->left = left;
        newNode->right = right;

        
        // move todos os nós restantes 2 posicoes para tras
        for(int i = 2; i < count; i++){
            nodes[i - 2] = nodes[i];
        }

        // adiciona o novo nó no fim da lista
        nodes[count - 2] = newNode;

        count--;
    }
    
    //quando restar só um nó, é a raiz da árvore
    return nodes[0];
}