#!/usr/bin/env python

import numpy as np
import scipy.integrate as integrate
import scipy.optimize as optimize
import scipy.signal as signal
from math import pi
from pprint import pprint

# ---------
# CONSTANTS

tau = 1.2
J = 8  # Maximum number of decompositions


# -------------
# INITIAL STATE

# Initial glottal source function
# def L(t):


def haarWaveletMother(t):
    """
    The mother Haar wavelet used to calculate the wavelet series.

    Args:
        t (float): The time parameter.

    Returns:
        float: The value of the mother wavelet.
    """
    if 0 <= t and t < 0.5:
        return 1.0
    elif 0.5 <= t and t < 1:
        return -1.0
    else:
        return 0.0


def haarWavelet(j, l):
    """
    A higher-order function to create a Haar wavelet.

    Args:
        j, l (int): The parameters of the wavelet.

    Returns:
        function: The wavelet function of index (j, l).
    """
    a, b = 2**(j / 2), 2**j

    def wav(t):
        return a * haarWaveletMother(b * t - l)
    return wav


def buildBasis(psi, tau):
    """
    Build the basis with the specified wavelets and scaling factors.

    Args:
        psi (function): The child wavelet function.
        tau (float): The scaling factor.

    Returns:
        dict: The basis with each variable mapped by index.
    """
    basis = {
        (j, l): psi(j, l)
        for j in range(J + 1)
        for l in range(0, 2**j)
    }
    basis[1] = lambda t: tau * t
    return basis


def buildOperatorC(basis):
    """
    Build the operator C with the specified basis.

    Args:
        basis (dict): The basis mapping to use.

    Returns:
        dict: The sequence of operators (C_mu) indexed by basis index.
    """
    return {
        mu: np.array([
            [proj(conv(basis_p, basis_f), basis_mu)
                for basis_f in basis.values()]
            for basis_p in basis.values()
        ])
        for mu, basis_mu in basis.items()
    }


# --------------
# LINEAR ALGEBRA


def proj(f, g):  # <f,g>
    """
    Projection product of two functions on L2 space as the integral of
    their product over [0, 1].

    Args:
        f, g (function): The two functions to project onto each other.

    Returns:
        float: The dot product of f and g.
    """
    return integrate.quad(lambda t: f(t) * g(t), 0, 1)


def conv(f, g):  # f*g
    """
    Convolution operator on L2 space as the sliding integral of f and g.

    Args:
        f, g (function): The two functions to apply the convolution to.

    Returns:
        function: The function resulting from the convolution.
    """
    return lambda t: integrate.quad(lambda s: f(t - s) * g(s), 0, 1)


def decompose(f, basis):  # F(f)
    """
    Coordinates of the function in L2 space in the specified basis.

    Args:
        f (function): The function to decompose.
        basis (dict): The basis in which to decompose the function.

    Returns:
        dict: The coordinates indexed by each basis' index.
    """
    return {
        i: proj(f, phi) for i, phi in basis.items()
    }


def recompose(x, basis):  # F⁻(f)
    """
    Recompose the function from a set of coordinates in that basis.

    Args:
        x (dict): The mapping of each basis index to the coordinate
                  of the function.
        basis (dict): The basis from which to recompose the function.

    Returns:
        function: The function recomposed from its coordinates.
    """
    def f(t):
        return sum([x[i] * phi(t) for i, phi in basis.items()])
    return f


# ----------------
# AM-GIF ALGORITHM


def converges(eps):
    """
    Tests if the algorithm converges well enough with a threshold.

    Args:
        eps (float): The maximal error distance for convergence.

    Returns:
        bool: True if and only if the algorithm converges.
    """
    return False


def amgif(me, pe, L, alpha, beta, tau, eps):
    """
    Computes the glottal source excitation and the vocal tract filter
    using the AM-GIF algorithm. (doi:10.1088/1361-6420/aa6eb8)

    Args:
        me (function): The measured speech signal data.
        pe (function): The characterizing function.
        L (numpy.ndarray): The matrix operator to enforce filter shape.
        alpha (float): The regularization parameter for VTF estimation.
        beta (float): The regularization parameter for GS estimation.
        tau (float): The scaling parameter.
        eps (float): The error threshold to test for convergence.

    Returns:
        function,: The estimated vocal tract filter.
        function : The estimated glottal source excitation.
    """
    basis = buildBasis(haarWavelet, tau)
    C = buildOperatorC(basis)

    di = decompose(me)
    yi = decompose(pe)

    y = yi

    while not converges(eps):
        A = np.array([y.T.dot(C[mu]) for mu in basis])

        # alpha =

        lhs = A.T.dot(A) + alpha * L.T.dot(L)
        rhs = A.T.dot(di)
        x = np.linalg.solve(lhs, rhs)

        B = np.array([C[mu].dot(x) for mu in basis])

        # beta =

        lhs = B.T.dot(B) + (beta + tau) * np.identity(len(basis))
        rhs = B.T.dot(di) + tau * yi
        y = np.linalg.solve(lhs, rhs)

    f = recompose(x)  # Vocal tract filter
    p = recompose(y)  # Glottal source excitation

    return f, p


# ------------------
# FORWARD SIMULATION


def glottalSource(f, fs, T0, Ee, te, tp, ta, tc, eps=1e-16):
    """
    Generator for the Liljencrants-Fant glottal source model

    Args:
        f (float): The frequency values where the spectrum has to be estimated.
        fs (float): The sampling frequency used for proper normalization of the pulse amplitude.
        T0  (float): The fundamental period (the duration of the glottal cycle), 1/f0.
        Ee (float): The amplitude of the pulse (e.g. 1).
        te, tp, ta (float): Glottal shape parameter.
        eps (float): The initial value for root finding.
    """
    Te = te * T0
    Tp = tp * T0
    Ta = ta * T0
    Tc = tc * T0

    wg = pi / Tp

    # e is expressed by an implicit equation
    def fb(e):
        part = np.exp(-e * (T0 - Te))
        return (1 - part - e * Ta, (T0 - Te) * part - Ta)

    e = optimize.root_scalar(fb, x0=1/(Ta + eps), fprime=True).root

    # a is expressed by another implicit equation
    A = (1 - np.exp(-e * (T0 - Te))) / (e**2 * Ta) - (T0 - Te) * np.exp(-e * (T0 - Te))
    cosWgTe = np.cos(wg * Te)
    sinWgTe = np.sin(wg * Te)

    def fa(a):
        return (a**2 + wg**2) * sinWgTe * A + wg * np.exp(-a * Te) + a * sinWgTe - wg * cosWgTe

    x = np.array([0 if i is 0 else 10**i for i in range(10)])
    idx = np.nonzero(np.diff(np.sign(fa(x)), 1))[0][0]
    a = optimize.root_scalar(fa, x0=x[idx], x1=x[idx+1]).root

    # E0 parameter
    E0 = -Ee / (np.exp(a * Te) * sinWgTe)

    T1 = np.arange(1 / fs, Te, step=1/fs)
    T2 = np.arange(Te, Tc, step=1/fs)

    # Differentiated
    dOP = E0 * np.exp(a * T1) * np.sin(wg * T1)
    dCP = (-Ee / (e * Ta)) * (np.exp(-e * (T2 - Te)) - np.exp(-e * (Tc - Te)))

    G = np.concatenate((gOP, gCP))

    return G
    # return lambda t: np.real(G * np.exp(t))


def vocalTractFilter(F, fs):
    """
    Generator for an all-pole vocal tract filter.
    """
    (F0, Bw0), *Ftail = F

    result = np.array(signal.iirpeak(F0, F0/Bw0, fs))

    for Fn, Bwn in Ftail:
        result += np.array(signal.iirpeak(Fn, Fn/Bwn, fs))

    return result


def main():
    # Testing parameters used in the original AM-GIF algorithm paper.

    fs = 16000
    tp = 0.47
    te = 0.65
    ta = 0.01
    tc = 1

    f = 100
    R = [(800, 80), (1150, 90), (2900, 120), (3900, 130), (4950, 140)]

    source = glottalSource(f, fs, 1/f, 1, te, tp, ta, tc)

    filtr = vocalTractFilter(R, fs)

    return source, filtr


if __name__ == '__main__':
    source, filtr = main()

    pprint(source)
    pprint(filtr)

