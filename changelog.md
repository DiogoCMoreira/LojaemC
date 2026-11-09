# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/),
and this project adheres to [Semantic Versioning](https://semver.org/).

## [Unreleased]

## [3.0.0] - 2026-11-09

### Added
- Criação de arquivo `loja.txt` como **tabela de texto** legível (formato do enunciado).
- Exportação para CSV (`loja.csv`).
- Dados iniciais com 4 produtos de exemplo (opção 1 do menu).
- Busca por **nome** ou **categoria** (case-insensitive, por substring).
- Ordenação por nome (A-Z / Z-A) e preço (crescente / decrescente).
- Verificação de **nomes duplicados** (case-insensitive).

### Changed
- Formato de armazenamento: de texto delimitado por `;` para **tabela com `|`**.
- Gravação reescrita inteira via arquivo temporário (`loja.tmp`) + `rename`.
- Largura das colunas da tabela agora é **dinâmica** (ajusta ao maior conteúdo).
- Suporte a UTF-8 na contagem de colunas (letras acentuadas contam como 1).

### Fixed
- Leitura tolerante a linhas de traço e cabeçalho (ignoradas automaticamente).

## [2.0.0] - 2026-11-09

### Added
- Persistência em arquivo binário (`produtos.dat`).
- ID automático incremental.
- Ordenação por ID, nome ou preço (Bubble Sort).
- Exportação para CSV.
- Alteração e exclusão com arquivo temporário.

### Changed
- Capacidade de listagem ampliada para 1000 produtos.

## [1.0.0] - 2026-11-09

### Added
- Sistema de estoque em memória (sem persistência).
- Inserir, remover, buscar (ID ou nome) e listar produtos.
- Validação de entrada (inteiros, floats, nome não vazio).

[Unreleased]: https://github.com/DiogoCMoreira/LojaemC/compare/v3.0.0...HEAD
[3.0.0]: https://github.com/DiogoCMoreira/LojaemC/compare/v2.0.0...v3.0.0
[2.0.0]: https://github.com/DiogoCMoreira/LojaemC/compare/v1.0.0...v2.0.0
[1.0.0]: https://github.com/DiogoCMoreira/LojaemC/releases/tag/v1.0.0