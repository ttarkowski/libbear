#!/bin/bash

file=evolution.dat
temp=temp.dat
out=plot.png

for i in `cat evolution.dat | awk '{ print $1 }' | uniq`
do
  cat ${file} | grep "^${i} " > ${temp}
  echo "\
    reset; \
    set t png; \
    set o \"${out}\"; \
    set title \"${i}\"; \
    set xlabel \"distance between atoms (Angstrom)\"; \
    set ylabel \"angle bewteen bonds (deg)\"; \
    set cblabel \"-total energy (Ry)\"
    set xrange [.5: 2.5]; \
    set yrange [0: 180]; \
    set zrange [-16: 12.5]; \
    set cbrange [-16: 12.5];
    set xtics .5; \
    set ytics 45; \
    set ztics 8; \
    set cbtics 8; \
    set view 0, 0; \
    set palette rgbformulae 33,13,10; \
    plot \"${temp}\" u 2:(180 * \$3 / pi):4 pt 7 ps 0.5 lc pal z notitle; \
    set o" | gnuplot
  mv ${out} `printf '%03d.png' ${i}`
done

gst-launch-1.0 -e multifilesrc location="%03d.png" \
  index=0 caps="image/png,framerate=(fraction)10/1" \
  ! pngdec ! videoconvert ! theoraenc ! oggmux \
  ! filesink location=result.ogv

rm ${temp} *.png

exit

