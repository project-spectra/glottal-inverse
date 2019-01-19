#!/usr/bin/env python3

import numpy as np
from scipy import integrate, interpolate, optimize, signal
from scipy.io import wavfile
from pprint import pprint
from math import pi

import matplotlib.pyplot as plt

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


def recompose(x, basis):  # F-(f)
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


# --------------
# SIGNAL FILTERS


def parallel(x, y):
    b0, a0 = x
    b1, a1 = y

    bt = np.polyadd(np.polymul(b0, a1), np.polymul(b1, a0))
    at = np.polymul(a1, a0)

    return bt, at


def series(x, y):
    b0, a0 = x
    b1, a1 = y

    bt = np.polymul(b0, b1)
    at = np.polymul(a0, a1)

    return bt, at


def normalize(S):
    return S / np.max(np.abs(S))


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


def Rpp_D(T0, Te, Ta):
    return 1 - ((T0 - Te) / Ta) / (np.exp((T0 - Te) / Ta) - 1)


def Rpp_Tx(T0, Te, Tp, Ta):
    D = Rpp_D(T0, Te, Ta)
    return Te * (1 - (.5 * Te**2 - Te * Tp) / (2 * Te**2 - 3 * Te * Tp + 6 * Ta * (Te - Tp) * D))


def Rpp(K, T0, Te, Tp, Ta):
    Tx = Rpp_Tx(T0, Te, Tp, Ta)
    expT = np.exp(-(T0 - Te) / Ta)

    def Uder(t):
        if np.any([t < 0, t > T0]):
            return Uder(t % T0)

        return np.piecewise(t, [t <= Te, t > Te], [
            lambda t: 4 * K * t * (Tp - t) * (Tx - t),
            lambda t: Uder(Te) * (np.exp(-(t - Te) / Ta) - expT) / (1 - expT)
        ])

    def U(t):
        if np.any([t < 0, t > T0]):
            return U(t % T0)

        return np.piecewise(t, [t <= Te, t > Te], [
            lambda t: K * t**2 * (t**2 - 4 / 3 * t * (Tp + Tx) + 2 * Tp * Tx),
            lambda t: U(Te) + Ta * Uder(Te) * (1 - np.exp(-(t - Te) / Ta) - ((t - Te) / Ta) * expT) / (1 - expT)
        ])

    return U, Uder


def glottalSource(fs, E, T0, Oq, am, Qa):
    """
    Generator for the Rosenberg++ glottal source model

    Args:
        fs (float): The sampling frequency used for proper normalization of the pulse amplitude.
        E (float): The amplitude of the pulse (e.g. 1).
        T0 (float): The fundamental period (the duration of the glottal cycle), 1/f0.
        Oq (float): The open quotient (ratio of the open phase over the glottal cycle).
        am (float): The asymmetry coefficient.
        Qa (float): The return quotient (Qa = 0 : abrupt closure).

    Returns:
        function: The time-domain glottal flow derivative function on [0, T0].
    """

    # Find K, Te, Tp, Ta from these expressions:
    #
    # E = 4 * K * Te * (Tp - Te) * (Tx - Te)
    # Oq = Te / T0
    # am = Tp / Te
    # Qa = Ta / ((1 - Oq) * T0)

    Te = Oq * T0
    Tp = am * Te
    Ta = Qa * (T0 - Te)
    Tx = Rpp_Tx(T0, Te, Tp, Ta)

    K = E / (4 * Te * (Tp - Te) * (Tx - Te))

    U, Uder = Rpp(K, T0, Te, Tp, Ta)

    return Uder


def vocalTractFilter(F, fs, n):
    """
    Generator for an all-pole vocal tract filter.
    """
    (F0, Bw0), *Ftail = F

    # Parallel peaks
    filtr = signal.iirpeak(F0, F0/Bw0, fs)

    for Fn, Bwn in Ftail:
        filtr = parallel(filtr, signal.iirpeak(Fn, Fn/Bwn, fs))

    # Add a Butterworth filter to simulate the frequency slope
    # This one is in series
    # Order n => 20n dB/dec slope
    filtr = series(filtr, signal.butter(n, 100, fs=fs))

    b, a = filtr

    return lambda x: signal.filtfilt(b, a, x)
    #return bt, at


def main():
    # Testing parameters used in the original AM-GIF algorithm paper.

    fs = 16000
    E = 1
    Oq = 0.1
    am = 0.9
    Qa = 0.07

    f = 125
    R = [(800, 80), (1150, 90), (2900, 120), (3900, 130), (4950, 140)]

    source = glottalSource(fs, E, 1/f, Oq, am, Qa)

    filtr = vocalTractFilter(R, fs, 1)  # 20 dB/dec

    length = 5
    t = np.linspace(0, length, num=int(length*fs), endpoint=False)

    U = normalize(source(t))
    V = normalize(filtr(U))

    wavfile.write('synth_source.wav', fs, U)
    wavfile.write('synth_speech.wav', fs, V)

    if True:
        plt.figure()

        #plt.plot(t, filtr(t), label='Filter')
        plt.plot(t, U, label='Source signal')
        plt.plot(t, V, label='Speech signal')
        plt.legend()

        #f, t, Sxx = signal.spectrogram(U, fs, window='hamming', nfft=8192, nperseg=512)
        #plt.pcolormesh(t, f, Sxx)

        plt.suptitle('Oq = {}'.format(Oq))
        plt.xlabel('Time (seconds)')
        plt.xlim(0, 5/f)
        plt.grid()
        plt.show()


if __name__ == '__main__':
    main()

