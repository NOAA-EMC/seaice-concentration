import os
import sys
import numpy as np
import netCDF4
from netCDF4 import Dataset
from math import *

def oiv2(lat, lon):
  dlat = 0.25
  dlon = 0.25
  firstlat = -89.875
  firstlon = 0.125
  j = round( (lat - firstlat)/dlat )
  i = round( (lon - firstlon)/dlon )
  return (j,i)


sstgrid = Dataset('avhrr-only-v2.20180228.nc', 'r', format='NETCDF4')

sst_nlats = len(sstgrid.dimensions["lat"])
sst_nlons = len(sstgrid.dimensions["lon"])
print("sst_nlats = ",sst_nlats)
print("sst_nlons = ",sst_nlons)

sst = np.zeros((sst_nlats, sst_nlons))
anom = np.zeros((sst_nlats, sst_nlons))
err  = np.zeros((sst_nlats, sst_nlons))
ice_sst = np.zeros((sst_nlats, sst_nlons))

sst   = sstgrid.variables["sst"][0,0,:,:] 
anom  = sstgrid.variables["anom"] [0,0,:,:] 
err   = sstgrid.variables["err"][0,0,:,:] 
ice_sst   = sstgrid.variables["ice"][0,0,:,:] 

print("sst ",sst.max(), sst.min() )
print("ice_sst ",ice_sst.max(), ice_sst.min() )

j,i = oiv2(80, 5)
print("j, i ",j,i)
print(len(sst))
print(sst[j,i])
exit(0)

# oiv2 sst runs south to north, half-integral points
for j in range(0,sst_nlats):
  print("sst ",j, sst[j,:].min())
