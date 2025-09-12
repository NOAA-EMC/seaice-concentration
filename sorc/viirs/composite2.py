import sys
from math import *
import datetime
import numpy as np
import netCDF4 as nc

from grid import *

'''
Collect multiple composite1 files -- t, a, sigma t, sigma a, count, and 
  * produce a pooled value for all
  * apply filter to the result -- filter is designed for/with 24 hours of data. Performance on just 6 is unknown.
  * write out 5' .nc grids of t, a, sigmat, sigmaa, count
'''
#-------------------------------------------------------------

def getin(fin, conc, temp, suma2, sumt2):
  nx = len(conc[:,0])
  ny = len(conc[0,:])
  sigmaconc = np.zeros((nx, ny))
  sigmatemp = np.zeros((nx, ny))

  obscount = 0
  for line in fin:
    words = line.split()
    #i,j,lat,lon,amean,asigma,tmean,tsigma,count
    i   = int(words[0])
    j   = int(words[1])
    lat = float(words[2])
    lon = float(words[3])
    conc[i,j]      = float(words[4])
    sigmaconc[i,j] = float(words[5])
    temp[i,j]      = float(words[6])
    sigmatemp[i,j] = float(words[7])
    count[i,j]     = int(words[8])
    suma2[i,j] = conc[i,j]**2 + sigmaconc[i,j]**2
    sumt2[i,j] = temp[i,j]**2 + sigmatemp[i,j]**2

    obscount += 1

  suma2 *= count
  sumt2 *= count
  conc  *= count
  temp  *= count
  #print('sum2 in fn',suma2.max(), sumt2.max(), flush=True )
  del sigmaconc, sigmatemp
  return obscount


target_grid = global_5min()
nx = target_grid.nx
ny = target_grid.ny
nmax = nx*ny

sumtemp = np.zeros((nx, ny))
sumconc = np.zeros((nx, ny))
sumsumt2 = np.zeros((nx, ny))
sumsuma2 = np.zeros((nx, ny))
sumcount = np.zeros((nx, ny))

for fnum in range(1,len(sys.argv)):
  fin = open(sys.argv[fnum],"r")
  #debug: print(sys.argv[fnum], flush=True)
  temp = np.zeros((nx, ny))
  conc = np.zeros((nx, ny))
  count = np.zeros((nx, ny))
  sumt2 = np.zeros((nx, ny))
  suma2 = np.zeros((nx, ny))
  getin(fin, conc, temp, suma2, sumt2)

  sumtemp  += temp
  sumsumt2 += sumt2
  sumconc  += conc
  sumsuma2 += suma2
  sumcount += count
  #print('file inside sum2',sumsuma2.max(), sumsumt2.max(), suma2.max(), sumt2.max() )
  #print("file inside conc",conc.max(), temp.max(), count.max() )

  del conc, temp, suma2, sumt2, count

#print('outside sum2',sumsuma2.max(), sumsumt2.max() )

indices = sumcount.nonzero()
for k in range(0, len(indices[0])):
  i = indices[0][k]
  j = indices[1][k]
  sumtemp[i,j] /= sumcount[i,j]
  sumconc[i,j] /= sumcount[i,j]
  sumsuma2[i,j] /= sumcount[i,j]
  sumsumt2[i,j] /= sumcount[i,j]

#debug: print("temp",sumtemp.max(), "conc", sumconc.max(), "count",sumcount.max() )
#print('sum2',sumsuma2.max(), sumsumt2.max() )

sigmatemp = np.zeros((nx, ny))
sigmaconc = np.zeros((nx, ny))
dummy = np.zeros((nx, ny))

sigmaconc = sumsuma2 - sumconc*sumconc
sigmatemp = sumsumt2 - sumtemp*sumtemp

sigmaconc = np.fmax(dummy, sigmaconc)
sigmatemp = np.fmax(dummy, sigmatemp)

sigmaconc = np.sqrt(sigmaconc)
sigmatemp = np.sqrt(sigmatemp)
#print('sigmas',sigmaconc.max(), sigmatemp.max() )
#debug: exit(0)

#-------------------------------------------------------------
#filter
mask = np.zeros((nx, ny))
indices = sumcount.nonzero()
z = latpt()
for k in range(0, len(indices[0])):
  i = indices[0][k]
  j = indices[1][k]
  target_grid.locate(i,j,z)

  if (sumtemp[i,j] <= 273.865):
    if (sumconc[i,j] > 86.985):
      if (sumcount[i,j]/cos(pi/180*z.lat)   > 105.570):
        mask[i,j] = 1
    else:
      if (sumtemp[i,j] <= 268.525):
        mask[i,j] = 1


#write out (to netcdf ultimately)
#sumcount *= mask
#sumconc  *= mask
#sumtemp  *= mask
#sigmaconc *= mask
#sigmatemp *= mask

indices = mask.nonzero()
for k in range(0, len(indices[0])):
  i = indices[0][k]
  j = indices[1][k]
  target_grid.locate(i,j,z)
  print(i,j,z.lat,z.lon,sumconc[i,j], sigmaconc[i,j], sumtemp[i,j], sigmatemp[i,j], sumcount[i,j]) 

#debug: 
exit(0)


#-------------------------------------------------------------
# Note that we're only concerned with points that satisfy this leaf's conditions
for i in range(0,count):
  #if (ary[i,4] <= 265.761 and ary[i,4] > 225.656 and ary[i,3] > 42.849): 
  #if (ary[i,4] <= 265.761 and ary[i,4] > 218.670 and ary[i,5] < 22.832): 
  #  pcount += 1
  #  if (y[i] > 0):
  #    picount += 1
  #    count11 += 1
  #  else:
  #    count10 += 1
  #else:
  #  if (y[i] > 0):
  #    count01 += 1
  

  if (ary[i,4] <= 273.865):
    if (ary[i,0] > 86.985):
      if (ary[i,3] > 105.570):
        pcount += 1
        if (y[i] > 0):
          picount += 1
          count11 += 1
        else:
          count10 += 1
    else:
      if (ary[i,4] <= 268.525):
        pcount += 1
        if (y[i] > 0):
          picount += 1
          count11 += 1
        else:
          count10 += 1


