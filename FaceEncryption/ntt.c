#include "ntt.h"
#include <stdio.h>

#define N 8
#define Q 257
#define PSI 2
#define OMEGA 4

int mod_pow(int base, int exp) {
    int result = 1;

    while (exp > 0) {
        if (exp % 2 == 1)
            result = result * base % Q;

        base = base * base % Q;
        exp /= 2;
    }

    return result;
}

int mod(int x) {
    x %= Q;
    if (x < 0)
        x += Q;
    return x;
}

void transform(int a[N], int root) {

    // Bit reversal
    for (int i = 1, j = 0; i < N; i++) {
        int bit = N / 2;

        while (j & bit) {
            j ^= bit;
            bit /= 2;
        }

        j ^= bit;

        if (i < j) {
            int temp = a[i];
            a[i] = a[j];
            a[j] = temp;
        }
    }

    // Cooley-Tukey butterflies
    for (int length = 2; length <= N; length *= 2) {

        int step = mod_pow(root, N / length);

        for (int start = 0; start < N; start += length) {
            int w = 1;

            for (int j = 0; j < length / 2; j++) {

                int u = a[start + j];
                int v = a[start + j + length / 2] * w % Q;

                a[start + j] = mod(u + v);
                a[start + j + length / 2] = mod(u - v);

                w = w * step % Q;
            }
        }
    }
}

void ntt(const int a[N], int result[N]) {

    for (int i = 0; i < N; i++)
        result[i] = mod(a[i]) * mod_pow(PSI, i) % Q;

    transform(result, OMEGA);
}

void inverse_ntt(const int a[N], int result[N]) {

    int omega_inv = mod_pow(OMEGA, Q - 2);
    int psi_inv = mod_pow(PSI, Q - 2);
    int n_inv = mod_pow(N, Q - 2);

    for (int i = 0; i < N; i++)
        result[i] = mod(a[i]);

    transform(result, omega_inv);

    for (int i = 0; i < N; i++) {
        result[i] = result[i] * n_inv % Q;
        result[i] = result[i] * mod_pow(psi_inv, i) % Q;
    }
}

void ntt_multiply(const int a[N], const int b[N], int result[N]) {

    int A[N], B[N], C[N];

    ntt(a, A);
    ntt(b, B);

    for (int i = 0; i < N; i++)
        C[i] = A[i] * B[i] % Q;

    inverse_ntt(C, result);
}

int main() {

    int a[N] = {1, 0, 1, 0, 0, 0, 0, 0};
    int b[N] = {1, 1, 0, 0, 0, 0, 0, 0};

    int A[N], recovered[N], product[N];

    ntt(a, A);
    inverse_ntt(A, recovered);
    ntt_multiply(a, b, product);

    printf("Forward NTT: ");
    for (int i = 0; i < N; i++)
        printf("%d ", A[i]);

    printf("\nInverse NTT: ");
    for (int i = 0; i < N; i++)
        printf("%d ", recovered[i]);

    printf("\nMultiplication: ");
    for (int i = 0; i < N; i++)
        printf("%d ", product[i]);

    printf("\n");

    return 0;
}
