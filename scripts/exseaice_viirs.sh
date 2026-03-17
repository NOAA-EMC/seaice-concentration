#!/bin/bash

#Parameters have been set up by J job
#Robert Grumbine
# 9 Oct 2025

# Note that the analysis job can run without any contents in the output from this.
# That is what the 'touch' at the end is ensuring.

# J job sets comout based on cycle, = COMOUTbase.$day for 00, 06, 12 cycles
#                                   = COMOUTbase.$daym1 for 18z cycle
#
#

# Environment to run: COMOUT, DCOMROOT, PDY, PDYm1, PDYm2
#                     USHseaice_analysis
set -x

echo zzz entered exseaice_viirs

export PYTHONPATH=$PYTHONPATH:$PACKAGEROOT/seaice_analysis.v4.5.1/sorc/mmablib/py
if [ ! -d $PACKAGEROOT/seaice_analysis.v4.5.1/sorc/mmablib/py ] ; then
  echo could not find mmablib/py
  exit 1
fi
echo zzz prepared python

day=$PDY
if [ $cyc == '00' ] ; then
  hours='10 09 08 07 06 05'
elif [ $cyc == '06' ] ; then
  hours='16 15 14 13 12 11'
elif [ $cyc == '12' ] ; then
  hours='22 21 20 19 18 17'
elif [ $cyc == '18' ] ; then
  day=$PDYm1
  hours='04 03 02 01 00' #handle 23 separately, PDYm2
else
  err_exit exseaice_viirs: illegal cycle $cyc, exiting
#  exit 1
fi


echo zzzzz working on viirs $PDY cycle=$cyc
for inst in j01 npp n21
do
  for hh in $hours
  do
    $USHseaice_analysis/composite.py \
    $DCOMROOT/$day/wgrdbul/IST/JRR-IceConcentration*_${inst}_s${day}${hh}*.nc \
    > viirs.$inst.$cyc.${day}$hh 
    # Handle no file case 
    if [ ! -f viirs.$inst.$cyc.${day}$hh ] ; then
      touch viirs.$inst.$cyc.${day}$hh
    fi
  done
done

# special case for 23z
if [ $cyc == '18' ] ; then
  day=$PDYm2
  hours='23'
  for inst in j01 npp n21
  do
    for hh in $hours
    do
      $USHseaice_analysis/composite.py \
      $DCOMROOT/$day/wgrdbul/IST/JRR-IceConcentration*_${inst}_s${day}${hh}*.nc \
      > viirs.$inst.$cyc.${day}$hh 
      # Handle no file case 
      if [ ! -f viirs.$inst.$cyc.${day}$hh ] ; then
        touch viirs.$inst.$cyc.${day}$hh
      fi
    done
  done
fi


mv viirs.*.$cyc.*?? $COMOUT

