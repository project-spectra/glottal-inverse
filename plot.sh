#!/bin/bash

script=$(mktemp)

plot_time() {
    echo | gnuplot <<EOFMarker
        set term wxt enh font "arial,12"
        set datafile sep ","

        set title "$1"

        set xrange [0:255]

        bind all 'd' 'done = 1'
        done = 0
        while (!done) {
            plot "$2" with lines
            replot
        }
EOFMarker
}

plot_time 'IAIF glottal flow' \
          'iaif_source.dat' &

plot_time 'AMGIF glottal flow' \
          'amgif_source.dat' &

wait
