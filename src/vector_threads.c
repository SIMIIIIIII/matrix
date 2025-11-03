#include <math.h>
#include <pthread.h>
#include "../headers/vector_threads.h"


typedef struct {
    vector* x;      
    vector* y;      
    vector* z;
    double val;    
    uint64_t debut; 
    uint64_t fin;  
    char op;
    double result;
} thread_arg;


typedef struct{
    vector* v;
    double r;
} Resultat;


void* init_thread(void* arg) {
    thread_arg* t_arg = (thread_arg*)arg;
    for (uint64_t i = t_arg->debut; i < t_arg->fin; i++) {
        t_arg->x->values[i] = 0;
    }
    return NULL;
}


vector* init_vector(uint64_t m, int n_threads) {
    if (m <= 0) {
        fprintf(stderr, "Erreur : taille de vecteur nulle.\n");
        return NULL;
    }
    vector* v = (vector*)malloc(sizeof(vector));
    if (v == NULL) {
        fprintf(stderr, "Problème lors de l'allocation de l'espace mémoire pour un vecteur : %s\n", strerror(errno));
        return NULL;
    }
    v->m = m;

    v->values = (double*)malloc(m*sizeof(double));
    if (v->values == NULL) {
        fprintf(stderr, "Problème lors de l'allocation de l'espace mémoire pour un vecteur : %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (n_threads > v->m) n_threads = v->m;
    if (n_threads <= 0) n_threads = 1;

    pthread_t threads[n_threads];
    thread_arg* thread_data[n_threads];

    uint64_t taille = v->m / n_threads;
    uint64_t reste = v->m % n_threads;
    uint64_t current_start = 0;

    for (uint64_t i = 0; i < n_threads; i++) {

        uint64_t extra = (i < reste) ? 1 : 0;
        uint64_t taille_courante = taille + extra;

        thread_data[i] = (thread_arg*)malloc(sizeof(thread_arg));
        if (thread_data[i] == NULL){
            for (int j = 0; j < i; j++) {
                pthread_cancel(threads[j]);
                free(thread_data[j]);
            }
            fprintf(stderr, "Problème lors de l'allocation de l'espace mémoire pour un vecteur : %s\n", strerror(errno));
            return NULL;
        }

        thread_data[i]->x = v;
        thread_data[i]->debut = current_start;
        thread_data[i]->fin = current_start + taille_courante;

        if (pthread_create(&threads[i], NULL, init_thread, thread_data[i]) != 0) {
            
            for (int j = 0; j < i; j++) {
                pthread_cancel(threads[j]);
                free(thread_data[j]);
            }
            fprintf(stderr, "Problème lors de l'allocation de l'espace mémoire pour un vecteur : %s\n", strerror(errno));
            return NULL;
        }
        current_start += taille_courante;
    }

    for (int i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
        free(thread_data[i]);
    }
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


void* op_thread(void* arg) {
    thread_arg* t_arg = (thread_arg*)arg;
    for (uint64_t i = t_arg->debut; i < t_arg->fin; i++) {
        if (t_arg->op == '+')
        {
            t_arg->z->values[i] = t_arg->x->values[i] + t_arg->y->values[i];
        }
        else if (t_arg->op == '-') {
            t_arg->z->values[i] = t_arg->x->values[i] - t_arg->y->values[i];
        }
        else if (t_arg->op == '*') {
            t_arg->result += t_arg->x->values[i] * t_arg->y->values[i];
        }
        else t_arg->z->values[i] = t_arg->x->values[i] * t_arg->val;
    }
    pthread_exit(NULL);
}


Resultat* op_v_v(vector* x, vector* y, double val, char op, int n_threads) {
    if (x == NULL || y == NULL || x->m != y->m) {
        fprintf(stderr, "dimenssions des vecteurs incompatible.\n");
        return NULL;
    }

    vector* z;
    if (op != '*'){
        z = init_vector(x->m, n_threads);
        if (z == NULL) return NULL;
    }

    if (n_threads > x->m) n_threads = x->m;
    if (n_threads <= 0) n_threads = 1;

    pthread_t threads[n_threads];
    thread_arg* args[n_threads];

    uint64_t taille = x->m / n_threads;
    uint64_t reste = x->m % n_threads;
    uint64_t current_start = 0;

    for (int i = 0; i < n_threads; i++) {
        uint64_t extra = (i < reste) ? 1 : 0;
        uint64_t taille_courante = taille + extra;
        args[i] = (thread_arg*)malloc(sizeof(thread_arg));

        if (args[i] == NULL)
        {
            for (int j = 0; j < i; j++) {
                pthread_cancel(threads[j]);
                free(args[j]);
            }
            free_vector(&z);
            fprintf(stderr, "Il y a un avec l'allocation de la mémoire %d\n", i);
            return NULL;
        }
        

        args[i]->x = x;
        args[i]->y = y;
        args[i]->z = z;
        args[i]->debut = current_start;
        args[i]->fin = current_start + taille_courante;
        args[i]->op = op;
        args[i]->result =0.0;
        args[i]->val = val;

        if (pthread_create(&threads[i], NULL, op_thread, args[i]) != 0) {
            
            for (int j = 0; j < i; j++) {
                pthread_cancel(threads[j]);
                free(args[j]);
            }
            free_vector(&z);
            fprintf(stderr, "Il y a un probème dans la création du thread %d\n", i);
            return NULL;
        }
        current_start += taille_courante;
    }

    double r = 0.0;

    for (int i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
        r += args[i]->result;
        free(args[i]);
    }

    Resultat* resultat = (Resultat*)malloc(sizeof(Resultat));
    if (resultat == NULL)
    {
        fprintf(stderr, "problème lors de l'allocation de memoire.\n");
        return NULL;
    }
    
    resultat->r = r;
    resultat->v = z;
    return resultat;
}


vector* add_sub_v_v(vector* v, vector* w, char op, int n_threads){
    Resultat* resultat = op_v_v(v, w, 0.0, op, n_threads);
    if (resultat == NULL) return NULL;
    return resultat->v;
}


double dot_prod(vector* u, vector* v, int n_threads) {
    Resultat* resultat = op_v_v(u, v, 0.0, '*', n_threads);
    if (resultat == NULL) return 0.0;
    return resultat->r;
} 


double norm(vector* v, int n_threads) {
    double r = dot_prod(v, v, n_threads);
    return sqrt(r);
}


vector* mult_v_scalaire(vector* v, double s, int n_threads){
    Resultat* resultat = op_v_v(v, v, s, 'm', n_threads);
    if (resultat == NULL) return NULL;
    return resultat->v;
}


vector* liste_to_vector(double* l, int n, int n_threads){
    if (l == NULL || n <= 0) return NULL;
    vector* v = init_vector(n, n_threads);

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