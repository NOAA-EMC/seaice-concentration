import os
import sys
import numpy as np
import numpy.ma as ma
import netCDF4
from netCDF4 import Dataset

#----------------------------------------------
from tools import *
#----------------------------------------------
## Define utilities for doing the assessment:

def bayes(xvec, xcrit, label, unknown, fout = sys.stdout ):
  warm = ma.masked_array(xvec > xcrit)
  warm = ma.logical_and(warm, unknown)
  nwarm = len(warm.nonzero()[0]) 
  lmask = np.logical_and(landmask, warm)
  imask = np.logical_and(icemask, warm)
  omask = np.logical_and(watermask, warm)
  pwarm = float(nwarm)/float(nobs)
  pover_land  = len(lmask.nonzero()[0])/nlandpts
  pover_water = len(omask.nonzero()[0])/nwaterpts
  pover_ice   = len(imask.nonzero()[0])/nicepts
  if (pwarm > 0):
    print(label, "hot ", xcrit,
      "{:5.3f}".format(pover_ice * pice / pwarm) ,
      "{:5.3f}".format(pover_land * pland / pwarm) ,
      "{:5.3f}".format(pover_water * pwater / pwarm), nwarm, file = fout )

  cold = ma.masked_array(xvec < xcrit)
  cold = ma.logical_and(cold, unknown)
  ncold = len(cold.nonzero()[0]) 
  lmask = np.logical_and(landmask, cold)
  imask = np.logical_and(icemask, cold)
  omask = np.logical_and(watermask, cold)
  pcold = float(ncold)/float(nobs)
  pover_land  = len(lmask.nonzero()[0])/nlandpts
  pover_water = len(omask.nonzero()[0])/nwaterpts
  pover_ice   = len(imask.nonzero()[0])/nicepts
  if (pcold > 0):
    print(label,"cold ",xcrit,
      "{:5.3f}".format(pover_ice * pice / pcold) ,
      "{:5.3f}".format(pover_land * pland / pcold) ,
      "{:5.3f}".format(pover_water * pwater / pcold), ncold, file = fout )

def dr(x, y, label, unknown, fout = sys.stdout):
  ratio = delta(x,y)
  tc = np.linspace(ratio.min(), ratio.max(), num=100)
  for i in range(0,len(tc)):
    bayes(ratio, tc[i], label, unknown, fout)
  del ratio

##----------------------------------------------
#
tb = np.zeros((7))

icenc = Dataset(sys.argv[1], 'r', format='NETCDF4')
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
ice_land = icefix.variables["land"]     [:,:] 

ice_post = np.zeros((nlats, nlons))
ice_post = icefix.variables["posteriori"][:,:] 

ice_longitude = icefix.variables["longitude"][:,:] 
ice_latitude  = icefix.variables["latitude"] [:,:] 
ice_distance  = icefix.variables["distance_to_land"][:,:] 
ice_distance /= 1000.   #Convert to km

#debug print("adding icefix to matchup", flush=True)
for k in range(0,len(all)):
  all[k].add_icefix(ice_land, ice_post, ice_distance)
#debug print("done adding in ice fixed",flush=True)
#exit(0)

# create logical masks:
unknown = ma.masked_array(satid > -1) # unknown points, which starts as all of them
#unknown = ma.masked_array(satid != 248) # unknown points, which is anything not F15
known = ma.logical_not(unknown)
print("unknown, known lens: ",len(unknown.nonzero()[0]), len(known.nonzero()[0])  , flush=True)
nobs = len(unknown.nonzero()[0])
if (nobs == 0):
  print("no observations to work with nobs npts: ",nobs, npts)
  exit(1)

#--------------------------------------------------------
# Use SST from qdoi v2, including its sea ice cover
#sstgrid = Dataset('avhrr-only-v2.20180228.nc', 'r', format='NETCDF4')
sstgrid = Dataset(sys.argv[2], 'r', format='NETCDF4')
sst_nlats = len(sstgrid.dimensions["lat"])
sst_nlons = len(sstgrid.dimensions["lon"])

sst = np.zeros((sst_nlats, sst_nlons))
ice_sst = np.zeros((sst_nlats, sst_nlons))

sst   = sstgrid.variables["sst"][0,0,:,:]
ice_sst   = sstgrid.variables["ice"][0,0,:,:]

for k in range(0,len(all)):
  all[k].add_oiv2(sst, ice_sst)

#debug print("done adding in sst ",flush=True)
#exit(0)
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
icemask   = ma.logical_and(icemask, unknown)
landmask  = ma.logical_and(landmask, unknown)
watermask = ma.logical_and(watermask, unknown)

#Distinguish between water and ice-covered water
not_ice   = np.logical_not(icemask)
watermask = ma.logical_and(watermask, not_ice)

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

#----------------------------------------------------------------
fout = open("round1","w")
for thot in range (75, 315):
  bayes(t19v, thot, "t19v", unknown, fout)
  bayes(t19h, thot, "t19h", unknown, fout)
  bayes(t37v, thot, "t37v", unknown, fout)
  bayes(t37h, thot, "t37h", unknown, fout)
  bayes(t85v, thot, "t85v", unknown, fout)
  bayes(t85h, thot, "t85h", unknown, fout)
#  bayes(t22v, thot, "t22v", unknown, fout)

dr(t19v, t19h, "drt19vt19h", unknown, fout)
dr(t19v, t37v, "drt19vt37v", unknown, fout)
dr(t19v, t37h, "drt19vt37h", unknown, fout)
dr(t19v, t85v, "drt19vt85v", unknown, fout)
dr(t19v, t85h, "drt19vt85h", unknown, fout)
dr(t19h, t37v, "drt19ht37v", unknown, fout)
dr(t19h, t37h, "drt19ht37h", unknown, fout)
dr(t19h, t85v, "drt19ht85v", unknown, fout)
dr(t19h, t85h, "drt19ht85h", unknown, fout)
dr(t37v, t37h, "drt37vt37h", unknown, fout)
dr(t37v, t85v, "drt37vt85v", unknown, fout)
dr(t37v, t85h, "drt37vt85h", unknown, fout)
dr(t37h, t85v, "drt37ht85v", unknown, fout)
dr(t37h, t85h, "drt37ht85h", unknown, fout)
dr(t85v, t85h, "drt85vt85h", unknown, fout)
# 22v not usable from f15
#dr(t19v, t22v, "drt19vt22v", unknown, fout)
#dr(t19h, t22v, "drt19ht22v", unknown, fout)
#dr(t22v, t37v, "drt22vt37v", unknown, fout)
#dr(t22v, t37h, "drt22vt37h", unknown, fout)
#dr(t22v, t85v, "drt22vt85v", unknown, fout)
#dr(t22v, t85h, "drt22vt85h", unknown, fout)

fout.close()
#exit(0)

#----------------------------------------------------------------

#  Next step, pre-filter based on perfect land filters, perfect ice-free ocean filters
#  Exclude 22v because of F15
#
# Land points:
#satellite-based land mask
#Start with false everywhere and then 'or' in the trues:
sland_mask  = ma.masked_array(t19h > 3000)
swater_mask = ma.masked_array(t19h > 3000)
sland_mask  = ma.logical_or(sland_mask, known)
swater_mask = ma.logical_or(swater_mask, known)

# perfect not-ice, very good water:
tmp = ma.masked_array(delta(t37v, t85v) < -0.08223462165004075)
swater_mask = ma.logical_or(swater_mask, tmp)
tmp = ma.masked_array(delta(t37h, t85h) < -0.18216759628719753)
swater_mask = ma.logical_or(swater_mask, tmp)
tmp = ma.masked_array(delta(t37v, t85h) < -0.05941281460150324)
swater_mask = ma.logical_or(swater_mask, tmp)

# perfect not-ice, mixed land/water
swater_mask = ma.logical_or(swater_mask, ma.masked_array(t85v > 264))
swater_mask = ma.logical_or(swater_mask, ma.masked_array(t85h > 264))

# perfect not-ice, very good land:
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t37v > 265))
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t37h > 257))
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t19v > 268))
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t19h > 259))
tmp = ma.masked_array(delta(t19v, t19h) < 0.011795428064134385)
sland_mask = ma.logical_or(sland_mask, tmp)
tmp = ma.masked_array(delta(t37v, t37h) < 0.007027243122910009)
sland_mask = ma.logical_or(sland_mask, tmp)
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t37v < 184))

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
fout2 = open("tb2","w")

tmp = np.zeros((7,nobs))

for i in range(0,nobs):
    all[unknown_indices[0][i] ].show(fout2)
    for j in range(0,7):
      tmp[j,i] = all[unknown_indices[0][i] ].tb[j]

for j in range (0,6):
  for k in range (j+1,7):
    print(j,k, np.corrcoef(tmp[j], tmp[k]) )

fout2.close()
#-----------------------------------------------

fout = open("round2","w")
for thot in range (75, 315):
  bayes(t19v, thot, "t19v", unknown, fout)
  bayes(t19h, thot, "t19h", unknown, fout)
#  bayes(t22v, thot, "t22v", unknown, fout)
  bayes(t37v, thot, "t37v", unknown, fout)
  bayes(t37h, thot, "t37h", unknown, fout)
  bayes(t85v, thot, "t85v", unknown, fout)
  bayes(t85h, thot, "t85h", unknown, fout)

dr(t19v, t19h, "drt19vt19h", unknown, fout)
dr(t19v, t37v, "drt19vt37v", unknown, fout)
dr(t19v, t37h, "drt19vt37h", unknown, fout)
dr(t19v, t85v, "drt19vt85v", unknown, fout)
dr(t19v, t85h, "drt19vt85h", unknown, fout)
dr(t19h, t37v, "drt19ht37v", unknown, fout)
dr(t19h, t37h, "drt19ht37h", unknown, fout)
dr(t19h, t85v, "drt19ht85v", unknown, fout)
dr(t19h, t85h, "drt19ht85h", unknown, fout)
dr(t37v, t37h, "drt37vt37h", unknown, fout)
dr(t37v, t85v, "drt37vt85v", unknown, fout)
dr(t37v, t85h, "drt37vt85h", unknown, fout)
dr(t37h, t85v, "drt37ht85v", unknown, fout)
dr(t37h, t85h, "drt37ht85h", unknown, fout)
dr(t85v, t85h, "drt85vt85h", unknown, fout)
# 22v not usable from F15, but is otherwise
#dr(t19v, t22v, "drt19vt22v", unknown, fout)
#dr(t19h, t22v, "drt19ht22v", unknown, fout)
#dr(t22v, t37v, "drt22vt37v", unknown, fout)
#dr(t22v, t37h, "drt22vt37h", unknown, fout)
#dr(t22v, t85v, "drt22vt85v", unknown, fout)
#dr(t22v, t85h, "drt22vt85h", unknown, fout)

fout.close() 
