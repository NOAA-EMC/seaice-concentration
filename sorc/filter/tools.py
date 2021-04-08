import os
import sys
import numpy as np
import numpy.ma as ma

#----------------------------------------------
def oiv2(lat, lon):
  dlat = 0.25
  dlon = 0.25
  firstlat = -89.875
  firstlon = 0.125
  if (lon < 0):
    lon += 360.
  j = int(round( (lat - firstlat)/dlat ))
  i = int(round( (lon - firstlon)/dlon ))
  return (j,i)

def rg12th(lat, lon):
  dlat = -1./12.
  dlon = 1./12.
  firstlat = 90. - dlat/2.
  firstlon = dlon/2.
  if (lon < 0):
    lon += 360.
  j = int(round( (lat - firstlat)/dlat ))
  i = int(round( (lon - firstlon)/dlon ))
  return (j,i)

def delta(x,y):
  return (x-y)/(x+y)

#----------------------------------------------
#matchup :
#longitude, latitude, quality, land, icec; 
#    ice_land, ice_post, ice_distance; sst, ice_sst
class match:

  def __init__(self, satid=248, latitude = 95., longitude = 95., icec = 95., land = 95, quality = 95, ice_land = 95, ice_post = 95, ice_distance = 95., sst = 95., ice_sst = 95.):
    self.satid = satid
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
    print(self.satid, "{:9.4f}".format(self.longitude), "{:8.4f}".format(self.latitude), 
               "{:.2f}".format(self.icec), "{:.2f}".format(self.land), self.quality, 
          "{:3d}".format(self.ice_land), "{:3d}".format(self.ice_post), 
          "{:7.2f}".format(self.ice_distance), 
          "{:5.2f}".format(self.sst), "{:.2f}".format(self.ice_sst), 
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

# Define utilities for doing the assessment:

#def bayes(xvec, xcrit, label, unknown, fout = sys.stdout ):
#  warm = ma.masked_array(xvec > xcrit)
#  warm = ma.logical_and(warm, unknown)
#  nwarm = len(warm.nonzero()[0]) 
#  lmask = np.logical_and(landmask, warm)
#  imask = np.logical_and(icemask, warm)
#  omask = np.logical_and(watermask, warm)
#  pwarm = float(nwarm)/float(nobs)
#  pover_land  = len(lmask.nonzero()[0])/nlandpts
#  pover_water = len(omask.nonzero()[0])/nwaterpts
#  pover_ice   = len(imask.nonzero()[0])/nicepts
#  if (pwarm > 0):
#    print(label, "hot ", xcrit,
#      "{:5.3f}".format(pover_ice * pice / pwarm) ,
#      "{:5.3f}".format(pover_land * pland / pwarm) ,
#      "{:5.3f}".format(pover_water * pwater / pwarm), nwarm, file = fout )
#
#  cold = ma.masked_array(xvec < xcrit)
#  cold = ma.logical_and(cold, unknown)
#  ncold = len(cold.nonzero()[0]) 
#  lmask = np.logical_and(landmask, cold)
#  imask = np.logical_and(icemask, cold)
#  omask = np.logical_and(watermask, cold)
#  pcold = float(ncold)/float(nobs)
#  pover_land  = len(lmask.nonzero()[0])/nlandpts
#  pover_water = len(omask.nonzero()[0])/nwaterpts
#  pover_ice   = len(imask.nonzero()[0])/nicepts
#  if (pcold > 0):
#    print(label,"cold ",xcrit,
#      "{:5.3f}".format(pover_ice * pice / pcold) ,
#      "{:5.3f}".format(pover_land * pland / pcold) ,
#      "{:5.3f}".format(pover_water * pwater / pcold), ncold, file = fout )
#
#def dr(x, y, label, unknown, fout = sys.stdout):
#  ratio = delta(x,y)
#  tc = np.linspace(ratio.min(), ratio.max(), num=100)
#  for i in range(0,len(tc)):
#    bayes(ratio, tc[i], label, unknown, fout)
#  del ratio

