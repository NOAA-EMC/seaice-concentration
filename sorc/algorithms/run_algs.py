import os
import sys
import numpy as np
import numpy.ma as ma
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

def delta(x,y):
  return (x-y)/(x+y)


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
    self.tb = np.zeros((7))
    #print("done with init", flush=True)

  def show(self, fout = sys.stdout):
    print("{:9.4f}".format(self.longitude), "{:8.4f}".format(self.latitude), 
               "{:.2f}".format(self.icec), "{:.2f}".format(self.land), self.quality, 
          "{:3d}".format(self.ice_land), "{:3d}".format(self.ice_post), 
                   "{:7.2f}".format(self.ice_distance), 
          "  ", "{:.2f}".format(self.sst), "{:.2f}".format(self.ice_sst), 
          "  ", "{:6.2f}".format(self.tb[0]),
           "{:6.2f}".format(self.tb[1]),
           "{:6.2f}".format(self.tb[2]),
           "{:6.2f}".format(self.tb[3]),
           "{:6.2f}".format(self.tb[4]),
           "{:6.2f}".format(self.tb[5]),
           "{:6.2f}".format(self.tb[6]),
          file=fout)

  def add_tb(self, tb):
    for i in range (0,7):
      self.tb[i] = tb[i]

  def add_oiv2(self, sst, ice_sst):
    j,i = oiv2(self.latitude, self.longitude)
    self.sst = sst[j,i]
    self.ice_sst = ice_sst[j,i]

  def add_icefix(self, ice_land, ice_post, ice_distance):
    j,i = rg12th(self.latitude, self.longitude)
    self.ice_land = ice_land[j,i]
    self.ice_post = ice_post[j,i]
    self.ice_distance = ice_distance[j,i]

  def __getitem__(self, i):
    return(tb[i])

###############################################################

tb = np.zeros((7))

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

all = []
npts = nobs
#npts = 1000
for k in range(0,npts):
  tmp = match(longitude = longitude[k], latitude = latitude[k], 
                quality = quality[k], land = land[k], icec = icec[k])
  all.append(tmp) 
  tb[0] = t19v[k]
  tb[1] = t19h[k]
  tb[2] = t22v[k]
  tb[3] = t37v[k]
  tb[4] = t37h[k]
  tb[5] = t85v[k]
  tb[6] = t85h[k]
  all[k].add_tb(tb)

#exit(0)

#----------------------------------------------
#read in skip file
#read in land mask file
#read in distance to land

icefix = Dataset('seaice_fixed_fields.nc', 'r', format='NETCDF4')
nlats = len(icefix.dimensions["nlats"])
nlons = len(icefix.dimensions["nlons"])

ice_longitude = np.zeros((nlats, nlons),dtype="double") 
ice_latitude = np.zeros((nlats, nlons),dtype="double") 
ice_distance = np.zeros((nlats, nlons),dtype="float") 

ice_land = np.zeros((nlats, nlons))
ice_land      = icefix.variables["land"]     [:,:] 

ice_post = np.zeros((nlats, nlons))
ice_post      = icefix.variables["posteriori"][:,:] 

ice_longitude = icefix.variables["longitude"][:,:] 
ice_latitude  = icefix.variables["latitude"] [:,:] 
ice_distance  = icefix.variables["distance_to_land"][:,:] 
ice_distance /= 1000.   #Convert to km

for k in range(0,len(all)):
  all[k].add_icefix(ice_land, ice_post, ice_distance)
#exit(0)

#--------------------------------------------------------
# Use SST from qdoi v2, including its sea ice cover
sstgrid = Dataset('avhrr-only-v2.20180228.nc', 'r', format='NETCDF4')
sst_nlats = len(sstgrid.dimensions["lat"])
sst_nlons = len(sstgrid.dimensions["lon"])

sst = np.zeros((sst_nlats, sst_nlons))
ice_sst = np.zeros((sst_nlats, sst_nlons))

sst   = sstgrid.variables["sst"][0,0,:,:]
ice_sst   = sstgrid.variables["ice"][0,0,:,:]

#anom = np.zeros((sst_nlats, sst_nlons))
#err  = np.zeros((sst_nlats, sst_nlons))
#anom  = sstgrid.variables["anom"] [0,0,:,:]
#err   = sstgrid.variables["err"][0,0,:,:]

for k in range(0,len(all)):
  all[k].add_oiv2(sst, ice_sst)

#---------------------------------------------------------------------
#----------------------------------------------
#------------- All collected now, print out : ----------
#fout = open("all_tb","w")
#for i in range(0,len(all)):
#  #if (all[i].ice_land != 157):
#    all[i].show()
#fout.close()
#--------------------------------------------------------

del ice_land
del icec
del sst

ice_land = np.zeros((npts))
icec = np.zeros((npts))
sst  = np.zeros((npts))
for i in range(0,npts):
  ice_land[i] = all[i].ice_land
  icec[i] = all[i].ice_sst
  sst[i]  = all[i].sst

# create logical masks:
unknown = ma.masked_array(ice_land > -1) # unknown points, which starts as all of them

#include coast points as being land (sidelobe issues)
landmask = ma.masked_array(ice_land >= 157 )
watermask = ma.masked_array(ice_land < 100)
icemask   = ma.masked_array(icec > 0)

#Distinguish between water and ice-covered water
not_ice = np.logical_not(icemask)
watermask = ma.logical_and(watermask, not_ice)

# Get the indices of the 'true' points
mland = landmask.nonzero()
water = watermask.nonzero()
iceindices = icemask.nonzero()

nicepts = len(iceindices[0])
nlandpts = len(mland[0])
nwaterpts = len(water[0])
#---------------------------------------------------------------------
#  All data read in and apportioned, 
#     ice, land, water, and unknown pts. masks defined
#
#---------------------------------------------------------------------
print("n ice, land, water, nobs ",nicepts, nlandpts, nwaterpts, nobs)
print("p ice, land, water, nobs ",nicepts/float(nobs), nlandpts/float(nobs), 
         nwaterpts/float(nobs), nobs, flush=True)
pwater = nwaterpts/float(nobs)
pland  = nlandpts/float(nobs)
pice   = nicepts/float(nobs)

#----------------------------------------------------------------
#  Next step, pre-filter based on perfect land filters, perfect ice-free ocean filters
#  Exclude 22v because of F15
# Land points:
#satellite-based land mask
#Start with false everywhere and then 'or' in the trues:
sland_mask = ma.masked_array(t19h > 3000)

sland_mask = ma.logical_or(sland_mask, ma.masked_array(t19v > 265 ) )
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t19h > 252 ) )
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t37v > 267 ) )
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t37h > 260 ) )
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t85v > 281 ) )
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t85h > 278 ) )
tmp = ma.masked_array(delta(t19h, t37v) >  0.07100180784861249 )
sland_mask = ma.logical_or(sland_mask, tmp)
tmp = ma.masked_array(delta(t19v, t19h) <  0.006025966971811622 )
sland_mask = ma.logical_or(sland_mask, tmp)
tmp = ma.masked_array(delta(t37v, t37h) <  0.0024425569507810804 )
sland_mask = ma.logical_or(sland_mask, tmp)

sland_mask = ma.logical_or(sland_mask, ma.masked_array(t19v < 173) )
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t85v < 152) )
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t85h < 146) )

tmp = ma.masked_array(delta(t19v,t19h) <  0.003012983485905811 )
sland_mask = ma.logical_or(sland_mask, tmp)

#satellite-based land mask
sland_indices = sland_mask.nonzero()
nsland_pts = len(sland_indices[0])
print("number of satellite-caught land pts: ",nsland_pts)

# Water points:
#perfect satellite-based ice-free water mask
swater_mask = ma.masked_array(t19h > 3000)
tmp = ma.masked_array(delta(t19v, t85v) <  -0.15903521396897055)
swater_mask = ma.logical_or(swater_mask, tmp)

# Perfect not-ice, almost perfectly water:
tmp = ma.masked_array(delta(t37h, t85h) < -0.17274572903459723)
swater_mask = ma.logical_or(swater_mask, tmp)
tmp = ma.masked_array(delta(t19v, t37h) < -0.008470142881075546)
swater_mask = ma.logical_or(swater_mask, tmp)
tmp = ma.masked_array(delta(t19v, t85h) < -0.12797022016361506)
swater_mask = ma.logical_or(swater_mask, tmp)
tmp = ma.masked_array(delta(t19v, t85v) < -0.15477002254038147)
swater_mask = ma.logical_or(swater_mask, tmp)

# Very good not-ice (only 1%), very good water:
tmp = ma.masked_array(delta(t19v, t85v) < -0.12064849111166867)
swater_mask = ma.logical_or(swater_mask, tmp)
tmp = ma.masked_array(delta(t19h, t85h) < -0.2371562842768852)
swater_mask = ma.logical_or(swater_mask, tmp)

swater_indices = swater_mask.nonzero()
nswater_pts = len(swater_indices[0])
print("number of satellite-caught water pts: ",nswater_pts)

print("fraction of all points which are known after first pass: ",
       float(nswater_pts+nsland_pts)/float(nobs))

known = ma.logical_or(swater_mask, sland_mask)
unknown = ma.logical_not(known)
unknown_indices = unknown.nonzero()
nobs = len(unknown_indices[0])
print("nobs for round 2: ",nobs, flush=True)


#--------------------------------------------------------------------
from algorithms import *

#"""tiepts: w37v    my37v   fy37v    w37h  my37h    fy37h   w18v   my18v    fy18v   w18h    my18h  fy18h    w6v     my6v    fy6v    w6h   my6h    fy6h    w10v    my10v   fy10v  w10h    my10h  fy10h    w22v    my22v  fy22v    w22h"""
#"""my22h   fy22h   my85v    fy85v my85h    fy85h  w85v  w85h"""

# Northern Hemisphere tie-points:
tiepts_nh = [209.81, 187.18, 246.29, 145.29, 175.72, 235.15, 183.72, 219.66, 251.56, 108.46, 201.66, 237.16, 161.35, 242.91, 251.59, 82.13, 218.74, 232.14, 167.34, 235.26, 250.89, 88.26, 212.47, 234.16, 196.41, 208.82, 250.18, 128.23, 192.09, 236.42, 178.01, 229.52, 169.52, 220.94, 243.20, 196.94]

# Southern Hemisphere tie-points:
tiepts_sh = [209.81, 187.18, 246.29, 145.29, 175.72, 235.15, 183.72, 219.66, 251.56, 108.46, 201.66, 237.16, 161.35, 242.91, 251.59, 82.13, 218.74, 232.14, 167.34, 235.26, 250.89, 88.26, 212.47, 234.16, 196.41, 208.82, 250.18, 128.23, 192.09, 236.42, 178.01, 229.52, 169.52, 220.94, 243.20, 196.94]

#for k in range(0,nobs):
for k in range(0,nobs):
  i = unknown_indices[0][k]
  lat = all[i].latitude
  tbtmp = all[i].tb
  tb18v = tbtmp[0]
  tb18h = tbtmp[1]
  tb22v = tbtmp[2]
  tb37v = tbtmp[3]
  tb37h = tbtmp[4]
  tb85v = tbtmp[5]
  tb85h = tbtmp[6]

  if (lat > 0):
    tiepts = tiepts_nh
  else:
    tiepts = tiepts_sh

  CT_asi = asi(tb85v, tb85h)*100
  #CT_nasa2 = nasa2(tb18v, tb18h, tb37v, tb85v, tb85h)*100
  CT_near90 = near90(tb85v, tb85h, tiepts)*100
  CT_near90_linear = near90_linear(tb85v, tb85h)*100
  CT_P90 = P90(tb85v, tb85h)*100
  CT_tud = tud(tb18v, tb37v, tb85v, tb85h, tiepts)*100
  CT_bootstrap_f = bootstrap_f(tb18v, tb37v, tiepts)*100
  CT_bootstrap_p = bootstrap_p(tb37v, tb37h, tiepts)*100
  CT_bristol = bristol(tb18v, tb37v, tb37h, tiepts)*100
  CT_calval = calval(tb37v,tb18v,tiepts)*100
  CT_nasa = nasa(tb18v, tb18h, tb37v, tiepts)*100
  CT_norsex = norsex(tb18v,tb37v,tiepts)*100
  CT_nrl = nrl(tb37v, tb37h)*100
  CT_pr = pr(tb18v, tb18h, tb37v, tb37h, tiepts)*100
  CT_twochannel18 = twochannel18(tb18v, tb18h, tiepts)*100
  CT_twochannel37 = twochannel37(tb37v, tb37h, tiepts)*100
  CT_twochannel37_linear = twochannel37_linear(tb37v,tb37h)*100
  CT_UMass = UMass(tb18v,tb37v,tiepts)*100

    #CT_nasa2 ,
  print(k, all[i].ice_sst*100., 'suite',
    CT_asi         , CT_near90      , CT_near90_linear , CT_P90    , CT_tud ,
    CT_bootstrap_f , CT_bootstrap_p , CT_bristol       , CT_calval , CT_nasa ,
    CT_norsex      , CT_nrl         , CT_pr            , CT_twochannel18  , CT_twochannel37 ,
    CT_twochannel37_linear , CT_UMass,
    flush=True )
