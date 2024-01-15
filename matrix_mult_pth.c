/* File:     
 *     matrix_mult_pth.c
 *
 * Purpose:  
 *     Computes the product of 2 n x n matrices. Matrices are divided into an array of √p x √p blocks. 
 *     Threads are distributed suuch that a thread of rank k, 0 ≤ k ≤ p − 1, is mapped to block Pxy, where x = 
 *      ⌊k√p⌋ and y = k%√p
 *
 * Input:
 *     n: Size of matrix
 *     command line: number of threads
 *
 * Output:
 *     output file "data_output", detailing the size of the resulting matrix, the resulting matrix itself, and the time it took to run the code.
 *
 * Compile: 
 *      gcc -g -Wall -o lab1 lab1_IO.o matrix_mult_pth.o -lpthread -lm
 * Usage:
 *     ./lab1 <thread_count>
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"
#include "lab1_IO.h"

/* Global variables */
int     thread_count;
int **A; // Matrix A
int **B; // Matrix B
int **C; // Matrix C
int n;  // Size of Matrices A and B (n x n)

/* Serial functions */
void Usage(char* prog_name);
void Print_matrix(char* title, int **A, int n);

/* Parallel function */
void *Pth_mat_mult(void* rank);

int main(int argc, char* argv[]) {
   long       thread;
   pthread_t* thread_handles;
   double start, end, Time;

    GET_TIME(start); // start runtime of code

    printf("Enter n\n"); // prompt user to enter n, size of matrix
    scanf("%d", &n);

    if (argc != 2) Usage(argv[0]);
    thread_count = atoi(argv[1]);

    printf("The number of threads is %d\n", thread_count);

    // Check if the number of threads is divisible with n^2
    if (n * n % thread_count != 0) {
        fprintf(stderr, "Error: n^2 must be divisible by the number of threads.\n");
        exit(1);
    }
    // Check if thread_count is a square number
    else if (sqrt(thread_count) * sqrt(thread_count) != thread_count) {
        fprintf(stderr, "Number of threads must be a square number.\n");
        exit(1);
    }

    thread_handles = malloc(thread_count * sizeof(pthread_t));

    // Allocate memory for matrices
    A = malloc(n * sizeof(int*));
    B = malloc(n * sizeof(int*));
    C = malloc(n * sizeof(int*));

    int ind;

    for (ind = 0; ind < n; ind++) {
        A[ind] = malloc(n * sizeof(int));
        B[ind] = malloc(n * sizeof(int));
        C[ind] = malloc(n * sizeof(int));
    }

    // generate matrices for A and B

    Lab1_loadinput(&A, &B, &n); // Load the matrices into A and B

    Print_matrix("Matrix A: ", A, n);
    Print_matrix("Matrix B: ", B, n);

    // Deploying pThreads to compute Matrix C:
    for (thread = 0; thread < thread_count; thread++) {
        printf("Thread %ld created.\n", thread);
        pthread_create(&thread_handles[thread], NULL, Pth_mat_mult, (void*)thread);
    }

    for (thread = 0; thread < thread_count; thread++) {
        printf("Thread %ld joined.\n", thread);
        pthread_join(thread_handles[thread], NULL);
    }

    Print_matrix("The product (Matrix C) is: ", C, n);

    GET_TIME(end); // end runtime of code
    Time = end - start; // Get runtime of code
    Lab1_saveoutput(C, &n, Time); // Load resulting matrix in C

    // free up allocated space for matrices
    free(A);
    free(B);
    free(C);

   return 0;
}  /* main */


/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   print a message showing what the command line should
 *            be, and terminate
 * In arg :   prog_name
 */
void Usage (char* prog_name) {
   fprintf(stderr, "usage: %s <thread_count>\n", prog_name);
   exit(0);
}  /* Usage */

/*------------------------------------------------------------------
 * Function:       Pth_mat_mult
 * Purpose:        Multiply an nxn matrix A with an nxn matrix B
 * In arg:         rank
 * Global in vars: A, B, n
 * Global out var: C
 */
void* Pth_mat_mult(void* rank) {
    long my_rank = (long)rank;
    int i, j, m;
    int local_n = n / sqrt(thread_count); // Dimension of each block
    int x = floor(my_rank / sqrt(thread_count)); // x = ⌊k√p⌋
    int y = my_rank % (int)sqrt(thread_count); // y = k%√p

    int my_first_row = x * local_n; // Ensure the row and columns are bounded between (n/√p)x and (n/√p)(x+1)-1
    int my_last_row = (x + 1) * local_n - 1;
    int my_first_col = y * local_n;
    int my_last_col = (y + 1) * local_n - 1;

    for (i = my_first_row; i <= my_last_row; i++) {
        for (j = my_first_col; j <= my_last_col; j++) {
            C[i][j] = 0; // Initialize C element
            for (m = 0; m < n; m++) {
                C[i][j] += A[i][m] * B[m][j]; // compute multiplication between elements
            }
        }
    }

    return NULL;
} /* Pth_mat_mult */

/*------------------------------------------------------------------
 * Function:    Print_matrix
 * Purpose:     Print an n x n matrix
 * In args:     title, A, n
 */
void Print_matrix(char* title, int** A, int n) {
    int i, j;

    printf("%s\n", title);
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++)
            printf("%4d ", A[i][j]);  // Use %d for integers
        printf("\n");
    }
}  /* Print_matrix */
