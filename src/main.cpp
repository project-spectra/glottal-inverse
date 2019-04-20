#include <atomic>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <iomanip>

#include <experimental/iterator>

#include <portaudio.h>

#include "audio_in.h"
#include "pitch.h"
#include "iaif.h"
#include "normalize.h"
#include "gci_sedreams.h"
#include "glottal.h"
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

        // estimate GCIs
        auto gci = gci_sedreams(g, SAMPLE_RATE, f0est);

        // estimate glottal parameters
        auto vp = estimateGlottal(g, dg, SAMPLE_RATE, gci);

        // get mean Oq
        double OQmean(0.);
        size_t nbValid(0);
        for (auto& frame : vp) {
            if (frame.valid) {
                std::cout << "OQ["<<nbValid<<"] = "<<frame.QOQ<<std::endl; 
                OQmean += frame.QOQ;
                nbValid++;
            }
        }
        if (nbValid > 0) {
            OQmean /= nbValid;
        }

        // print results
        std::cout << "  (*) Estimated:" << std::endl
                  << "      - f0: " << (int) f0est << " Hz" << std::endl
                  << "      - Mean Oq: " << std::setprecision(1) << OQmean << " (" << nbValid << " valid frames)" << std::endl;

        // write and plot
        writePlotData(g, GNUPLOT_FILE_IAIF_SOURCE);
        writePlotData(dg, GNUPLOT_FILE_IAIF_SOURCE_DERIV);

    }

    std::cout << " ==== Exiting safely ====" << std::endl;

    free(data);

    return EXIT_SUCCESS;
}

void terminate() {
    Pa_Terminate();
}
