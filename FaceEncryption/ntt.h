
#ifndef NTT_H
#define NTT_H

#define N 8
#define Q 257
#define PSI 2
#define OMEGA 4

void ntt(const int a[N], int result[N]);

void inverse_ntt(const int a[N], int result[N]);

void ntt_multiply(const int a[N], const int b[N], int result[N]);

#endif
