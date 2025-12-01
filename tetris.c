#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_HASH 10

//-----------------------------------------
// ESTRUTURAS
//-----------------------------------------

// Árvore binária da mansão
typedef struct Sala {
    char nome[50];
    char pista[50];
    struct Sala *esq;
    struct Sala *dir;
} Sala;

// BST para pistas coletadas
typedef struct NodoBST {
    char pista[50];
    struct NodoBST *esq;
    struct NodoBST *dir;
} NodoBST;

// Hash para pista → suspeito
typedef struct NodoHash {
    char pista[50];
    char suspeito[50];
    struct NodoHash *prox;
} NodoHash;

typedef struct {
    NodoHash *lista[TAM_HASH];
} HashTable;


//-----------------------------------------
// FUNÇÕES DA HASH
//-----------------------------------------

int hash(char *pista) {
    int soma = 0;
    for (int i = 0; pista[i] != '\0'; i++)
        soma += pista[i];
    return soma % TAM_HASH;
}

void inserirNaHash(HashTable *t, char *pista, char *suspeito) {
    int h = hash(pista);

    NodoHash *novo = malloc(sizeof(NodoHash));
    strcpy(novo->pista, pista);
    strcpy(novo->suspeito, suspeito);
    novo->prox = t->lista[h];
    t->lista[h] = novo;
}

char *encontrarSuspeito(HashTable *t, char *pista) {
    int h = hash(pista);
    NodoHash *aux = t->lista[h];

    while (aux != NULL) {
        if (strcmp(aux->pista, pista) == 0)
            return aux->suspeito;
        aux = aux->prox;
    }
    return NULL;
}


//-----------------------------------------
// BST – PISTAS COLETADAS
//-----------------------------------------

NodoBST* inserirBST(NodoBST *raiz, char *pista) {
    if (raiz == NULL) {
        NodoBST *novo = malloc(sizeof(NodoBST));
        strcpy(novo->pista, pista);
        novo->esq = novo->dir = NULL;
        return novo;
    }

    if (strcmp(pista, raiz->pista) < 0)
        raiz->esq = inserirBST(raiz->esq, pista);
    else
        raiz->dir = inserirBST(raiz->dir, pista);

    return raiz;
}


//-----------------------------------------
// VERIFICAÇÃO FINAL
//-----------------------------------------

typedef struct {
    int maria;
    int joao;
    int lucas;
} ContadorSuspeitos;

void contarSuspeitos(NodoBST *no, HashTable *table, ContadorSuspeitos *c) {
    if (no == NULL) return;

    contarSuspeitos(no->esq, table, c);

    char *sus = encontrarSuspeito(table, no->pista);
    if (sus != NULL) {
        if (strcmp(sus, "Maria") == 0) c->maria++;
        else if (strcmp(sus, "Joao") == 0) c->joao++;
        else if (strcmp(sus, "Lucas") == 0) c->lucas++;
    }

    contarSuspeitos(no->dir, table, c);
}

void verificarSuspeitoFinal(NodoBST *raiz, HashTable *table) {
    ContadorSuspeitos c = {0,0,0};

    contarSuspeitos(raiz, table, &c);

    printf("\n--- RESULTADO FINAL ---\n");
    printf("Pontos: Maria=%d, Joao=%d, Lucas=%d\n", c.maria, c.joao, c.lucas);

    if (c.maria >= c.joao && c.maria >= c.lucas)
        printf("Suspeito final: MARIA\n");
    else if (c.joao >= c.maria && c.joao >= c.lucas)
        printf("Suspeito final: JOAO\n");
    else
        printf("Suspeito final: LUCAS\n");
}


//-----------------------------------------
// EXPLORAÇÃO DA MANSÃO
//-----------------------------------------

void explorar(Sala *atual, NodoBST **pistas) {
    if (atual == NULL) return;

    char opcao;
    printf("\nVocê entrou na sala: %s\n", atual->nome);
    printf("Pista encontrada: %s\n", atual->pista);

    *pistas = inserirBST(*pistas, atual->pista);

    printf("Escolha caminho:\n");
    printf("e - esquerda\n");
    printf("d - direita\n");
    printf("s - sair\n");
    scanf(" %c", &opcao);

    if (opcao == 'e') explorar(atual->esq, pistas);
    else if (opcao == 'd') explorar(atual->dir, pistas);
    else if (opcao == 's') return;
}


//-----------------------------------------
// MAIN – MONTA MANSÃO E EXECUTA JOGO
//-----------------------------------------

Sala* novaSala(char *nome, char *pista) {
    Sala *s = malloc(sizeof(Sala));
    strcpy(s->nome, nome);
    strcpy(s->pista, pista);
    s->esq = s->dir = NULL;
    return s;
}

int main() {

    // Tabela Hash
    HashTable tabela;
    for (int i = 0; i < TAM_HASH; i++) tabela.lista[i] = NULL;

    inserirNaHash(&tabela, "pegada", "Maria");
    inserirNaHash(&tabela, "luva", "Joao");
    inserirNaHash(&tabela, "cabelo", "Lucas");
    inserirNaHash(&tabela, "relógio", "Maria");
    inserirNaHash(&tabela, "anel", "Joao");

    // Mansão (árvore fixa)
    Sala *entrada = novaSala("Entrada", "pegada");
    entrada->esq = novaSala("Biblioteca", "cabelo");
    entrada->dir = novaSala("Cozinha", "luva");
    entrada->esq->esq = novaSala("Porão", "anel");
    entrada->esq->dir = novaSala("Estúdio", "relógio");

    NodoBST *pistas = NULL;

    explorar(entrada, &pistas);
    verificarSuspeitoFinal(pistas, &tabela);

    return 0;
}