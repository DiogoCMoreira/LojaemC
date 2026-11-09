# 🛍️ Cadastro de Loja em C

**Cadastro de Loja em C** é um sistema de gerenciamento de estoque escrito em linguagem **C**, cujo banco de dados é um **arquivo texto no formato de tabela** (`loja.txt`), legível diretamente em qualquer editor. O projeto aplica **Bubble Sort**, manipulação de arquivos, ponteiros, structs e validação de entrada — tudo com comentários didáticos linha a linha.

> **Versão final** — Atividade Avaliativa 04: algoritmos de ordenação (Bubble Sort) + manipulação de arquivos. O arquivo `loja.txt` é uma **tabela em texto**, exatamente como no enunciado.

## 📋 Funcionalidades

*   **Criar arquivo com dados iniciais:** gera `loja.txt` já formatado como tabela, com 4 produtos de exemplo. Se o arquivo existir, pede confirmação antes de recriar.
*   **Inserir produto:** cadastro com **ID automático incremental**, nome, categoria, preço, quantidade e fornecedor. Impede **nomes duplicados** (case-insensitive).
*   **Listar produtos:** exibe tabela formatada com **ordenação escolhida**:
    *   Nome (A-Z) / Nome (Z-A)
    *   Preço (menor → maior) / Preço (maior → menor)
    *   Ordem de cadastro
*   **Alterar produto:** edita os dados pelo ID, preservando o ID original.
*   **Excluir produto:** remove pelo ID, deslocando os itens seguintes no vetor.
*   **Buscar:** por **parte do nome** ou **parte da categoria** (busca parcial, *case-insensitive*), com resultados em tabela.
*   **Exportar CSV:** gera `loja.csv` com cabeçalho e escape correto de aspas duplas.
*   **Entrada segura:** leitura robusta de inteiros e floats (aceita vírgula ou ponto), descarte de excesso de caracteres, remoção de espaços nas pontas e substituição de `|` por `/` nos textos digitados (para não quebrar a tabela).

## 🛠️ Tecnologias e Conceitos Aplicados

*   **Linguagem:** C (C99 ou superior — `for` com declaração inline, `strtof`).
*   **Structs:** modelo `Produto` com `id`, `nome`, `categoria`, `preco`, `quantidade` e `fornecedor`.
*   **Persistência em tabela texto:** o arquivo `loja.txt` é gravado como tabela com `|`, traços e cabeçalho — e lido de volta com tolerância a esse formato (linhas de traço e cabeçalho são ignoradas).
*   **Gravação atômica:** escrita em arquivo temporário (`loja.tmp`) + `remove`/`rename`, evitando corrupção.
*   **Ordenação:** **Bubble Sort** próprio com 4 critérios (nome A-Z/Z-A, preço crescente/decrescente).
*   **Manipulação de strings:** `strncpy`, `strtok`, `strcspn`, `strchr`, `tolower`, `memmove`, `aparar` (trim), comparação e busca case-insensitive manuais.
*   **Ponteiros:** percurso de vetores e structs com aritmética de ponteiros (`lista + n`, `p < lista + n`), além de deslocamento de itens na exclusão.
*   **Tabelas com largura dinâmica:** as colunas se ajustam ao maior conteúdo; suporte a **UTF-8** (letras acentuadas contam como 1 coluna).
*   **Exportação CSV:** escrita formatada com escape de aspas.

## 🆚 Evolução das Versões

| Recurso | v1 | v2 | v3 | **Final** |
|---|---|---|---|---|
| Armazenamento | Memória | Binário | Texto (`;`) | **Tabela texto (`\|`)** |
| Campos | ID, nome, preço, qtd | ID, nome, preço, qtd | + categoria, fornecedor | + categoria, fornecedor |
| ID | Informado | Automático | Automático | Automático |
| Alterar / Excluir | ❌ / ✅ | ✅ / ✅ | ✅ / ✅ | ✅ / ✅ |
| Ordenação | ❌ | 3 critérios | 4 critérios | **4 critérios** |
| Busca | ID/nome | ID/nome | nome/categoria | **nome/categoria** |
| Dados iniciais | ❌ | ❌ | ✅ | ✅ |
| Exportar CSV | ❌ | ✅ | ✅ | ✅ |
| Nomes duplicados | ❌ | ❌ | ✅ | ✅ |
| Arquivo legível como tabela | ❌ | ❌ | ❌ | **✅** |
| Largura de coluna dinâmica | ❌ | ❌ | ❌ | **✅** |

## 🚀 Como Compilar e Executar

### Pré-requisitos
Um compilador C instalado (GCC, Clang ou similar).

1. **Clone o repositório:**
   ```bash
   git clone https://github.com/DiogoCMoreira/LojaemC.git
   cd LojaemC