help([[
Load environment to run GFS on Hera
]])

local spack_mod_path=(os.getenv("spack_mod_path") or "None")
prepend_path("MODULEPATH", spack_mod_path)

load(pathJoin("stack-intel", (os.getenv("stack_intel_ver") or "None")))
load(pathJoin("stack-intel-oneapi-mpi", (os.getenv("stack_impi_ver") or "None")))

load(pathJoin("jasper", (os.getenv("jasper_ver") or "None")))
load(pathJoin("libpng", (os.getenv("libpng_ver") or "None")))

load(pathJoin("hdf5", (os.getenv("hdf5_ver") or "None")))
load(pathJoin("netcdf-c", (os.getenv("netcdf_c_ver") or "None")))
load(pathJoin("netcdf-fortran", (os.getenv("netcdf_fortran_ver") or "None")))

load(pathJoin("prod_util", (os.getenv("prod_util_ver") or "None")))
load(pathJoin("grib-util", (os.getenv("grib_util_ver") or "None")))
load(pathJoin("g2tmpl", (os.getenv("g2tmpl_ver") or "None")))
load(pathJoin("crtm", (os.getenv("crtm_ver") or "None")))
load(pathJoin("bufr", (os.getenv("bufr_ver") or "None")))
load(pathJoin("wgrib2", (os.getenv("wgrib2_ver") or "None")))

setenv("WGRIB2","wgrib2")

-- Stop gap fix for wgrib with spack-stack 1.6.0
-- TODO Remove this when spack-stack issue #1097 is resolved
setenv("WGRIB","wgrib")

whatis("Description: sea ice run environment")
