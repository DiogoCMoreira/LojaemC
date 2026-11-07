/*
 * Atividade Avaliativa 04 - Cadastro de Loja
 * Algoritmos de ordenacao (Bubble Sort) + manipulacao de arquivos
 *
 * O arquivo loja.txt guarda os dados como uma TABELA em texto,
 * exatamente no formato mostrado no enunciado:
 *
 * -----------------------------------------------
 * | ID | Produto | Categoria | Preco (R$) | ... |
 * -----------------------------------------------
 * | 1  | Camiseta Basica | Vestuario | 49.90 ... |
 */

#include <stdio.h>    // printf, fopen, fgets, fprintf, remove, rename
#include <stdlib.h>   // exit, strtol, strtof, atoi, atof
#include <string.h>   // strlen, strcpy, strncpy, strchr, strcspn, strtok, memmove
#include <ctype.h>    // tolower (comparar sem diferenciar maiuscula/minuscula)

#define ARQUIVO    "loja.txt"   // arquivo principal (tabela em texto)
#define TEMPORARIO "loja.tmp"   // arquivo temporario usado ao gravar
#define CSV        "loja.csv"   // arquivo exportado em formato CSV
#define TAM_NOME   50           // tamanho do campo produto (inclui o '\0')
#define TAM_CAT    30           // tamanho do campo categoria
#define TAM_FORN   30           // tamanho do campo fornecedor
#define MAX_LISTA  1000         // maximo de produtos mantidos em memoria
#define TAM_LINHA  512          // tamanho do buffer para ler uma linha do arquivo
#define W_ID       5            // largura fixa da coluna ID
#define W_PRECO    10           // largura fixa da coluna Preco
#define W_QTD      10           // largura fixa da coluna Quantidade

// Estrutura que representa um produto (uma linha da tabela)
typedef struct {                // inicio da definicao da struct
    int id;                     // identificador gerado automaticamente
    char nome[TAM_NOME];        // nome do produto
    char categoria[TAM_CAT];    // categoria do produto
    float preco;                // preco em reais
    int quantidade;             // quantidade em estoque
    char fornecedor[TAM_FORN];  // nome do fornecedor
} Produto;                      // nome do novo tipo: Produto

/* ===================== TEXTO E ENTRADA DE DADOS ===================== */

// Remove espacos do inicio e do fim de um texto e devolve o ponteiro do inicio
static char *aparar(char *s) {                      // recebe um ponteiro para o texto
    while (*s == ' ') s++;                          // avanca enquanto houver espaco no inicio
    char *fim = s + strlen(s);                      // aponta para o '\0' final
    while (fim > s && *(fim - 1) == ' ') fim--;     // recua sobre os espacos do fim
    *fim = '\0';                                    // corta o texto no ultimo caractere util
    return s;                                       // devolve o inicio ja sem espacos
}                                                   // fim de aparar

// Le um texto do teclado para 'dest' (ponteiro para vetor de char)
static void lerTexto(const char *msg, char *dest, int tam) {  // msg = pergunta, tam = tamanho do campo
    printf("%s", msg);                              // mostra a pergunta
    if (fgets(dest, tam, stdin) == NULL) {          // le no maximo tam-1 caracteres
        printf("\nEntrada encerrada.\n");           // fim da entrada (Ctrl+D / Ctrl+Z)
        exit(0);                                    // encerra o programa
    }                                               // fim do if
    char *nl = strchr(dest, '\n');                  // procura o Enter no texto lido
    if (nl) {                                       // se achou o Enter
        *nl = '\0';                                 // troca o Enter por fim de string
    } else {                                        // senao, o texto era maior que o campo
        int c;                                      // variavel para ler caractere a caractere
        while ((c = getchar()) != '\n' && c != EOF) { }  // descarta o que sobrou na linha
    }                                               // fim do if/else
    for (char *p = dest; *p; p++)                   // percorre o texto com um ponteiro
        if (*p == '|') *p = '/';                    // '|' e o separador do arquivo, entao troca
    char *ini = aparar(dest);                       // remove espacos das pontas
    if (ini != dest) memmove(dest, ini, strlen(ini) + 1);  // desloca o texto para o inicio
}                                                   // fim de lerTexto

// Le um numero inteiro, repetindo ate o usuario digitar um valor valido
static int lerInt(const char *msg) {                // msg = pergunta
    char buf[32], *fim;                             // buf guarda o texto, fim aponta onde a conversao parou
    long v;                                         // valor convertido
    while (1) {                                     // repete ate dar certo
        lerTexto(msg, buf, sizeof buf);             // le o texto digitado
        v = strtol(buf, &fim, 10);                  // converte para numero na base 10
        if (fim != buf && *fim == '\0') return (int)v;  // converteu tudo? entao e valido
        printf("Valor invalido. Tente de novo.\n"); // senao avisa e repete
    }                                               // fim do while
}                                                   // fim de lerInt

// Le um numero real nao negativo (aceita virgula ou ponto)
static float lerFloat(const char *msg) {            // msg = pergunta
    char buf[32], *fim;                             // buf guarda o texto, fim aponta onde a conversao parou
    float v;                                        // valor convertido
    while (1) {                                     // repete ate dar certo
        lerTexto(msg, buf, sizeof buf);             // le o texto digitado
        for (char *p = buf; *p; p++)                // percorre o texto com ponteiro
            if (*p == ',') *p = '.';                // troca virgula por ponto
        v = strtof(buf, &fim);                      // converte para float
        if (fim != buf && *fim == '\0' && v >= 0) return v;  // valido e nao negativo
        printf("Valor invalido. Tente de novo.\n"); // senao avisa e repete
    }                                               // fim do while
}                                                   // fim de lerFloat

/* ===================== COMPARACOES CASE-INSENSITIVE ===================== */

// Compara dois textos ignorando maiusculas/minusculas (retorno igual ao strcmp)
static int compararTexto(const char *a, const char *b) {  // a e b sao os textos
    while (*a && *b) {                              // enquanto nenhum dos dois acabou
        int ca = tolower((unsigned char)*a);        // letra de 'a' em minuscula
        int cb = tolower((unsigned char)*b);        // letra de 'b' em minuscula
        if (ca != cb) return ca - cb;               // achou diferenca: retorna a diferenca
        a++;                                        // avanca o ponteiro de 'a'
        b++;                                        // avanca o ponteiro de 'b'
    }                                               // fim do while
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);  // um acabou antes do outro
}                                                   // fim de compararTexto

// Retorna 1 se 'busca' aparece dentro de 'texto' (ignorando maiusculas/minusculas)
static int contemTexto(const char *texto, const char *busca) {  // texto = onde procurar
    size_t n = strlen(busca);                       // tamanho do texto procurado
    if (n == 0) return 1;                           // busca vazia casa com tudo
    for (; *texto; texto++) {                       // tenta casar a partir de cada posicao
        size_t i = 0;                               // quantas letras ja casaram
        while (i < n && texto[i] &&                 // enquanto houver letras para comparar
               tolower((unsigned char)texto[i]) == tolower((unsigned char)busca[i]))  // e forem iguais
            i++;                                    // letra igual: avanca
        if (i == n) return 1;                       // casou todas as letras
    }                                               // fim do for
    return 0;                                       // nao encontrou
}                                                   // fim de contemTexto

/* ===================== TABELA EM TEXTO (TELA E ARQUIVO) ===================== */

// Conta os caracteres visiveis de um texto (letra acentuada em UTF-8 conta 1)
static int comprimento(const char *s) {             // s = texto
    int n = 0;                                      // contador de caracteres
    for (; *s; s++)                                 // percorre o texto byte a byte
        if (((unsigned char)*s & 0xC0) != 0x80) n++;  // byte de continuacao UTF-8 nao conta
    return n;                                       // devolve a quantidade
}                                                   // fim de comprimento

// Imprime uma coluna de texto com largura fixa
static void coluna(FILE *f, const char *s, int largura) {  // f = tela ou arquivo
    fprintf(f, "| %s", s);                          // barra, espaco e o texto
    for (int i = comprimento(s); i < largura; i++)  // completa com espacos ate a largura
        fputc(' ', f);                              // imprime um espaco
    fputc(' ', f);                                  // espaco antes da proxima barra
}                                                   // fim de coluna

// Imprime uma linha de tracos com 'total' caracteres
static void tracejado(FILE *f, int total) {         // total = tamanho da linha
    for (int i = 0; i < total; i++)                 // repete 'total' vezes
        fputc('-', f);                              // imprime um traco
    fputc('\n', f);                                 // quebra de linha
}                                                   // fim de tracejado

// Imprime a tabela completa em 'f' (stdout para a tela, arquivo para gravar)
static void imprimirTabela(FILE *f, const Produto *v, int n) {  // v = vetor, n = quantidade
    int wNome = 7, wCat = 9, wForn = 10;            // larguras minimas = tamanho dos cabecalhos
    for (const Produto *p = v; p < v + n; p++) {    // percorre os produtos com ponteiro
        if (comprimento(p->nome) > wNome) wNome = comprimento(p->nome);  // ajusta largura do nome
        if (comprimento(p->categoria) > wCat) wCat = comprimento(p->categoria);  // ajusta a categoria
        if (comprimento(p->fornecedor) > wForn) wForn = comprimento(p->fornecedor);  // ajusta o fornecedor
    }                                               // fim do for
    int total = (W_ID + 2) + (wNome + 2) + (wCat + 2)  // largura total das colunas
              + (W_PRECO + 2) + (W_QTD + 2) + (wForn + 2) + 7;  // mais as 7 barras
    tracejado(f, total);                            // linha de tracos do topo
    fprintf(f, "| %-*s ", W_ID, "ID");              // cabecalho: coluna ID
    coluna(f, "Produto", wNome);                    // cabecalho: coluna Produto
    coluna(f, "Categoria", wCat);                   // cabecalho: coluna Categoria
    fprintf(f, "| %*s | %*s ", W_PRECO, "Preco (R$)", W_QTD, "Quantidade");  // cabecalho: preco e quantidade
    coluna(f, "Fornecedor", wForn);                 // cabecalho: coluna Fornecedor
    fputs("|\n", f);                                // fecha a linha do cabecalho
    tracejado(f, total);                            // linha de tracos abaixo do cabecalho
    for (const Produto *p = v; p < v + n; p++) {    // uma linha para cada produto
        fprintf(f, "| %-*d ", W_ID, p->id);         // coluna ID
        coluna(f, p->nome, wNome);                  // coluna Produto
        coluna(f, p->categoria, wCat);              // coluna Categoria
        fprintf(f, "| %*.2f | %*d ", W_PRECO, p->preco, W_QTD, p->quantidade);  // preco e quantidade
        coluna(f, p->fornecedor, wForn);            // coluna Fornecedor
        fputs("|\n", f);                            // fecha a linha do produto
    }                                               // fim do for
    tracejado(f, total);                            // linha de tracos do final
}                                                   // fim de imprimirTabela

/* ===================== LEITURA E GRAVACAO DO ARQUIVO ===================== */

// Converte uma linha da tabela em um Produto (retorna 1 se for linha de produto valida)
static int linhaParaProduto(const char *linha, Produto *p) {  // linha = texto lido, p = destino
    char copia[TAM_LINHA];                          // copia da linha (strtok modifica o texto)
    char *campo[6];                                 // ponteiros para os 6 campos
    int n = 0;                                      // quantos campos foram encontrados

    if (linha[0] != '|') return 0;                  // linhas de tracos nao sao produtos
    strncpy(copia, linha, TAM_LINHA - 1);           // copia a linha
    copia[TAM_LINHA - 1] = '\0';                    // garante o fim da string
    copia[strcspn(copia, "\r\n")] = '\0';           // remove a quebra de linha

    char *tok = strtok(copia, "|");                 // pega o primeiro campo (entre as barras)
    while (tok && n < 6) {                          // enquanto houver campo
        campo[n++] = aparar(tok);                   // guarda o campo sem espacos nas pontas
        tok = strtok(NULL, "|");                    // pega o proximo campo
    }                                               // fim do while
    if (n != 6) return 0;                           // linha incompleta: invalida

    p->id = atoi(campo[0]);                         // campo 0: ID
    if (p->id <= 0) return 0;                       // cabecalho ("ID") vira 0 e e ignorado
    strncpy(p->nome, campo[1], TAM_NOME - 1);       // campo 1: produto
    p->nome[TAM_NOME - 1] = '\0';                   // garante o fim da string
    strncpy(p->categoria, campo[2], TAM_CAT - 1);   // campo 2: categoria
    p->categoria[TAM_CAT - 1] = '\0';               // garante o fim da string
    p->preco = (float)atof(campo[3]);               // campo 3: preco
    p->quantidade = atoi(campo[4]);                 // campo 4: quantidade
    strncpy(p->fornecedor, campo[5], TAM_FORN - 1); // campo 5: fornecedor
    p->fornecedor[TAM_FORN - 1] = '\0';             // garante o fim da string
    return 1;                                       // linha lida com sucesso
}                                                   // fim de linhaParaProduto

// Carrega o arquivo para o vetor 'lista' e retorna quantos produtos leu
static int carregar(Produto *lista, int max) {      // lista = vetor de destino
    FILE *f = fopen(ARQUIVO, "r");                  // abre o arquivo para leitura
    char linha[TAM_LINHA];                          // buffer para cada linha
    int n = 0;                                      // quantidade de produtos lidos
    if (!f) return 0;                               // arquivo ainda nao existe
    while (n < max && fgets(linha, sizeof linha, f)) {  // le linha por linha
        if (linhaParaProduto(linha, lista + n))     // lista + n aponta para a posicao n do vetor
            n++;                                    // linha valida: conta o produto
    }                                               // fim do while
    fclose(f);                                      // fecha o arquivo
    return n;                                       // devolve a quantidade
}                                                   // fim de carregar

// Grava o vetor no arquivo: escreve em um arquivo TEMPORARIO e depois substitui o original
static int gravarTabela(const Produto *v, int n) {  // v = vetor, n = quantidade
    FILE *tmp = fopen(TEMPORARIO, "w");             // cria o arquivo temporario
    if (!tmp) {                                     // se nao conseguiu criar
        printf("Erro ao criar o arquivo temporario.\n");  // avisa o usuario
        return 0;                                   // indica falha
    }                                               // fim do if
    imprimirTabela(tmp, v, n);                      // escreve a tabela no temporario
    fclose(tmp);                                    // fecha o temporario
    remove(ARQUIVO);                                // apaga o original
    if (rename(TEMPORARIO, ARQUIVO) != 0) {         // o temporario vira o original
        printf("Erro ao substituir o arquivo.\n");  // avisa se falhar
        return 0;                                   // indica falha
    }                                               // fim do if
    return 1;                                       // indica sucesso
}                                                   // fim de gravarTabela

/* ===================== BUSCAS NO VETOR ===================== */

// Devolve um ponteiro para o produto com esse ID (ou NULL se nao existir)
static Produto *acharPorId(Produto *v, int n, int id) {  // v = vetor, n = quantidade
    for (Produto *p = v; p < v + n; p++)            // percorre o vetor com ponteiro
        if (p->id == id) return p;                  // achou: devolve o endereco
    return NULL;                                    // nao achou
}                                                   // fim de acharPorId

// Gera o proximo ID automatico (maior ID existente + 1)
static int proximoId(const Produto *v, int n) {     // v = vetor, n = quantidade
    int maior = 0;                                  // comeca em 0: o primeiro ID sera 1
    for (const Produto *p = v; p < v + n; p++)      // percorre o vetor com ponteiro
        if (p->id > maior) maior = p->id;           // guarda o maior ID visto
    return maior + 1;                               // proximo ID = maior + 1
}                                                   // fim de proximoId

// Retorna 1 se ja existe OUTRO produto com o mesmo nome (ignora o ID 'idIgnorar')
static int nomeExiste(const Produto *v, int n, const char *nome, int idIgnorar) {  // verifica duplicata
    for (const Produto *p = v; p < v + n; p++)      // percorre o vetor com ponteiro
        if (p->id != idIgnorar && compararTexto(p->nome, nome) == 0) return 1;  // mesmo nome
    return 0;                                       // nenhum nome igual
}                                                   // fim de nomeExiste

/* ===================== BUBBLE SORT ===================== */

// Troca o conteudo de dois produtos usando ponteiros
static void trocar(Produto *a, Produto *b) {        // a e b apontam para os produtos
    Produto tmp = *a;                               // guarda o primeiro
    *a = *b;                                        // copia o segundo para o primeiro
    *b = tmp;                                       // copia o guardado para o segundo
}                                                   // fim de trocar

// Decide se 'a' e 'b' estao fora de ordem segundo o criterio escolhido
static int deveTrocar(const Produto *a, const Produto *b, int criterio) {  // criterio de 1 a 4
    switch (criterio) {                             // escolhe o criterio
        case 1: return compararTexto(a->nome, b->nome) > 0;  // nome A-Z
        case 2: return compararTexto(a->nome, b->nome) < 0;  // nome Z-A
        case 3: return a->preco > b->preco;         // preco menor -> maior
        case 4: return a->preco < b->preco;         // preco maior -> menor
        default: return 0;                          // sem ordenacao
    }                                               // fim do switch
}                                                   // fim de deveTrocar

// Ordena o vetor 'v' com Bubble Sort (so em memoria, o arquivo nao muda)
static void bubbleSort(Produto *v, int n, int criterio) {  // v = vetor, n = quantidade
    for (int i = 0; i < n - 1; i++) {               // cada passada leva um item ao lugar certo
        for (int j = 0; j < n - 1 - i; j++) {       // compara pares vizinhos
            if (deveTrocar(v + j, v + j + 1, criterio))  // o par esta fora de ordem?
                trocar(v + j, v + j + 1);           // sim: troca os dois
        }                                           // fim do for interno
    }                                               // fim do for externo
}                                                   // fim de bubbleSort

/* ===================== OPERACOES DO CADASTRO ===================== */

// CRIAR: cria o arquivo de texto com os 4 produtos da tabela de exemplo
static void criarArquivo(void) {                    // opcao 1 do menu
    FILE *f = fopen(ARQUIVO, "r");                  // testa se o arquivo ja existe
    if (f) {                                        // se existe
        fclose(f);                                  // fecha o teste
        if (lerInt("Arquivo ja existe. Apagar tudo e recriar? (1=sim, 0=nao): ") != 1) {  // confirma
            printf("Cancelado.\n");                 // usuario desistiu
            return;                                 // volta ao menu
        }                                           // fim do if interno
    }                                               // fim do if
    static const Produto iniciais[] = {             // dados iniciais da tabela do enunciado
        {1, "Camiseta Basica",  "Vestuario",       49.90f, 30, "Moda Brasil"},   // produto 1
        {2, "Mouse Optico USB", "Informatica",     59.99f, 15, "TecnoShop"},     // produto 2
        {3, "Shampoo 300ml",    "Higiene Pessoal", 22.50f, 40, "Beleza Pura"},   // produto 3
        {4, "Cafe 500g",        "Alimentos",       18.00f, 25, "Graos do Sul"}   // produto 4
    };                                              // fim do vetor inicial
    if (gravarTabela(iniciais, 4))                  // grava os 4 produtos no arquivo
        printf("Arquivo %s criado com 4 produtos.\n", ARQUIVO);  // avisa o sucesso
}                                                   // fim de criarArquivo

// Le os dados de um produto; retorna 0 se algo for invalido ou o nome for duplicado
static int lerDados(Produto *p, const Produto *v, int n, int idIgnorar) {  // v/n = produtos ja cadastrados
    lerTexto("Produto: ", p->nome, TAM_NOME);       // le o nome
    if (p->nome[0] == '\0') {                       // nome vazio nao pode
        printf("Nome nao pode ser vazio.\n");       // avisa o usuario
        return 0;                                   // cancela
    }                                               // fim do if
    if (nomeExiste(v, n, p->nome, idIgnorar)) {     // verificacao de nome duplicado
        printf("Ja existe um produto com esse nome.\n");  // avisa o usuario
        return 0;                                   // cancela
    }                                               // fim do if
    lerTexto("Categoria: ", p->categoria, TAM_CAT); // le a categoria
    if (p->categoria[0] == '\0') strcpy(p->categoria, "-");  // categoria vazia vira "-"
    p->preco = lerFloat("Preco (R$): ");            // le o preco
    p->quantidade = lerInt("Quantidade: ");         // le a quantidade
    if (p->quantidade < 0) {                        // quantidade negativa nao pode
        printf("Quantidade invalida.\n");           // avisa o usuario
        return 0;                                   // cancela
    }                                               // fim do if
    lerTexto("Fornecedor: ", p->fornecedor, TAM_FORN);  // le o fornecedor
    if (p->fornecedor[0] == '\0') strcpy(p->fornecedor, "-");  // fornecedor vazio vira "-"
    return 1;                                       // dados validos
}                                                   // fim de lerDados

// INSERIR: adiciona um produto novo
static void inserirProduto(void) {                  // opcao 2 do menu
    static Produto lista[MAX_LISTA];                // vetor de estruturas com os produtos
    int n = carregar(lista, MAX_LISTA);             // carrega o arquivo para o vetor
    if (n >= MAX_LISTA) {                           // vetor cheio?
        printf("Limite de produtos atingido.\n");   // avisa o usuario
        return;                                     // volta ao menu
    }                                               // fim do if
    Produto novo;                                   // produto que sera cadastrado
    novo.id = proximoId(lista, n);                  // ID automatico incremental
    if (acharPorId(lista, n, novo.id)) {            // verificacao de ID duplicado
        printf("ID %d ja existe. Insercao cancelada.\n", novo.id);  // avisa o usuario
        return;                                     // cancela
    }                                               // fim do if
    printf("Novo produto (ID automatico: %d)\n", novo.id);  // mostra o ID gerado
    if (!lerDados(&novo, lista, n, 0)) return;      // le os dados (0 = nenhum ID a ignorar)
    lista[n] = novo;                                // coloca o produto no fim do vetor
    n++;                                            // agora ha um produto a mais
    if (gravarTabela(lista, n))                     // grava o vetor no arquivo
        printf("Produto inserido com sucesso!\n");  // avisa o sucesso
}                                                   // fim de inserirProduto

// ALTERAR (excluir = 0) ou EXCLUIR (excluir = 1); grava usando arquivo temporario
static void alterarOuExcluir(int excluir) {         // opcoes 4 e 5 do menu
    static Produto lista[MAX_LISTA];                // vetor de estruturas com os produtos
    int n = carregar(lista, MAX_LISTA);             // carrega o arquivo para o vetor
    int id = lerInt("ID do produto: ");             // pergunta qual produto
    Produto *alvo = acharPorId(lista, n, id);       // ponteiro para o produto escolhido
    if (!alvo) {                                    // NULL = nao existe
        printf("Produto nao encontrado.\n");        // avisa o usuario
        return;                                     // volta ao menu
    }                                               // fim do if
    if (excluir) {                                  // caso EXCLUIR
        for (Produto *p = alvo; p < lista + n - 1; p++)  // do alvo ate o penultimo
            *p = *(p + 1);                          // puxa o proximo produto para a posicao atual
        n--;                                        // agora ha um produto a menos
    } else {                                        // caso ALTERAR
        Produto novo;                               // produto com os dados novos
        novo.id = id;                               // o ID nao muda
        printf("Digite os novos dados:\n");         // orienta o usuario
        if (!lerDados(&novo, lista, n, id)) return; // le os dados (ignora o proprio ID no nome)
        *alvo = novo;                               // substitui o produto antigo pelo novo
    }                                               // fim do if/else
    if (gravarTabela(lista, n))                     // grava via arquivo temporario
        printf(excluir ? "Produto excluido!\n" : "Produto alterado!\n");  // avisa o sucesso
}                                                   // fim de alterarOuExcluir

// BUSCAR por nome ou por categoria (aceita so um pedaco do texto)
static void buscarProduto(void) {                   // opcao 6 do menu
    static Produto lista[MAX_LISTA], res[MAX_LISTA];  // todos os produtos e os resultados
    int n = carregar(lista, MAX_LISTA);             // carrega o arquivo para o vetor
    int r = 0;                                      // quantidade de resultados
    char termo[TAM_NOME];                           // texto procurado
    int op = lerInt("Buscar por: 1) Nome  2) Categoria: ");  // escolhe o tipo de busca
    if (op != 1 && op != 2) {                       // opcao invalida?
        printf("Opcao invalida.\n");                // avisa o usuario
        return;                                     // volta ao menu
    }                                               // fim do if
    lerTexto("Texto procurado: ", termo, TAM_NOME); // le o texto procurado
    for (Produto *p = lista; p < lista + n; p++) {  // percorre todos os produtos
        const char *campo = (op == 1) ? p->nome : p->categoria;  // campo onde procurar
        if (contemTexto(campo, termo))              // o campo contem o texto?
            res[r++] = *p;                          // sim: copia para os resultados
    }                                               // fim do for
    if (r == 0) {                                   // nenhum resultado?
        printf("Nenhum produto encontrado.\n");     // avisa o usuario
    } else {                                        // ha resultados
        imprimirTabela(stdout, res, r);             // mostra a tabela na tela
        printf("Total: %d produto(s)\n", r);        // mostra a quantidade
    }                                               // fim do if/else
}                                                   // fim de buscarProduto

// LISTAR todos os produtos, com a ordenacao escolhida (nao altera o arquivo)
static void listarProdutos(void) {                  // opcao 3 do menu
    static Produto lista[MAX_LISTA];                // vetor de estruturas com os produtos
    int n = carregar(lista, MAX_LISTA);             // carrega o arquivo para o vetor
    if (n == 0) {                                   // nada cadastrado?
        printf("Nenhum produto. Use a opcao 1 para criar o arquivo.\n");  // orienta o usuario
        return;                                     // volta ao menu
    }                                               // fim do if
    printf("Ordenar por:\n");                       // titulo do submenu
    printf(" 1) Nome (A-Z)\n 2) Nome (Z-A)\n");     // opcoes de nome
    printf(" 3) Preco (menor -> maior)\n 4) Preco (maior -> menor)\n");  // opcoes de preco
    printf(" 0) Ordem de cadastro\n");              // sem ordenar
    int crit = lerInt("Escolha: ");                 // le o criterio
    if (crit < 0 || crit > 4) crit = 0;             // opcao invalida: nao ordena
    bubbleSort(lista, n, crit);                     // ordena so o vetor em memoria
    imprimirTabela(stdout, lista, n);               // mostra a tabela na tela
    printf("Total: %d produto(s)\n", n);            // mostra a quantidade
}                                                   // fim de listarProdutos

// Escreve um texto entre aspas no CSV (duplica as aspas internas)
static void campoCSV(FILE *f, const char *s) {      // f = arquivo CSV
    fputc('"', f);                                  // aspas de abertura
    for (; *s; s++) {                               // percorre o texto
        if (*s == '"') fputc('"', f);               // aspas internas sao duplicadas
        fputc(*s, f);                               // escreve o caractere
    }                                               // fim do for
    fputc('"', f);                                  // aspas de fechamento
}                                                   // fim de campoCSV

// EXPORTAR os dados para CSV separado por virgula
static void exportarCSV(void) {                     // opcao 7 do menu
    static Produto lista[MAX_LISTA];                // vetor de estruturas com os produtos
    int n = carregar(lista, MAX_LISTA);             // carrega o arquivo para o vetor
    if (n == 0) {                                   // nada para exportar?
        printf("Nada para exportar.\n");            // avisa o usuario
        return;                                     // volta ao menu
    }                                               // fim do if
    FILE *f = fopen(CSV, "w");                      // cria o arquivo CSV
    if (!f) {                                       // falhou ao criar?
        printf("Erro ao criar o CSV.\n");           // avisa o usuario
        return;                                     // volta ao menu
    }                                               // fim do if
    fprintf(f, "id,produto,categoria,preco,quantidade,fornecedor\n");  // linha de cabecalho
    for (const Produto *p = lista; p < lista + n; p++) {  // uma linha para cada produto
        fprintf(f, "%d,", p->id);                   // coluna id
        campoCSV(f, p->nome);      fputc(',', f);   // coluna produto
        campoCSV(f, p->categoria); fputc(',', f);   // coluna categoria
        fprintf(f, "%.2f,%d,", p->preco, p->quantidade);  // colunas preco e quantidade
        campoCSV(f, p->fornecedor);                 // coluna fornecedor
        fputc('\n', f);                             // fim da linha
    }                                               // fim do for
    fclose(f);                                      // fecha o CSV
    printf("Exportado para %s (%d produtos).\n", CSV, n);  // avisa o sucesso
}                                                   // fim de exportarCSV

/* ===================== PROGRAMA PRINCIPAL ===================== */

int main(void) {                                    // ponto de entrada do programa
    int opcao;                                      // opcao escolhida no menu
    do {                                            // repete o menu ate escolher sair
        printf("\n===== CADASTRO DE LOJA =====\n"); // titulo do menu
        printf("1. Criar arquivo (dados iniciais)\n");  // opcao 1
        printf("2. Inserir produto\n");             // opcao 2
        printf("3. Listar produtos\n");             // opcao 3
        printf("4. Alterar produto\n");             // opcao 4
        printf("5. Excluir produto\n");             // opcao 5
        printf("6. Buscar por nome ou categoria\n");  // opcao 6
        printf("7. Exportar para CSV\n");           // opcao 7
        printf("0. Sair\n");                        // opcao 0
        opcao = lerInt("Escolha: ");                // le a opcao

        switch (opcao) {                            // executa a opcao escolhida
            case 1: criarArquivo();       break;    // criar o arquivo
            case 2: inserirProduto();     break;    // inserir
            case 3: listarProdutos();     break;    // listar
            case 4: alterarOuExcluir(0);  break;    // alterar
            case 5: alterarOuExcluir(1);  break;    // excluir
            case 6: buscarProduto();      break;    // buscar
            case 7: exportarCSV();        break;    // exportar CSV
            case 0: printf("Ate mais!\n"); break;   // despedida
            default: printf("Opcao invalida.\n");   // qualquer outra opcao
        }                                           // fim do switch
    } while (opcao != 0);                           // volta ao menu se nao for 0

    return 0;                                       // fim do programa sem erro
}                                                   // fim de main
