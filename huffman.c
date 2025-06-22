#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"
#include <string.h>
#define MAX_NODES 256

// le um arquivo e conta a frequência de cada byte
// cria e inicializa um novo nó da árvore de Huffman com caractere e frequência
// funcao de comparcao usada para ordenar os nós por frequência
// gera uma lista de nós a partir do array de frequências
// constrói a árvore de Huffman a partir de uma lista de nós com caracteres e suas frequência
// percorre a árvore de Huffman e gera os códigos binários para cada caractere folha
// compacta o arquivo original usando huffman e grava o resultado no arquivo de saida

int* CountFrequency(const char fileName[]);
Node* createNode(unsigned char character, int frequency);
int compareNode(const void* a, const void* b);
int generateNodeList(int* frequency, Node* nodeList[]);
Node* buildHuffmanTree(Node* nodes[], int count);
void generateCodes(Node* root, char* path, int depth, char* codes[256]);
void compress(const char* filePath, const char* fileOutput);

int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    compress(argv[1], argv[2]);
    
    // conta frequência
    int* freq = CountFrequency(argv[1]);
    if(!freq) return 1;

    // cria lista de nós
    Node* nodeList[256];
    int count = generateNodeList(freq, nodeList);
    free(freq);

    // ordena os nós por frequência
    qsort(nodeList, count, sizeof(Node*), compareNode);

    // constroi a árvore de huffma
    Node* raiz = buildHuffmanTree(nodeList, count);

    char* codes[256] = {0}; // vetor para armazenar os codigos
    char path[256];         // caminho temporário para montar os codigo

    generateCodes(raiz, path, 0, codes);

    for(int i = 0; i < 256; i++){
        if(codes[i]){
            printf("'%c' (%d): %s\n", (char)i, i, codes[i]);
            free(codes[i]);
        }
    }

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

    // quando restar só um nó, é a raiz da árvore
    return nodes[0];
}

// gera recursivamente os codigos binarios para cada caractere, percorrendo a árvore de huffman
// cada caminho da raiz até uma folha forma um código (esquerda = '0', direita = '1')
void generateCodes(Node* root, char* path, int depth, char* codes[256]){
    if(!root) return;

    // se for folha, salva o codigo
    if(!root->left && !root->right){
        path[depth] = '\0'; //termina a string
        codes[root->character] = strdup(path); // salva copia do codigo
        return;
    }

    //esquerda = 0
    path[depth] = '0';
    generateCodes(root->left, path, depth + 1, codes);

    //direita = 1
    path[depth] = '1';
    generateCodes(root->right, path, depth + 1, codes);
}

// compacta o arquivo original usando huffman e grava o resultado no arquivo de saida
void compress(const char* filePath, const char* fileOutput) {
    // abre arquivos para leitura e escrita
    // conta frequencia
    printf("Inciando compactação..");

    FILE *file = fopen(filePath, "rb");
    if(!file){
        perror("Erro ao abrir arquivo");
        return;
    }

    int* freq = CountFrequency(filePath);
    if(!freq) return;

    // cria lista de nós, constroi árvore, gera codigos
    Node* nodeList[256];
    int count = generateNodeList(freq, nodeList);
    qsort(nodeList, count, sizeof(Node*), compareNode);
    Node* root = buildHuffmanTree(nodeList, count);

    //gerar codigos
    char* codes[256] = {0};
    char path[256];
    generateCodes(root, path, 0, codes);

    // abrir arquivo de saida pra escrita
    FILE* output = fopen(fileOutput, "wb");
    if(!output){
        perror("Erro ao abrir arquivo para escrita");
        return;
    }

    // salva metadados
    fwrite(freq, sizeof(int), 256, output);

    // le arquivo original, escreve códigos em bits no arquivo destino
    unsigned char buffer = 0;
    int bitCount = 0;

    int c;
    while ((c = fgetc(file)) != EOF) {
        const char* code = codes[c];
        for (int i = 0; code[i] != '\0'; i++) {
            buffer <<= 1;
            if (code[i] == '1') buffer |= 1;
            bitCount++;

            if (bitCount == 8) {
                fwrite(&buffer, 1, 1, output);
                buffer = 0;
                bitCount = 0;
            }
        }
    }
    // escrever bits restantes se houver
    if (bitCount > 0) {
        buffer <<= (8 - bitCount); // completar os bits restantes com 0
        fwrite(&buffer, 1, 1, output);
    }

    // fecha arquivos e limpa memória
    fclose(file);
    fclose(output);
    for(int i=0; i<256; i++) {
        if(codes[i]) free(codes[i]);
    }
    free(freq);
}