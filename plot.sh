#/bin/bash

if (($#)); then
    list=$@
else
    list="speech.dat glottal_source_deriv.dat glottal_source.dat"
fi

for file in $list ; do
./.plot.py $file &
done
