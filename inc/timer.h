#ifndef TIMER_H
#define TIMER_H


# ifdef HAS_TIMER

#include <chrono>
#include <cstdio>

// For timing
using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;

#define TIMER_START  auto t1 = high_resolution_clock::now();
#define TIMER_END(name)  do { auto t2 = high_resolution_clock::now(); \
                              auto dur = duration_cast<duration<double>>(t2 - t1); \
                              fprintf(stderr, " >>> %s took %g seconds.\n", name, dur.count()); } while (false);

# else

#define TIMER_START
#define TIMER_END(name)

# endif


#endif // TIMER_H
