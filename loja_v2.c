#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ARQUIVO    "produtos.dat"
#define TEMPORARIO "produtos.tmp"
#define CSV        "produtos.csv"
#define TAM_NOME   50      /* campos com tamanho limitado */
#define MAX_LISTA  1000

typedef struct {
    int id;
    char nome[TAM_NOME];
    float preco;
    int quantidade;
} Produto;

/* ================= Entrada segura ================= */

static void lerTexto(const char *msg, char *dest, int tam) {
    printf("%s", msg);
    if (fgets(dest, tam, stdin) == NULL) {
        printf("\nEntrada encerrada.\n");
        exit(0);
    }
    char *nl = strchr(dest, '\n');
    if (nl) {
        *nl = '\0';
    } else {                       /* texto maior que o campo: descarta o resto */
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
}

static int lerInt(const char *msg) {
    char buf[32], *fim;
    long v;
    while (1) {
        lerTexto(msg, buf, sizeof buf);
        v = strtol(buf, &fim, 10);
        if (fim != buf && *fim == '\0') return (int)v;
        printf("Valor invalido. Tente de novo.\n");
    }
}

static float lerFloat(const char *msg) {
    char buf[32], *fim;
    float v;
    while (1) {
        lerTexto(msg, buf, sizeof buf);
        for (char *p = buf; *p; p++)
            if (*p == ',') *p = '.';           /* aceita virgula */
        v = strtof(buf, &fim);
        if (fim != buf && *fim == '\0' && v >= 0) return v;
        printf("Valor invalido. Tente de novo.\n");
    }
}

/* ================= Comparacoes case-insensitive ================= */

static int compararTexto(const char *a, const char *b) {
    while (*a && *b) {
        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);
        if (ca != cb) return ca - cb;
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

static int contemTexto(const char *texto, const char *busca) {
    size_t n = strlen(busca);
    if (n == 0) return 1;
    for (; *texto; texto++) {
        size_t i = 0;
        while (i < n && texto[i] &&
               tolower((unsigned char)texto[i]) == tolower((unsigned char)busca[i]))
            i++;
        if (i == n) return 1;
    }
    return 0;
}

/* ================= Acesso ao arquivo ================= */

/* Carrega o arquivo para um vetor de estruturas */
static int carregar(Produto lista[], int max) {
    FILE *f = fopen(ARQUIVO, "rb");
    int n = 0;
    if (!f) return 0;
    while (n < max && fread(&lista[n], sizeof(Produto), 1, f) == 1) n++;
    fclose(f);
    return n;
}

static int idExiste(int id) {
    FILE *f = fopen(ARQUIVO, "rb");
    Produto p;
    int achou = 0;
    if (!f) return 0;
    while (fread(&p, sizeof p, 1, f) == 1) {
        if (p.id == id) { achou = 1; break; }
    }
    fclose(f);
    return achou;
}

/* ID automatico incremental */
static int proximoId(void) {
    FILE *f = fopen(ARQUIVO, "rb");
    Produto p;
    int maior = 0;
    if (f) {
        while (fread(&p, sizeof p, 1, f) == 1)
            if (p.id > maior) maior = p.id;
        fclose(f);
    }
    return maior + 1;
}

/* ================= Exibicao em tabela ================= */

static void tracos(int n) {
    putchar('+');
    for (int i = 0; i < n; i++) putchar('-');
}

static void linha(void) {
    tracos(7); tracos(52); tracos(12); tracos(10);
    puts("+");
}

static void mostrarTabela(const Produto v[], int n) {
    linha();
    printf("| %-5s | %-50s | %10s | %8s |\n", "ID", "Nome", "Preco", "Qtd");
    linha();
    for (int i = 0; i < n; i++)
        printf("| %-5d | %-50s | %10.2f | %8d |\n",
               v[i].id, v[i].nome, v[i].preco, v[i].quantidade);
    linha();
    printf("Total: %d produto(s)\n", n);
}

/* ================= Bubble Sort ================= */

static int compararProdutos(const Produto *a, const Produto *b, int criterio) {
    if (criterio == 2) return compararTexto(a->nome, b->nome);
    if (criterio == 3) return (a->preco > b->preco) - (a->preco < b->preco);
    return a->id - b->id;
}

static void bubbleSort(Produto v[], int n, int criterio) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (compararProdutos(&v[j], &v[j + 1], criterio) > 0) {
                Produto tmp = v[j];
                v[j] = v[j + 1];
                v[j + 1] = tmp;
            }
        }
    }
}

/* ================= Operacoes ================= */

static int lerDados(Produto *p) {
    lerTexto("Nome: ", p->nome, TAM_NOME);
    if (p->nome[0] == '\0') {
        printf("Nome nao pode ser vazio.\n");
        return 0;
    }
    p->preco = lerFloat("Preco: ");
    p->quantidade = lerInt("Quantidade: ");
    if (p->quantidade < 0) {
        printf("Quantidade invalida.\n");
        return 0;
    }
    return 1;
}

static void inserirProduto(void) {
    Produto p;
    p.id = proximoId();
    if (idExiste(p.id)) {                      /* verificacao de ID duplicado */
        printf("ID %d ja existe. Insercao cancelada.\n", p.id);
        return;
    }
    printf("Novo produto (ID automatico: %d)\n", p.id);
    if (!lerDados(&p)) return;

    FILE *f = fopen(ARQUIVO, "ab");
    if (!f) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }
    fwrite(&p, sizeof p, 1, f);
    fclose(f);
    printf("Produto inserido com sucesso!\n");
}

/* Alterar e excluir usam arquivo temporario para evitar corrupcao de dados */
static void alterarOuExcluir(int excluir) {
    int id = lerInt("ID do produto: ");
    Produto novo, p;

    if (!idExiste(id)) {
        printf("Produto nao encontrado.\n");
        return;
    }

    if (!excluir) {
        novo.id = id;
        printf("Digite os novos dados:\n");
        if (!lerDados(&novo)) return;
    }

    FILE *orig = fopen(ARQUIVO, "rb");
    FILE *tmp  = fopen(TEMPORARIO, "wb");
    if (!orig || !tmp) {
        printf("Erro ao abrir os arquivos.\n");
        if (orig) fclose(orig);
        if (tmp) fclose(tmp);
        return;
    }

    while (fread(&p, sizeof p, 1, orig) == 1) {
        if (p.id == id) {
            if (excluir) continue;             /* nao copia = exclui */
            p = novo;
        }
        fwrite(&p, sizeof p, 1, tmp);
    }
    fclose(orig);
    fclose(tmp);

    remove(ARQUIVO);                           /* substitui o original pelo temporario */
    rename(TEMPORARIO, ARQUIVO);
    printf(excluir ? "Produto excluido!\n" : "Produto alterado!\n");
}

static void buscarProduto(void) {
    static Produto lista[MAX_LISTA], res[MAX_LISTA];
    int n = carregar(lista, MAX_LISTA), r = 0;
    int op = lerInt("Buscar por: 1) ID  2) Nome: ");

    if (op == 1) {
        int id = lerInt("ID: ");
        for (int i = 0; i < n; i++)
            if (lista[i].id == id) res[r++] = lista[i];
    } else if (op == 2) {
        char termo[TAM_NOME];
        lerTexto("Nome (ou parte): ", termo, TAM_NOME);
        for (int i = 0; i < n; i++)
            if (contemTexto(lista[i].nome, termo)) res[r++] = lista[i];
    } else {
        printf("Opcao invalida.\n");
        return;
    }

    if (r == 0) printf("Nenhum produto encontrado.\n");
    else mostrarTabela(res, r);
}

/* Lista ordenada; ordena so o vetor em memoria, o arquivo nao e alterado */
static void listarProdutos(void) {
    static Produto lista[MAX_LISTA];
    int n = carregar(lista, MAX_LISTA);
    if (n == 0) {
        printf("Estoque vazio.\n");
        return;
    }
    int crit = lerInt("Ordenar por: 1) ID  2) Nome  3) Preco: ");
    if (crit < 1 || crit > 3) crit = 1;
    bubbleSort(lista, n, crit);
    mostrarTabela(lista, n);
}

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
    fprintf(f, "id,nome,preco,quantidade\n");
    for (int i = 0; i < n; i++) {
        fprintf(f, "%d,\"", lista[i].id);
        for (const char *c = lista[i].nome; *c; c++) {
            if (*c == '"') fputc('"', f);      /* escapa aspas */
            fputc(*c, f);
        }
        fprintf(f, "\",%.2f,%d\n", lista[i].preco, lista[i].quantidade);
    }
    fclose(f);
    printf("Exportado para %s (%d produtos).\n", CSV, n);
}

/* ================= Programa principal ================= */

int main(void) {
    int opcao;
    do {
        printf("\n===== LOJA =====\n");
        printf("1. Inserir produto\n");
        printf("2. Alterar produto\n");
        printf("3. Excluir produto\n");
        printf("4. Buscar produto\n");
        printf("5. Listar (ordenado)\n");
        printf("6. Exportar para CSV\n");
        printf("0. Sair\n");
        opcao = lerInt("Escolha: ");

        switch (opcao) {
            case 1: inserirProduto();        break;
            case 2: alterarOuExcluir(0);     break;
            case 3: alterarOuExcluir(1);     break;
            case 4: buscarProduto();         break;
            case 5: listarProdutos();        break;
            case 6: exportarCSV();           break;
            case 0: printf("Ate mais!\n");   break;
            default: printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    return 0;
}
