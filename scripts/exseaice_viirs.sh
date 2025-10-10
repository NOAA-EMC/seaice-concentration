#!/bin/sh

#Parameters have been set up by J job
#Robert Grumbine
# 9 Oct 2025

# Note that the analysis job can run without any contents in the output from this.
# That is what the 'touch' at the end is ensuring.

# J job sets comout based on cycle, = COMOUTbase.$day for 00, 06, 12 cycles
#                                   = COMOUTbase.$dayp1 for 18z cycle

set -x
echo zzzzzzzzzzzzzzz  exseaice_viirs: DCOMROOT = $DCOMROOT

if [ $cyc == '00' ] ; then
  day=$PDYm1
  hours='18 19 20 21 22 23'
else
  day=$PDY
  if [ $cyc == '06' ] ; then
    hours='00 01 02 03 04 05'
  fi
  if [ $cyc == '12' ] ; then
    hours='06 07 08 09 10 11'
  fi
  if [ $cyc == '18' ] ; then
    hours='12 13 14 15 16 17'
  fi
fi

echo zzz $day $hh

# How to manage hh?  RG
echo zzzzz working on viirs $PDY $cyc
for inst in j01 npp n21
do
  for hh in $hours
  do
    python3 composite.py \
    $DCOMROOT/$PDY/viirs/JRR-IceConcentration*_${inst}_s${day}${hh}*.nc \
    > output.$inst.$cyc.${day}$hh 
    if [ ! -f output.$inst.$cyc.${day}$hh ] ; then
      touch output.$inst.$cyc.${day}$hh
    fi
  done
done
# Handle no file case RG
mv output.*.$cyc.${day}?? $COMOUT

