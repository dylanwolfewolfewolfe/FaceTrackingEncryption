
#include <stdlib.h>
#include "ring_lwe.h"

int mod_q(int x) {
    x %= Q;

    if (x < 0)
        x += Q;

    return x;
}

void random_polynomial(int a[N]) {
    for (int i = 0; i < N; i++)
        a[i] = rand() % Q;
}

void small_polynomial(int a[N]) {
    for (int i = 0; i < N; i++)
        a[i] = rand() % 3 - 1;
}

void polynomial_add(const int a[N], const int b[N], int result[N]) {
    for (int i = 0; i < N; i++)
        result[i] = mod_q(a[i] + b[i]);
}

void polynomial_subtract(const int a[N], const int b[N], int result[N]) {
    for (int i = 0; i < N; i++)
        result[i] = mod_q(a[i] - b[i]);
}

void key_generation(PublicKey *pk, PrivateKey *sk) {
    int e[N];
    int product[N];

    random_polynomial(pk->a);
    small_polynomial(sk->s);
    small_polynomial(e);

    ntt_multiply(pk->a, sk->s, product);

    polynomial_add(product, e, pk->b);
}

void key_encryption(
    const PublicKey *pk,
    const int message[N],
    Ciphertext *ct
) {
    int r[N], e1[N], e2[N];
    int product[N];

    small_polynomial(r);
    small_polynomial(e1);
    small_polynomial(e2);

    ntt_multiply(pk->a, r, product);
    polynomial_add(product, e1, ct->u);

    ntt_multiply(pk->b, r, product);
    polynomial_add(product, e2, ct->v);

    for (int i = 0; i < N; i++) {
        ct->v[i] = mod_q(
            ct->v[i] + message[i] * (Q / 2)
        );
    }
}

void key_decryption(
    const PrivateKey *sk,
    const Ciphertext *ct,
    int message[N]
) {
    int product[N];
    int result[N];

    ntt_multiply(ct->u, sk->s, product);

    polynomial_subtract(ct->v, product, result);

    for (int i = 0; i < N; i++) {
        if (result[i] >= Q / 4 &&
            result[i] < 3 * Q / 4) {
            message[i] = 1;
        }
        else {
            message[i] = 0;
        }
    }
}
