----------------------------------------------------------------------------------
--%Module  -----------------------------------------------------------------------
----                                                      Robert.Grumbine@noaa.gov 
----                                                      EMC/NCEP/NOAA
----   Sea ice concentration analysis 4.4.0
----_____________________________________________________
----
--proc ModulesHelp { } {
--        puts stderr "sea ice concentration analysis modules"
--}
--
whatis(" ice concentration analysis whatis description")

-- Make sure envvar is loaded
prereq("envvar/1.0")

--set ver $seaice_analysis_ver
--set lname seaice_analysis

-- Cactus -- WCOSS2
--export MMAB_BASE=~/rgdev/mmablib/

--load("imagemagick/"..os.getenv("imagemagick_ver"))
--load("prod_envir/"..os.getenv("prod_envir_ver"))
--load("prod_util/"..os.getenv("prod_util_ver"))

load("intel/"..os.getenv("intel_ver"))
load("craype/"..os.getenv("craype_ver"))
load("PrgEnv-intel/"..os.getenv("PrgEnv_intel_ver"))

--load("wgrib2/"..os.getenv("wgrib2_ver"))
load("w3nco/"..os.getenv("w3nco_ver"))
load("jasper/"..os.getenv("jasper_ver"))
load("libpng/"..os.getenv("libpng_ver"))
load("zlib/"..os.getenv("zlib_ver"))
--load("libjpeg/"..os.getenv("libjpeg_ver"))
load("bacio/"..os.getenv("bacio_ver"))
load("bufr/"..os.getenv("bufr_ver"))
load("g2/"..os.getenv("g2_ver"))

--load("hdf5/"..os.getenv("hdf5_ver"))
load("netcdf/"..os.getenv("netcdf_ver"))
load("cray-mpich/"..os.getenv("cray_mpich_ver"))
--load("grib_util/"..os.getenv("grib_util_ver"))
load("w3emc/"..os.getenv("w3emc_ver"))

-- Set other library variables

setenv("COMP","ftn")
setenv("FC","ftn")
setenv("CC","cc")
setenv("CPP","CC")
