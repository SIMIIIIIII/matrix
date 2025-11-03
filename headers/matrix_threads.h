#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "../headers/vector_threads.h"

#ifndef _MATRIX_H_
#define _MATRIX_H_

typedef struct {
    uint64_t m; // Nombre de lignes de la matrice
    uint64_t n; // Nombre de colonnes de la matrice
    double** values; // Les valeurs contenues dans la matrice
} matrix;

/**
 * Initialise une matrice de taille m x n avec des 0.
 * 
 * @param uint64_t m le nombre de lignes de la matrice
 * @param uint64_t n le nombre de colonnes de la matrice
 * 
 * @return un pointeur vers une matrice de taille m x n initialisée avec des 0.
 */
matrix* init_matrix(uint64_t, uint64_t);

/**
 * Imprime sur la sortie standard le contenu d'une matrice.
 * 
 * @param matrix* un pointeur vers la matrice à imprimer
 * 
 * @result la matrice a été imprimée sur la sortie standard.
 */
void print_matrix(matrix*);

/**
 * Calcule la somme de deux matrices
 * 
 * @param matrix* A la première matrice
 * @param matrix* B la seconde matrice
 * @param char op l'operation + ou moi
 * @param int n_threads le nombre de threads
 * 
 * @return C = A + B
 */
matrix* add_sub_m_m(matrix*, matrix*, char, int);


/**
 * Calcule la multuplication entre une matrice et un vecteur
 * @param matrix* A la matrice
 * @param vector* v le vecteur
 * @param int n_threads le nombre de threads
 * 
 * @return r = A*v
 */
vector* mult_m_v(matrix*, vector*, int);

/**
 * Multiplication de 2 matrices
 * @param matrix* A la matrice
 * @param matrix* B la matrice
 * @param int n_threads le nombre de threads
 *
 * @return C = A*B
 */
matrix* mult_m_m(matrix*, matrix*, int);

/**
 * Transpose la matrice
 * @param matrix* A la matrice à transposer 
 * @param int n_threads le nombre de threads
 * 
 * @return B = A^T
 */
matrix* transp(matrix*, int);

/**
 * La substitution arrère est utilisée pour résoudre un système Ux = b,
 * Savec U une matrice carrée triangulaire supérieure (avec des zéros en dessous de la diagonale principale).
 * @param A* ∈ Rm×m est une matrice triangulaire supérieure
 * @param v* ∈ Rm un vecteur de termes independant
 * @param int n_threads le nombre de threads
 * 
 * @return r = solution(Ux = b)
 */
vector* back_sub(matrix*, vector*, int);

/**
 * La décomposition QR d'une matrice
 * @param A* ∈ Rm×n, matrice à décomposer
 * @param Q* ∈ Rm×n
 * @param R* ∈ Rn×n
 * @param int n_threads le nombre de threads
 * 
 * @return Q une matrice ayant des colonnes orthogonales et R une matrice triangulaire supérieure.
 */
void qr(matrix*, matrix*, matrix*, int);

/**
 * Résolution d’un système linéaire surdéterminé
 * @param A* ∈ Rm×n, matrice à décomposer
 * @param v* ∈ Rm un vecteur de termes independant
 * @param int n_threads le nombre de threads
 * 
 * @return r = solution(Ux = b)
 */
vector* lstsq(matrix*, vector*, int);

matrix* vectors_to_matrix(vector**, int);

void free_matrice(matrix**);

#endif /* _MATRIX_H_ */