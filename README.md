# Voice analysis software - tailored for trans voice analysis

Written by Clo (Rika) Ichinose, under MIT License

---

## Dependencies

* soundio 1.1.0
* cereal `51cbda5` (build only)
* tiny-dnn `c0f576f` (build only)

---

## How to use

```sh
mkdir build && cd build
cmake ..
make
```
Builds release variant binaries.

```sh
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
```
Builds debug variant binaries.

To run, go to the project's root directory and run `bin/gif`.
You can also call the program with an argument to load an audio file instead of line-in.

---

## Sources and references

* [COVAREP's GitHub repository](https://github.com/covarep/covarep)
* [Mathworks MATLAB Documentation](https://www.mathworks.com/help)
* Drugman, Thomas & Dutoit, Thierry. (2009). Glottal closure and opening instant detection from speech signals. [INTERSPEECH. 2891-2894.](https://www.semanticscholar.org/paper/Glottal-closure-and-opening-instant-detection-from-Drugman-Dutoit/d47245d0d5bc57f36a51ac82abe99582046ea518)
* P. A. Naylor, A. Kounoudes, J. Gudnason and M. Brookes, "Estimation of Glottal Closure Instants in Voiced Speech Using the DYPSA Algorithm," in IEEE Transactions on Audio, Speech, and Language Processing, vol. 15, no. 1, pp. 34-43, Jan. 2007.
* M. R. P. Thomas, J. Gudnason and P. A. Naylor, "Estimation of Glottal Closing and Opening Instants in Voiced Speech Using the YAGA Algorithm," in IEEE Transactions on Audio, Speech, and Language Processing, vol. 20, no. 1, pp. 82-91, Jan. 2012. [doi: 10.1109/TASL.2011.2157684](https://doi.org/10.1109/TASL.2011.2157684)
[doi: 10.1109/TASL.2006.876878](https://doi.org/10.1109/TASL.2006.876878)
* Kane, John & Scherer, Stefan & P Morency, L & Gobl, C. (2013). A comparative study of glottal open quotient estimation techniques. [Proceedings of the Annual Conference of the International Speech Communication Association, INTERSPEECH. 1658-1662.](https://www.isca-speech.org/archive/archive_papers/interspeech_2013/i13_1658.pdf)
* Conrad Sanderson and Ryan Curtin. An Open Source C++ Implementation of Multi-Threaded Gaussian Mixture Models, k-Means and Expectation Maximisation. [International Conference on Signal Processing and Communication Systems, 2017.](http://arma.sourceforge.net/arma_gmm_spcs_2017.pdf)

---

Readme was last updated on Apr.29, 2019
