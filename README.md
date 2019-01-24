# (Attempt) Implementation of a glottal inverse filtering algorithm

Written by Chloé (Rika) Ichinose, under MIT License

---

## Dependencies

* GNU Scientific Library 2.5 with CBLAS
* PortAudio v19

---

## How to use

```sh
make BUILD_TYPE=Release
```
Builds release variant binaries.

```sh
make
# or
make BUILD_TYPE=Debug
```
Builds ddebug variant binaries.

See `Makefile` for more make targets.

---

## Sources and references

* Alternating minimisation for glottal inverse filtering - [Ismael Rodrigo Bleyer et al 2017 Inverse Problems 33 065005](https://doi.org/10.1088/1361-6420/aa6eb8)
* COVAREP's implementation of the IAIF algorithm - [COVAREP's GitHub repository](https://github.com/covarep/covarep/blob/master/glottalsource/)
* The algebra of harmonic functions for a matrix-valued transfer operator - [arXiv:math/0611539v3 (math.FA) 7 Jun 2007](https://arxiv.org/pdf/math/0611539.pdf)
* MATLAB's explanation of their LPC analysis algorithm - [Mathworks MATLAB Documentation lpc](https://www.mathworks.com/help/signal/ref/lpc.html)
* Time-domain digital filter representations - [DSPRELATED.com Introduction to Digital Filters](https://www.dsprelated.com/freebooks/filters/Time_Domain_Digital_Filter.html)

**TODO: List all references**

---

Readme was last updated on Jan 23., 2019
