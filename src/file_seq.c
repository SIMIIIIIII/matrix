#include "../headers/file_seq.h"

double read_double(FILE* file){
    if (file != NULL)
    {   
        double r[1];
        int flag = fread(r, sizeof(double), 1, file);
        if (flag != 1) {
            fprintf(stderr, "echec de lecture du double dans le fichier d'entrée : %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        return r[0];
    }
    else exit(EXIT_FAILURE);
}


int write_double(double x, FILE* file){
    if (file != NULL)
    {
        int flag = fwrite(&x, sizeof(double), 1, file);
        if (flag != 1) {
            fprintf(stderr, "echec d'ecriture du double dans le fichier de sortie : %s\n", strerror(errno));
            return -1;
        }
        return 0;
    }
    else return -1;
}


vector* read_vector(FILE* file) {
    if (file == NULL) {fprintf(stderr, "le fichier est null : %s\n", strerror(errno)); return NULL;}

    uint64_t m;
    if (fread(&m, sizeof(uint64_t), 1, file) != 1) {
        fprintf(stderr, "echec de lecture de la taille dans le fichier d'entrée: %s\n", strerror(errno));
        return NULL;
    }
    m = be64toh(m);

    vector* v = init_vector(m);
    if (v == NULL) return NULL;

    if (fread(v->values, sizeof(double), v->m, file) != v->m) {
        free_vector(&v);
        fprintf(stderr, "echec de lecture de lecture du vecteur dans le fichier d'entrée: %s\n", strerror(errno));
        return NULL;
    }
    return v;
}


int write_vector(vector* v, FILE* file) {
    if (file == NULL || v == NULL) {
        fprintf(stderr, "fichier ou vecteur de sortie est null : %s\n", strerror(errno)); 
        return -1;
    }
    
    uint64_t m = htobe64(v->m);
    if (fwrite(&m, sizeof(uint64_t), 1, file) != 1) {
        fprintf(stderr, "echec d'ecriture de la taille du vecteur dans le fichier de sortie: %s\n", strerror(errno));
        return -1;
    }
    if (fwrite(v->values, sizeof(double), v->m, file) != v->m) {
        fprintf(stderr, "echec d'ecriture du vecteur dans le fichier de sortie: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}


matrix *read_matrix(FILE* file){
    if (file == NULL) return NULL;

    int64_t m;
    if (fread(&m, sizeof(uint64_t), 1, file) != 1) {
        fprintf(stderr, "echec de lecture du nombre de lignes dans le fichier d'entrée : %s\n", strerror(errno));
        return NULL;
    }
    m = be64toh(m);

    int64_t n;
    if (fread(&n, sizeof(uint64_t), 1, file) != 1) {
        fprintf(stderr, "echec de lecture du nombre des colonnes dans le fichier d'entrée : %s\n", strerror(errno));
        return NULL;}
    n = be64toh(n);

    matrix* M = init_matrix(m, n);
    if (M == NULL) return NULL;
    

    for (int i = 0; i < M->m; i++)
    {   
        uint64_t j;
        if (fread(&j, sizeof(uint64_t), 1, file) != 1)
        {
            free_matrice(&M);
            fprintf(stderr, "echec de lecture de la la matrice dans le fichier de sortie : %s\n", strerror(errno));
            return NULL;
        }
        j = be64toh(j);

        if (fread(M->values[j], sizeof(double), M->n, file) != M->n)
        {
            free_matrice(&M);
            fprintf(stderr, "echec de lecture de la la matrice dans le fichier de sortie : %s\n", strerror(errno));
            return NULL;
        }
    }
    return M;
}


int write_matrix(matrix* M, FILE* file){
    if (file == NULL) {
        fprintf(stderr, "le fichier de sortie est NULL : %s\n", strerror(errno));    
        return -1;
    }

    uint64_t m = htobe64(M->m);
    if (fwrite(&m, sizeof(uint64_t), 1, file) != 1) {
        fprintf(stderr, "echec d'ecriture de nombre de lignes dans le fichier de sortie : %s\n", strerror(errno));
        return -1;
    }

    uint64_t n = htobe64(M->n);
    if (fwrite(&n, sizeof(uint64_t), 1, file) != 1) {
        fprintf(stderr, "echec d'ecriture de nombre des colonnes dans le fichier de sortie : %s\n", strerror(errno));
        return -1;
    }
    for (int i = 0; i < M->m; i++)
    {
        uint64_t j = htobe64(i);
        if (fwrite(&j, sizeof(uint64_t), 1, file) != 1) {
            fprintf(stderr, "echec d'ecriture de la matrice dans le fichier de sortie : %s\n", strerror(errno));
            return -1;}
        if (fwrite(M->values[i], sizeof(double), M->n, file) != M->n) {
            fprintf(stderr, "echec d'ecriture de la matrice dans le fichier de sortie : %s\n", strerror(errno));
            return -1;
    }}
    return 0;
}


int read_QR(FILE* file, matrix* Q, matrix* R){
    if (Q->n != R->m || R->m != R->n) return -1;

    uint64_t m;
    if (fread(&m, sizeof(uint64_t), 1, file) != 1) return -1;
    if (be64toh(m) != Q->m) return -1;

    uint64_t n;
    if (fread(&n, sizeof(uint64_t), 1, file) != 1) return -1;
    if (be64toh(n) != Q->n) return -1;

    for (int i = 0; i < Q->m; i++)
    {   
        uint64_t j;
        if (fread(&j, sizeof(uint64_t), 1, file) != 1) return -1;
        j = be64toh(j);
        if (fread(Q->values[j], sizeof(double), Q->n, file) != Q->n) return -1;
    }

    for (int i = 0; i < R->m; i++)
    {   
        uint64_t j;
        if (fread(&j, sizeof(uint64_t), 1, file) != 1) return -1;
        j = be64toh(j);
        if (fread(R->values[j], sizeof(double), R->n, file) != R->n) return -1;
    }
    return 0;
}


int write_QR(matrix* Q, matrix* R, FILE* file){
    if (Q->n != R->m || R->m != R->n) return -1;
    write_matrix(Q, file);

    for (int i = 0; i < R->m; i++)
    {
        int64_t j = htobe64(i);
        if (fwrite(&j, sizeof(uint64_t), 1, file) != 1) return -1;
        if (fwrite(Q->values[i], sizeof(double), Q->n, file) != Q->n) return -1;
    }
    return 0;
}