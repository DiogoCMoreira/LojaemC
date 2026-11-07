/*
 * Atividade Avaliativa 04 - Cadastro de Loja
 * Algoritmos de ordenacao (Bubble Sort) + manipulacao de arquivos
 * Os dados ficam em um arquivo de TEXTO (loja.txt), um produto por linha:
 *     id;produto;categoria;preco;quantidade;fornecedor
 */

#include <stdio.h>    // printf, fopen, fgets, fprintf, remove, rename
#include <stdlib.h>   // exit, strtol, strtof, atoi, atof
#include <string.h>   // strlen, strcpy, strncpy, strchr, strcspn, strtok
#include <ctype.h>    // tolower (comparacao sem diferenciar maiuscula/minuscula)

#define ARQUIVO    "loja.txt"   // arquivo principal com os dados
#define TEMPORARIO "loja.tmp"   // arquivo temporario usado ao alterar/excluir
#define CSV        "loja.csv"   // arquivo exportado em formato CSV
#define TAM_NOME   30           // tamanho maximo do campo produto (com o '\0')
#define TAM_CAT    20           // tamanho maximo do campo categoria
#define TAM_FORN   25           // tamanho maximo do campo fornecedor
#define MAX_LISTA  1000         // quantidade maxima de produtos em memoria
#define TAM_LINHA  256          // tamanho do buffer para ler uma linha do arquivo

#define W_ID    5               // largura das colunas da tabela exibida na tela
#define W_NOME  (TAM_NOME - 1)
#define W_CAT   (TAM_CAT - 1)
#define W_PRECO 10
#define W_QTD   5
#define W_FORN  (TAM_FORN - 1)

// Estrutura que representa um produto (uma linha da tabela)
typedef struct {
    int id;                     // identificador gerado automaticamente
    char nome[TAM_NOME];        // nome do produto
    char categoria[TAM_CAT];    // categoria do produto
    float preco;                // preco em reais
    int quantidade;             // quantidade em estoque
    char fornecedor[TAM_FORN];  // nome do fornecedor
} Produto;

/* ===================== ENTRADA DE DADOS ===================== */

// Le um texto do teclado para 'dest' (ponteiro para o vetor de char)
static void lerTexto(const char *msg, char *dest, int tam) {
    printf("%s", msg);                              // mostra a pergunta
    if (fgets(dest, tam, stdin) == NULL) {          // le ate tam-1 caracteres
        printf("\nEntrada encerrada.\n");           // fim da entrada (Ctrl+D / Ctrl+Z)
        exit(0);                                    // encerra o programa
    }
    char *nl = strchr(dest, '\n');                  // procura o Enter no texto lido
    if (nl) {
        *nl = '\0';                                 // troca o Enter por fim de string
    } else {
        int c;                                      // texto maior que o campo:
        while ((c = getchar()) != '\n' && c != EOF) { }  // descarta o que sobrou
    }
    for (char *p = dest; *p; p++)                   // percorre o texto com ponteiro
        if (*p == ';') *p = ',';                    // ';' e o separador do arquivo, entao troca
}

// Le um numero inteiro, repetindo ate o usuario digitar um valor valido
static int lerInt(const char *msg) {
    char buf[32], *fim;                             // buf guarda o texto, fim aponta onde a conversao parou
    long v;
    while (1) {
        lerTexto(msg, buf, sizeof buf);             // le o texto digitado
        v = strtol(buf, &fim, 10);                  // converte para numero (base 10)
        if (fim != buf && *fim == '\0') return (int)v;  // converteu tudo? entao e valido
        printf("Valor invalido. Tente de novo.\n"); // senao pede de novo
    }
}

// Le um numero real nao negativo (aceita virgula ou ponto)
static float lerFloat(const char *msg) {
    char buf[32], *fim;
    float v;
    while (1) {
        lerTexto(msg, buf, sizeof buf);             // le o texto digitado
        for (char *p = buf; *p; p++)                // troca virgula por ponto
            if (*p == ',') *p = '.';
        v = strtof(buf, &fim);                      // converte para float
        if (fim != buf && *fim == '\0' && v >= 0) return v;  // valido e nao negativo
        printf("Valor invalido. Tente de novo.\n");
    }
}

/* ===================== COMPARACOES CASE-INSENSITIVE ===================== */

// Compara dois textos ignorando maiusculas/minusculas (retorno como strcmp)
static int compararTexto(const char *a, const char *b) {
    while (*a && *b) {                              // enquanto nenhum acabou
        int ca = tolower((unsigned char)*a);        // letra de 'a' em minuscula
        int cb = tolower((unsigned char)*b);        // letra de 'b' em minuscula
        if (ca != cb) return ca - cb;               // achou diferenca: retorna
        a++;                                        // avanca o ponteiro de 'a'
        b++;                                        // avanca o ponteiro de 'b'
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);  // um acabou antes
}

// Retorna 1 se 'busca' aparece dentro de 'texto' (ignorando maiusculas/minusculas)
static int contemTexto(const char *texto, const char *busca) {
    size_t n = strlen(busca);                       // tamanho do texto procurado
    if (n == 0) return 1;                           // busca vazia casa com tudo
    for (; *texto; texto++) {                       // tenta casar a partir de cada posicao
        size_t i = 0;                               // quantas letras ja casaram
        while (i < n && texto[i] &&
               tolower((unsigned char)texto[i]) == tolower((unsigned char)busca[i]))
            i++;                                    // letra igual: avanca
        if (i == n) return 1;                       // casou todas as letras
    }
    return 0;                                       // nao encontrou
}

/* ===================== LEITURA E GRAVACAO NO ARQUIVO ===================== */

// Converte uma linha do arquivo em um Produto (retorna 1 se deu certo)
static int linhaParaProduto(const char *linha, Produto *p) {
    char copia[TAM_LINHA];                          // copia para o strtok poder modificar
    char *campo[6];                                 // ponteiros para os 6 campos
    int n = 0;                                      // quantos campos foram achados

    strncpy(copia, linha, TAM_LINHA - 1);           // copia a linha
    copia[TAM_LINHA - 1] = '\0';                    // garante o fim da string
    copia[strcspn(copia, "\r\n")] = '\0';           // remove quebra de linha

    char *tok = strtok(copia, ";");                 // pega o primeiro campo
    while (tok && n < 6) {                          // enquanto houver campo
        campo[n++] = tok;                           // guarda o ponteiro do campo
        tok = strtok(NULL, ";");                    // pega o proximo campo
    }
    if (n != 6) return 0;                           // linha incompleta: invalida

    p->id = atoi(campo[0]);                         // campo 0: ID
    if (p->id <= 0) return 0;                       // ID invalido
    strncpy(p->nome, campo[1], TAM_NOME - 1);       // campo 1: produto
    p->nome[TAM_NOME - 1] = '\0';
    strncpy(p->categoria, campo[2], TAM_CAT - 1);   // campo 2: categoria
    p->categoria[TAM_CAT - 1] = '\0';
    p->preco = (float)atof(campo[3]);               // campo 3: preco
    p->quantidade = atoi(campo[4]);                 // campo 4: quantidade
    strncpy(p->fornecedor, campo[5], TAM_FORN - 1); // campo 5: fornecedor
    p->fornecedor[TAM_FORN - 1] = '\0';
    return 1;                                       // linha lida com sucesso
}

// Grava um Produto no arquivo 'f' como uma linha de texto
static void salvarProduto(FILE *f, const Produto *p) {
    fprintf(f, "%d;%s;%s;%.2f;%d;%s\n",             // mesmo formato lido acima
            p->id, p->nome, p->categoria, p->preco, p->quantidade, p->fornecedor);
}

// Carrega o arquivo inteiro para o vetor 'lista' e retorna quantos produtos leu
static int carregar(Produto *lista, int max) {
    FILE *f = fopen(ARQUIVO, "r");                  // abre para leitura
    char linha[TAM_LINHA];
    int n = 0;
    if (!f) return 0;                               // arquivo nao existe ainda
    while (n < max && fgets(linha, sizeof linha, f)) {  // le linha por linha
        if (linhaParaProduto(linha, lista + n))     // lista + n aponta para a posicao n
            n++;                                    // linha valida: conta o produto
    }
    fclose(f);                                      // fecha o arquivo
    return n;
}

// Retorna 1 se ja existe um produto com esse ID
static int idExiste(int id) {
    static Produto lista[MAX_LISTA];                // vetor auxiliar
    int n = carregar(lista, MAX_LISTA);             // carrega os produtos
    for (Produto *p = lista; p < lista + n; p++)    // percorre com ponteiro
        if (p->id == id) return 1;
    return 0;
}

// Gera o proximo ID automatico (maior ID existente + 1)
static int proximoId(void) {
    static Produto lista[MAX_LISTA];
    int n = carregar(lista, MAX_LISTA);
    int maior = 0;                                  // comeca em 0: primeiro ID sera 1
    for (Produto *p = lista; p < lista + n; p++)
        if (p->id > maior) maior = p->id;
    return maior + 1;
}

// Retorna 1 se ja existe OUTRO produto com o mesmo nome (ignora o ID 'idIgnorar')
static int nomeExiste(const char *nome, int idIgnorar) {
    static Produto lista[MAX_LISTA];
    int n = carregar(lista, MAX_LISTA);
    for (Produto *p = lista; p < lista + n; p++)
        if (p->id != idIgnorar && compararTexto(p->nome, nome) == 0) return 1;
    return 0;
}

/* ===================== EXIBICAO EM TABELA ===================== */

// Imprime 'n' tracos precedidos de '+'
static void tracos(int n) {
    putchar('+');
    for (int i = 0; i < n; i++) putchar('-');
}

// Imprime a linha horizontal da tabela
static void linha(void) {
    tracos(W_ID + 2); tracos(W_NOME + 2); tracos(W_CAT + 2);
    tracos(W_PRECO + 2); tracos(W_QTD + 2); tracos(W_FORN + 2);
    puts("+");
}

// Imprime uma coluna de texto com largura fixa (conta letras acentuadas UTF-8 como 1)
static void coluna(const char *s, int largura) {
    int cols = 0;                                   // quantos caracteres visiveis ja saiu
    printf("| ");
    for (const char *p = s; *p; p++) {              // percorre o texto com ponteiro
        putchar(*p);                                // imprime o byte
        if (((unsigned char)*p & 0xC0) != 0x80)     // byte de continuacao UTF-8 nao conta
            cols++;
    }
    for (; cols < largura; cols++) putchar(' ');    // completa com espacos
    putchar(' ');
}

// Mostra um vetor de produtos em formato de tabela (nao altera o arquivo)
static void mostrarTabela(const Produto *v, int n) {
    linha();
    printf("| %-*s ", W_ID, "ID");                  // cabecalho
    coluna("Produto", W_NOME);
    coluna("Categoria", W_CAT);
    printf("| %*s | %*s ", W_PRECO, "Preco (R$)", W_QTD, "Qtd");
    coluna("Fornecedor", W_FORN);
    puts("|");
    linha();
    for (const Produto *p = v; p < v + n; p++) {    // uma linha por produto
        printf("| %-*d ", W_ID, p->id);
        coluna(p->nome, W_NOME);
        coluna(p->categoria, W_CAT);
        printf("| %*.2f | %*d ", W_PRECO, p->preco, W_QTD, p->quantidade);
        coluna(p->fornecedor, W_FORN);
        puts("|");
    }
    linha();
    printf("Total: %d produto(s)\n", n);
}

/* ===================== BUBBLE SORT ===================== */

// Troca o conteudo de dois produtos usando ponteiros
static void trocar(Produto *a, Produto *b) {
    Produto tmp = *a;                               // guarda o primeiro
    *a = *b;                                        // copia o segundo para o primeiro
    *b = tmp;                                       // copia o guardado para o segundo
}

// Decide se 'a' e 'b' estao fora de ordem segundo o criterio escolhido
static int deveTrocar(const Produto *a, const Produto *b, int criterio) {
    switch (criterio) {
        case 1: return compararTexto(a->nome, b->nome) > 0;  // nome A-Z
        case 2: return compararTexto(a->nome, b->nome) < 0;  // nome Z-A
        case 3: return a->preco > b->preco;                  // preco menor -> maior
        case 4: return a->preco < b->preco;                  // preco maior -> menor
        default: return 0;                                   // sem ordenacao
    }
}

// Ordena o vetor 'v' com Bubble Sort (so em memoria, o arquivo nao muda)
static void bubbleSort(Produto *v, int n, int criterio) {
    for (int i = 0; i < n - 1; i++) {               // cada passada leva o "maior" ao fim
        for (int j = 0; j < n - 1 - i; j++) {       // compara pares vizinhos
            if (deveTrocar(v + j, v + j + 1, criterio))
                trocar(v + j, v + j + 1);           // fora de ordem: troca
        }
    }
}

/* ===================== OPERACOES DO CADASTRO ===================== */

// CRIAR: cria o arquivo de texto com os 4 produtos da tabela de exemplo
static void criarArquivo(void) {
    FILE *f = fopen(ARQUIVO, "r");                  // testa se o arquivo ja existe
    if (f) {
        fclose(f);
        if (lerInt("Arquivo ja existe. Apagar tudo e recriar? (1=sim, 0=nao): ") != 1) {
            printf("Cancelado.\n");
            return;
        }
    }
    static const Produto iniciais[] = {             // dados iniciais da tabela
        {1, "Camiseta Basica",  "Vestuario",       49.90f, 30, "Moda Brasil"},
        {2, "Mouse Optico USB", "Informatica",     59.99f, 15, "TecnoShop"},
        {3, "Shampoo 300ml",    "Higiene Pessoal", 22.50f, 40, "Beleza Pura"},
        {4, "Cafe 500g",        "Alimentos",       18.00f, 25, "Graos do Sul"}
    };
    f = fopen(ARQUIVO, "w");                        // "w" cria/zera o arquivo
    if (!f) {
        printf("Erro ao criar o arquivo.\n");
        return;
    }
    for (int i = 0; i < 4; i++)
        salvarProduto(f, &iniciais[i]);             // grava cada produto
    fclose(f);
    printf("Arquivo %s criado com 4 produtos.\n", ARQUIVO);
}

// Le os dados de um produto; retorna 0 se algo for invalido ou nome duplicado
static int lerDados(Produto *p, int idIgnorar) {
    lerTexto("Produto: ", p->nome, TAM_NOME);
    if (p->nome[0] == '\0') {                       // nome vazio nao pode
        printf("Nome nao pode ser vazio.\n");
        return 0;
    }
    if (nomeExiste(p->nome, idIgnorar)) {           // verificacao de duplicata
        printf("Ja existe um produto com esse nome.\n");
        return 0;
    }
    lerTexto("Categoria: ", p->categoria, TAM_CAT);
    if (p->categoria[0] == '\0') strcpy(p->categoria, "-");   // vazio vira "-"
    p->preco = lerFloat("Preco (R$): ");
    p->quantidade = lerInt("Quantidade: ");
    if (p->quantidade < 0) {
        printf("Quantidade invalida.\n");
        return 0;
    }
    lerTexto("Fornecedor: ", p->fornecedor, TAM_FORN);
    if (p->fornecedor[0] == '\0') strcpy(p->fornecedor, "-");
    return 1;
}

// INSERIR: adiciona um produto novo no final do arquivo
static void inserirProduto(void) {
    Produto p;
    p.id = proximoId();                             // ID automatico incremental
    if (idExiste(p.id)) {                           // verificacao de ID duplicado
        printf("ID %d ja existe. Insercao cancelada.\n", p.id);
        return;
    }
    printf("Novo produto (ID automatico: %d)\n", p.id);
    if (!lerDados(&p, 0)) return;                   // 0 = nenhum ID a ignorar
    FILE *f = fopen(ARQUIVO, "a");                  // "a" acrescenta no final
    if (!f) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }
    salvarProduto(f, &p);                           // grava a linha
    fclose(f);
    printf("Produto inserido com sucesso!\n");
}

// ALTERAR (excluir = 0) ou EXCLUIR (excluir = 1) usando arquivo temporario
static void alterarOuExcluir(int excluir) {
    int id = lerInt("ID do produto: ");
    Produto novo, atual;
    char linhaTxt[TAM_LINHA];

    if (!idExiste(id)) {
        printf("Produto nao encontrado.\n");
        return;
    }
    if (!excluir) {                                 // alterar: le os dados novos antes
        novo.id = id;                               // o ID nao muda
        printf("Digite os novos dados:\n");
        if (!lerDados(&novo, id)) return;           // ignora o proprio ID na checagem de nome
    }

    FILE *orig = fopen(ARQUIVO, "r");               // arquivo original (leitura)
    FILE *tmp  = fopen(TEMPORARIO, "w");            // arquivo temporario (escrita)
    if (!orig || !tmp) {
        printf("Erro ao abrir os arquivos.\n");
        if (orig) fclose(orig);
        if (tmp) fclose(tmp);
        return;
    }
    while (fgets(linhaTxt, sizeof linhaTxt, orig)) {    // copia linha por linha
        if (!linhaParaProduto(linhaTxt, &atual)) continue;  // ignora linha invalida
        if (atual.id == id) {                       // achou o produto
            if (excluir) continue;                  // excluir: simplesmente nao copia
            salvarProduto(tmp, &novo);              // alterar: grava os dados novos
        } else {
            salvarProduto(tmp, &atual);             // outros produtos: copia igual
        }
    }
    fclose(orig);
    fclose(tmp);
    remove(ARQUIVO);                                // apaga o original
    rename(TEMPORARIO, ARQUIVO);                    // o temporario vira o original
    printf(excluir ? "Produto excluido!\n" : "Produto alterado!\n");
}

// BUSCAR por nome ou por categoria (aceita so um pedaco do texto)
static void buscarProduto(void) {
    static Produto lista[MAX_LISTA], res[MAX_LISTA];    // todos e resultados
    int n = carregar(lista, MAX_LISTA);
    int r = 0;                                      // quantidade de resultados
    int op = lerInt("Buscar por: 1) Nome  2) Categoria: ");
    char termo[TAM_NOME];

    if (op != 1 && op != 2) {
        printf("Opcao invalida.\n");
        return;
    }
    lerTexto("Texto procurado: ", termo, TAM_NOME);
    for (Produto *p = lista; p < lista + n; p++) {
        const char *campo = (op == 1) ? p->nome : p->categoria;  // campo escolhido
        if (contemTexto(campo, termo))
            res[r++] = *p;                          // copia para os resultados
    }
    if (r == 0) printf("Nenhum produto encontrado.\n");
    else mostrarTabela(res, r);
}

// LISTAR todos os produtos, com a ordenacao escolhida
static void listarProdutos(void) {
    static Produto lista[MAX_LISTA];
    int n = carregar(lista, MAX_LISTA);
    if (n == 0) {
        printf("Nenhum produto. Use a opcao 1 para criar o arquivo.\n");
        return;
    }
    printf("Ordenar por:\n");
    printf(" 1) Nome (A-Z)\n 2) Nome (Z-A)\n");
    printf(" 3) Preco (menor -> maior)\n 4) Preco (maior -> menor)\n");
    printf(" 0) Ordem de cadastro\n");
    int crit = lerInt("Escolha: ");
    if (crit < 0 || crit > 4) crit = 0;             // opcao invalida: sem ordenar
    bubbleSort(lista, n, crit);                     // ordena so o vetor em memoria
    mostrarTabela(lista, n);
}

// Escreve um texto entre aspas no CSV (duplica aspas internas)
static void campoCSV(FILE *f, const char *s) {
    fputc('"', f);
    for (; *s; s++) {
        if (*s == '"') fputc('"', f);               // escapa aspas
        fputc(*s, f);
    }
    fputc('"', f);
}

// EXPORTAR para CSV com virgula
static void exportarCSV(void) {
    static Produto lista[MAX_LISTA];
    int n = carregar(lista, MAX_LISTA);
    if (n == 0) {
        printf("Nada para exportar.\n");
        return;
    }
    FILE *f = fopen(CSV, "w");
    if (!f) {
        printf("Erro ao criar o CSV.\n");
        return;
    }
    fprintf(f, "id,produto,categoria,preco,quantidade,fornecedor\n");  // cabecalho
    for (const Produto *p = lista; p < lista + n; p++) {
        fprintf(f, "%d,", p->id);
        campoCSV(f, p->nome);      fputc(',', f);
        campoCSV(f, p->categoria); fputc(',', f);
        fprintf(f, "%.2f,%d,", p->preco, p->quantidade);
        campoCSV(f, p->fornecedor);
        fputc('\n', f);
    }
    fclose(f);
    printf("Exportado para %s (%d produtos).\n", CSV, n);
}

/* ===================== PROGRAMA PRINCIPAL ===================== */

int main(void) {
    int opcao;
    do {
        printf("\n===== CADASTRO DE LOJA =====\n");
        printf("1. Criar arquivo (dados iniciais)\n");
        printf("2. Inserir produto\n");
        printf("3. Listar produtos\n");
        printf("4. Alterar produto\n");
        printf("5. Excluir produto\n");
        printf("6. Buscar por nome ou categoria\n");
        printf("7. Exportar para CSV\n");
        printf("0. Sair\n");
        opcao = lerInt("Escolha: ");

        switch (opcao) {
            case 1: criarArquivo();       break;
            case 2: inserirProduto();     break;
            case 3: listarProdutos();     break;
            case 4: alterarOuExcluir(0);  break;
            case 5: alterarOuExcluir(1);  break;
            case 6: buscarProduto();      break;
            case 7: exportarCSV();        break;
            case 0: printf("Ate mais!\n"); break;
            default: printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    return 0;
}
