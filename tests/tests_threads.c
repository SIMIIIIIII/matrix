#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <time.h>
#include <stdio.h>

#include "../headers/vector_threads.h"
#include "../headers/matrix_threads.h"


void test_add_sub_v_v(void){
    srand(time(NULL));
    vector* v = init_vector(1000, 4);
    vector* w = init_vector(1000, 4);
    if (w == NULL){
        fprintf(stderr, "c'est icicicicicicicicicic.\n");
    }
    
    double min = 1.0;
    double max = 10.0;
    
    for (int i = 0; i < 1000; i++)
    {
        v->values[i] = min + ((double)rand() / RAND_MAX) * (max - min);
        w->values[i] = min + ((double)rand() / RAND_MAX) * (max - min);
    }
    

    vector* plus = add_sub_v_v(v, w, '+', 4);
    vector* moin = add_sub_v_v(v, w, '-', 4);

    for (int i = 0; i < 1000; i++){
        CU_ASSERT_DOUBLE_EQUAL(plus->values[i], v->values[i] + w->values[i], 1e-3);
        CU_ASSERT_DOUBLE_EQUAL(moin->values[i], v->values[i] - w->values[i], 1e-3);
    }
    
    free_vector(&v);
    free_vector(&w);
    free_vector(&plus);
    free_vector(&moin);

    vector* v2 = init_vector(3, 1);
    vector* w2 = init_vector(2, 1);

    CU_ASSERT_PTR_NULL(add_sub_v_v(NULL, NULL, '+', -1));
    CU_ASSERT_PTR_NULL(add_sub_v_v(NULL, v2, '-', 0));
    CU_ASSERT_PTR_NOT_NULL(add_sub_v_v(v2, v2, '+', 0));
    CU_ASSERT_PTR_NULL(add_sub_v_v(w2, v2, '-', 2));
    CU_ASSERT_PTR_NULL(add_sub_v_v(v2, NULL, '+', 3));
    free_vector(&v2);
    free_vector(&w2);
}


void test_dot_prod(void){
    srand(time(NULL));
    vector* v = init_vector(1000, 4);
    vector* w = init_vector(1000, 4);
    
    double min = 1.0;
    double max = 10.0;
    
    for (int i = 0; i < v->m; i++)
    {
        v->values[i] = min + ((double)rand() / RAND_MAX) * (max - min);
        w->values[i] = min + ((double)rand() / RAND_MAX) * (max - min);
    }

    double r = 0;
    for (int i = 0; i < v->m; i++)
    {
        r += v->values[i] * w->values[i];
    }
    
    CU_ASSERT_DOUBLE_EQUAL(dot_prod(v, w, 4), r, 1e-3)

    vector* u = init_vector(900, 4);
    for (int i = 0; i < u->m; i++)
    {
        w->values[i] = i;
    }
    CU_ASSERT_DOUBLE_EQUAL(dot_prod(u, w, 4), 0, 1e-3);
    free_vector(&u);
    free_vector(&v);
    free_vector(&w);
}


void test_norm(void){
    srand(time(NULL));
    vector* v = init_vector(1000, 4);
    
    double min = 1.0;
    double max = 10.0;
    
    for (int i = 0; i < 1000; i++)
    {
        v->values[i] = min + ((double)rand() / RAND_MAX) * (max - min);
    }

    double r = 0;
    for (int i = 0; i < 1000; i++)
    {
        r += v->values[i] * v->values[i];
    }
    
    CU_ASSERT_DOUBLE_EQUAL(norm(v, 4), sqrt(r), 1e-3);

    free_vector(&v);
}


void test_mult_v_scalaire(void){
    srand(time(NULL));
    vector* v = init_vector(1000, 4);
    
    double min = 1.0;
    double max = 10.0;
    
    for (int i = 0; i < 1000; i++)
    {
        v->values[i] = min + ((double)rand() / RAND_MAX) * (max - min);
    }

    vector* z = mult_v_scalaire(v, 2.3, 4);
    for (int i = 0; i < v->m; i++)
    {
        CU_ASSERT_DOUBLE_EQUAL(z->values[i], v->values[i]*2.3, 1e-3);
    }
    

    free_vector(&v);
    free_vector(&z);
}


void test_add_sub_m_m(void){
    matrix* A = init_matrix(1000, 1000);
    matrix* B = init_matrix(1000, 1000);

    srand(time(NULL));
    double min = 1.0;
    double max = 10.0;

    for (int i = 0; i < A->m; i++)
    {
        for (int j = 0; j < A->n; j++)
        {
            A->values[i][j] = min + ((double)rand() / RAND_MAX) * (max - min);
            B->values[i][j] = min + ((double)rand() / RAND_MAX) * (max - min);
        }
    }


    matrix* R1 = add_sub_m_m(A, B, '+', 4);
    matrix* R2 = add_sub_m_m(A, B, '-', 4);

    for (int i = 0; i < A->m; i++)
    {
        for (int j = 0; j < A->n; j++)
        {
            CU_ASSERT_DOUBLE_EQUAL(R1->values[i][j], A->values[i][j] + B->values[i][j], 1e-3);
            CU_ASSERT_DOUBLE_EQUAL(R2->values[i][j], A->values[i][j] - B->values[i][j], 1e-3);
        }
    }

    matrix* C = init_matrix(1,3);
    CU_ASSERT_PTR_NULL(add_sub_m_m(C, A, '+', 4));
    CU_ASSERT_PTR_NULL(add_sub_m_m(C, A, '-', 4));
    CU_ASSERT_PTR_NULL(add_sub_m_m(C, NULL, '+', 4));
    CU_ASSERT_PTR_NULL(add_sub_m_m(C, NULL, '-', 4));
    CU_ASSERT_PTR_NULL(add_sub_m_m(NULL, NULL, '-', 4));
    CU_ASSERT_PTR_NULL(add_sub_m_m(NULL, NULL, '+', 4));
    CU_ASSERT_PTR_NULL(add_sub_m_m(NULL, C, '-', 4));
    CU_ASSERT_PTR_NULL(add_sub_m_m(NULL, C, '+', 4));

    free_matrice(&C);
    free_matrice(&A);
    free_matrice(&B);
    free_matrice(&R1);
    free_matrice(&R2);
}


void test_mult_m_v(void){
    srand(time(NULL));

    matrix* M = init_matrix(750, 1000);
    vector* v = init_vector(1000, 4);
    vector* expected = init_vector(750, 4);

    double min = 1.0;
    double max = 10.0;

    for (int i = 0; i < M->m; i++)
    {
        for (int j = 0; j < M->n; j++)
        {
            if (i == 0)
            {
                v->values[j] = min + ((double)rand() / RAND_MAX) * (max - min);
            }
            
            M->values[i][j] = min + ((double)rand() / RAND_MAX) * (max - min);
            expected->values[i] += M->values[i][j] * v->values[j];
        }
    }

    vector* r = mult_m_v(M, v, 4);

    for (int i = 0; i < expected->m; i++)
    {
        CU_ASSERT_DOUBLE_EQUAL(r->values[i], expected->values[i], 1e-3);
    }

    free_vector(&expected);
    free_vector(&r);
    
    matrix* A = init_matrix(5, 3);
    vector* v2 = init_vector(1, 4);

    CU_ASSERT_PTR_NULL(mult_m_v(A, v2, 3));
    CU_ASSERT_PTR_NOT_NULL(mult_m_v(M, v, 0));
    CU_ASSERT_PTR_NOT_NULL(mult_m_v(M, v , -1));
    CU_ASSERT_PTR_NULL(mult_m_v(NULL, v2, 5));
    CU_ASSERT_PTR_NULL(mult_m_v(NULL, NULL, 3));
    CU_ASSERT_PTR_NULL(mult_m_v(A, NULL, 3));

    free_matrice(&A);
    free_matrice(&M);
    free_vector(&v);
    free_vector(&v2);
}


void test_mult_m_m(void){
    matrix* A = init_matrix(800, 1000);
    matrix* B = init_matrix(1000, 700);
    matrix* expected = init_matrix(800, 700);

    srand(time(NULL));
    double min = 1.0;
    double max = 10.0;

    for (int i = 0; i < A->n; i++)
    {
        for (int j = 0; j < A->n; j++)
        {
            if (i < A->m )
            {
                A->values[i][j] = min + ((double)rand() / RAND_MAX) * (max - min);
            }
            if (j < B->n)
            {
                B->values[i][j] = min + ((double)rand() / RAND_MAX) * (max - min);
            }  
        }
    }

    for (int i = 0; i < B->n ; i++)
    {
        //ligne de A
        for (int j = 0; j < A->m; j++)
        {
            //colonne A
            for (int k = 0; k < A->n; k++)
            {
                
                expected->values[j][i] += A->values[j][k] * B->values[k][i];
            } 
        } 
    }
    
    matrix* R = mult_m_m(A, B, 4);

    for (int i = 0; i < R->m; i++)
    {
        for (int j = 0; j < R->n; j++)
        {
            CU_ASSERT_DOUBLE_EQUAL(R->values[i][j], expected->values[i][j], 1e-3);
        }
    }

    free_matrice(&R);
    free_matrice(&expected);

    matrix* M = init_matrix(1,2);
    CU_ASSERT_PTR_NOT_NULL(mult_m_m(A, B, 0));
    CU_ASSERT_PTR_NOT_NULL(mult_m_m(A, B, -1));
    CU_ASSERT_PTR_NULL(mult_m_m(A, M, 3));
    CU_ASSERT_PTR_NULL(mult_m_m(M, A, 2));
    CU_ASSERT_PTR_NULL(mult_m_m(NULL, M, 3));
    CU_ASSERT_PTR_NULL(mult_m_m(NULL, NULL, 3));
    CU_ASSERT_PTR_NULL(mult_m_m(M, NULL, 2));
    
    free_matrice(&A);
    free_matrice(&B);
    free_matrice(&M);
}


void test_transp(void){
    matrix* A = init_matrix(1000, 1000);

    srand(time(NULL));
    double min = 1.0;
    double max = 10.0;

    for (int i = 0; i < A->m; i++)
    {
        for (int j = 0; j < A->n; j++)
        {
            A->values[i][j] = min + ((double)rand() / RAND_MAX) * (max - min);
        }
    }

    matrix* B = transp(A, 4);

    for (int i = 0; i < B->m; i++)
    {
        for (int j = 0; j < B->n; j++)
        {
            CU_ASSERT_DOUBLE_EQUAL(B->values[i][j], A->values[j][i], 1e-3);
        }
    }

    CU_ASSERT_PTR_NULL(transp(NULL, 2));
    free_matrice(&A); free_matrice(&B);
}


void test_back_sub(void){
    srand(time(NULL));
    matrix* A = init_matrix(2,2);
    vector* v = init_vector(2, 1);

    A->values[0][0] = 1;
    A->values[0][1] = 2;
    A->values[1][1] = 3;
    v->values[0] = 5;
    v->values[1] = 3;

    vector* r = back_sub(A, v, 1);
    CU_ASSERT_DOUBLE_EQUAL(r->values[0], 3, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(r->values[1], 1, 1e-3);

    free_matrice(&A);
    free_vector(&v);
    free_vector(&r);

    matrix* A2 = init_matrix(3,3);
    vector* v2 = init_vector(3, 1);
    
    double a[] = {1,1,-2,0,1,-1,0,0,3};
    double v3[] = {-3, -1, 9};

    int k = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            A2->values[i][j] = a[k];
            k++;
        }
    }

    for (int i = 0; i < 3; i++)
    {
        v2->values[i] = v3[i];
    }

    vector* r2 = back_sub(A2, v2, 1);

    for (int i = 0; i < 3; i++)
    {
        CU_ASSERT_DOUBLE_EQUAL(r2->values[i], i+1, 1e-3);
    }
    
    vector* D = init_vector(1, 1);
    CU_ASSERT_PTR_NULL(back_sub(A2, D, 1));
    CU_ASSERT_PTR_NULL(back_sub(NULL, D, 1));
    CU_ASSERT_PTR_NULL(back_sub(NULL, NULL, 1));
    CU_ASSERT_PTR_NULL(back_sub(A2, NULL, 1));

    free_vector(&D);
    free_matrice(&A2);
    free_vector(&v2);
    free_vector(&r2);
}


void test_qr(void){
    matrix* A = init_matrix(3, 3);
    matrix* Q = init_matrix(3, 3);
    matrix* R = init_matrix(3, 3);

    srand(time(NULL));

    double a[3][3] = {
        {1,1,1},
        {1,2,3},
        {1, 3, 5}};
    
    for (int i = 0; i < A->m; i++)
    {
        for (int j = 0; j < A->n; j++)
        {
            A->values[i][j] = a[i][j];
        }
    }
    qr(A, Q, R, 2);

    matrix* temp = init_matrix(3, 3);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            temp->values[i][j] = 0.0;
            for (int k = 0; k < 3; k++) {
                temp->values[i][j] += Q->values[i][k] * R->values[k][j];
            }
            CU_ASSERT_DOUBLE_EQUAL(temp->values[i][j], a[i][j], 1e-3);
        }
    }

    free_matrice(&temp);
    free_matrice(&A);
    free_matrice(&Q);
    free_matrice(&R);
}


void test_lstsq(void){
    srand(time(NULL));
    matrix* A = init_matrix(3, 2);
    vector* v = init_vector(3, 3);

    double a[3][2] = {
        {2, 1},
        {1, -1},
        {1, 2}};

    double v2[3] = {4, 1, 5};

    for (int i = 0; i < A->m; i++)
    {
        v->values[i] = v2[i];
        for (int j = 0; j < A->n; j++) A->values[i][j] = a[i][j];
    }
    
    vector* r = lstsq(A, v, 3);
    CU_ASSERT_PTR_NOT_NULL(r);
    CU_ASSERT_EQUAL(r->m, 2);

    double r2[2] = {1.6667, 1.3333};
    for (int i = 0; i < r->m; i++) CU_ASSERT_DOUBLE_EQUAL(r->values[i], r2[i], 1e-3);
    
    vector* v3 = init_vector(2, 3);
    CU_ASSERT_PTR_NULL(lstsq(A, v3, 3));

    free_matrice(&A);
    free_vector(&v);
    free_vector(&r);
    free_vector(&v3);
}

#include "../headers/file.h"

void test_file(void){
    //write double
    double x = 3.5;
    FILE* file = fopen("test.dat", "wb");
    CU_ASSERT_PTR_NOT_NULL(file);
    CU_ASSERT_EQUAL(write_double(x, file), 0);
    fclose(file);

    //read double
    FILE* file2 = fopen("test.dat", "rb");
    CU_ASSERT_DOUBLE_EQUAL(read_double(file2), x, 1e-3);
    fclose(file2);

    CU_ASSERT_EQUAL(write_double(x, NULL), -1);
}

void test_write_vector(void){
    //write vector
    srand(time(NULL));
    int n_threads = 1 + ((int)rand() / RAND_MAX) * (1000 - 1);
    vector* v = init_vector(5, n_threads);
    double v2[5] = {3,-4,2,8,-1};
    
    for (int i = 0; i < 5; i++) v->values[i] = v2[i];
    
    FILE* file = fopen("test.dat", "wb");
    CU_ASSERT_EQUAL(write_vector(v, file), 0);
    fclose(file);
    free_vector(&v);

    //read vector
    FILE* file2 = fopen("test.dat", "rb");
    vector* v3 = read_vector(file2, n_threads);
    CU_ASSERT_PTR_NOT_NULL(v3)
    CU_ASSERT_EQUAL(write_vector(NULL, file2), -1);
    fclose(file2);
    if (v3 != NULL && v3->values != NULL){
        for (int i = 0; i < v3->m; i++) CU_ASSERT_DOUBLE_EQUAL(v3->values[i], v2[i], 1e-3);
    }
    CU_ASSERT_EQUAL(write_vector(v3, NULL), -1);
    
    free_vector(&v3);
}

void test_write_matrix(void){
    matrix* M = init_matrix(2, 3);
    double m[2][3] = {{2.3, 0.1, 3}, {2.5, 5.0, 1.2}};

    for (int i = 0; i < M->m; i++)
    {
        for (int j = 0; j < M->n; j++) M->values[i][j] = m[i][j];
    }

    FILE* file = fopen("test.dat", "wb");
    write_matrix(M, file);
    fclose(file);
    free_matrice(&M);

    FILE* file2 = fopen("test.dat", "rb");
    matrix* M2 = read_matrix(file2);
    fclose(file2);
    
    for (int i = 0; i < M2->m; i++)
    {
        for (int j = 0; j < M2->n; j++) CU_ASSERT_DOUBLE_EQUAL(m[i][j], M2->values[i][j], 1e-3);
    }
    free_matrice(&M2);
}

void test_write_qr(void){
    matrix* Q = init_matrix(3, 2);
    matrix* R = init_matrix(2, 2);

    FILE* file = fopen("test.dat", "wb");
    write_QR(Q, R, file);
    fclose(file);
    free_matrice(&R); free_matrice(&Q);

    matrix* Q2 = init_matrix(3, 2);
    matrix* R2 = init_matrix(2, 2);

    FILE* file2 = fopen("test.dat", "rb");
    read_QR(file2, Q2, R2);
    fclose(file2);

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (i < 2) CU_ASSERT_DOUBLE_EQUAL(0, R2->values[i][j], 1e-3)
            CU_ASSERT_DOUBLE_EQUAL(0, Q2->values[i][j], 1e-3);
        }
    }
    free_matrice(&R2); free_matrice(&Q2);
}


int main(int argc, char** argv) {
    srand(time(NULL));
    if (CUE_SUCCESS != CU_initialize_registry()) {return CU_get_error();}
    CU_pSuite test_thread = CU_add_suite("test_thread", NULL, NULL);
    if (test_thread == NULL) {CU_cleanup_registry(); return CU_get_error();}
    if ((CU_add_test(test_thread, "add_sub_v_v", test_add_sub_v_v) == NULL) ||
    (CU_add_test(test_thread, "dot_prod", test_dot_prod) == NULL) ||
    (CU_add_test(test_thread, "norm", test_norm) == NULL) || 
    (CU_add_test(test_thread, "mult_v_scalaire", test_mult_v_scalaire) == NULL) ||
    (CU_add_test(test_thread, "add_sub_m_m", test_add_sub_m_m) == NULL) ||
    (CU_add_test(test_thread, "mult_m_v", test_mult_m_v) == NULL) || 
    (CU_add_test(test_thread, "mult_m_m", test_mult_m_m) == NULL) || 
    (CU_add_test(test_thread, "transp", test_transp) == NULL) || 
    (CU_add_test(test_thread, "back_sub", test_back_sub) == NULL) || 
    (CU_add_test(test_thread, "qr", test_qr) == NULL) || 
    (CU_add_test(test_thread, "lstsq", test_lstsq) == NULL) ||
    (CU_add_test(test_thread, "file", test_file) == NULL) ||
    (CU_add_test(test_thread, "write_vector", test_write_vector) == NULL) ||
    (CU_add_test(test_thread, "write_matrix", test_write_matrix) == NULL) /*||
    (CU_add_test(test_thread, "write_qr", test_write_qr) == NULL)*/) {
        CU_cleanup_registry(); return CU_get_error();};

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_basic_show_failures(CU_get_failure_list());
    int failed_tests = CU_get_number_of_failures();
    CU_cleanup_registry();
    return (failed_tests > 0) ? 1 : 0;
}