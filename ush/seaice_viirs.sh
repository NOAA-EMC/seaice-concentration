#!/bin/bash

# Note that the analysis job can run without any contents in the output from this.
# That is what the 'touch' at the end is ensuring.
#
# Environment to run: COMOUT, DCOMROOT, PDY, PDYm1, PDYm2
#                     USH/seaice_viirs
set -x

if [ $# -ne 3 ]; then
  echo need 3 inputs: day, hh, inst  
  exit
fi

day=$1
hh=$2
inst=$3
export PS4='$SECONDS + seaice_viirs.${inst}.${day}${hh}: '

echo zzz entered exseaice_viirs

export PYTHONPATH=$PYTHONPATH:$PACKAGEROOT/seaice_analysis.v4.5.1/sorc/mmablib/py
if [ ! -d $PACKAGEROOT/seaice_analysis.v4.5.1/sorc/mmablib/py ] ; then
  echo could not find mmablib/py
  exit 1
fi
echo zzz prepared python

$USHseaice_analysis/composite.py $DCOMROOT/$day/wgrdbul/IST/JRR-IceConcentration*_${inst}_s${day}${hh}*.nc \
    > viirs.$inst.$cyc.${day}$hh 
# Handle no file case 
if [ ! -f viirs.$inst.$cyc.${day}$hh ] ; then
  touch viirs.$inst.$cyc.${day}$hh
fi

cp -p viirs.$inst.$cyc.${day}$hh $COMOUT

echo " done with viirs.$inst.$cyc.${day}$hh processing, exiting...."
