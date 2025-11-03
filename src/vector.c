#include "../headers/vector.h"
#include <pthread.h>
#include <stdio.h>
#include <math.h>

vector* init_vector(uint64_t m) {
    if (m <= 0) {
        fprintf(stderr, "Erreur : taille de vecteur nulle.\n");
        return NULL;
    }
    vector* v = (vector*)malloc(sizeof(vector));
    if (v == NULL) {fprintf(stderr, "Problème lors de l'allocation de l'espace mémoire pour un vecteur : %s\n", strerror(errno)); exit(EXIT_FAILURE);}
    v->m = m;
    v->values = (double*)malloc(m*sizeof(double));
    if (v->values == NULL) {fprintf(stderr, "Problème lors de l'allocation de l'espace mémoire pour un vecteur : %s\n", strerror(errno)); exit(EXIT_FAILURE);}
    for (uint64_t i = 0; i < m; i++) {v->values[i] = 0;}
    return v;
}

void print_vector(vector* v) {
    if (v == NULL) {
        fprintf(stderr, "Erreur: vecteur NULL.\n");
        return;
    }
    if (v->m == 0) {printf("[]\n"); return;}
    printf("[%f", v->values[0]);
    for (uint64_t i = 1; i < v->m; i++) {
        printf(" %f", v->values[i]);
    }
    printf("]\n");
}

vector* add_v_v(vector* x, vector* y) {
    if (x == NULL || y == NULL || x->m != y->m ) return NULL;

    vector* z = init_vector(x->m);
    for (uint64_t i = 0; i < x->m; i++) {
        z->values[i] = x->values[i] + y->values[i];
    }
    return z;
}

vector* sub_v_v(vector *x, vector *y){
    if (x == NULL || y == NULL || x->m != y->m ) return NULL;

    vector* z = init_vector(x->m);
    for (uint64_t i = 0; i < x->m; i++) {
        z->values[i] = x->values[i] - y->values[i];
    }
    return z;
}

double dot_prod(vector* u, vector* v){
    if (u == NULL || v == NULL || u->m != v->m ) return 0.0;
    
    double x = 0;
    uint64_t m = v->m;

    for (uint64_t i = 0; i < m; i++) {
        x += u->values[i] * v->values[i];
    }
    return x;
}

double norm(vector* v){
    if (v == NULL) return 0.0;
    return sqrt(dot_prod(v, v));
}

vector* mult_v_scalaire(vector* v, double s){
    if (v == NULL) {
        fprintf(stderr, "Erreur: vecteur NULL.\n");
        return NULL;
    } 
    vector* r = init_vector(v->m);

    for (uint64_t i = 0; i < v->m; i++) r->values[i] = v->values[i] * s;

    return r;
}

vector* liste_to_vector(double* l, int n){
    if (l == NULL || n <= 0) return NULL;
    vector* v = init_vector(n);

    for (uint64_t i = 0; i < n; i++) v->values[i] = l[i];
    return v;
}

void free_vector(vector **v) {
    if (v == NULL || *v == NULL) return;
    free((*v)->values);
    (*v)->values = NULL;
    free(*v);
    *v = NULL;
}