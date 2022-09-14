import os
import sys
import numpy as np
import numpy.ma as ma
import netCDF4
from netCDF4 import Dataset

from filtering import *

###############################################################

tb = np.zeros((7))

icenc = Dataset('l2out.f248.51.nc', 'r', format='NETCDF4')
nobs = len(icenc.dimensions["nobs"])
print("nobs = ",nobs)
longitude = np.zeros((nobs)) 
latitude = np.zeros((nobs)) 
icec = np.zeros((nobs)) 
quality = np.zeros((nobs), dtype='int') 
satid = np.zeros((nobs), dtype='int') 
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
satid = icenc.variables["satid"][:] 
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
for k in range(0,npts):
  tmp = match(satid = satid[k], longitude = longitude[k], latitude = latitude[k], 
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

print("done reading in",flush=True)
# create logical masks:
#unknown = ma.masked_array(ice_land > -1) # unknown points, which starts as all of them
unknown = ma.masked_array(satid != 248) # unknown points, which is anything not F15
known = ma.logical_not(unknown)
print("unknown, known lens: ",len(unknown.nonzero()[0]), len(known.nonzero()[0])  , flush=True)
nobs = len(unknown.nonzero()[0])

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

print("done adding in ice fixed",flush=True)
#exit(0)

#--------------------------------------------------------
# Use SST from qdoi v2, including its sea ice cover
#sstgrid = Dataset('avhrr-only-v2.20180228.nc', 'r', format='NETCDF4')
sstgrid = Dataset('avhrr-only.nc', 'r', format='NETCDF4')
sst_nlats = len(sstgrid.dimensions["lat"])
sst_nlons = len(sstgrid.dimensions["lon"])

sst = np.zeros((sst_nlats, sst_nlons))
ice_sst = np.zeros((sst_nlats, sst_nlons))

sst   = sstgrid.variables["sst"][0,0,:,:]
ice_sst   = sstgrid.variables["ice"][0,0,:,:]

for k in range(0,len(all)):
  all[k].add_oiv2(sst, ice_sst)

print("done adding in sst ",flush=True)
#---------------------------------------------------------------------
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


#include coast points as being land (sidelobe issues)
icemask   = ma.masked_array(icec > 0)
landmask  = ma.masked_array(ice_land >= 157 )
watermask = ma.masked_array(ice_land < 100)
icemask = ma.logical_and(icemask, unknown)
landmask = ma.logical_and(landmask, unknown)
watermask = ma.logical_and(watermask, unknown)

#Distinguish between water and ice-covered water
not_ice = np.logical_not(icemask)
watermask = ma.logical_and(watermask, not_ice)

#---------------------------------------------------------------------
# RG: Could be its own function (landmask, landmask, watermask) getting
#    nice, nland, nwater, and computing pwater, pland, pice
# Get the indices of the 'true' points
iceindices = icemask.nonzero()
mland      = landmask.nonzero()
water      = watermask.nonzero()

nicepts   = len(iceindices[0])
nlandpts  = len(mland[0])
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
#---------------------------------------------------------------------

#----------------------------------------------------------------
fout = open("round1","w")
for thot in range (75, 315):
  bayes(t19v, thot, "t19v", unknown, nobs, landmask, icemask, watermask, fout)
  bayes(t19h, thot, "t19h", unknown, nobs, landmask, icemask, watermask, fout)
  bayes(t22v, thot, "t22v", unknown, nobs, landmask, icemask, watermask, fout)
  bayes(t37v, thot, "t37v", unknown, nobs, landmask, icemask, watermask, fout)
  bayes(t37h, thot, "t37h", unknown, nobs, landmask, icemask, watermask, fout)
  bayes(t85v, thot, "t85v", unknown, nobs, landmask, icemask, watermask, fout)
  bayes(t85h, thot, "t85h", unknown, nobs, landmask, icemask, watermask, fout)

dr(t19v, t19h, "drt19vt19h", unknown, nobs, landmask, icemask, watermask, fout)
dr(t19v, t22v, "drt19vt22v", unknown, nobs, landmask, icemask, watermask, fout)
dr(t19v, t37v, "drt19vt37v", unknown, nobs, landmask, icemask, watermask, fout)
dr(t19v, t37h, "drt19vt37h", unknown, nobs, landmask, icemask, watermask, fout)
dr(t19v, t85v, "drt19vt85v", unknown, nobs, landmask, icemask, watermask, fout)
dr(t19v, t85h, "drt19vt85h", unknown, nobs, landmask, icemask, watermask, fout)
dr(t19h, t22v, "drt19ht22v", unknown, nobs, landmask, icemask, watermask, fout)
dr(t19h, t37v, "drt19ht37v", unknown, nobs, landmask, icemask, watermask, fout)
dr(t19h, t37h, "drt19ht37h", unknown, nobs, landmask, icemask, watermask, fout)
dr(t19h, t85v, "drt19ht85v", unknown, nobs, landmask, icemask, watermask, fout)
dr(t19h, t85h, "drt19ht85h", unknown, nobs, landmask, icemask, watermask, fout)
dr(t22v, t37v, "drt22vt37v", unknown, nobs, landmask, icemask, watermask, fout)
dr(t22v, t37h, "drt22vt37h", unknown, nobs, landmask, icemask, watermask, fout)
dr(t22v, t85v, "drt22vt85v", unknown, nobs, landmask, icemask, watermask, fout)
dr(t22v, t85h, "drt22vt85h", unknown, nobs, landmask, icemask, watermask, fout)
dr(t37v, t37h, "drt37vt37h", unknown, nobs, landmask, icemask, watermask, fout)
dr(t37v, t85v, "drt37vt85v", unknown, nobs, landmask, icemask, watermask, fout)
dr(t37v, t85h, "drt37vt85h", unknown, nobs, landmask, icemask, watermask, fout)
dr(t37h, t85v, "drt37ht85v", unknown, nobs, landmask, icemask, watermask, fout)
dr(t37h, t85h, "drt37ht85h", unknown, nobs, landmask, icemask, watermask, fout)
dr(t85v, t85h, "drt85vt85h", unknown, nobs, landmask, icemask, watermask, fout)
fout.close()

exit(0)

#----------------------------------------------------------------

#  Next step, pre-filter based on perfect land filters, perfect ice-free ocean filters
#  Exclude 22v because of F15
#
# Land points:
#satellite-based land mask
#Start with false everywhere and then 'or' in the trues:
sland_mask  = ma.masked_array(t19h > 3000)
swater_mask = ma.masked_array(t19h > 3000)
sland_mask = ma.logical_or(sland_mask, known)
swater_mask = ma.logical_or(swater_mask, known)

# tb filters, perfect not-ice, very good land:
#hot side:
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t19v > 270))
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t19h > 263))
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t22v > 270))
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t37v > 267))
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t37h > 262))
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t85v > 270))
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t85h > 263))
# Cold side:
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t19v < 176))
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t22v < 185))
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t37v < 195))
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t85v < 184))
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t85h < 170))


# dr filters, perfect not-ice, very good water:
tmp = ma.masked_array(delta(t37v, t85v) <  -0.08223462165004075)
swater_mask = ma.logical_or(swater_mask, tmp)
tmp = ma.masked_array(delta(t19v, t22v) <  -0.05371907187832736)
swater_mask = ma.logical_or(swater_mask, tmp)
tmp = ma.masked_array(delta(t37h, t85h) < -0.17385640469464386)
swater_mask = ma.logical_or(swater_mask, tmp)
tmp = ma.masked_array(delta(t19v, t85v) < -0.14244067066847677)
swater_mask = ma.logical_or(swater_mask, tmp)

#very good not-ice, very good water
tmp = ma.masked_array(t19v >= 176 )
tmp = ma.logical_and(tmp, t19v < 191)
swater_mask = ma.logical_or(swater_mask, tmp)
swater_mask = ma.logical_or(swater_mask, t19h < 126)

tmp = ma.masked_array(delta(t19v, t22v) <  -0.045881619056065914) #extends above
swater_mask = ma.logical_or(swater_mask, tmp)
tmp = ma.masked_array(delta(t19v, t37v) < -0.05464559974092431)
swater_mask = ma.logical_or(swater_mask, tmp)
tmp = ma.masked_array(delta(t19v, t85v) < -0.13226512947467844) #extends above
swater_mask = ma.logical_or(swater_mask, tmp)


#satellite-based land mask
sland_indices = sland_mask.nonzero()
nsland_pts    = len(sland_indices[0])
print("number of satellite-caught land pts: ",nsland_pts)

# satellite-based water points: 
swater_indices = swater_mask.nonzero()
nswater_pts    = len(swater_indices[0])
print("number of satellite-caught water pts: ",nswater_pts)

# -----------------  Prepare masks for second pass ------------------
known   = ma.logical_or(swater_mask, sland_mask)
unknown = ma.logical_not(known)
unknown_indices = unknown.nonzero()
nobs   = len(unknown_indices[0])
print("nobs for round 2: ",nobs, flush=True)

landmask  = ma.logical_and(landmask , unknown)
icemask   = ma.logical_and(icemask , unknown)
watermask = ma.logical_and(watermask , unknown)
nicepts   = len(icemask.nonzero()[0])
nlandpts  = len(landmask.nonzero()[0])
nwaterpts = len(watermask.nonzero()[0])
pwater   = nwaterpts/float(nobs)
pland    = nlandpts/float(nobs)
pice     = nicepts/float(nobs)
print("after first pass, nobs, ice, land, water ",nobs, nicepts, nlandpts, 
        nwaterpts, pice, pland, pwater, flush=True)

#----------------------------------------------------------------
#  Repeat original process check on the remaining points --
#    -- can we improve the identification of land and ice-free water?
#    -- next step: finding a filter for 'not-ice', maybe land or water,
#          but definitely not sea ice
#    -- or next: finding a filter for 'is sea ice'
#-----------------------------------------------
#fout2 = open("tb2","w")
#for i in range(0,nobs):
#    all[unknown_indices[0][i] ].show(fout2)
#fout2.close()
#-----------------------------------------------

fout = open("round2","w")
for thot in range (125, 275):
  bayes(t19v, thot, "t19v", unknown, nobs, landmask, icemask, watermask, fout)
  bayes(t19h, thot, "t19h", unknown, nobs, landmask, icemask, watermask, fout)
  bayes(t22v, thot, "t22v", unknown, nobs, landmask, icemask, watermask, fout)
  bayes(t37v, thot, "t37v", unknown, nobs, landmask, icemask, watermask, fout)
  bayes(t37h, thot, "t37h", unknown, nobs, landmask, icemask, watermask, fout)
  bayes(t85v, thot, "t85v", unknown, nobs, landmask, icemask, watermask, fout)
  bayes(t85h, thot, "t85h", unknown, nobs, landmask, icemask, watermask, fout)

dr(t19v, t19h, "drt19vt19h", unknown, fout)
dr(t19v, t22v, "drt19vt22v", unknown, fout)
dr(t19v, t37v, "drt19vt37v", unknown, fout)
dr(t19v, t37h, "drt19vt37h", unknown, fout)
dr(t19v, t85v, "drt19vt85v", unknown, fout)
dr(t19v, t85h, "drt19vt85h", unknown, fout)
dr(t19h, t22v, "drt19ht22v", unknown, fout)
dr(t19h, t37v, "drt19ht37v", unknown, fout)
dr(t19h, t37h, "drt19ht37h", unknown, fout)
dr(t19h, t85v, "drt19ht85v", unknown, fout)
dr(t19h, t85h, "drt19ht85h", unknown, fout)
dr(t22v, t37v, "drt22vt37v", unknown, fout)
dr(t22v, t37h, "drt22vt37h", unknown, fout)
dr(t22v, t85v, "drt22vt85v", unknown, fout)
dr(t22v, t85h, "drt22vt85h", unknown, fout)
dr(t37v, t37h, "drt37vt37h", unknown, fout)
dr(t37v, t85v, "drt37vt85v", unknown, fout)
dr(t37v, t85h, "drt37vt85h", unknown, fout)
dr(t37h, t85v, "drt37ht85v", unknown, fout)
dr(t37h, t85h, "drt37ht85h", unknown, fout)
dr(t85v, t85h, "drt85vt85h", unknown, fout)

fout.close() 
#
#----------------------------------------------------------------
from algorithms import *

#tie points -- added to algorithms

fout = open("unknown","w")

for k in range(0,len(unknown_indices[0])):
  i = unknown_indices[0][k]
  x = all[unknown_indices[0][k]]
  if (x.latitude > 0):
    CT = nasa(t19v[i], t19h[i], t37v[i], tiepts_nh)
  else:
    CT = nasa(t19v[i], t19h[i], t37v[i], tiepts_sh)
  CT=min(1.,CT)

  print("{:9.4f}".format(x.longitude), "{:8.4f}".format(x.latitude),
        "{:.2f}".format(x.land), 
        "{:3d}".format(x.ice_land), "{:3d}".format(x.ice_post),
        "{:7.2f}".format(x.ice_distance),
        "  ", "{:.2f}".format(x.sst), "{:.2f}".format(x.ice_sst),
        "  ", "{:6.2f}".format(x.tb[0]),
           "{:6.2f}".format(x.tb[1]),
           "{:6.2f}".format(x.tb[2]),
           "{:6.2f}".format(x.tb[3]),
           "{:6.2f}".format(x.tb[4]),
           "{:6.2f}".format(x.tb[5]),
           "{:6.2f}".format(x.tb[6]),
           CT, CT - x.ice_sst,
          file=fout)
