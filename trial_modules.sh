module purge
module avail 2> avail.1

module load envvar/1.0
module load imagemagick/7.0.8-7
module load prod_envir/2.0.5
module load prod_util/2.0.8

module list
module avail 2> avail.2

#Can be loaded directly -- required for additional modules
module load intel/19.1.3.304
module load craype/2.7.8
module load PrgEnv-intel/8.1.0

#Additional:
module load wgrib2/2.0.8
module load w3nco/2.4.1


module load jasper/2.0.25
module load libpng/1.6.37
module load zlib/1.2.11
module load libjpeg/9c
module load bacio/2.4.1
module load bufr/11.5.0
module load g2/3.4.5

module list
module avail 2> avail.3

for f in hdf5/1.10.6 netcdf/4.7.4 cray-mpich/8.1.7 grib_util/1.2.3 w3emc/2.7.3
do
  echo trying $f
  #module spider $f
  module load $f
done

module list
module avail 2> avail.4

for f in bufr_dumplist dumpjb ips impi lsf 
do
  echo trying $f
  module spider $f
  module load $f
done

module list
