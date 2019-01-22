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
* COVAREP's implementation of an LF glottal source model - [COVAREP's GitHub repository](https://github.com/covarep/covarep/blob/master/glottalsource/glottal_models/gfm_spec_lf.m)

**TODO: List all references**

---

Readme was last updated on Jan 22., 2019
