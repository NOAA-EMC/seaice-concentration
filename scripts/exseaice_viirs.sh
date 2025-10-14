#!/bin/sh

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
#                     EXDIR
set -x
set -e

source $HOME/env3.12/bin/activate
export PYTHONPATH=$PYTHONPATH:$HOME/rgops/mmablib/py

day=$PDY
if [ $cyc == '00' ] ; then
  hours='10 09 08 07 06 05'
elif [ $cyc == '06' ] ; then
  hours='16 15 14 13 12 11'
elif [ $cyc == '12' ] ; then
  hours='22 21 20 19 18 17'
elif [ $cyc == '18' ] ; then
  day=$PDYm1
  hours='04 03 02 01 00' #handle 23 separately, PDYm1
else
  echo exseaice_viirs: illegal cycle $cyc, exiting
  exit 1
fi


echo zzzzz working on viirs $PDY $cyc
for inst in j01 npp n21
do
  for hh in $hours
  do
    python3 $EXDIR/composite.py \
    $DCOMROOT/$day/wgrdbul/IST/JRR-IceConcentration*_${inst}_s${day}${hh}*.nc \
    > output.$inst.$cyc.${day}$hh 
    # Handle no file case 
    if [ ! -f output.$inst.$cyc.${day}$hh ] ; then
      touch output.$inst.$cyc.${day}$hh
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
      python3 $EXDIR/composite.py \
      $DCOMROOT/$day/wgrdbul/IST/JRR-IceConcentration*_${inst}_s${day}${hh}*.nc \
      > output.$inst.$cyc.${day}$hh 
      # Handle no file case 
      if [ ! -f output.$inst.$cyc.${day}$hh ] ; then
        touch output.$inst.$cyc.${day}$hh
      fi
    done
  done
fi


mv output.*.$cyc.${day}?? $COMOUT

