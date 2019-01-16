import numpy as np
import scipy.integrate as integrate


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


# ---------
# ALGORITHM


def converges(eps):
    """
    Tests if the algorithm converges well enough with a threshold.

    Args:
        eps (float): The maximal error distance for convergence.

    Returns:
        bool: True if and only if the algorithm converges.
    """
    return False


def amgif(me, p0, L, alpha, beta, eps, tau):
    """
    Computes the glottal source excitation and the vocal tract filter
    using the AM-GIF algorithm. (doi:10.1088/1361-6420/aa6eb8)

    Args:
        me (function): The measured speech signal data.
        p0 (function): The characterizing function.
        L (numpy.ndarray): The matrix operator to enforce filter shape.
        alpha (float): The regularization parameter for VTF estimation.
        beta (float): The regularization parameter for GS estimation.
        eps (float): The error threshold to test for onvergence.
        tau (float): The scaling parameter.

    Returns:
        function,: The estimated vocal tract filter.
        function : The estimated glottal source excitation.
    """
    basis = buildBasis(haarWavelet, tau)
    C = buildOperatorC(basis)

    di = decompose(me)
    yi = decompose(p0)

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


def __main__():
    pass
