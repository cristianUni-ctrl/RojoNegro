#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int dato;
    char color; // 'R' = Rojo, 'B' = Negro
    struct node *padre, *izq, *der;
} Node;

#define ROJO 'R'
#define NEGRO 'B'

// Prototipos
void insertar(Node **raiz, int dato);
void eliminar(Node **raiz, int dato);
void inorder(Node *raiz);
void liberar(Node *raiz);

// Utilidades internas
void rotacionIzq(Node **raiz, Node *x);
void rotacionDer(Node **raiz, Node *x);
void corregirInsercion(Node **raiz, Node *z);
void corregirEliminacion(Node **raiz, Node *x);
Node* minimo(Node *n);
Node* buscar(Node *raiz, int dato);
void reemplazar(Node **raiz, Node *u, Node *v);

// Rotaciones
void rotacionIzq(Node **raiz, Node *x) {
    Node *y = x->der;
    x->der = y->izq;
    if (y->izq) y->izq->padre = x;
    y->padre = x->padre;
    if (!x->padre) *raiz = y;
    else if (x == x->padre->izq) x->padre->izq = y;
    else x->padre->der = y;
    y->izq = x;
    x->padre = y;
}

void rotacionDer(Node **raiz, Node *x) {
    Node *y = x->izq;
    x->izq = y->der;
    if (y->der) y->der->padre = x;
    y->padre = x->padre;
    if (!x->padre) *raiz = y;
    else if (x == x->padre->der) x->padre->der = y;
    else x->padre->izq = y;
    y->der = x;
    x->padre = y;
}

// Inserción
void insertar(Node **raiz, int dato) {
    Node *z = (Node *)malloc(sizeof(Node));
    if (!z) exit(EXIT_FAILURE);
    z->dato = dato;
    z->color = ROJO;
    z->izq = z->der = z->padre = NULL;

    Node *y = NULL;
    Node *x = *raiz;

    while (x) {
        y = x;
        x = (dato < x->dato) ? x->izq : x->der;
    }

    z->padre = y;
    if (!y) *raiz = z;
    else if (dato < y->dato) y->izq = z;
    else y->der = z;

    corregirInsercion(raiz, z);
}

// Corrección después de insertar
void corregirInsercion(Node **raiz, Node *z) {
    while (z != *raiz && z->padre->color == ROJO) {
        if (z->padre == z->padre->padre->izq) {
            Node *y = z->padre->padre->der;
            if (y && y->color == ROJO) {
                z->padre->color = NEGRO;
                y->color = NEGRO;
                z->padre->padre->color = ROJO;
                z = z->padre->padre;
            } else {
                if (z == z->padre->der) {
                    z = z->padre;
                    rotacionIzq(raiz, z);
                }
                z->padre->color = NEGRO;
                z->padre->padre->color = ROJO;
                rotacionDer(raiz, z->padre->padre);
            }
        } else {
            Node *y = z->padre->padre->izq;
            if (y && y->color == ROJO) {
                z->padre->color = NEGRO;
                y->color = NEGRO;
                z->padre->padre->color = ROJO;
                z = z->padre->padre;
            } else {
                if (z == z->padre->izq) {
                    z = z->padre;
                    rotacionDer(raiz, z);
                }
                z->padre->color = NEGRO;
                z->padre->padre->color = ROJO;
                rotacionIzq(raiz, z->padre->padre);
            }
        }
    }
    (*raiz)->color = NEGRO;
}

// Buscar un nodo
Node* buscar(Node *raiz, int dato) {
    while (raiz && raiz->dato != dato) {
        raiz = (dato < raiz->dato) ? raiz->izq : raiz->der;
    }
    return raiz;
}

// Mínimo en subárbol
Node* minimo(Node *n) {
    while (n->izq) n = n->izq;
    return n;
}

// Reemplazo de nodo
void reemplazar(Node **raiz, Node *u, Node *v) {
    if (!u->padre) *raiz = v;
    else if (u == u->padre->izq) u->padre->izq = v;
    else u->padre->der = v;
    if (v) v->padre = u->padre;
}

// Eliminación de nodo
void eliminar(Node **raiz, int dato) {
    Node *z = buscar(*raiz, dato);
    if (!z) {
        printf("Nodo no encontrado.\n");
        return;
    }

    Node *y = z;
    Node *x;
    char y_color = y->color;

    if (!z->izq) {
        x = z->der;
        reemplazar(raiz, z, z->der);
    } else if (!z->der) {
        x = z->izq;
        reemplazar(raiz, z, z->izq);
    } else {
        y = minimo(z->der);
        y_color = y->color;
        x = y->der;

        if (y->padre == z) {
            if (x) x->padre = y;
        } else {
            reemplazar(raiz, y, y->der);
            y->der = z->der;
            y->der->padre = y;
        }

        reemplazar(raiz, z, y);
        y->izq = z->izq;
        y->izq->padre = y;
        y->color = z->color;
    }

    free(z);
    if (y_color == NEGRO) {
        corregirEliminacion(raiz, x);
    }
}

// Corrección después de eliminar
void corregirEliminacion(Node **raiz, Node *x) {
    while (x != *raiz && (!x || x->color == NEGRO)) {
        if (x == x->padre->izq) {
            Node *w = x->padre->der;
            if (w && w->color == ROJO) {
                w->color = NEGRO;
                x->padre->color = ROJO;
                rotacionIzq(raiz, x->padre);
                w = x->padre->der;
            }
            if ((!w->izq || w->izq->color == NEGRO) &&
                (!w->der || w->der->color == NEGRO)) {
                w->color = ROJO;
                x = x->padre;
            } else {
                if (!w->der || w->der->color == NEGRO) {
                    if (w->izq) w->izq->color = NEGRO;
                    w->color = ROJO;
                    rotacionDer(raiz, w);
                    w = x->padre->der;
                }
                w->color = x->padre->color;
                x->padre->color = NEGRO;
                if (w->der) w->der->color = NEGRO;
                rotacionIzq(raiz, x->padre);
                x = *raiz;
            }
        } else {
            Node *w = x->padre->izq;
            if (w && w->color == ROJO) {
                w->color = NEGRO;
                x->padre->color = ROJO;
                rotacionDer(raiz, x->padre);
                w = x->padre->izq;
            }
            if ((!w->izq || w->izq->color == NEGRO) &&
                (!w->der || w->der->color == NEGRO)) {
                w->color = ROJO;
                x = x->padre;
            } else {
                if (!w->izq || w->izq->color == NEGRO) {
                    if (w->der) w->der->color = NEGRO;
                    w->color = ROJO;
                    rotacionIzq(raiz, w);
                    w = x->padre->izq;
                }
                w->color = x->padre->color;
                x->padre->color = NEGRO;
                if (w->izq) w->izq->color = NEGRO;
                rotacionDer(raiz, x->padre);
                x = *raiz;
            }
        }
    }
    if (x) x->color = NEGRO;
}

// Inorden
void inorder(Node *raiz) {
    if (!raiz) return;
    inorder(raiz->izq);
    printf("%d(%c) ", raiz->dato, raiz->color);
    inorder(raiz->der);
}

// Liberar memoria del árbol
void liberar(Node *raiz) {
    if (!raiz) return;
    liberar(raiz->izq);
    liberar(raiz->der);
    free(raiz);
}

// Menú principal
int main() {
    Node *raiz = NULL;
    int opcion, valor;

    do {
        printf("\n--- Árbol Rojo-Negro ---\n");
        printf("1. Insertar\n2. Eliminar\n3. Mostrar (inorden)\n4. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                printf("Valor a insertar: ");
                scanf("%d", &valor);
                insertar(&raiz, valor);
                break;
            case 2:
                printf("Valor a eliminar: ");
                scanf("%d", &valor);
                eliminar(&raiz, valor); 
                break;
            case 3:
                printf("Recorrido inorden: ");
                inorder(raiz);
                printf("\n");
                break;
            case 4:
                liberar(raiz);
                printf("Saliendo...\n");
                break;
            default:
                printf("Opción inválida.\n");
        }

    } while (opcion != 4);

    return 0;
}
