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

print("done adding in sst ",flush=True)
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

# Define utilities for doing the assessment:

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

#----------------------------------------------------------------
#  Commented out because this has already (prior runs) been used to 
#    find the perfect land and water filters
#fout = open("round1","w")
#for thot in range (95, 320):
#  bayes(t19v, thot, "t19v", unknown, fout)
#  bayes(t19h, thot, "t19h", unknown, fout)
#  bayes(t22v, thot, "t22v", unknown, fout)
#  bayes(t37v, thot, "t37v", unknown, fout)
#  bayes(t37h, thot, "t37h", unknown, fout)
#  bayes(t85v, thot, "t85v", unknown, fout)
#  bayes(t85h, thot, "t85h", unknown, fout)
#
#dr(t19v, t19h, "drt19vt19h", unknown, fout)
#dr(t19v, t22v, "drt19vt22v", unknown, fout)
#dr(t19v, t37v, "drt19vt37v", unknown, fout)
#dr(t19v, t37h, "drt19vt37h", unknown, fout)
#dr(t19v, t85v, "drt19vt85v", unknown, fout)
#dr(t19v, t85h, "drt19vt85h", unknown, fout)
#dr(t19h, t22v, "drt19ht22v", unknown, fout)
#dr(t19h, t37v, "drt19ht37v", unknown, fout)
#dr(t19h, t37h, "drt19ht37h", unknown, fout)
#dr(t19h, t85v, "drt19ht85v", unknown, fout)
#dr(t19h, t85h, "drt19ht85h", unknown, fout)
#dr(t22v, t37v, "drt22vt37v", unknown, fout)
#dr(t22v, t37h, "drt22vt37h", unknown, fout)
#dr(t22v, t85v, "drt22vt85v", unknown, fout)
#dr(t22v, t85h, "drt22vt85h", unknown, fout)
#dr(t37v, t37h, "drt37vt37h", unknown, fout)
#dr(t37v, t85v, "drt37vt85v", unknown, fout)
#dr(t37v, t85h, "drt37vt85h", unknown, fout)
#dr(t37h, t85v, "drt37ht85v", unknown, fout)
#dr(t37h, t85h, "drt37ht85h", unknown, fout)
#dr(t85v, t85h, "drt85vt85h", unknown, fout)
#print(flush=True, file=fout)
#fout.close()

#-----------------------------------------------
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

# Allow some ice, but <= 1%
tmp = ma.masked_array(delta(t19v, t19h) <  0.018077900915434868 )
sland_mask = ma.logical_or(sland_mask, tmp)
tmp = ma.masked_array(delta(t37v, t37h) <  0.012212784753905402 )
sland_mask = ma.logical_or(sland_mask, tmp)
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t85h < 164 ) )

# <= 3%
sland_mask = ma.logical_or(sland_mask, ma.masked_array(t85h < 171 ) )
tmp = ma.masked_array(delta(t19v, t37v) > 0.04663001497586572 )
sland_mask = ma.logical_or(sland_mask, tmp)


#satellite-based land mask
sland_indices = sland_mask.nonzero()
nsland_pts = len(sland_indices[0])
print("number of satellite-caught land pts: ",nsland_pts)

# Water points: --------------------------------------------------------
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
for thot in range (100, 285):
  bayes(t19v, thot, "t19v", unknown, fout)
  bayes(t19h, thot, "t19h", unknown, fout)
  bayes(t22v, thot, "t22v", unknown, fout)
  bayes(t37v, thot, "t37v", unknown, fout)
  bayes(t37h, thot, "t37h", unknown, fout)
  bayes(t85v, thot, "t85v", unknown, fout)
  bayes(t85h, thot, "t85h", unknown, fout)

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

#----------------------------------------------------------------
exit(0)

fout = open("round3","w")
for thot in range (100, 320):
  bayes(t19v, thot, "t19v", unknown, fout)
  bayes(t19h, thot, "t19h", unknown, fout)
  bayes(t22v, thot, "t22v", unknown, fout)
  bayes(t37v, thot, "t37v", unknown, fout)
  bayes(t37h, thot, "t37h", unknown, fout)
  bayes(t85v, thot, "t85v", unknown, fout)
  bayes(t85h, thot, "t85h", unknown, fout)

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
print(flush=True,file=fout)

#-----------------------------------------------
fout2 = open("trimmed_tb3","w")
for i in range(0,nobs):
    all[unknown_indices[0][i] ].show(fout2)
fout2.close()
#-----------------------------------------------
