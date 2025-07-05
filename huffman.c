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

    unsigned char buffer = 0;
    int bitCount = 0;
    int c;
    long bytesRead = 0;
    int lastPercent = -1;

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
        bytesRead++;

        // atualiza barra de progresso a cada 1% lido
        int percent = (int)((bytesRead * 100) / fileSize32);
        if (percent != lastPercent) {
            showProgressBar(percent);
            lastPercent = percent;
        }
    }
    printf("\n");


    // se restaram bits nao gravados,
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

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        // constroi caminho relativo
        char newCurrentPath[1024];
        if (strlen(currentPath) > 0)
            snprintf(newCurrentPath, sizeof(newCurrentPath), "%s/%s", currentPath, entry->d_name);
        else
            snprintf(newCurrentPath, sizeof(newCurrentPath), "%s", entry->d_name);


        // obtem informacoes do caminho completo
        char fullEntryPath[2048];
        snprintf(fullEntryPath, sizeof(fullEntryPath), "%s/%s", basePath, newCurrentPath);

        struct stat pathStat;
        if (stat(fullEntryPath, &pathStat) == -1) {
            perror("stat");
            continue;
        }

        // se for diretorio, continua recursao; se for arquivo regular, comprime
        if (S_ISDIR(pathStat.st_mode)) {
            walkAndCompress(basePath, newCurrentPath, output); // recursao
        } else if (S_ISREG(pathStat.st_mode)) {
            compressSingleFileToStream(fullEntryPath, newCurrentPath, output);
        }
    }

    closedir(dir);
}

// descompacta arquivos de um .huff, recriando pastas e arquivos no destino
void decompressFolderFromHuff(const char* huffPath, char* outputDir) {
    FILE* input = fopen(huffPath, "rb");
    if (!input) {
        fprintf(stderr, "Erro ao abrir arquivo .huff\n");
        return;
    }

    int isFirst = 1;
    int isSingleFile = 0;
    char relativePath[1024];

    // extrai nome base para outputDir
    strncpy(outputDir, huffPath, 1024);
    outputDir[1023] = '\0';
    char* dot = strrchr(outputDir, '.');
    if (dot && strcmp(dot, ".adr") == 0) *dot = '\0';

    // detecta se é um arquivo único ou pasta com base no nome .adr
    const char* baseName = strrchr(huffPath, '/');
    if (!baseName) baseName = huffPath; else baseName++; // pula a barra

    // verifica se tem mais de uma extensão antes do .adr → ex: arquivo.txt.adr
    char temp[1024];
    strncpy(temp, baseName, sizeof(temp));
    temp[sizeof(temp) - 1] = '\0';
    char* lastDot = strrchr(temp, '.');
    if (lastDot && strcmp(lastDot, ".adr") == 0) {
        *lastDot = '\0'; // remove .adr
        isSingleFile = strchr(temp, '.') != NULL; // tem outro ponto antes = é arquivo
    }

    // cria pasta só se for múltiplos arquivos/subpastas
    if (!isSingleFile) {
        mkdir(outputDir, 0755);
    }

    while (!feof(input)) {
        // le tamanho do caminho relativo
        uint16_t pathLen;
        if (fread(&pathLen, sizeof(uint16_t), 1, input) != 1) break;

        // le caminho relativo
        memset(relativePath, 0, sizeof(relativePath));
        fread(relativePath, 1, pathLen, input);

        // le tamanho original do arquivo
        uint32_t originalSize;
        fread(&originalSize, sizeof(uint32_t), 1, input);

        // le tabela de frequência
        int freq[256];
        fread(freq, sizeof(int), 256, input);

        // gera arvore de huffman a partir da frequência
        Node* nodeList[256];
        int count = generateNodeList(freq, nodeList);
        qsort(nodeList, count, sizeof(Node*), compareNode);
        Node* root = buildHuffmanTree(nodeList, count);

        // monta caminho completo para o arquivo a ser criado
        char fullPath[2048];
        if (isSingleFile) {
            snprintf(fullPath, sizeof(fullPath), "%s", outputDir); // salva direto no outputDir
        } else {
            snprintf(fullPath, sizeof(fullPath), "%s/%s", outputDir, relativePath); // pasta + subcaminho
        }

        // cria diretorios necessários para o caminho do arquivo
        createDirsForFile(fullPath);

        // abre o arquivo para escrita
        FILE* outFile = fopen(fullPath, "wb");
        if (!outFile) {
            fprintf(stderr, "Erro ao criar arquivo %s\n", fullPath);
            freeTree(root);
            break;
        }

        // decodifica os bits do .huff-
        //ate alcançar o tamanho original
        Node* current = root;
        unsigned char byte;
        int decodedBytes = 0;
        int lastPercent = -1;

        // percorre os bits do arquivo ate alcançar o tamanho original
        while (decodedBytes < originalSize && fread(&byte, 1, 1, input) == 1) {
            for (int i = 7; i >= 0; i--) {
                int bit = (byte >> i) & 1;
                current = bit ? current->right : current->left;

                // se chegou a uma folha, escreve no arquivo
                if (!current->left && !current->right) {
                    fputc(current->character, outFile);
                    current = root;
                    decodedBytes++;

                    // atualiza barra de progresso a cada 1% decodificado
                    int percent = (int)((decodedBytes * 100) / originalSize);
                    if (percent != lastPercent) {
                        showProgressBar(percent);
                        lastPercent = percent;
                    }

                    if (decodedBytes == originalSize) break;
                }
            }
        }
        printf("\n");

        // fecha o arquivo de saida e libera a arvore de huffman
        fclose(outFile);
        freeTree(root);
    }
    fclose(input);
}

// verifica se o caminho é um arquivo regular
int is_file(const char* path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) return 0; // erro ao acessar
    return S_ISREG(path_stat.st_mode); // arquivo regular
}

// verifica se o caminho é um diretorio
int is_folder(const char* path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) return 0; // erro ao acessar
    return S_ISDIR(path_stat.st_mode); // diretório
}

// compacta uma pasta inteira em um único arquivo .huff
void compressEntry(const char* inputPath) {
    char outputPath[1024];

    // gera nome do arquivo .adr automaticamente
    snprintf(outputPath, sizeof(outputPath), "%s.adr", inputPath);

    FILE* output = fopen(outputPath, "wb");
    if (!output) {
        fprintf(stderr, "Erro ao criar arquivo de saída: %s\n", outputPath);
        return;
    }

    printf("Compactando para: %s\n", outputPath);

    if (is_file(inputPath)) {
        // para arquivo simples
        const char* relativeName = strrchr(inputPath, '/');
        if (!relativeName) relativeName = inputPath;
        else relativeName++; // pula o '/'
        compressSingleFileToStream(inputPath, relativeName, output);
    } else if (is_folder(inputPath)) {
        // para pasta, faz compressao recursiva
        walkAndCompress(inputPath, "", output);
    } else {
        fprintf(stderr, "Erro: caminho inválido para compressão: %s\n", inputPath);
    }
    fclose(output);
}

// descompacta um arquivo .adr para sua pasta original
void decompressEntry(const char* inputPath) {
    if (!is_file(inputPath)) {
        printf("Erro: %s não é um arquivo válido\n", inputPath);
        return;
    }

    // checa se termina com .adr
    const char* ext = strrchr(inputPath, '.');
    if (ext && strcmp(ext, ".adr") == 0) {
        char outputDir[1024];
        decompressFolderFromHuff(inputPath, outputDir);
    } else {
        printf(RED "Erro: formato de arquivo não suportado para descompactação: %s\n" RESET, inputPath);
    }
}