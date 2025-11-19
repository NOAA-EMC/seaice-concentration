#!/bin/sh

tagm=20250101
tag=20250102
while [ $tag -le 20250131 ]
do
  d4=`echo $tag | cut -c5-8`
  d4m=`echo $tagm | cut -c5-8`

  ./seaice_viirs data/alpha.$d4  out.$d4
  ./viirs_blend  age.$d4m new.$d4m out.$d4 age.$d4 new.$d4

  tagm=$tag
  tag=`expr $tag + 1`
  tag=`$HOME/bin/dtgfix3 $tag`
done
