n = 8
q = 257
psi = 2
omega = 4


def ntt(a):
    twisted = []

    for i in range(n):
        twisted.append(a[i] * pow(psi, i, q) % q)

    result = [0] * n
    for k in range(n):
        for j in range(n):
            result[k] += twisted[j] * pow(omega, j * k, q) % q

    result[k] %= q
    return result

def inverse_ntt(a):
    # Inverse Number Theoretic Transform
    pass

def ntt_multiply(a, b):
    # Multiply using NTT
    pass