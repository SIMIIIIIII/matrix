#include "../headers/matrix.h"

matrix* init_matrix(uint64_t m, uint64_t n) {
    matrix* A = (matrix*)malloc(sizeof(matrix));
    if (A == NULL) {fprintf(stderr, "Problème lors de l'allocation de l'espace mémoire pour une matrice : %s\n", strerror(errno));exit(EXIT_FAILURE);}
    A->m = m; A->n = n;
    A->values = (double**)malloc(m*sizeof(double*));
    if (A->values == NULL) {fprintf(stderr, "Problème lors de l'allocation de l'espace mémoire pour une matrice : %s\n", strerror(errno));free_matrice(&A);exit(EXIT_FAILURE);}
    for (uint64_t i = 0; i < m; i++) {
        A->values[i] = (double*)malloc(n*sizeof(double));
        if (A->values[i] == NULL) {fprintf(stderr, "Problème lors de l'allocation de l'espace mémoire pour une matrice : %s\n", strerror(errno));free_matrice(&A);exit(EXIT_FAILURE);}

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


matrix* add_m_m(matrix* A, matrix* B) {
    if(A == NULL || B == NULL) {fprintf(stderr, "Problème car l'une des matrices est NULL.\n");return NULL;}
    if(A -> n != B -> n || A -> m != B -> m) {fprintf(stderr, "Problème car la matrice et la matrice B n'ont pas les mêmes dimensions.");return NULL;}
    uint64_t m = A->m;
    uint64_t n = A->n;

    matrix* C = init_matrix(m,n);

    if(C == NULL) {fprintf(stderr,"Probème dans l'initialisation de la matrice résultat");return NULL;}

    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            C->values[i][j] = A->values[i][j] + B->values[i][j];
        }
    }
    return C;
}

matrix* sub_m_m(matrix* A, matrix* B){
    if(A == NULL || B == NULL) {fprintf(stderr, "Problème car l'une des matrices est NULL.\n");return NULL;}
    if(A -> n != B -> n || A -> m != B -> m) {fprintf(stderr, "Problème car la matrice A et la matrice B n'ont pas les mêmes dimensions.");return NULL;}
    uint64_t m = A->m;
    uint64_t n = A->n;

    matrix* C = init_matrix(m,n);
    
    if(C == NULL) {fprintf(stderr,"Probème dans l'initialisation de la matrice résultat");return NULL;}

    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            C->values[i][j] = A->values[i][j] - B->values[i][j];
        }
    }
    return C;
}

vector* mult_m_v(matrix* M, vector* v){
    if(M == NULL || v == NULL) {fprintf(stderr,"Problème car la matrice M ou le vecteur v est null.");return NULL;}
    if( M -> n != v -> m) {fprintf(stderr,"Problème de dimensions.");return NULL;}

    vector* r = init_vector(M->m);

    if(r == NULL){fprintf(stderr,"Problème d'allocation pour le vecteur résultat r.");return NULL;}

    for (int i = 0; i < M->m ; i++)
    {
        for (int j = 0; j < M->n; j++)
        {
            r->values[i] += M->values[i][j] * v->values[j];
        } 
    }
    return r;
}


matrix* mult_m_m(matrix* A, matrix* B){
    if(A == NULL || B == NULL) {fprintf(stderr,"Problème car la matrice A ou la matrice B est null.");return NULL;}
    if(A -> n != B -> m) {fprintf(stderr,"Problème car la matrice A et la matrice B n'ont pas des dimensions compatibles pour la multiplication matricielle.");return NULL;}

    matrix* C = init_matrix(A->m, B->n);

    //colone de B
    for (int i = 0; i < B->n ; i++)
    {
        //ligne de A
        for (int j = 0; j < A->m; j++)
        {
            //colonne A
            for (int k = 0; k < A->n; k++)
            {
                
                C->values[j][i] += A->values[j][k] * B->values[k][i];
            } 
        } 
    }
    return C;
}


matrix* transp(matrix* A){
    if(A == NULL){fprintf(stderr,"La mtrice A est null.");return NULL;}

    matrix* B = init_matrix(A->n, A->m);

    if(B == NULL){fprintf(stderr,"Problème lors de l'allocation de mémoire pour la matrice transposée.");return NULL;}

    for (int i = 0; i < A->m; i++)
    {
        for (int j = 0; j < A->n; j++)
        {
            B->values[j][i] = A->values[i][j];
        }
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

vector* back_sub(matrix* A, vector* b){
    if (A->m != A->n || A->n != b->m){
        return NULL;
    }

    vector* r = init_vector(b->m);
    int m = b->m;

    for (int i = m-1; i >= 0; i--)
    {
        r->values[i] = b->values[i];

        for (int j = i + 1; j < m; j++)
        {
            r->values[i] -= A->values[i][j] * r->values[j];
        }

        if (fabs(A->values[i][i]) < DBL_EPSILON  && fabs(r->values[i]) > DBL_EPSILON)
        {
            fprintf(stderr, "Aucune solution");
            exit(EXIT_FAILURE);
        }
        else if (fabs(A->values[i][i]) < DBL_EPSILON  && fabs(r->values[i]) < DBL_EPSILON)
        {
            r->values[i] = (double) rand() / A->values[i][i];
            fprintf(stderr, "Infinité de solutions");
        }
        else
        {
            r->values[i] /= A->values[i][i];
        } 
    }
    return r;
}

void copyMatrix(matrix* A, matrix* B) {
    if (A->m != B->m || A->n != B->n) exit(EXIT_FAILURE);

    for (int i = 0; i < A->m; i++) {
        for (int j = 0; j < B->n; j++) {
            B->values[i][j] = A->values[i][j];
        }
    }
}

vector* getColumn(matrix* A, int column){
    if (column < 0 || column >= A->n) exit(EXIT_FAILURE);
    
    vector* v = init_vector(A->m);

    for (int i = 0; i < A->m; i++)
    {
        v->values[i] = A->values[i][column];
    }
    return v;
}

void qr(matrix* A, matrix* Q, matrix* R) {
    if (Q->n != R->m ){
        exit(EXIT_FAILURE);
    }

    // s'assurer que R soi initialiser à 0
    for (int i = 0; i < R->m; i++) {
        for (int j = 0; j < R->n; j++) {
            R->values[i][j] = 0.0;
        }
    }

    // Copier A à Q
    copyMatrix(A, Q);

    for (int i = 0; i < A->n; i++) {
        vector* col_i = getColumn(Q, i);
        R->values[i][i] = norm(col_i);

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
            vector* col_i_new = getColumn(Q, i);
            vector* col_j = getColumn(Q, j);
            R->values[i][j] = dot_prod(col_i_new, col_j);

            for (int k = 0; k < Q->m; k++) {
                Q->values[k][j] -= R->values[i][j] * Q->values[k][i];
            }

            free_vector(&col_i_new);
            free_vector(&col_j);
        }
        free_vector(&col_i);
    }
}

vector* lstsq(matrix* A, vector* b){
    if (A->m < A->n || A->m != b->m)
    {
        return NULL;
    }
    
    matrix* Q = init_matrix(A->m, A->n);
    matrix* R = init_matrix(A->n, A->n);
    

    qr(A, Q, R);

    matrix* QT = transp(Q);
    free_matrice(&Q);
    vector* b2 = mult_m_v(QT, b);
    free_matrice(&QT);


    vector* r = back_sub(R, b2);
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
