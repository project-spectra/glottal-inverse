#!/bin/bash

script=$(mktemp)

plot_time() {
    echo | gnuplot <<EOFMarker
        set term wxt enh font "arial,12"
        set datafile sep ","

        set title "$1"

        set xrange [0:0.06]
        set yrange [-1:1]

        bind all 'd' 'done = 1'
        done = 0
        while (!done) {
            plot "$2" with lines
            replot
        }
EOFMarker
}

plot_time 'Glottal flow' \
          'est_source_flow.dat' &

plot_time 'Glottal flow derivative' \
          'est_source_deriv.dat' &

wait
