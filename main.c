#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Vincoli
typedef struct numeroCaratteri{
    char alfabeto[65];
    int pres[64];
} numeroCaratteri;

// Strutture dati dizionario
typedef struct node{
    char* word_color;
    struct node *father;
    struct node *left;
    struct node *right;
    struct node *next;
} node;

typedef struct BST_node{
    node* word;
    struct BST_node *left, *right;
} BST_node;

int lunghezza, lung_lista = 0;
char* obiettivo;
int tentativiRimasti;
int inGame = 0;
char primo_tentativo = 'f';
char* output;

// Strutture dati
node *nuovo;
node **dizionario;
BST_node *NIL_bst, *temp_tree;
node *NIL, *NIL_list;
node *list, *fix, *prev_of_fix;

// Vincoli
char* ok;
char** presente;
char nonPresente[63];
struct numeroCaratteri* conteggioPresenze;

// Variabili per gestione vincoli
int k = 0;
int l = 0;

// Dichiarazioni obbligatorie
int HASH_Function(char word);
void partita();

// Funzioni strutture dati
node *left_rotate(node * t, node * x) {
    node* y = x->right;
    x->right = y->left;
    if(y->left != NIL)
        y->left->father = x;
    y->father = x->father;
    if(x->father == NIL)
        t = y;
    else {
        if (x == x->father->left)
            x->father->left = y;
        else
            x->father->right = y;
    }
    y->left = x;
    x->father = y;
    return t;
}

node *right_rotate(node * t, node * x) {
    node* y = x->left;
    x->left = y->right;
    if(y->right != NIL)
        y->right->father = x;
    y->father = x->father;
    if(y->father == NIL)
        t = y;
    else {
        if(x == x->father->left)
            x->father->left = y;
        else
            x->father->right = y;
    }
    y->right = x;
    x->father = y;
    return t;
}

//'!' (rosso) & '?' (nero) sono i colori dei nodi
node* RB_fixup_insert(node *t, node *z){
    node* x;
    node* y;
    if(z == t)
        t->word_color[lunghezza] = '?';
    else {
        x = z->father;
        if(x->word_color[lunghezza] == '!') {
            if (x == x->father->left) {
                y = x->father->right;
                if (y->word_color[lunghezza] == '!') {
                    x->word_color[lunghezza] = '?';
                    y->word_color[lunghezza] = '?';
                    x->father->word_color[lunghezza] = '!';
                    return RB_fixup_insert(t, x->father);
                } else {
                    if (z == x->right) {
                        z = x;
                        t = left_rotate(t, z);
                        x = z->father;
                    }
                    x->word_color[lunghezza] = '?';
                    x->father->word_color[lunghezza] = '!';
                    t = right_rotate(t, x->father);
                }
            } else {
                y = x->father->left;
                if (y->word_color[lunghezza] == '!') {
                    x->word_color[lunghezza] = '?';
                    y->word_color[lunghezza] = '?';
                    x->father->word_color[lunghezza] = '!';
                    return RB_fixup_insert(t, x->father);
                } else {
                    if (z == x->left) {
                        z = x;
                        t = right_rotate(t, z);
                        x = z->father;
                    }
                    x->word_color[lunghezza] = '?';
                    x->father->word_color[lunghezza] = '!';
                    t = left_rotate(t, x->father);
                }
            }
        }
    }
    return t;
}

node* RB_insert(node *t, char *word) {
    node* x = t;
    node* y = NIL;
    while (x != NIL) {
        y = x;
        if (strncmp(word, x->word_color, lunghezza) < 0)
            x = x->left;
        else
            x = x->right;
    }
    node *z = (node *) malloc(sizeof(node));
    z->word_color = (char *) malloc((lunghezza + 2));
    for(int i = 0; i < lunghezza; i++)
        z->word_color[i] = word[i];
    z->father = y;
    if (y == NIL)
        t = z;
    else {
        if (strncmp(z->word_color, y->word_color, lunghezza) < 0)
            y->left = z;
        else
            y->right = z;
    }
    z->left = NIL;
    z->right = NIL;
    z->word_color[lunghezza] = '!';
    z->word_color[lunghezza + 1] = '\0';
    z->next = NIL;
    t = RB_fixup_insert(t, z);
    nuovo = z;
    return t;
}

void RB_traversal(node *t) {
    if(t != NIL) {
        RB_traversal(t->left);
        for(int i = 0; i < lunghezza; i++)
            printf("%c", t->word_color[i]);
        printf("\n");
        RB_traversal(t->right);
    }
}

void list_insert(node *x) {
    if (list == NIL_list) {
        list = x;
        x->next = NIL_list;
    }
    else {
        x->next = list;
        list = x;
    }
    lung_lista++;
}

void list_delete(node *prev, node *x) {
    if(prev != NIL_list)
        prev->next = x->next;
    else
        list = x->next;
    x->next = NIL_list;
    lung_lista--;
}

void reset_list() {
    node * x = list;
    while (x != NIL_list) {
        list = x->next;
        x->next = NIL;
        x = list;
    }
    lung_lista = 0;
}

void stampa_filtrate(){
    if (primo_tentativo == 'v') {
        node *x = list;
        while (x != NIL_list) {
            char *word = (char *) malloc((lunghezza + 1));
            for (int i = 0; i < lunghezza; i++)
                word[i] = x->word_color[i];
            word[lunghezza] = '\0';
            printf("%s\n", word);
            x = x->next;
        }
    } else {
        for(int i = 0; i < 64; i++)
            RB_traversal(dizionario[i]);
    }
}

BST_node *BST_insert(BST_node *t, node *z) {
    BST_node *x = t;
    BST_node *y = NIL_bst;
    while(x != NIL_bst) {
        y = x;
        if(strncmp(z->word_color, x->word->word_color, lunghezza) < 0)
            x = x->left;
        else
            x = x->right;
    }
    BST_node *p = (BST_node *) malloc(sizeof(BST_node));
    p->left = NIL_bst;
    p->right = NIL_bst;
    p->word = z;
    if(y == NIL_bst)
        return p;
    else {
        if (strncmp(p->word->word_color, y->word->word_color, lunghezza) < 0)
            y->left = p;
        else
            y->right = p;
    }
    return t;
}

void list_fix(BST_node *x) {
    if (x != NIL_bst) {
        list_fix(x->left);
        while (fix != NIL_list && strncmp(x->word->word_color, fix->word_color,  lunghezza) > 0) {
            prev_of_fix = fix;
            fix = fix->next;
        }
        if(prev_of_fix == NIL_list) {
            x->word->next = list;
            list = x->word;
        }
        else {
            prev_of_fix->next = x->word;
            x->word->next = fix;
        }
        prev_of_fix = x->word;
        lung_lista++;
        list_fix(x->right);
        free(x);
    }
}

// Ritorna y se presente, n altrimenti
char RB_exists(node *t, char *word) {
    if(t == NIL)
        return 'n';
    if(strncmp(word, t->word_color, lunghezza) == 0)
        return 'y';
    if(strncmp(word, t->word_color, lunghezza) < 0)
        return RB_exists(t->left, word);
    return RB_exists(t->right, word);
}

char exists(char *word) {
    return RB_exists(dizionario[HASH_Function(word[0])], word);
}

// Gestione HASH
int HASH_Function(const char c){
    int f = (int) c;
    if (f == 45)
        return 0;
    if (f >= 48 && f <= 57)
        return f-47;
    if (f >= 65 && f <= 90)
        return f-54;
    if (f == 95)
        return 37;
    if (f >= 97 && f <= 122)
        return f-59;
    return -1;
}

void HASH_insert(char *word) {
    int p = HASH_Function(word[0]);
    dizionario[p] = RB_insert(dizionario[p], word);
}

// Ritorna la riga della lettera cercata
int inMatrice(char** matrice, char cercato){
    int trovato = -1;
    for (int j = 0; j < l + 1; ++j) {
        if (cercato == matrice[j][0]) {
            trovato = j;
            break;
        }
    }
    return trovato;
}

int trovaIndice(const char alfabeto[65], char cercato){
    int i = 0;
    while (alfabeto[i] != cercato)
        i++;
    return i;
}

// Output e vincoli
void stampa_tentativo(const char *tentato) {
    for (int i = 0; i < lunghezza; ++i) {
        output[i] = '/';
        // Lettera nel posto giusto
        if (obiettivo[i] == tentato[i]) {
            output[i] = '+';
            if (ok[i] == '?') {
                ok[i] = tentato[i];
                int index_i = inMatrice(presente, tentato[i]);
                for (int j = 0; j < l; ++j) {
                    if (j != index_i)
                        presente[j][i + 1] = '*';
                }
            }
        }
    }
    for (int i = 0; i < lunghezza; ++i) {
        if (output[i] == '+')
            continue;
        int presenza = 0;
        for (int j = 0; j < lunghezza; ++j) {
            if (tentato[i] == obiettivo[j] && i != j)
                presenza++;
            // Lettera nel posto sbagliato
            if (output[j] != '/' && tentato[j] == tentato[i] && i != j)
                presenza--;
        }
        // Lettera non presente
        if (presenza > 0) {
            output[i] = '|';
            int index_i = inMatrice(presente, tentato[i]);
            if (index_i >= 0)
                presente[index_i][i + 1] = '*';
            else {
                presente[l][i + 1] = '*';
                for (int j = 0; j < lunghezza + 1; ++j) {
                    if (j != i + 1) {
                        presente[l][j] = tentato[i];
                    }
                }
                l++;
            }
        }
    }
}

// Vincoli
void aggiornaPresente(){
    for (int i = 0; i < lunghezza; ++i) {
        for (int j = 0; j < lunghezza; ++j) {
            if (ok[i] != '?' && inMatrice(presente, ok[i]) != j)
                presente[j][i + 1] = '*';
        }
    }
}

void aggiornaOk(){
    for (int i = 0; i < l; ++i) {
        int conto = 0, v = 0;
        int altezza[lunghezza];
        for (int j = 1; j < lunghezza + 1; ++j) {
            if (presente[i][j] != '*') {
                conto++;
                altezza[v] = j - 1;
                v++;
            }
        }
        if (conto == (-1) * conteggioPresenze->pres[trovaIndice(conteggioPresenze->alfabeto, ok[i])])
            while (v < 0) {
                ok[altezza[v]] = presente[i][0];
                v--;
            }
    }
}

void salva_vincoli(const char tentativo[lunghezza + 1]) {
    int temp_presenze[64] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < lunghezza; ++i) {
        if (output[i] == '|') {
            for (int j = 0; j < lunghezza; ++j) {
                if (tentativo[i] == tentativo[j] && output[j] == '/') {
                    presente[inMatrice(presente, tentativo[i])][j + 1] = '*';
                    aggiornaPresente();
                    aggiornaOk();
                }
            }
        }
    }
    for (int i = 0; i < lunghezza; ++i) {
        if (output[i] == '/') {
            int bool = 0;
            // Se in soprannumero
            for (int j = 0; j < lunghezza; ++j) {
                if (tentativo[i] == ok[j]) {
                    bool = 1;
                    break;
                }
            }
            if (bool == 1)
                continue;
            if (inMatrice(presente, tentativo[i]) != -1) {
                continue;
            }
            // Se già in nonPresente
            int m = 0;
            while (nonPresente[m] != '?') {
                if (tentativo[i] == nonPresente[m]) {
                    bool = 1;
                    break;
                }
                m++;
            }
            // Se va aggiunta
            if (bool == 0) {
                nonPresente[k] = tentativo[i];
                k++;
            }
        }
    }
    for (int i = 0; i < lunghezza; ++i) {
        temp_presenze[trovaIndice(conteggioPresenze->alfabeto, tentativo[i])]++;
    }
    for (int i = 0; i < lunghezza; ++i) {
        int pres = 0;
        for (int j = 0; j < lunghezza; ++j) {
            if (tentativo[i] == tentativo[j])
                if (output[i] != '/' && output[j] != '/')
                    pres++;
        }
        int pres_index = trovaIndice(conteggioPresenze->alfabeto, tentativo[i]);
        if (conteggioPresenze->pres[pres_index] >= 0) {
            if (temp_presenze[pres_index] > pres)
                conteggioPresenze->pres[pres_index] = pres * (-1);
            if (temp_presenze[pres_index] == pres) {
                if (pres > conteggioPresenze->pres[pres_index])
                    conteggioPresenze->pres[pres_index] = pres;
            }
        }
    }
}

int valuta_parola(const char *parola){
    for (int i = 0; i < lunghezza; ++i) {
        // Elimina i nodi che non hanno almeno una lettera nel posto giusto
        if (ok[i] != '?' && parola[i] != ok[i])
            return 0;
        // Elimina i nodi con lettere nel posto sbagliato
        int index = inMatrice(presente, parola[i]);
        if (index != -1 && presente[index][i + 1] != parola[i]) {
            return 0;
        }
        // Elimina i nodi con almeno una lettera non presente nell'obiettivo
        int n = 0;
        while (nonPresente[n] != '?') {
            if (nonPresente[n] == parola[i])
                return 0;
            n++;
        }
    }
    int temp_presenze[64] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < lunghezza; ++i) {
        temp_presenze[trovaIndice(conteggioPresenze->alfabeto, parola[i])]++;
    }
    int num_caratteri = 0;
    for (int i = 0; i < 64; ++i) {
        if (conteggioPresenze->pres[i] == 0)
            continue;
        if (conteggioPresenze->pres[i] < 0) {
            if (temp_presenze[i] != conteggioPresenze->pres[i] * (-1))
                return 0;
            num_caratteri += (conteggioPresenze->pres[i] * -1);
        }
        if (conteggioPresenze->pres[i] > 0) {
            if (conteggioPresenze->pres[i] > temp_presenze[i])
                return 0;
            num_caratteri += conteggioPresenze->pres[i];
        }
        if (num_caratteri >= lunghezza)
            break;
    }
    return 1;
}

void fix_albero(node *albero) {
    if (albero != NIL) {
        fix_albero(albero->right);
        if (valuta_parola(albero->word_color) == 1)
            list_insert(albero);
        fix_albero(albero->left);
    }
}

void sistema_incompatibili(){
    if (primo_tentativo == 'v') {
        // Per ogni elemento della lista verifica se rispetta le condizioni
        node *temp = list;
        node *temp_prev = NIL_list;
        node *z;
        while (temp != NIL_list) {
            z = temp->next;
            if (valuta_parola(temp->word_color) == 0)
                list_delete(temp_prev, temp);
            else
                temp_prev = temp;
            temp = z;
        }
    } else {
        // Verifica ogni elemento di ogni albero
        for (int i = 63; i >= 0; i--) {
            fix_albero(dizionario[i]);
        }
    }
}

// Funzioni partita
void setup(){
    NIL = (node *) malloc(sizeof(node));
    NIL->word_color = malloc((lunghezza+2));
    for(int i = 0; i < lunghezza; i++)
        NIL->word_color[i] = '*';
    NIL->word_color[lunghezza] = '?';
    NIL->word_color[lunghezza+1] = '\0';
    dizionario = (node **) malloc(64 * sizeof(node *));
    for (int i = 0; i < 64; i++)
        dizionario[i] = NIL;
    NIL_list = (node *) malloc(sizeof(node));
    list = NIL_list;
    fix = NIL_list;
    NIL_bst = (BST_node *) malloc(sizeof (BST_node));
    temp_tree = NIL_bst;
    output = (char *) malloc(lunghezza + 1);
    output[lunghezza] = '\0';
}

void setup_vincoli(){
    ok = (char*) malloc(lunghezza + 1);
    ok[lunghezza] = '\0';
    presente = (char **) malloc((lunghezza + 1) * sizeof (char*));
    for (int i = 0; i < lunghezza + 1; ++i) {
        presente[i] = (char*) malloc(lunghezza + 2);
        presente[i][lunghezza + 1] = '\0';
    }
    conteggioPresenze = (numeroCaratteri*) malloc(sizeof (numeroCaratteri));
    strcpy(conteggioPresenze->alfabeto, "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_");
}

void reset_vincoli(){
    for (int i = 0; i < lunghezza; ++i) {
        ok[i] = '?';
        if (presente[i][0] != '?') {
            for (int j = 0; j < lunghezza + 1; ++j) {
                presente[i][j] = '?';
            }
        }
    }
    for (int i = 0; i < 63; ++i) {
        if (nonPresente[i] != '?')
            nonPresente[i] = '?';
        if (conteggioPresenze->pres[i] != 0)
            conteggioPresenze->pres[i] = 0;
    }
    conteggioPresenze->pres[63] = 0;
}

void reset_vars(){
    inGame = 0;
    primo_tentativo = 'f';
    l = 0;
    k = 0;
}

void primo_inserimento(){
    char *word = (char *) malloc(20 * sizeof(char));
    while (1) {
        if (scanf("%s", word) <= 0)
            return;
        if (word[0] != '+') {
            HASH_insert(word);
        } else {
            if (strcmp(word, "+nuova_partita") == 0) {
                break;
            }
            else if (strcmp(word, "+stampa_filtrate") == 0)
                stampa_filtrate();
        }
    }
}

void inserimento(){
    char *word = (char *) malloc(18);
    while (1) {
        if (scanf("%s", word) <= 0)
            return;
        if (word[0] != '+') {
            HASH_insert(word);
            if (primo_tentativo == 'v') {
                if (valuta_parola(word) == 1) {
                    temp_tree = BST_insert(temp_tree, nuovo);
                }
            }
        } else {
            if (strcmp(word, "+inserisci_inizio") == 0)
                continue;
            if (strcmp(word, "+inserisci_fine") == 0){
                if (primo_tentativo == 'v') {
                    fix = list;
                    prev_of_fix = NIL_list;
                    list_fix(temp_tree);
                    temp_tree = NIL_bst;
                }
                if (inGame == 1)
                    partita();
                else
                    inserimento();
            }
            if (strcmp(word, "+nuova_partita") == 0) {
                partita();
            }
        }
    }
}

void partita() {
    if (inGame == 0) {
        obiettivo = (char *) malloc(lunghezza + 1);
        if (scanf("%s", obiettivo) <= 0)
            return;
        if (scanf("%d", &tentativiRimasti) <= 0)
            return;
        inGame = 1;
    }
    char* tentativo = (char*) malloc(18);
    while (tentativiRimasti > 0) {
        if (scanf("%s", tentativo) <= 0)
            return;
        if (tentativo[0] != '+'){
            if (exists(tentativo) == 'y'){
                // Vittoria
                if (strcmp(obiettivo, tentativo) == 0){
                    printf("ok\n");
                    reset_list();
                    reset_vincoli();
                    reset_vars();
                    inserimento();
                }
                else {
                    tentativiRimasti--;
                    if (tentativiRimasti > 0) {
                        stampa_tentativo(tentativo);
                        salva_vincoli(tentativo);
                        sistema_incompatibili();
                        printf("%s\n%d\n", output, lung_lista);
                        if (primo_tentativo == 'f')
                            primo_tentativo = 'v';
                    } else {
                        // Tentativi terminati
                        if (primo_tentativo == 'v') {
                            stampa_tentativo(tentativo);
                            salva_vincoli(tentativo);
                            sistema_incompatibili();
                            printf("%s\n%d\nko\n", output, lung_lista);
                            reset_list();
                            reset_vincoli();
                            reset_vars();
                            inserimento();
                        } else {
                            inserimento();
                        }
                    }
                }
            }
            else
                printf("not_exists\n");
        }
        else if (strcmp(tentativo, "+inserisci_inizio") == 0)
            inserimento();
        else if (strcmp(tentativo, "+stampa_filtrate") == 0)
            stampa_filtrate();
        else if (strcmp(tentativo, "+nuova_partita") == 0){
            reset_vars();
            reset_list();
            reset_vincoli();
            partita();
        }
    }
}

int main() {
    if (scanf("%d", &lunghezza) <= 0)
        return -1;
    setup();
    setup_vincoli();
    reset_vincoli();
    primo_inserimento();
    partita();
    return 0;
}