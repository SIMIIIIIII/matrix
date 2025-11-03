#include "../headers/matrix_threads.h"
#include <pthread.h>
#include <stdio.h>

typedef struct {
    matrix* m;
    vector* v;
    vector* result;
    uint64_t start;
    uint64_t end;
} thread_data_mv_t;


typedef struct
{
    matrix* A;
    matrix* B;
    matrix* C;
    size_t row_start;
    size_t row_end;
} matmul_thread_args_t;


typedef struct {
    matrix* A;
    matrix* B;
    matrix* C;
    uint64_t start_add_mm;
    uint64_t end_add_mm;
} thread_data_add_mm_t;


typedef struct {
    matrix* A;
    matrix* B;
    matrix* C;
    uint64_t row_debut; // Ligne de départ pour le thread
    uint64_t row_fin;   // Ligne de fin pour le thread
    char op;
} thread_datas;


typedef struct {
    matrix* A;
    matrix* B;
    int start_row;
    int end_row;
} ThreadData;


matrix* init_matrix(uint64_t m, uint64_t n) {
    matrix* A = (matrix*)malloc(sizeof(matrix));
    if (A == NULL) {
        fprintf(stderr, "Problème lors de l'allocation de l'espace mémoire pour une matrice : %s\n", strerror(errno));
        return NULL;
    }
    A->m = m; A->n = n;
    A->values = (double**)malloc(m*sizeof(double*));

    if (A->values == NULL) {
        fprintf(stderr, "Problème lors de l'allocation de l'espace mémoire pour une matrice : %s\n", strerror(errno));
        free_matrice(&A);
        return NULL;
    }

    for (uint64_t i = 0; i < m; i++) {
        A->values[i] = (double*)malloc(n*sizeof(double));
        if (A->values[i] == NULL) {
            fprintf(stderr, "Problème lors de l'allocation de l'espace mémoire pour une matrice : %s\n", strerror(errno));
            free_matrice(&A);
            return NULL;
        }

        memset(A->values[i], 0, n * sizeof(double)); // Remplace la boucle d'initialisation
    }
    return A;
}


void print_matrix(matrix* A) {
    if (A->m == 0) {printf("[[]]\n"); return;}
    printf("[[%f", A->values[0][0]);
    for (uint64_t j = 1; j < A->n; j++) {printf(" %f", A->values[0][j]);}
    if (A->m == 1) {printf("]]\n"); return;}
    printf("]\n");
    for (uint64_t i = 1; i < A->m; i++) {
        printf(" [%f", A->values[i][0]);
        for (uint64_t j = 1; j < A->n; j++) {printf(" %f", A->values[i][j]);}
        if (i == A->m - 1) {printf("]]\n"); return;}
        printf("]\n");
    }
}


void* add_sub_rows(void* arg) {
    thread_datas* data = (thread_datas*)arg;
    for (uint64_t i = data->row_debut; i < data->row_fin; i++) { // Corrigé : unit64_t -> uint64_t
        for (uint64_t j = 0; j < data->C->n; j++) {
            if (data->op == '-'){
                data->C->values[i][j] = data->A->values[i][j] - data->B->values[i][j];
            }
            else data->C->values[i][j] = data->A->values[i][j] + data->B->values[i][j];
        }
    }
    return NULL;
}


matrix* add_sub_m_m(matrix* A, matrix* B, char op, int n_threads) {
    if (A == NULL || B == NULL) {
        fprintf(stderr, "Erreur : l'une des matrices est NULL.\n");
        return NULL;
    }
    if (A->m != B->m || A->n != B->n) {
        fprintf(stderr, "Erreur : dimensions incompatibles (%lu x %lu) vs (%lu x %lu).\n", 
                A->m, A->n, B->m, B->n);
        return NULL;
    }

    matrix* C = init_matrix(A->m, A->n);
    if (C == NULL) {
        fprintf(stderr, "Erreur : échec de l'initialisation de la matrice résultat.\n");
        return NULL;
    }

    if (n_threads > (int)A->m) n_threads = (int)A->m;
    if (n_threads <= 0) n_threads = 1;


    pthread_t threads[n_threads];
    thread_datas* thread_args[n_threads];

    uint64_t row_thread = A->m / n_threads;
    uint64_t extra_row = A->m % n_threads; 

    uint64_t current_row = 0;
    for (int i = 0; i < n_threads; i++) {
        thread_args[i] = (thread_datas*)malloc(sizeof(thread_datas));
        if (thread_args[i] == NULL){
            fprintf(stderr, "Erreur : échec de l'allocation de la mémoire' %d.\n", i);
            for (int j = 0; j < i; j++) {
                pthread_cancel(threads[j]);
                free(thread_args[i]);
            }
            free_matrice(&C);
            return NULL;
        }

        thread_args[i]->A = A;
        thread_args[i]->B = B;
        thread_args[i]->C = C;
        thread_args[i]->row_debut = current_row;
        thread_args[i]->row_fin = current_row + row_thread + (i < (int)extra_row ? 1 : 0);
        thread_args[i]->op = op;

        if (pthread_create(&threads[i], NULL, add_sub_rows, thread_args[i]) != 0) {
            fprintf(stderr, "Erreur : échec de la création du thread %d.\n", i);
            for (int j = 0; j < i; j++) {
                pthread_cancel(threads[j]);
                free(thread_args[i]);
            }
             
            free_matrice(&C);
            return NULL;
        }
        current_row = thread_args[i]->row_fin;
    }

    for (int i = 0; i < n_threads; i++) { 
        pthread_join(threads[i], NULL);
        free(thread_args[i]);
    }
    return C;
}


void* thread_mult_m_v(void* arg) {
    thread_data_mv_t* data = (thread_data_mv_t*)arg;

    for (uint64_t i = data->start; i < data->end; i++) {
        double sum = 0;
        for (uint64_t j = 0; j < data->m->n; j++) {
            sum += data->m->values[i][j] * data->v->values[j];
        }
        data->result->values[i] = sum;
    }
    pthread_exit(NULL);
}


vector* mult_m_v(matrix* M, vector* v, int n_threads) { // Tom
    if (M == NULL || v == NULL || M->n != v->m) return NULL;

    if (n_threads > (int)v->m) n_threads = v->m;
    if (n_threads <= 0) n_threads = 1;

    vector* result = init_vector(M->m, n_threads);
    if (result == NULL) return NULL;

    pthread_t threads[n_threads];
    thread_data_mv_t thread_data[n_threads];

    uint64_t chunk_size = M->m / n_threads;

    for (int i = 0; i < n_threads; i++) {
        thread_data[i].m = M;
        thread_data[i].v = v;
        thread_data[i].result = result;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == n_threads - 1) ? M->m : (i + 1) * chunk_size;

        if (pthread_create(&threads[i], NULL, thread_mult_m_v, &thread_data[i]) != 0) {
            free_vector(&result);
            for (int j = 0; j < i; j++) pthread_cancel(threads[i]);
            
            fprintf(stderr, "Il y a un problème dans la création du thread.%d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    return result;
}


void *matmul_worker(void *arg)
{
    matmul_thread_args_t *args = (matmul_thread_args_t *)arg;

    for (int i = 0; i < args->B->n; i++)
    {
        for (int j = args->row_start; j < args->row_end; j++)
        {
            for (int k = 0; k < args->A->n; k++)
            {
                args->C->values[j][i] += args->A->values[j][k] * args->B->values[k][i];
            }
        }
    }
    
    return NULL;
}


matrix* mult_m_m(matrix* A, matrix* B, int n_threads){
    if (A == NULL || B == NULL || A->n != B->m){
        fprintf(stderr, "les tailles de 2 matrices sont incompatibles: %s\n", strerror(errno));
        return NULL;
    }

    matrix* C = init_matrix(A->m, B->n);
    if (C == NULL) return NULL;

    if (n_threads == 0) n_threads = 1;
    if (n_threads > A->m) n_threads = A->m;

    pthread_t threads[n_threads];
    matmul_thread_args_t args[n_threads];

    size_t rows_per_thread = A->m / n_threads;
    size_t remainder = A->m % n_threads;

    size_t row = 0;
    for (size_t i = 0; i < n_threads; ++i)
    {
        size_t extra = i < remainder ? 1 : 0;
        args[i] = (matmul_thread_args_t){
            .A = A, .B = B, .C = C, .row_start = row, .row_end = row + rows_per_thread + extra};

        if (pthread_create(&threads[i], NULL, matmul_worker, &args[i]) != 0){
            fprintf(stderr, "erreur lors de la création des threads: %s\n", strerror(errno));
            for (int j = 0; j < i; j++)
            {
                pthread_cancel(threads[j]);
            }
            return NULL;
        }
        row = args[i].row_end;
    }

    for (size_t i = 0; i < n_threads; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    return C;
}


void *transpose_worker(void *arg)
{
    matmul_thread_args_t *args = (matmul_thread_args_t *)arg;
    for (size_t i = args->row_start; i < args->row_end; ++i)
    {
        for (size_t j = 0; j < args->A->n; ++j)
        {
            args->B->values[j][i] = args->A->values[i][j];
        }
    }
    return NULL;
}


matrix* transp(matrix* A, int n_threads){
    if (A == NULL){
        fprintf(stderr, "la matrice d'entrée est nulle: %s\n", strerror(errno));
        return NULL;
    }
    matrix* B = init_matrix(A->n, A->m);
    if (B == NULL) return NULL;

    if (n_threads == 0) n_threads = 1;
    if (n_threads > A->m) n_threads = A->m;

    pthread_t threads[n_threads];
    matmul_thread_args_t args[n_threads];

    size_t rows_per_thread = A->m / n_threads;
    size_t remainder = A->m % n_threads;

    size_t row = 0;
    for (size_t i = 0; i < n_threads; ++i)
    {
        size_t extra = i < remainder ? 1 : 0;
        args[i] = (matmul_thread_args_t){
            .A = A, .B = B, .row_start = row, .row_end = row + rows_per_thread + extra};
        if (pthread_create(&threads[i], NULL, transpose_worker, &args[i]) != 0){
            fprintf(stderr, "erreur lors de la création des threads: %s\n", strerror(errno));
            for (int j = 0; j < i; j++)
            {
                pthread_cancel(threads[j]);
            }
            return NULL;
        }
        row = args[i].row_end;
    }

    for (size_t i = 0; i < n_threads; ++i)
    {
        pthread_join(threads[i], NULL);
    }
    return B;
}


void add_vector_to_matrix(matrix* M, vector* v, int colonne){
    if(M->n != v->m ){
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < v->m; i++)
    {
        M->values[i][colonne] = v->values[i];
    } 
}


vector* back_sub(matrix* A, vector* b, int n_threads){
    if (A == NULL || b == NULL || A->m != A->n || A->n != b->m){
        fprintf(stderr, "les demeission des matrice et vecteur sont incompatibles\n");
        return NULL;
    }

    if (n_threads == 0) n_threads = 1;
    if (n_threads > b->m) n_threads = b->m;

    vector* r = init_vector(b->m, n_threads);
    if (r==NULL) return NULL;

    for (int i = b->m-1; i >= 0; i--)
    {
        r->values[i] = b->values[i];

        for (int j = i + 1; j < b->m; j++)
        {
            r->values[i] -= A->values[i][j] * r->values[j];
        }

        if (fabs(A->values[i][i]) < DBL_EPSILON  && fabs(r->values[i]) > DBL_EPSILON)
        {
            fprintf(stderr, "Aucune solution\n");
            return NULL;
        }
        else if (fabs(A->values[i][i]) < DBL_EPSILON  && fabs(r->values[i]) < DBL_EPSILON)
        {
            r->values[i] = (double) rand() / A->values[i][i];
            fprintf(stderr, "Infinité de solutions\n");
        }
        else
        {
            r->values[i] /= A->values[i][i];
        } 
    }
    return r;
}


void* copy_partial_matrix(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    matrix* A = data->A;
    matrix* B = data->B;

    for (int i = data->start_row; i < data->end_row; ++i) {
        for (int j = 0; j < A->n; ++j) {
            B->values[i][j] = A->values[i][j];
        }
    }
    return NULL;
}


void copyMatrix(matrix* A, matrix* B, int n_threads) {
    if (A == NULL || B == NULL) {
        fprintf(stderr, "la matrice d'entrée est nulle: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (A->m != B->m || A->n != B->n) {
        fprintf(stderr, "les 2 matrices n'ont pas la même dimenssion: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (n_threads == 0) n_threads = 1;
    if (n_threads > A->m) n_threads = A->m;

    pthread_t threads[n_threads];
    ThreadData thread_data[n_threads];

    int rows_per_thread = A->m / n_threads;

    for (int i = 0; i < n_threads; ++i) {
        thread_data[i].A = A;
        thread_data[i].B = B;
        thread_data[i].start_row = i * rows_per_thread;
        thread_data[i].end_row = (i == n_threads - 1) ? A->m : (i + 1) * rows_per_thread;
        if (pthread_create(&threads[i], NULL, copy_partial_matrix, &thread_data[i]) != 0){
            fprintf(stderr, "erreur lors de la création des threads: %s\n", strerror(errno));
            for (int j = 0; j < i; j++)
            {
                pthread_cancel(threads[j]);
            }
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < n_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
}


vector* getColumn(matrix* A, int column, int n_threads){
    if (column < 0 || column >= A->n) return NULL;

    if (n_threads == 0) n_threads = 1;
    if (n_threads > A->m) n_threads = A->m;

    vector* v = init_vector(A->m, n_threads);

    for (int i = 0; i < A->m; i++) v->values[i] = A->values[i][column];
    return v;
}


void qr(matrix* A, matrix* Q, matrix* R, int n_threads) {
    if (Q->n != R->m || R->m != R->n){
        exit(EXIT_FAILURE);
    }

    // s'assurer que R soi initialiser à 0
    for (int i = 0; i < R->m; i++) {
        for (int j = 0; j < R->n; j++) {
            R->values[i][j] = 0.0;
        }
    }

    // Copier A à Q
    copyMatrix(A, Q, n_threads);

    for (int i = 0; i < A->n; i++) {
        vector* col_i = getColumn(Q, i, n_threads);
        R->values[i][i] = norm(col_i, n_threads);

        if (R->values[i][i] == 0) {
            fprintf(stderr, "Singular matrix: Zero column norm detected.\n");
            free_vector(&col_i);
            return;
        }

        // Normaliser
        for (int j = 0; j < Q->m; j++) {
            Q->values[j][i] /= R->values[i][i];
        }

        // Orthogonaliser
        for (int j = i + 1; j < Q->n; j++) {
            vector* col_i_new = getColumn(Q, i, n_threads);
            vector* col_j = getColumn(Q, j, n_threads);
            R->values[i][j] = dot_prod(col_i_new, col_j, n_threads);

            for (int k = 0; k < Q->m; k++) {
                Q->values[k][j] -= R->values[i][j] * Q->values[k][i];
            }

            free_vector(&col_i_new);
            free_vector(&col_j);
        }
        free_vector(&col_i);
    }
}


vector* lstsq(matrix* A, vector* b, int n_threads){
    if (A->m < A->n || A->m != b->m)
    {
        return NULL;
    }
    
    matrix* Q = init_matrix(A->m, A->n);
    matrix* R = init_matrix(A->n, A->n);
    

    qr(A, Q, R, n_threads);

    matrix* QT = transp(Q, n_threads);
    free_matrice(&Q);
    vector* b2 = mult_m_v(QT, b, n_threads);
    free_matrice(&QT);


    vector* r = back_sub(R, b2, n_threads);
    free_matrice(&R);
    free_vector(&b2);
    return r;
}


void free_matrice(matrix** m) {
    if (m == NULL || *m == NULL) return;

    if ((*m)->values != NULL) {
        for (int i = 0; i < (*m)->m; i++) {
            free((*m)->values[i]);
            (*m)->values[i] = NULL;
        }
        free((*m)->values);
        (*m)->values = NULL;
    }

    free(*m);
    *m = NULL;
}
