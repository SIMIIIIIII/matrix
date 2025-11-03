#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <time.h>
#include <stdio.h>

#include "../headers/vector.h"
#include "../headers/matrix.h"
#include "../headers/file_seq.h"

void test_add_sub_v_v(void) {
    uint64_t m = 1000;
    vector* v = init_vector(m);
    vector* w = init_vector(m);
    for (uint64_t i = 0; i < m; i++) {
        v->values[i] = (double) rand()/2.0;
        w->values[i] = (double) rand()/2.0;
    }
    vector* z = add_v_v(v, w);
    vector* z2 = sub_v_v(v, w);
    
    for (uint64_t i = 0; i < m; i++) {
        CU_ASSERT_DOUBLE_EQUAL(z->values[i], v->values[i] + w->values[i], 1e-3);
        CU_ASSERT_DOUBLE_EQUAL(z2->values[i], v->values[i] - w->values[i], 1e-3);
    }

    CU_ASSERT_PTR_NULL(add_v_v(NULL, v));
    CU_ASSERT_PTR_NULL(add_v_v(w, NULL));
    CU_ASSERT_PTR_NULL(add_v_v(NULL, NULL));
    vector* x = init_vector(3);
    CU_ASSERT_PTR_NULL(add_v_v(v, x));
    CU_ASSERT_PTR_NULL(sub_v_v(NULL, v));
    CU_ASSERT_PTR_NULL(sub_v_v(w, NULL));
    CU_ASSERT_PTR_NULL(sub_v_v(NULL, NULL));
    vector* y = init_vector(3);
    CU_ASSERT_PTR_NULL(sub_v_v(v, x));

    free_vector(&v); free_vector(&w); free_vector(&z); free_vector(&x);
    free_vector(&z); free_vector(&y);

}


void test_dot_prod(void){
    vector* u = init_vector(1000);
    vector* v = init_vector(1000);

    for (int i = 0; i < u->m; i++)
    {
        u->values[i] = i;
        v->values[i] = i;
    }

    double j = 0;
    for (int i = 0; i < u->m; i++)
    {
        j+= v->values[i] * u->values[i];
    }
    CU_ASSERT_DOUBLE_EQUAL(dot_prod(u, v), j, 1e-3);

    vector* w = init_vector(900);
    for (int i = 0; i < w->m; i++)
    {
        w->values[i] = i;
    }
    CU_ASSERT_DOUBLE_EQUAL(dot_prod(u, w), 0, 1e-3);
    
    free_vector(&u);
    free_vector(&v);
    free_vector(&w);
}

void test_norm(void){
    vector* w = init_vector(1000);
    for (int i = 0; i < w->m; i++)
    {
        w->values[i] = i;
    }
    double r = 0;
    for (int i = 0; i < w->m; i++)
    {
        r += w->values[i] * w->values[i];
    }
    CU_ASSERT_DOUBLE_EQUAL(norm(w), sqrt(r), 1e-3);

    free_vector(&w);
}

void test_add_sub_m_m(void) {
    uint64_t m = 1000; uint64_t n = 1000;
    matrix* A = init_matrix(m, n);
    matrix* B = init_matrix(m, n);
    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            A->values[i][j] = (double) rand()/2.0;
            B->values[i][j] = (double) rand()/2.0;
        }
    }
    matrix* C = add_m_m(A, B);
    matrix* C2 = sub_m_m(A, B);
    
    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            CU_ASSERT_DOUBLE_EQUAL(C->values[i][j], A->values[i][j] + B->values[i][j], 1e-3);
            CU_ASSERT_DOUBLE_EQUAL(C2->values[i][j], A->values[i][j] - B->values[i][j], 1e-3);
        }
    }
    matrix* D = init_matrix(1, 1);
    CU_ASSERT_PTR_NULL(add_m_m(A, D));
    CU_ASSERT_PTR_NULL(sub_m_m(D, A));
    CU_ASSERT_PTR_NULL(add_m_m(D, NULL));
    CU_ASSERT_PTR_NULL(sub_m_m(D, NULL));
    CU_ASSERT_PTR_NULL(sub_m_m(NULL, NULL));
    CU_ASSERT_PTR_NULL(add_m_m(NULL, NULL));
    CU_ASSERT_PTR_NULL(sub_m_m(NULL, C));
    CU_ASSERT_PTR_NULL(add_m_m(NULL, C));

    free_matrice(&C);
    free_matrice(&A); free_matrice(&B); free_matrice(&C); free_matrice(&D);
    free_matrice(&C2);
}


void test_mult_m_v(void){
    matrix* M = init_matrix(750, 1000);
    vector* v = init_vector(1000);
    vector* expected = init_vector(750);

    for (int i = 0; i < M->m; i++)
    {
        for (int j = 0; j < M->n; j++)
        {
            if (i == 0)
            {
                v->values[j] = i;
            }
            
            M->values[i][j] = i;
            expected->values[i] += M->values[i][j] * v->values[j];
        }
    }


    vector* r = mult_m_v(M, v);
    for (int i = 0; i < expected->m; i++)
    {
        CU_ASSERT_DOUBLE_EQUAL(r->values[i], expected->values[i], 1e-3);
    }

    matrix* A = init_matrix(5, 3);
    vector* v2 = init_vector(1);

    CU_ASSERT_PTR_NULL(mult_m_v(A, v2));
    CU_ASSERT_PTR_NOT_NULL(mult_m_v(M, v));
    CU_ASSERT_PTR_NOT_NULL(mult_m_v(M, v));
    CU_ASSERT_PTR_NULL(mult_m_v(NULL, v2));
    CU_ASSERT_PTR_NULL(mult_m_v(NULL, NULL));
    CU_ASSERT_PTR_NULL(mult_m_v(A, NULL));

    free_matrice(&A);
    free_matrice(&M);
    free_vector(&v);
    free_vector(&v2);

    free_matrice(&M); free_vector(&v); free_vector(&r);
    free_vector(&expected);
}

void test_mult_m_m(void){
    matrix* A = init_matrix(800,1000);
    matrix* B = init_matrix(1000,700);
    matrix* expected = init_matrix(800, 700);

    for (int i = 0; i < A->n; i++)
    {
        for (int j = 0; j < A->n; j++)
        {
            if (i < A->m )
            {
                A->values[i][j] = i;
            }
            if (j < B->n)
            {
                B->values[i][j] = i;
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

    matrix* C = mult_m_m(A, B);

    for (int i = 0; i < C->m; i++)
    {
        for (int j = 0; j < C->n; j++)
        {
            CU_ASSERT_DOUBLE_EQUAL(C->values[i][j], expected->values[i][j], 1e-3);
        }
    }

    free_matrice(&expected);

    matrix* M = init_matrix(1,2);
    CU_ASSERT_PTR_NOT_NULL(mult_m_m(A, B));
    CU_ASSERT_PTR_NOT_NULL(mult_m_m(A, B));
    CU_ASSERT_PTR_NULL(mult_m_m(A, M));
    CU_ASSERT_PTR_NULL(mult_m_m(M, A));
    CU_ASSERT_PTR_NULL(mult_m_m(NULL, M));
    CU_ASSERT_PTR_NULL(mult_m_m(NULL, NULL));
    CU_ASSERT_PTR_NULL(mult_m_m(M, NULL));
    
    free_matrice(&A);
    free_matrice(&B);
    free_matrice(&M);
    free_matrice(&C);
}

void test_transp(void){
    matrix* A = init_matrix(1000,1000);

    double x = 0;

    for (int i = 0; i < A->m; i++)
    {
        for (int j = 0; j < A->n; j++)
        {
            A->values[i][j] = x;
            x++;
        }
    }

    matrix* B = transp(A);

    for (int i = 0; i < A->m; i++)
    {
        for (int j = 0; j < A->n; j++)
        {
            CU_ASSERT_DOUBLE_EQUAL(A->values[i][j], B->values[j][i], 1e-3);
        }
    }
    CU_ASSERT_PTR_NULL(transp(NULL));

    free_matrice(&A);
    free_matrice(&B);
}

void test_liste_to_vector(void){
    vector* v = init_vector(15);
    double l[15];

    for (int i = 0; i < 15; i++) l[i] = (double) rand()/2.0;

    liste_to_vector(l, 15);

    for (int i = 0; i < 15; i++) CU_ASSERT_DOUBLE_EQUAL(v->values[i], l[i], 1e-3);

    vector* r = mult_v_scalaire(v, 2.3);
    for (int i = 0; i < 15; i++) CU_ASSERT_DOUBLE_EQUAL(r->values[i], l[i]*2.3, 1e-3);

    free_vector(&v);
    free_vector(&r);
}

void test_back_sub(void){
    matrix* A = init_matrix(2,2);
    vector* v = init_vector(2);

    A->values[0][0] = 1;
    A->values[0][1] = 2;
    A->values[1][1] = 3;
    v->values[0] = 5;
    v->values[1] = 3;

    vector* r = back_sub(A, v);
    CU_ASSERT_DOUBLE_EQUAL(r->values[0], 3, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(r->values[1], 1, 1e-3);

    free_matrice(&A);
    free_vector(&v);
    free_vector(&r);

    matrix* A2 = init_matrix(3,3);
    vector* v2 = init_vector(3);
    
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

    vector* r2 = back_sub(A2, v2);

    for (int i = 0; i < 3; i++)
    {
        CU_ASSERT_DOUBLE_EQUAL(r2->values[i], i+1, 1e-3);
    }
    
    vector* D = init_vector(1);
    CU_ASSERT_PTR_NULL(back_sub(A2, D));

    free_vector(&D);
    free_matrice(&A2);
    free_vector(&v2);
    free_vector(&r2);
}

void test_qr(void){
    matrix* A = init_matrix(3, 3);
    matrix* Q = init_matrix(3, 3);
    matrix* R = init_matrix(3, 3);

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
    qr(A, Q, R);

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
    matrix* A = init_matrix(3, 2);
    vector* v = init_vector(3);

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
    
    vector* r = lstsq(A, v);
    CU_ASSERT_PTR_NOT_NULL(r);
    CU_ASSERT_EQUAL(r->m, 2);

    double r2[2] = {1.6667, 1.3333};
    for (int i = 0; i < r->m; i++) CU_ASSERT_DOUBLE_EQUAL(r->values[i], r2[i], 1e-3);
    
    vector* v3 = init_vector(2);
    CU_ASSERT_PTR_NULL(lstsq(A, v3));

    free_matrice(&A);
    free_vector(&v);
    free_vector(&r);
    free_vector(&v3);
}

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
    vector* v = init_vector(5);
    double v2[5] = {3,-4,2,8,-1};
    
    for (int i = 0; i < 5; i++) v->values[i] = v2[i];
    
    FILE* file = fopen("test.dat", "wb");
    CU_ASSERT_EQUAL(write_vector(v, file), 0);
    fclose(file);
    free_vector(&v);

    //read vector
    FILE* file2 = fopen("test.dat", "rb");
    vector* v3 = read_vector(file2);
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
    CU_pSuite test_basic_op = CU_add_suite("test_sequentielle", NULL, NULL);
    if (test_basic_op == NULL) {CU_cleanup_registry(); return CU_get_error();}
    if ((CU_add_test(test_basic_op, "add_sub_v_v", test_add_sub_v_v) == NULL) ||
        (CU_add_test(test_basic_op, "add_sub_m_m", test_add_sub_m_m) == NULL) ||
        (CU_add_test(test_basic_op, "dot_prod", test_dot_prod) == NULL) ||
        (CU_add_test(test_basic_op, "norm", test_norm) == NULL) ||
        (CU_add_test(test_basic_op, "mult_m_v", test_mult_m_v) == NULL) ||
        (CU_add_test(test_basic_op, "mult_m_m", test_mult_m_m) == NULL) ||
        (CU_add_test(test_basic_op, "transp", test_transp) == NULL) ||
        (CU_add_test(test_basic_op, "back_sub", test_back_sub) == NULL) ||
        (CU_add_test(test_basic_op, "qr", test_qr) == NULL) ||
        (CU_add_test(test_basic_op, "lstsq", test_lstsq) == NULL) || 
        (CU_add_test(test_basic_op, "file", test_file) == NULL) ||
        (CU_add_test(test_basic_op, "write_vector", test_write_vector) == NULL) ||
        (CU_add_test(test_basic_op, "write_matrix", test_write_matrix) == NULL) ||
        (CU_add_test(test_basic_op, "write_qr", test_write_qr) == NULL)) {
            CU_cleanup_registry(); return CU_get_error();
    }
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_basic_show_failures(CU_get_failure_list());
    int failed_tests = CU_get_number_of_failures();
    CU_cleanup_registry();
    return (failed_tests > 0) ? 1 : 0;
}