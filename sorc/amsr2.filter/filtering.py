import sys
import numpy as np
import numpy.ma as ma

#generalizing 2021 filtering process
#----------------------------------------------
# llgrid tools

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
  dlon =  1./12.
  firstlat = 90. - dlat/2.
  firstlon = dlon/2.
  if (lon < 0):
    lon += 360.
  j = round( (lat - firstlat)/dlat )
  i = round( (lon - firstlon)/dlon )
  return (j,i)

#----------------------------------------------
# satellite 
# delta ratio
def delta(x,y):
  return (x-y)/(x+y)

#----------------------------------------------
# satellite to ice/ocean matchup class
#matchup :
#longitude, latitude, quality, land, icec;
#    ice_land, ice_post, ice_distance; sst, ice_sst
class match:

  def __init__(self, satid = 0, latitude = 95., longitude = 95., icec = 95., land = 95, quality = 95, ice_land = 95, ice_post = 95, ice_distance = 95., sst = 95., ice_sst = 95.):
    self.ntb       = 7
    self.satid     = satid
    self.latitude  = latitude
    self.longitude = longitude
    self.icec = icec
    self.land = land
    self.quality  = quality
    self.ice_land = 95
    self.ice_post = 95
    self.ice_distance = 95.
    self.sst     = 95.
    self.ice_sst = 95.
    self.tb      = np.zeros((self.ntb))
    #debug print("done with init", flush=True)

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
    for i in range (0,self.ntb):
      self.tb[i] = tb[i]

  def add_oiv2(self, sst, ice_sst):
    j,i          = oiv2(self.latitude, self.longitude)
    self.sst     = sst[j,i]
    self.ice_sst = ice_sst[j,i]

  def add_icefix(self, ice_land, ice_post, ice_distance):
    j,i = rg12th(self.latitude, self.longitude)
    self.ice_land     = ice_land[j,i]
    self.ice_post     = ice_post[j,i]
    self.ice_distance = ice_distance[j,i]

  def __getitem__(self, i):
    return(tb[i])

#---------------------------------------------------------------
# Define utilities for doing the assessment:
def mask_stats(nobs, landmask, icemask, watermask, fout = sys.stdout):
  # Get the indices of the 'true' points
  iceindices = icemask.nonzero()
  mland      = landmask.nonzero()
  water      = watermask.nonzero()
  nicepts   = len(iceindices[0])
  nlandpts  = len(mland[0])
  nwaterpts = len(water[0])
  #debug print("n ice, land, water, nobs ",nicepts, nlandpts, nwaterpts, nobs)

  pice   = nicepts/float(nobs)
  pland  = nlandpts/float(nobs)
  pwater = nwaterpts/float(nobs)
  #debug print("p ice, land, water, nobs ",pice, pland, pwater, nobs, flush=True)
  del iceindices, mland, water
  return (nicepts, nlandpts, nwaterpts, pice, pland, pwater)


def bayes(xvec, xcrit, label, unknown, nobs, landmask, icemask, watermask, fout = sys.stdout ):
  stats = mask_stats(nobs, landmask, icemask, watermask, fout = sys.stdout )
  nicepts   = stats[0]
  nlandpts  = stats[1]
  nwaterpts = stats[2]
  pice      = stats[3]
  pland     = stats[4]
  pwater    = stats[5]
  #debug print("stats = ",stats, flush=True)
  bayes_stats = np.zeros((3))

  filters = []

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
  if (pwarm > 0):     #ignore if the filter is never tripped
    bayes_stats[0] = pover_ice * pice / pwarm
    bayes_stats[1] = pover_land * pland / pwarm
    bayes_stats[2] = pover_water * pwater / pwarm
    #print out perfect filters -- debugging
    #debug if ( (np.any(bayes_stats == 1.0) or np.any(bayes_stats == 0.0)) ):
    #debug   print(label, "hot ", xcrit,
    #debug     "{:6.4f}".format(bayes_stats[0]) ,
    #debug     "{:6.4f}".format(bayes_stats[1]) ,
    #debug     "{:6.4f}".format(bayes_stats[2]), nwarm, file = fout, flush=True ) 

    #place to collect the filters
    x = filter(label, "hot", xcrit, bayes_stats, nwarm)
    filters.append(x)

  cold = ma.masked_array(xvec < xcrit)
  cold = ma.logical_and(cold, unknown)
  ncold = len(cold.nonzero()[0])
  pcold = float(ncold)/float(nobs)

  lmask = np.logical_and(landmask, cold)
  imask = np.logical_and(icemask, cold)
  omask = np.logical_and(watermask, cold)
  pover_land  = len(lmask.nonzero()[0])/nlandpts
  pover_water = len(omask.nonzero()[0])/nwaterpts
  pover_ice   = len(imask.nonzero()[0])/nicepts
  
  if (pcold > 0):
    bayes_stats[0] = pover_ice * pice / pcold
    bayes_stats[1] = pover_land * pland / pcold
    bayes_stats[2] = pover_water * pwater / pcold
    #print out perfect filters -- debugging
    #debug if ( (np.any(bayes_stats == 1.0) or np.any(bayes_stats == 0.0)) ):
    #debug   print(label,"cold ",xcrit,
    #debug     "{:6.4f}".format(bayes_stats[0]) ,
    #debug     "{:6.4f}".format(bayes_stats[1]) ,
    #debug     "{:6.4f}".format(bayes_stats[2]), ncold, file = fout, flush=True )

    #place to collect the filters
    x = filter(label, "cold", xcrit, bayes_stats, ncold)
    filters.append(x)

  del bayes_stats
  return filters


def dr(x, y, label, unknown, nobs, landmask, icemask, watermask, fout = sys.stdout):
  filters = []

  ratio = delta(x,y)
  tc = np.linspace(ratio.min(), ratio.max(), num=100)
  for i in range(0,len(tc)):
    tmp = bayes(ratio, tc[i], label, unknown, nobs, landmask, icemask, watermask, fout)
    if (len(tmp) > 0):
      for i in range(0,len(tmp)):
        filters.append(tmp[i])
      del tmp
  del ratio

  #debug print(label," dr filters:",len(filters))
  #debug2 for i in range(0,len(filters)):
  #debug2   print(i," ",end="")
  #debug2   filters[i].show()
    
  return filters

#----------------------------------------------------------------

class filter:
# order in the bayes_stats
  ice = 0
  land = 1
  water = 2
#Tb order
  t19v = 0
  t19h = 1
  t22v = 2
  t37v = 3
  t37h = 4
  t85v = 5
  t85h = 6
# dictionary of names to indices:

  def __init__(self, name, type, value, bayes_stats, npts):
    self.name = name
    self.type = type
    self.value = value
    self.npts  = npts
    self.stats = np.zeros((3))
    self.stats[0] = bayes_stats[0]
    self.stats[1] = bayes_stats[1]
    self.stats[2] = bayes_stats[2]

  def show(self, fout = sys.stdout):
    print(self.name, self.type, self.value, 
       ('{:6.4f} '*3).format(self.stats[0], self.stats[1], self.stats[2]),
       self.npts, file=fout)

  def perfect(self):
    return (np.any(self.stats == 1.0) or np.any(self.stats == 0.0)) 

  def add(filters, tmp):
    #debug print("adding ",len(tmp)," filters", flush=True)
    for i in range (0,len(tmp)):
      filters.append(tmp[i])

  def better(self, other):
# return true if self is a better filter than the other for filter type 'index'
# perfect_is --> 1.0
# perfect_isnot --> 0.0
# for each ice, land, water
# better : self.npts > other.npts
    if (self.perfect() and other.perfect() ):
      return(self.npts > other.npts)
    else:
      return False

  def apply(self, tb, index, is_or_isnt):
#   is = logical, do you want that this _is_ true/filtered (arg=True), 
#         or that it is _not_ true 
    if (self.name[0:1] == "dr"):
      iname = self.name[2:5]
      jname = self.name[6:9]
      ipol  = self.name[5]
      jpol  = self.name[9]
      print("apply ",iname, jname, ipol, jpol)
#      if (self.type == "cold"): #use less than
#      elif (self.type == "hot"): #use > 
#      else:
#        print("no allowed comparison type:",self.type)
#   else:
#     simple temperature test
    return True
