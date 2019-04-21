#!/bin/bash

sleep 2

script=$(mktemp)

plot_time() {
    echo | gnuplot <<EOFMarker
        set term qt enh font "arial,12"
        set datafile sep ","

        set title "$1"

        set xrange [0:0.2]
        set yrange $3

        plot "$2" with lines

        bind all 'd' 'done = 1'
        done = 0
        while (!done) {
            plot "$2" with lines
            replot
        }
EOFMarker
}

plot_time 'LPC residual' \
          'iaif_source.dat' \
          '[-1:1]' &

plot_time 'Estimated glottal flow derivative' \
          'iaif_source_deriv.dat' \
          '[-1:1]' &


wait
