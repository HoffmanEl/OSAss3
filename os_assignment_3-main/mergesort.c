#include "mergesort.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* Merge two sorted subarrays A[leftstart..leftend] and A[rightstart..rightend] into B, then copy back to A */
void merge(int leftstart, int leftend, int rightstart, int rightend) {
    int i = leftstart;
    int j = rightstart;
    int k = leftstart;

    while (i <= leftend && j <= rightend) {
        if (A[i] <= A[j]) {
            B[k++] = A[i++];
        } else {
            B[k++] = A[j++];
        }
    }

    while (i <= leftend) B[k++] = A[i++];
    while (j <= rightend) B[k++] = A[j++];

    // copy merged back to A
    for (i = leftstart; i <= rightend; i++) {
        A[i] = B[i];
    }
}

/* Standard recursive mergesort for the base case */
void my_mergesort(int left, int right) {
    if (left >= right) return;

    int mid = (left + right) / 2;
    my_mergesort(left, mid);
    my_mergesort(mid + 1, right);
    merge(left, mid, mid + 1, right);
}

/* Build argument struct for parallel_mergesort */
struct argument * buildArgs(int left, int right, int level) {
    struct argument *arg = malloc(sizeof(struct argument));
    arg->left = left;
    arg->right = right;
    arg->level = level;
    return arg;
}

/* Parallel mergesort using pthreads up to cutoff level */
void * parallel_mergesort(void *arg) {
    struct argument *a = (struct argument *) arg;
    int left = a->left;
    int right = a->right;
    int level = a->level;

    if (left >= right) return NULL;

    if (level >= cutoff) {
        my_mergesort(left, right);
        return NULL;
    }

    int mid = (left + right) / 2;
    pthread_t left_thread, right_thread;
    struct argument *left_arg = buildArgs(left, mid, level + 1);
    struct argument *right_arg = buildArgs(mid + 1, right, level + 1);

    pthread_create(&left_thread, NULL, parallel_mergesort, left_arg);
    pthread_create(&right_thread, NULL, parallel_mergesort, right_arg);

    pthread_join(left_thread, NULL);
    pthread_join(right_thread, NULL);

    merge(left, mid, mid + 1, right);

    // Free only the child args
    free(left_arg);
    free(right_arg);

    return NULL;
}
