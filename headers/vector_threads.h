#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef _VECTOR_H_
#define _VECTOR_H_

typedef struct {
    uint64_t m; // La taille du vecteur
    double* values; // Les valeurs contenues dans le vecteur
} vector;

/**
 * Initialise un vecteur de taille m avec des zéros.
 * 
 * @param uint64_t m la taille du vecteur
 * @param int n_threads le nombre de thread
 * 
 * @return un pointeur vers un vecteur de taille m initialisé avec des 0.
 */
vector* init_vector(uint64_t, int);

/**
 * Imprime sur la sortie standard le contenu d'un vecteur.
 * 
 * @param vector* un pointeur vers le vecteur à imprimer
 * 
 * @result le vecteur a été imprimé sur la sortie standard.
 */
void print_vector(vector*);

/**
 * Calcule la somme ou la soustraction de deux vecteurs
 * 
 * @param vector* x le premier vecteur
 * @param vector* y le second vecteur
 * @param char op l'operation + ou -
 * @param int n_threads nombre de thread
 * 
 * @return z = x +- y
 */
vector* add_sub_v_v(vector*, vector*, char, int);

/**
 * calcule le produit scalaire entre 2 vecteurs
 * @param vector* x le premier vecteur
 * @param vector* y le second vecteur
 * @param int n_threads nombre de thread
 *
 * @return x.y = x[0]y[0] + x[1]y[1] + .. x[n]y[n]
 */
double dot_prod(vector*, vector*, int);

/**
 * Calcule la norme d'un vecteur
 * @param v le vecteur
 * @param int n_threads nombre de thread
 * 
 * @return square(v[0]² + v[1]² + ... + v[n]²)
 */
double norm(vector*, int);

/**
 * multiplie le vecteur par un reel
 * @param v* le vecteur
 * @param s le scalaire
 * @param int n_threads nombre de thread
 *
 * @return un pointeur vers un vecteur contenant v*s
 */
vector* mult_v_scalaire(vector*, double, int);


/**
 * liberer la memoire
 */
void free_vector(vector**);


/**
 * transforme une liste en vecteur
 * @param l[] liste
 * @param n la taille
 * @param int n_threads nombre de thread
 * 
 * @result un pointeur vers un vecteur de taille n contenant tout les éléments de la liste;
 */
vector* liste_to_vector(double*, int, int);

#endif /* _VECTOR_H_ */