#!/bin/sh

pwd=`pwd`;

if [ -z $MMAB_BASE ] ; then
  echo MMAB_BASE for mmablib has not been defined
  exit
fi
  
if [ ! -f makeall.mk ] ; then
  cp ../makeall.mk .
  if [ $? -ne 0 ] ; then
    echo could not find makeall.mk, aborting
    exit 1
  fi
fi
if [ -z $MMAB_LIBF4 ] ; then
  export MMAB_LIBF4=$MMAB_BASE/libombf_4.a
fi

for d in seaice_amsrbufr.fd seaice_iceamsr2.Cd 
do
  cd $pwd/$d
  make
  cd ..
done
