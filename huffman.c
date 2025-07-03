#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>
#include <stdint.h>

#include "huffman.h"
#include "colors.h"

#define MAX_NODES 256

// verifica se o caminho fornecido é um diretorio
// retorna 1 se for diretorio, 0 caso contrario ou erro
int isDirectory(const char* path) {
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    return S_ISDIR(st.st_mode);
}

// libera recursivamente a memoria da arvore de huffman
// percorre os nós em pós ordem, liberando todos os nós filhos antes do nó atual
void freeTree(Node* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

// cria diretorios ausentes para um caminho de arquivo
void createDirsForFile(const char* filePath) {
    // faz uma copia do caminho porque-
    //dirname pode modificar a string original
    char pathCopy[1024];
    strncpy(pathCopy, filePath, sizeof(pathCopy));

    // extrai apenas a parte do diretório
    char* dirPath = dirname(pathCopy);

    // 'temp' para construir o caminho incrementalmente
    char temp[1024];
    strcpy(temp, "");

    // divide o caminho por barras "/" e cria diretorios um a um
    char* token = strtok(dirPath, "/");
    while (token != NULL) {
        strcat(temp, token);
        strcat(temp, "/");

        // se o diretorio ainda nao existe,
        //cria com permissoes 0777
        if (access(temp, F_OK) != 0) {
            mkdir(temp, 0777);
        }

        token = strtok(NULL, "/");
    }
}

// terminal candy: simple progress bar
void showProgressBar(int percent) {
    int barWidth = 50;
    int pos = (percent * barWidth) / 100;

    printf("\r["); // \r volta pro começo da linha
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos)
            printf("█");
        else
            printf(" ");
    }
    printf("] %d%%", percent);
    fflush(stdout); // força o terminal a atualizar a linha
}

// le um arquivo e conta a frequência de cada byte
int* CountFrequency(const char fileName[]){
    // aloca memoria para 256 inteiros e inicializa com zero
    int *frequency = calloc(256, sizeof(int));

    if (!frequency) {
        fprintf(stderr, RED "Erro ao alocar memória para frequências: %s\n" RESET, strerror(errno));
        return NULL;
    }

    // abre o arquivo em modo binário de leitura
    FILE *file = fopen(fileName, "rb");
    if (file == NULL) {
        printf(RED "Erro ao abrir arquivo" RESET);
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
        if(depth == 0) {
            // arvore com só um nó, código "0"
            path[depth] = '0';
            path[depth + 1] = '\0';
        } else {
            path[depth] = '\0'; //termina a string normalmente
        }
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
void compress(const char* filePath, const char* outputPath) {
    // abre arquivos para leitura e escrita
    // conta frequencia
    FILE *file = fopen(filePath, "rb");
    if(!file){
        fprintf(stderr, RED "Erro ao abrir arquivo: %s\n" RESET, strerror(errno));
        return;
    }

    fseek(file, 0, SEEK_END); // vai ate o fim
    long fileSize = ftell(file); // pega o tamanho
    rewind(file); // volta pro começo

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
    FILE* output = fopen(outputPath, "wb");
    if(!output){
        fprintf(stderr, RED "Erro ao abrir arquivo para escrita %s\n" RESET, strerror(errno));
        return;
    }

    // salva metadados
    int uniqueCount = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) uniqueCount++;
    }

    // grava no arquivo a quantidade de caracteres unicos com frequência > 0
    fwrite(&uniqueCount, sizeof(int), 1, output);

    // para cada caractere que ocorre no arquivo original, grava o caractere e sua frequência
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            unsigned char ch = (unsigned char)i;
            fwrite(&ch, sizeof(unsigned char), 1, output); // grava o caractere
            fwrite(&freq[i], sizeof(int), 1, output); //grava a frequência correspondente
        }
    }

    // le arquivo original, escreve códigos em bits no arquivo destino
    unsigned char buffer = 0;
    int bitCount = 0;
    long processed = 0;
    int lastPercent = -1;

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

        processed++;
        int percent = (processed * 100) / fileSize;
        if (percent != lastPercent) {
            showProgressBar(percent);
            lastPercent = percent;
        }
    }
    printf("\n");

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

// descompacta o arquivo comprimido com huffman e grava o resultado no arquivo de saída
void decompress(const char* filePath, const char* outputPath) {

    // abre o arquivo compactado para leitura em modo binario
    FILE* file = fopen(filePath, "rb");
    if(!file){
        fprintf(stderr, RED "Erro ao abrir arquivo compactado %s\n" RESET, strerror(errno));
        return;
    }

    // pega tamanho do arquivo .huff
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);


    // abre o arquivo de saída para escrita em modo binario
    FILE* output = fopen(outputPath, "wb");
    if(!output){
        fprintf(stderr, RED "Erro ao criar arquivo de saída %s\n" RESET, strerror(errno));
        fclose(file);
        return;
    }

    // le o cabeçalho: as frequências dos 256 bytes (metadados salvos na compressao)
    int freq[256] = {0};
    int uniqueCount = 0;

    // le quantos caracteres unicos foram salvos no cabeçalho
    if (fread(&uniqueCount, sizeof(int), 1, file) != 1) {
        fprintf(stderr, RED "Erro ao ler uniqueCount %s\n" RESET, strerror(errno));
        fclose(file);
        fclose(output);
        return;
    }

    // le cada caractere e sua frequência
    for (int i = 0; i < uniqueCount; i++) {
        unsigned char ch;
        int f;
        fread(&ch, sizeof(unsigned char), 1, file);
        fread(&f, sizeof(int), 1, file);
        freq[ch] = f;
    }

    // reconstrói a árvore de huffman usando as frequências lidas
    Node* nodeList[256];
    int count = generateNodeList(freq, nodeList);
    qsort(nodeList, count, sizeof(Node*), compareNode);
    Node* root = buildHuffmanTree(nodeList, count);

    if (!root) {
        printf(RED "Erro: arvore de huffman vazia.\n" RESET);
        fclose(file);
        fclose(output);
        return;
    }

    // percorre os bits do arquivo compactado e reconstrói o conteudo original
    Node* current = root;
    long processed = 0;
    int lastPercent = -1;

    int byte;
    while ((byte = fgetc(file)) != EOF) {
        for (int i = 7; i >= 0; i--) {
            int bit = (byte >> i) & 1; // extrai bit da esquerda para direita

            // navega pela arvore de acordo com o bit
            current = (bit == 0) ? current->left : current->right;

            // se chegou a um caractere (folha), escreve no arquivo de saída
            if (current->left == NULL && current->right == NULL) {
                //printf("Caractere reconstruído: %02X\n", current->character);
                fputc(current->character, output);
                current = root; // volta para o início da arvore
            }
        }

        // barra de progresso
        processed++;
        int percent = (processed * 100) / fileSize;
        if (percent != lastPercent) {
            showProgressBar(percent);
            lastPercent = percent;
        }
    }
    printf("\n");

    // fecha os arquivos e libera a memória usada
    fclose(file);
    fclose(output);
}

// compacta um unico arquivo e grava no .huff com caminho relativo e metadados
void compressSingleFileToStream(const char* filePath, const char* relativePath, FILE* output) {
    FILE* file = fopen(filePath, "rb");
    if (!file) {
        fprintf(stderr, "Erro ao abrir arquivo %s\n", filePath);
        return;
    }

    // calcula frequencia dos bytes
    int* freq = CountFrequency(filePath);
    if (!freq) {
        fclose(file);
        return;
    }

    // cria arvore de huffman e gera os codigos
    Node* nodeList[256];
    int count = generateNodeList(freq, nodeList);
    qsort(nodeList, count, sizeof(Node*), compareNode);
    Node* root = buildHuffmanTree(nodeList, count);

    char* codes[256] = {0};
    char path[256];
    generateCodes(root, path, 0, codes);

    // escreve caminho relativo
    uint16_t pathLen = (uint16_t)strlen(relativePath);
    fwrite(&pathLen, sizeof(uint16_t), 1, output);
    fwrite(relativePath, 1, pathLen, output);

    // escreve tamanho original do arquivo
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);
    uint32_t fileSize32 = (uint32_t)fileSize;
    fwrite(&fileSize32, sizeof(uint32_t), 1, output);

    // escreve tabela de frequência
    fwrite(freq, sizeof(int), 256, output);

    // codifica os dados e grava bit a bit
    unsigned char buffer = 0;
    int bitCount = 0;
    int c;

    // codifica os dados byte a byte usando huffman e grava os bits compactados no arquivo
    while ((c = fgetc(file)) != EOF) {
        const char* code = codes[c];
        for (int i = 0; code[i] != '\0'; i++) {
            buffer <<= 1;
            if (code[i] == '1') buffer |= 1;
            bitCount++;

            // quando 8 bits sao preenchidos,
            //grava o byte no arquivo
            if (bitCount == 8) {
                fwrite(&buffer, 1, 1, output);
                buffer = 0;
                bitCount = 0;
            }
        }
    }

    // Se restaram bits nao gravados,
    // preenche com zeros a direita e grava o ultimo byte
    if (bitCount > 0) {
        buffer <<= (8 - bitCount);
        fwrite(&buffer, 1, 1, output);
    }

    // libera memoria
    fclose(file);
    free(freq);
    for (int i = 0; i < 256; i++) {
        if (codes[i]) free(codes[i]);
    }
    freeTree(root);
}

// percorre a pasta recursivamente e compacta todos os arquivos encontrados
void walkAndCompress(const char* basePath, const char* currentPath, FILE* output) {
    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", basePath, currentPath);

    DIR* dir = opendir(fullPath);
    if (!dir) {
        fprintf(stderr, "Erro ao abrir diretório: %s\n", fullPath);
        return;
    }
}

// compacta uma pasta inteira em um único arquivo .huff
void compressFolder(const char* folderPath, const char* outputHuff) {}

// descompacta arquivos de um .huff, recriando pastas e arquivos no destino
void decompressFolderFromHuff(const char* huffPath, const char* outputDir) {}