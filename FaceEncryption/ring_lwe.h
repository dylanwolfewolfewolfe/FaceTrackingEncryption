
#ifndef RING_LWE_H
#define RING_LWE_H

#include "ntt.h"

typedef struct {
    int a[N];
    int b[N];
} PublicKey;

typedef struct {
    int s[N];
} PrivateKey;

typedef struct {
    int u[N];
    int v[N];
} Ciphertext;

void key_generation(PublicKey *pk, PrivateKey *sk);

void key_encryption(
    const PublicKey *pk,
    const int message[N],
    Ciphertext *ct
);

void key_decryption(
    const PrivateKey *sk,
    const Ciphertext *ct,
    int message[N]
);

#endif
