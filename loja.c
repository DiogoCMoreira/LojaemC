#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PRODUTOS 100
#define TAM_NOME 50

typedef struct {
    int codigo;
    char nome[TAM_NOME];
    float preco;
    int quantidade;
} Produto;

static Produto estoque[MAX_PRODUTOS];
static int total = 0;

/* ---------- Entrada segura ---------- */

static void lerTexto(const char *msg, char *dest, int tam) {
    printf("%s", msg);
    if (fgets(dest, tam, stdin) == NULL) {
        printf("\nEntrada encerrada.\n");
        exit(0);
    }
    dest[strcspn(dest, "\n")] = '\0';
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
            if (*p == ',') *p = '.';   /* aceita virgula */
        v = strtof(buf, &fim);
        if (fim != buf && *fim == '\0' && v >= 0) return v;
        printf("Valor invalido. Tente de novo.\n");
    }
}

/* ---------- Auxiliares ---------- */

static int indicePorCodigo(int codigo) {
    for (int i = 0; i < total; i++)
        if (estoque[i].codigo == codigo) return i;
    return -1;
}

static int contemTexto(const char *texto, const char *busca) {
    char a[TAM_NOME], b[TAM_NOME];
    int i;
    for (i = 0; texto[i] && i < TAM_NOME - 1; i++) a[i] = tolower((unsigned char)texto[i]);
    a[i] = '\0';
    for (i = 0; busca[i] && i < TAM_NOME - 1; i++) b[i] = tolower((unsigned char)busca[i]);
    b[i] = '\0';
    return strstr(a, b) != NULL;
}

static void mostrarProduto(const Produto *p) {
    printf("Cod: %-5d | %-30s | R$ %8.2f | Qtd: %d\n",
           p->codigo, p->nome, p->preco, p->quantidade);
}

/* ---------- Operacoes ---------- */

static void inserirProduto(void) {
    if (total >= MAX_PRODUTOS) {
        printf("Estoque cheio!\n");
        return;
    }
    Produto p;
    p.codigo = lerInt("Codigo: ");
    if (indicePorCodigo(p.codigo) != -1) {
        printf("Ja existe um produto com esse codigo.\n");
        return;
    }
    lerTexto("Nome: ", p.nome, TAM_NOME);
    if (p.nome[0] == '\0') {
        printf("Nome nao pode ser vazio.\n");
        return;
    }
    p.preco = lerFloat("Preco: ");
    p.quantidade = lerInt("Quantidade: ");
    if (p.quantidade < 0) {
        printf("Quantidade invalida.\n");
        return;
    }
    estoque[total++] = p;
    printf("Produto inserido com sucesso!\n");
}

static void removerProduto(void) {
    int codigo = lerInt("Codigo do produto a remover: ");
    int i = indicePorCodigo(codigo);
    if (i == -1) {
        printf("Produto nao encontrado.\n");
        return;
    }
    for (int j = i; j < total - 1; j++)
        estoque[j] = estoque[j + 1];
    total--;
    printf("Produto removido!\n");
}

static void buscarProduto(void) {
    int op = lerInt("Buscar por: 1) Codigo  2) Nome: ");
    if (op == 1) {
        int i = indicePorCodigo(lerInt("Codigo: "));
        if (i == -1) printf("Produto nao encontrado.\n");
        else mostrarProduto(&estoque[i]);
    } else if (op == 2) {
        char termo[TAM_NOME];
        int achou = 0;
        lerTexto("Nome (ou parte): ", termo, TAM_NOME);
        for (int i = 0; i < total; i++) {
            if (contemTexto(estoque[i].nome, termo)) {
                mostrarProduto(&estoque[i]);
                achou = 1;
            }
        }
        if (!achou) printf("Nenhum produto encontrado.\n");
    } else {
        printf("Opcao invalida.\n");
    }
}

static void listarProdutos(void) {
    if (total == 0) {
        printf("Estoque vazio.\n");
        return;
    }
    for (int i = 0; i < total; i++)
        mostrarProduto(&estoque[i]);
}

/* ---------- Programa principal ---------- */

int main(void) {
    int opcao;
    do {
        printf("\n===== LOJA =====\n");
        printf("1. Inserir produto\n");
        printf("2. Remover produto\n");
        printf("3. Buscar produto\n");
        printf("4. Listar todos\n");
        printf("0. Sair\n");
        opcao = lerInt("Escolha: ");

        switch (opcao) {
            case 1: inserirProduto(); break;
            case 2: removerProduto(); break;
            case 3: buscarProduto();  break;
            case 4: listarProdutos(); break;
            case 0: printf("Ate mais!\n"); break;
            default: printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    return 0;
}
