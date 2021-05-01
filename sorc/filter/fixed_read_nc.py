import os
import sys
import numpy as np
import netCDF4
from netCDF4 import Dataset

icenc = Dataset('seaice_fixed_fields.nc', 'r', format='NETCDF4')
nlats = len(icenc.dimensions["nlats"])
nlons = len(icenc.dimensions["nlons"])

print("nlats = ",nlats)
print("nlons = ",nlons)

longitude = np.zeros((nlats, nlons),dtype="double") 
latitude = np.zeros((nlats, nlons),dtype="double") 
distance = np.zeros((nlats, nlons),dtype="float") 

land = np.zeros((nlats, nlons))
land      = icenc.variables["land"]     [:,:] 
print("land: ",land.max(), land.min() )

post = np.zeros((nlats, nlons))
post      = icenc.variables["posteriori"][:,:] 
print("post: ",post.max(), post.min() )

longitude = icenc.variables["longitude"][:,:] 
latitude  = icenc.variables["latitude"] [:,:] 
distance  = icenc.variables["distance_to_land"][:,:] 
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

