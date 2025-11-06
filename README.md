## Projet Matrix 

Ce projet consiste à réaliser des opérations sur les matrices et vecteurs.
J'ai la version sequentielle et la version concurrente disponible, les tests pour le 2 version peuvent être executés,
mais seule la version concurrente est executable avec l'executable main.

## Lancer le programme

Pour compiler le projet, entrez la commande

```sh
make
```

L'exécutable est alors généré à la racine du projet. Pour l'utiliser, entrez la commande

```sh
./main [-v] [-f output_stream] name_op input_file_A [input_file_B]
```

Les arguments entre crochets sont optionnels.
Les opérations disponibles pour les vecteurs sont : ```add_v_v```, ```sub_v_v```, ```dot_prod``` et ```norm```.
Les opérations disponibles pour les matrices sont : ```add_m_m```, ```sub_m_m```, ```mult_m_v```, ```mult_m_m```, ```transp```, ```back_sub``` et ```lstsq```.

Pour appeler les fonctions ```add_v_v``` et ```sub_v_v```, vous dever passer deux fichiers en argument, contenant les vecteurs encodé comme indiqué ci-bas.
Pareil pour ```add_m_m```, ```sub_m_m```, et ```mult_m_m``` avec les matrices. Pour les fonctions ```mult_m_v```, ```back_sub``` et ```lstsq```, le premier fichier contient une matrice et le deuxième un vecteur.


La commande ```./main -h``` permet d'avoir des explications sur les différents paramètres.

Les fichiers d'entrée et de sortie seront au format binaire. Leur contenu dépend de ce que doit réaliser l’opération.
Si le résultat d’une fonction est un double, le fichier ne contiendra que ce dernier.

Pour ce qui est des vecteurs (que ce soit pour un fichier d’entrée ou de sortie), le fichier contient
dans l’ordre les valeurs suivantes :

- La taille ```m``` du vecteur. Cette valeur est un entier non-signé encodé sur 64 bits en big-endian. Les entiers ne sont pas représentés de la même façon sur tous les ordinateurs. Les fonctions ```htobe64``` et ```be64toh``` vous seront utiles;
- Toutes les valeurs contenues dans le vecteur dans l’ordre (donc d’abord celle à l’indice 0, puis celle à l’indice 1. . .). Les valeurs contenues dans les vecteurs sont des double.

Pour ce qui est des matrices (entrée ou sortie), le fichier contient :
- Le nombre de lignes ```m``` de la matrice (entier non-signé encodé sur 64 bits en big-endian) ;
- Le nombre de colonnes ```n``` de la matrice (entier non-signé encodé sur 64 bits en big-endian).
Notons que si m est nul, alors n est nul et inversément ;
- Les lignes de la matrice. Chaque ligne commence par un ```uint64_t``` (en big-endian) indiquant le numéro de la ligne, et est suivi des n valeurs (double) de cette ligne.

Pour lancer les tests, tapez pour la version sequentielle la commande

```sh
make test_base
```

et pour la version concurrente, la commande

```sh
make test
```

Enfin, pour nettoyer le projet en enlevant les fichiers objets, les fichiers .dat de testes sur les les fichiers et les exécutables, utilisez la commande

```sh
make clean
```

## Arborescence du projet

```
./
├── headers (contient les fichiers headers)
│   ├── file.h
|   ├── matrix_threads.h
│   ├── matrix.h
|   ├── portable_endian.h
|   ├── vector_threads.h
│   ├── vector.h
├── src (contient les fichiers sources)
|   ├── file.c
|   ├── main.c
|   ├── matrix_threads.c
|   ├── matrix.c
|   ├── vector_threads.c
|   ├── vector.c
├── tests (contient les codes des tests)
|   ├── test_files.c
|   ├── test_basic_op.c
|   ├── tests_matrix_threads.c
|   ├── tests_vector_threads.h
├── Makefile
└── README.md
```
