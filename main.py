#!/usr/bin/env python3

import numpy as np
from scipy import integrate, interpolate, optimize, signal
from scipy.io import wavfile
from pprint import pprint
from math import pi

import sounddevice as sd
import threading

import matplotlib.pyplot as plt

# ---------
# CONSTANTS

tau = 1.2
J = 8  # Maximum number of decompositions

fs = 44100
CHUNK = 4096

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


def normalize(S):
    return S / np.max(np.abs(S))


def discretize(S):
    return np.int16(normalize(S) * 32767)


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


def Rpp(E, T0, Oq, am, Qa):
    """
    Generator for the Rosenberg++ glottal source model

    Args:
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

    K = -E / (4 * Te * (Tp - Te) * (Tx - Te))

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


def VTF(formants):
    """
    Generator for an all-pole vocal tract filter.
    """
    F, Bw = np.transpose(formants)

    nsecs = len(F)
    R = np.exp(-pi * Bw / fs)  # Pole radii
    theta = 2 * pi * F / fs  # Pole angles
    poles = R * np.exp(1j * theta)

    poles = np.concatenate((poles, np.conj(poles)))

    return signal.zpk2sos([], poles, 1)


# --------------------
# REAL-TIME GENERATION


def startPlayback():
    event = threading.Event()

    stream = sd.OutputStream(
        samplerate=fs,
        blocksize=CHUNK,
        channels=1,
        dtype='float32',
        callback=callback,
        finished_callback=event.set)

    with stream:
        stream.start()
        event.wait()


s_f0 = s_Oq = s_am = s_Qa = None
s_formants = [None]
s_source = s_filter = None
s_soszi = s_lastF0 = None
s_updated = False
def updateArgs(*, f0, Oq, am, Qa, formants):
    global s_f0, s_Oq, s_am, s_Qa, s_formants, s_updated

    T0 = 1 / f0

    # Update source function
    if (s_f0, s_Oq, s_am, s_Qa) != (f0, Oq, am, Qa):
        (s_f0, s_Oq, s_am, s_Qa) = (f0, Oq, am, Qa)
        s_updated = True

    # Update filter function
    if any([a != b for a, b in zip(formants, s_formants)]):
        s_formants = formants
        s_updated = True


s_time = 0
def callback(outdata, frames, time, status):
    global s_time, s_updated, s_source, s_filter, s_soszi

    if s_updated:
        s_updated = False

        lastT0 = 1 / (s_lastF0 if s_lastF0 else s_f0)
        # Wait until the end of the cycle before updating anything
        if s_time % lastT0 < 2 / fs:
            s_time = 0

            s_source = Rpp(1, 1/s_f0, s_Oq, s_am, s_Qa)[1]
            s_filter = VTF(s_formants)
            s_soszi = signal.sosfilt_zi(s_filter)

    t0, s_time = s_time, s_time + frames / fs
    t = np.linspace(t0, s_time, num=frames, endpoint=True)

    # Generate samples
    U = s_source(t)
    V, s_soszi = signal.sosfilt(s_filter, U, zi=s_soszi)

    outdata[:,0] = np.float32(normalize(V))


# ----------------
# MAIN ENTRY POINT


def main():
    # Testing parameters used in the original AM-GIF algorithm paper.

    f0 = 200

    Oq = 0.6
    am = 0.8
    Qa = 0.1

    R = [(650, 80), (1080, 90), (2650, 120), (2900, 130), (3250, 140)]

    updateArgs(f0=f0, Oq=Oq, am=am, Qa=Qa, formants=R)

    startPlayback()

    if False:
        NFFT = 1024

        plt.figure()

        plt.subplot(211)

        plt.suptitle(r'$f_0={}\ Hz,\ O_q={},\ \alpha_m={},\ Q_a={}$'.format(f, Oq, am, Qa))
        plt.plot(t, u, label='Glottal source')
        plt.plot(t, v, label='Speech signal')
        plt.xlim(0, 5/f)
        plt.xticks(
                [i/f for i in range(6)],
                ['$0$' if i is 0 else '$T_0$' if i is 1 else '${}T_0$'.format(i) for i in range(6)]
        )
        plt.ylabel('Amplitude')
        plt.grid()
        plt.legend()

        plt.subplot(223)

        plt.specgram(u, Fs=fs, NFFT=1024, mode='magnitude', scale='linear')
        plt.xlabel('Time (seconds)')
        plt.ylabel('Frequency (Hz)')
        plt.ylim(0, 5000)

        plt.subplot(224)

        plt.specgram(v, Fs=fs, NFFT=1024, mode='magnitude', scale='linear')
        plt.xlabel('Time (seconds)')
        plt.ylim(0, 5000)
        plt.yticks(())

        plt.show()


if __name__ == '__main__':
    main()

