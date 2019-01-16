from typing import Tuple, Function
import numpy as np
import scipy.integrate as integrate

# CONSTANTS

tau = 1.2
J = 8  # Maximum number of decompositions

# ---------

# Initial glottal source function
# L =


def motherWavelet(t):
    """
    The mother wavelet (Haar) used to calculate the wavelet series.

    Args:
        t (float): The time parameter.

    Returns:
        float: The value of the mother wavelet.
    """
    if 0 <= t and t < .5:
        return 1.
    elif .5 <= t and t < 1:
        return -1.
    else:
        return 0.


def childWavelet(j, l):
    """
    A higher-order function to create a child wavelet from the mother
    wavelet. (Haar)

    Args:
        j, l (int): The parameters of the child wavelet.

    Returns:
        function: The child wavelet with index (j, l).
    """
    a, b = 2**(j/2), 2**j

    def wav(t):
        return a*motherWavelet(b*t-l)
    return wav


# Wavelet basis
basis = {
    (j, l): childWavelet(j, l)
    for l in range(0, 2**j)
    for j in range(J + 1)
}

# Wavelet indices
indices = basis.keys()


def dot(f, g):  # <f,g>
    """
    Dot product of two functions on L2 space as the integral of
    their product over [0, 1].

    Args:
        f, g (function): The two functions to apply the dot product to.

    Returns:
        float: The dot product of f and g.
    """
    return integrate.quad(lambda t: f(t)*g(t), 0, 1)


def conv(f, g):  # f*g
    """
    Convolution operator on L2 space as the sliding integral of f and g.

    Args:
        f, g (function): The two functions to apply the convolution to.

    Returns:
        function: The function resulting from the convolution.
    """
    return lambda t: integrate.quad(lambda s: f(t-s)*g(s), 0, 1)


def decompose(f):  # F(f)
    """
    Returns the coordinates of the function.
    """
    return np.array([dot(f, basis[i]) for i in indices])


def recompose(x):  # F⁻(f)
    """
    Returns the function with the specified coordinates.
    """
    def f(t):
        return sum([x[k] * basis[i] for k, i in enumerate(indices)])
    return f


def converges():
    """
    Returns true iff the convergence threshold is passed.
    """
    return false


def computeC(p, f):
    """
    Returns the operator C for the given functions.
    """
    p = decompose(p)
    f = decompose(f)

    return [
        np.array([
            [dot(conv(basis[pi], basis[fi]), basis[mu])
                for fi in range(1, 2**(J+1)+1)]
            for pi in range(1, 2**(J+1)+1)
        ])
        for mu in range(1, 2**(J+1)+1)
    ]


def amgif(m, p, C, alpha, beta, tau):
    """
    Estimates the glottal source and vocal tract filter.
    """
    basis[1] = lambda t: tau * t

    di = decompose(m)
    yi = decompose(p)

    y = yi

    while not converges():
        A = np.array([y.T.dot(C[mu]) for mu in range(1, 2**(J+1)+1)])

        # alpha =

        lhs = A.T.dot(A) + alpha * L.T.dot(L.T, L)
        rhs = A.T.dot(di)
        x = np.linalg.solve(lhs, rhs)

        B = np.array([C[mu].dot(x) for mu in range(1, 2**(J+1)+1)])

        # beta =

        lhs = B.T.dot(B) + (alpha + tau) * np.identity(J)
        rhs = B.T.dot(di) + tau * yi
        y = np.linalg.solve(lhs, rhs)

    f = recompose(x)
    p = recompose(y)

    return x, y
