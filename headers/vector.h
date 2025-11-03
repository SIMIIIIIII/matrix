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
 * 
 * @return un pointeur vers un vecteur de taille m initialisé avec des 0.
 */
vector* init_vector(uint64_t);

/**
 * Imprime sur la sortie standard le contenu d'un vecteur.
 * 
 * @param vector* un pointeur vers le vecteur à imprimer
 * 
 * @result le vecteur a été imprimé sur la sortie standard.
 */
void print_vector(vector*);

/**
 * Calcule la somme de deux vecteurs
 * 
 * @param vector* x le premier vecteur
 * @param vector* y le second vecteur
 * 
 * @return z = x + y
 */
vector* add_v_v(vector*, vector*);

/**
 * Calcule la difference entre deux vecteurs
 * 
 * @param vector* x le premier vecteur
 * @param vector* y le second vecteur
 * 
 * @return z = x - y
 */
vector* sub_v_v(vector*, vector*);

/**
 * calcule le produit scalaire entre 2 vecteurs
 * @param vector* x le premier vecteur
 * @param vector* y le second vecteur
 *
 * @return x.y = x[0]y[0] + x[1]y[1] + .. x[n]y[n]
 */
double dot_prod(vector*, vector*);

/**
 * Calcule la norme d'un vecteur
 * @param v le vecteur
 * @return square(v[0]² + v[1]² + ... + v[n]²)
 */
double norm(vector*);

/**
 * multiplie le vecteur par un reel
 * @param v* le vecteur
 * @param s le scalaire
 *
 * @return un pointeur vers un vecteur contenant v*s
 */
vector* mult_v_scalaire(vector*, double);

/**
 * liberer la memoire
 */
void free_vector(vector**);

/**
 * transforme une liste en vecteur
 * @param l[] liste
 * @param n la taille
 * 
 * @result un pointeur vers un vecteur de taille n contenant tout les éléments de la liste;
 */
vector* liste_to_vector(double*, int);

double mult_v_v(vector*, vector*);

#endif /* _VECTOR_H_ */