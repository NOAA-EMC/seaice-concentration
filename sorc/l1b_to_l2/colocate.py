import os
import sys
import numpy as np
import netCDF4
from netCDF4 import Dataset

#----------------------------------------------
def oiv2(lat, lon):
  dlat = 0.25
  dlon = 0.25
  firstlat = -89.875
  firstlon = 0.125
  if (lon < 0):
    lon += 360.
  j = round( (lat - firstlat)/dlat )
  i = round( (lon - firstlon)/dlon )
  return (j,i)

def rg12th(lat, lon):
  dlat = -1./12.
  dlon = 1./12.
  firstlat = 90. - dlat/2.
  firstlon = dlon/2.
  if (lon < 0):
    lon += 360.
  j = round( (lat - firstlat)/dlat )
  i = round( (lon - firstlon)/dlon )
  return (j,i)

#----------------------------------------------
#matchup :
#longitude, latitude, quality, land, icec; 
#    ice_land, ice_post, ice_distance; sst, ice_sst
class match:

  def __init__(self, latitude = 95., longitude = 95., icec = 95., land = 95, quality = 95, ice_land = 95, ice_post = 95, ice_distance = 95., sst = 95., ice_sst = 95.):
    self.latitude = latitude
    self.longitude = longitude
    self.icec = icec
    self.land = land
    self.quality = quality
    self.ice_land = 95
    self.ice_post = 95
    self.ice_distance = 95.
    self.sst = 95.
    self.ice_sst = 95.
    #print("done with init", flush=True)

  def show(self, fout = sys.stdout):
    print("{:9.4f}".format(self.longitude), "{:8.4f}".format(self.latitude), 
               "{:.2f}".format(self.icec), "{:.2f}".format(self.land), self.quality, 
          "{:3d}".format(self.ice_land), "{:3d}".format(self.ice_post), 
                   "{:7.2f}".format(self.ice_distance), 
          "  ", "{:.2f}".format(self.sst), "{:.2f}".format(self.ice_sst), file=fout)

  def add_l2(self, longitude, latitude, quality, land, icec):
    self.longitude = longitude
    self.latitude  = latitude
    self.quality   = quality
    self.land      = land
    self.icec      = icec

  def add_oiv2(self, sst, ice_sst):
    j,i = oiv2(self.latitude, self.longitude)
    self.sst = sst[j,i]
    self.ice_sst = ice_sst[j,i]

  def add_icefix(self, ice_land, ice_post, ice_distance):
    j,i = rg12th(self.latitude, self.longitude)
    self.ice_land = ice_land[j,i]
    self.ice_post = ice_post[j,i]
    self.ice_distance = ice_distance[j,i]

###############################################################

icenc = Dataset('l2out.f248.51.nc', 'r', format='NETCDF4')
nobs = len(icenc.dimensions["nobs"])
    #print("nobs = ",nobs)
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

#print("longitudes: ",longitude.max(), longitude.min() );
#print("latitudes: ",latitude.max(), latitude.min() );
#print("quality: ",quality.max(), quality.min() );
#print("land: ",land.max(), land.min() );
#print("icec: ",icec.max(), icec.min() );

all = []
npts = 10000

#for k in range(0,nobs):
for k in range(0,npts):
  tmp = match(longitude = longitude[k], latitude = latitude[k], 
                quality = quality[k], land = land[k], icec = icec[k])
  all.append(tmp) 
  #print(k,longitude[k], latitude[k], quality[k], land[k], icec[k])
  #all[k].show()
  #tmp.show()

#for k in range(0,len(all)):
#  print(k,longitude[k], latitude[k], quality[k], land[k], icec[k])
#  all[k].show()

print("done creating 'all'", len(all))

#exit(0)

#----------------------------------------------
#read in skip file
#read in land mask file
#read in distance to land

icefix = Dataset('seaice_fixed_fields.nc', 'r', format='NETCDF4')
nlats = len(icefix.dimensions["nlats"])
nlons = len(icefix.dimensions["nlons"])
#print("fixed nlats = ",nlats)
#print("fixed nlons = ",nlons)

ice_longitude = np.zeros((nlats, nlons),dtype="double") 
ice_latitude = np.zeros((nlats, nlons),dtype="double") 
ice_distance = np.zeros((nlats, nlons),dtype="float") 

ice_land = np.zeros((nlats, nlons))
ice_land      = icefix.variables["land"]     [:,:] 
#print("land: ",ice_land.max(), ice_land.min() )

ice_post = np.zeros((nlats, nlons))
ice_post      = icefix.variables["posteriori"][:,:] 
#print("post: ",ice_post.max(), ice_post.min() )

ice_longitude = icefix.variables["longitude"][:,:] 
ice_latitude  = icefix.variables["latitude"] [:,:] 
ice_distance  = icefix.variables["distance_to_land"][:,:] 
ice_distance /= 1000.
#print("longitudes: ",ice_longitude.max(), ice_longitude.min() )
#print("latitudes: ",ice_latitude.max(), ice_latitude.min() )
#print("distance: ",ice_distance.max(), ice_distance.min() )

for k in range(0,len(all)):
  all[k].add_icefix(ice_land, ice_post, ice_distance)
#  if (all[k].ice_land != 157):
#    all[k].show()
#exit(0)


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
#x,edges = np.histogram(ice_land,range=(0,255), bins=255)
#for i in range (0,255):
#  if not (x[i] == 0):
#    print(i,x[i])
#
#y,edges = np.histogram(ice_post, range=(0,255), bins=255)
#for i in range (0,255):
#  if not (y[i] == 0):
#    print(i,y[i])

#----------------------------------------------
# Use SST from qdoi v2, including its sea ice cover
sstgrid = Dataset('avhrr-only-v2.20180228.nc', 'r', format='NETCDF4')

sst_nlats = len(sstgrid.dimensions["lat"])
sst_nlons = len(sstgrid.dimensions["lon"])
#print("sst_nlats = ",sst_nlats)
#print("sst_nlons = ",sst_nlons)

sst = np.zeros((sst_nlats, sst_nlons))
anom = np.zeros((sst_nlats, sst_nlons))
err  = np.zeros((sst_nlats, sst_nlons))
ice_sst = np.zeros((sst_nlats, sst_nlons))

sst   = sstgrid.variables["sst"][0,0,:,:]
anom  = sstgrid.variables["anom"] [0,0,:,:]
err   = sstgrid.variables["err"][0,0,:,:]
ice_sst   = sstgrid.variables["ice"][0,0,:,:]

#print("sst ",sst.max(), sst.min())
#print("ice_sst ",ice_sst.max(), ice_sst.min() )

for k in range(0,len(all)):
  all[k].add_oiv2(sst, ice_sst)

#------------- All collected now, print out where not land: ----------

for i in range(0,len(all)):
  if (all[i].ice_land != 157):
    all[i].show()


  

#histogram of tb for each channel, for ice areas
# -- max, min
#bayes p(ice | tb_i)
#bayes p(land | tb_i)

#####
