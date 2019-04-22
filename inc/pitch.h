#ifndef PITCH_H
#define PITCH_H


#include <valarray>

using valarray = std::valarray<double>;

#define DECL_PITCH(name)  bool pitch_ ## name (const valarray& frame, double *f0res, double *T0res)


DECL_PITCH(AMDF);


#endif // PITCH_H
