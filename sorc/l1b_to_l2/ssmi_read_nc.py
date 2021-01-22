import os
import sys
import numpy as np
import netCDF4
from netCDF4 import Dataset

icenc = Dataset('l2out.f248.51.nc', 'r', format='NETCDF4')
nobs = len(icenc.dimensions["nobs"])
print("nobs = ",nobs)
longitude = np.zeros((nobs)) 
latitude = np.zeros((nobs)) 
icec = np.zeros((nobs)) 
quality = np.zeros((nobs), dtype='int') 
land = np.zeros((nobs)) 
dtg1 = np.zeros((nobs), dtype='int') 
dtg2 = np.zeros((nobs), dtype='int') 
t19v = np.zeros((nobs)) 
t19h = np.zeros((nobs)) 
t22v = np.zeros((nobs)) 
t37v = np.zeros((nobs)) 
t37h = np.zeros((nobs)) 
t85v = np.zeros((nobs)) 
t85h = np.zeros((nobs)) 

longitude = icenc.variables["longitude"][:] 
latitude = icenc.variables["latitude"][:] 
icec = icenc.variables["ice_concentration"][:] 
quality = icenc.variables["quality"][:] 
land = icenc.variables["land_flag"][:] 
dtg1 = icenc.variables["dtg_yyyymmdd"][:] 
dtg2 = icenc.variables["dtg_hhmm"][:] 
t19v = icenc.variables["tb_19V"][:] 
t19h = icenc.variables["tb_19H"][:] 
t22v = icenc.variables["tb_22V"][:] 
t37v = icenc.variables["tb_37V"][:] 
t37h = icenc.variables["tb_37H"][:] 
t85v = icenc.variables["tb_85V"][:] 
t85h = icenc.variables["tb_85H"][:] 

print("longitudes: ",longitude.max(), longitude.min() );
print("latitudes: ",latitude.max(), latitude.min() );
print("dtg2: ",dtg2.max(), dtg2.min() );
print("quality: ",quality.max(), quality.min() );
print("land: ",land.max(), land.min() );
print("icec: ",icec.max(), icec.min() );

#----------------------------------------------
#read in skip file
#read in land mask file
#read in distance to land

icefix = Dataset('seaice_fixed_fields.nc', 'r', format='NETCDF4')
nlats = len(icefix.dimensions["nlats"])
nlons = len(icefix.dimensions["nlons"])

print("nlats = ",nlats)
print("nlons = ",nlons)

longitude = np.zeros((nlats, nlons),dtype="double") 
latitude = np.zeros((nlats, nlons),dtype="double") 
distance = np.zeros((nlats, nlons),dtype="float") 

land = np.zeros((nlats, nlons))
land      = icefix.variables["land"]     [:,:] 
print("land: ",land.max(), land.min() )

post = np.zeros((nlats, nlons))
post      = icefix.variables["posteriori"][:,:] 
print("post: ",post.max(), post.min() )

longitude = icefix.variables["longitude"][:,:] 
latitude  = icefix.variables["latitude"] [:,:] 
distance  = icefix.variables["distance_to_land"][:,:] 
print("longitudes: ",longitude.max(), longitude.min() )
print("latitudes: ",latitude.max(), latitude.min() )
print("distance: ",distance.max(), distance.min() )

#Land Flag values:
#0   -- water
#157 -- land
#195 -- coast (a bounding curve runs through the grid cell)
#
#Posteriori flag values:
#2 -- ?
#158 -- Ocean > 26 C
#159 -- Ocean > 24 C
#160 -- Ocean > 22 C
#161 -- Ocean > 19 C
#162 -- Ocean > 15 C
#163 -- Ocean > 9 C
#164 -- Ocean > 2.15 C 275.3 K
#165 -- Ocean > -3 C
#170 -- Inland > 7 C
#171 -- Inland > 4 C
#172 -- Inland > 2.15 C
#173 -- Inland > 0 C
#174 -- Inland > -3 C
#224 -- undefined

x,edges = np.histogram(land,range=(0,255), bins=255)
for i in range (0,255):
  if not (x[i] == 0):
    print(i,x[i])

y,edges = np.histogram(post, range=(0,255), bins=255)
for i in range (0,255):
  if not (y[i] == 0):
    print(i,y[i])

#----------------------------------------------
# Use SST from qdoi v2, including its sea ice cover
sstgrid = Dataset('avhrr-only-v2.20180228.nc', 'r', format='NETCDF4')

sst_nlats = len(sstgrid.dimensions["lat"])
sst_nlons = len(sstgrid.dimensions["lon"])
print("sst_nlats = ",sst_nlats)
print("sst_nlons = ",sst_nlons)

sst   = sstgrid.variables["sst"][:,:] 
anom  = sstgrid.variables["anom"] [:,:] 
err   = sstgrid.variables["err"][:,:] 
ice_sst   = sstgrid.variables["ice"][:,:] 

print("sst ",sst.max(), sst.min() )
print("ice_sst ",ice_sst.max(), ice_sst.min() )
#----------------------------------------------

#histogram of tb for each channel, for ice areas
# -- max, min
#bayes p(ice | tb_i)
#bayes p(land | tb_i)

def alpha(x):
  print("hello alpha")


