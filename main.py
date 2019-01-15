from typing import Tuple, Function
import numpy as np
import scipy.integrate as integrate

## CONSTANTS

tau = 1.2
MAX_LEVEL = 8  # Maximum number of decompositions

## ---------

# Initial glottal source function
#L = 

# Scaling function
scaling = lambda t: tau * t

# Mother wavelet (Haar)
motherWavelet = lambda t:
		  1 if 0 <= t and t < .5
	else -1 if .5 <= t and t < 1:
	else  0


# Wavelet basis
basis = {
	1: motherWavelet,
	(j, l): lambda t: 1/2**j * motherWavelet((t-2**j*l)/2**j)
		for l in range(0, 2**j)
		for j in range(MAX_LEVEL + 1)
}

# Wavelet indices
indices = basis.keys()


def dot(f, g):  # < , >
	"""
	Returns the dot product of the two functions.
	"""
	return integrate.quad(lambda t: f(t)*g(t), 0, 1)
	
def decompose(f):  # F
	"""
	Returns the coordinates of the function.
	"""
	return np.array([dot(f,basis[i]) for i in indices])

def converges():
	"""
	Returns true iff the convergence threshold is passed.
	"""
	return false

def amgif(C):
	di = decompose(m)
	yi = decompose(p)
	
	y = yi
	
	while not converges():
		A = np.array([(y.T.dot(C[r])] for r in range(MAX_LEVEL)])
		
		#alpha =
		
		lhs = A.T.dot(A) + alpha * L.T.dot(L.T, L)
		rhs = A.T.dot(di)
		x = np.linalg.solve(lhs, rhs)
		
		B = np.array([[C[r].dot(x)] for r in range(MAX_LEVEL)])
		
		#beta =
		
		lhs = B.T.dot(B) + (alpha + tau) * np.identity(MAX_LEVEL)
		rhs = B.T.dot(di) + tau * yi
	
	return x, y
