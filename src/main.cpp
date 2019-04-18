#ifndef PRECOMP

#include <atomic>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <iomanip>

#include <portaudio.h>

#include "audio_in.h"
#include "pitch.h"
#include "iaif.h"
#include "amgif.h"
#include "linalg.h"
#include "operators.h"
#include "derivative.h"
#include "glottal_phases.h"
#include "gnuplot.h"
#include "gsl_util.h"


volatile std::atomic<bool> stop;

void terminate();
void inthand(int signum) {
    (void) signum;
    stop = true;
}

int main() {

    PaError err;
    PaStream *stream;
    window_data *data;
  
    data = static_cast<window_data *>(malloc(
        sizeof(window_data) + WINDOW_LENGTH * sizeof(sample)
    ));

    err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio init failure: " << Pa_GetErrorText(err) << std::endl;
        free(data);
        return EXIT_FAILURE;
    }

    std::atexit(terminate);

    if (!openStream(&stream, data)) {
        std::cerr << "Exiting with error..." << std::endl;
        free(data);
        return EXIT_FAILURE;
    }
    
    std::cout << " ==== Recording ====" << std::endl;

    static_vector(md);

    static_vector(g);
    static_vector(dg);

    double f0est, T0est;
    double Oq, Cq;

    while (!stop) {
        // record a window
        data->frameIndex = 0;
        recordWindow(stream);

        // convert to doubles
        for (size_t i = 0; i < WINDOW_LENGTH; ++i) {
            gsl_vector_set(md, i, data->recordedSamples[i]);
        }
        
        if (!pitch_AMDF(md, &f0est, &T0est)) {
            //std::cout << "  (/)  No voiced speech detected" << std::endl;
            continue;
        }
        
        // estimate glottal source with IAIF
        computeIAIF(g, dg, md);

        // estimate GCI and GOI
        // find min and max peak of dEGG
        estimatePhases(g, T0est, &Oq, &Cq);

        std::cout << "  (*) Estimated:" << std::endl
                  << "      - f0: " << (int) f0est << " Hz" << std::endl
                  << "      - Cq: " << std::setprecision(2) << Cq << std::endl 
                  << "      - Oq: " << std::setprecision(2) << Oq << std::endl;

        // write and plot
        writePlotData(g, GNUPLOT_FILE_IAIF_SOURCE);
        writePlotData(dg, GNUPLOT_FILE_IAIF_SOURCE_DERIV);

        /*
        std::cout << "- Estimating with AM-GIF..." << std::endl;

        // initialize AM-GIF parameters TODO estimate alpha&beta
        double alpha, beta, tau, eps;
        alpha = 4;
        beta = 5;
        tau = 3.;
        eps = 1e-4;

        // estimate with AM-GIF
        //  m: signal function
        //  f: input function
        //  p: charac function
        std::tie(m, f, p) = computeAMGIF(Cbuf, md, g, L, alpha, beta, tau, eps);
        normalize(m);
        normalize(f);
        normalize(p);
        
        writePlotData(p, GNUPLOT_FILE_AMGIF_SOURCE);

        gsl_vector_free(m);
        gsl_vector_free(f);
        gsl_vector_free(p);
        */
    }

    std::cout << " ==== Exiting safely ====" << std::endl;

    free(data);

    return EXIT_SUCCESS;
}

void terminate() {
    Pa_Terminate();
}

#endif // PRECOMP
