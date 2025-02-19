#!/bin/sh

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


for d in seaice_avhrrfilter.Cd seaice_avhrrbufr.fd
do
  cd $d
  make
  cd ..
done
