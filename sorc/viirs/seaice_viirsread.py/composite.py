#!/usr/bin/env python3
'''
Loop over input arg list (JRR-IceConcentration*)
 and composite the concentration and temperatures on to global_12th grids
 write out in netcdf
fname are in this convention: 
  "20220828/JRR-IceConcentration_v2r3_j01_s202208281036198_e202208281037426_c202208281059540.nc"
Robert.Grumbine
25 November 2025
'''

import sys
import datetime
from math import sqrt

import numpy as np
import numpy.ma as ma
import netCDF4 as nc

from latpt import *
from grid import *

#---------------------------------------------------------------------------
#For output grid:
target_grid = global_5min()
tsumx  = np.zeros((target_grid.ny,target_grid.nx))
tsumx2 = np.zeros((target_grid.ny,target_grid.nx))
csumx  = np.zeros((target_grid.ny,target_grid.nx))
csumx2 = np.zeros((target_grid.ny,target_grid.nx))
gcount = np.zeros((target_grid.ny,target_grid.nx),dtype=int)
#debug: print("target grid dimensions",target_grid.ny,target_grid.nx, file=sys.stderr)

nfiles = 0
totnp = 0

for fname in sys.argv[1:]:
  #debug: print(n, fname,flush=True)

  try:
    viirs = nc.Dataset(fname, 'r')
  except:
    print("Could not open fname: ",fname,flush=True, file=sys.stderr)
    continue

  #debug: print("dimensions ",len(viirs.dimensions['Columns']), len(viirs.dimensions['Rows']) )
  nfiles += 1

  #This is a masked array, determined by fill value
  conc = viirs.variables['IceConc'][:,:]
  #debug: print(nfiles,"conc ",conc.max(), conc.min(),flush=True, file=sys.stderr )
  indices = conc.nonzero()

  npts = len(indices[0])
  if (npts == 0):
      continue
  totnp += len(indices[0])

  #Geography:
  lats = viirs.variables['Latitude'][:,:]
  lons = viirs.variables['Longitude'][:,:]

  #QC:
  temp = viirs.variables['IceSrfTemp'][:,:]
  #debug: print(nfiles,"temp ",temp.max(), temp.min(),flush=True, file=sys.stderr )

  #Start Working:
  for k in range(0,len(indices[0])):
      i = indices[1][k]
      j = indices[0][k]
      #verbose: print(lons[j,i], lats[j,i], conc[j,i], " pt")
      # for gridding
      iloc = target_grid.inv_locate(lats[j,i],lons[j,i])
      ti = int(iloc[0]+0.5)
      if (ti == target_grid.nx):
        ti = 0
      tj = int(iloc[1]+0.5)
      gcount[tj,ti] += 1
      c =  conc[j,i]
      csumx[tj,ti]  += c
      csumx2[tj,ti] += c*c
      t =  temp[j,i]
      tsumx[tj,ti]  += t
      tsumx2[tj,ti] += t*t
      #debug print(j, i, tj, ti, lons[j,i], lats[j,i], conc[j,i], t, " pt", flush=True)

  #debug: if (nfiles >= 100): break

print("total number of ice conc observations: ",totnp, file=sys.stderr)

z = latpt()
cellcount = 0
mask = ma.masked_array(gcount > 0)
indices = mask.nonzero()
#set grids to weather
#create lat-lon grids for nc output
for k in range(0,len(indices[0])):
    i = indices[1][k]
    j = indices[0][k]
    csumx[j,i] /= gcount[j,i]
    csumx2[j,i] = sqrt(max(0., csumx2[j,i]/gcount[j,i] - csumx[j,i]*csumx[j,i]) )
    tsumx[j,i] /= gcount[j,i]
    tsumx2[j,i] = sqrt(max(0., tsumx2[j,i]/gcount[j,i] - tsumx[j,i]*tsumx[j,i]) )
    target_grid.locate(i,j,z)

    print(i,j,z.lat, z.lon, csumx[j,i], csumx2[j,i], tsumx[j,i], \
            tsumx2[j,i], gcount[j,i], flush=True, file=sys.stdout)
    cellcount += 1

print("gcount, avg: ",gcount.max(), gcount.min(), tsumx.max(), tsumx.min(), \
        tsumx2.max(), tsumx2.min(),file=sys.stderr  )
print("cellcount = ",cellcount,file=sys.stderr)

sys.exit(0)

#----------------------------------------------------------------------------------
# Here and below is pre-adaptation for netcdf output to be used downstream
# It is fully functional, but not polished
#----------------------------------------------------------------------------------

class ncout:
    """
    class ncout is for managing the writing of netcdf output
    """

    def __init__(self, nx, ny):
      self.nx = nx
      self.ny = ny
      self.count = 0  # count of variables
      self.var   = [] # variables
      self.lons = np.zeros((self.ny,self.nx))
      self.lats = np.zeros((self.ny,self.nx))
      dlon = 360./nx
      flon = dlon/2
      dlat = 180./ny
      flat = 90.-dlat/2
      tlons = np.linspace(flon, 360-flon, nx)
      #RG: Verify that this is correct direction and values
      tlats = np.linspace(flat, -90+dlat/2, ny)
      #debug: print(tlats)
      #debug: print(tlons)
      for ii in range(0,nx):
        self.lats[:,ii] = tlats[:]
      for ii in range(0,ny):
        self.lons[ii,:] = tlons[:]
      del tlons, tlats

    def ncopen(self, fout):
      """
      ncout.ncopen(fout) opens fout for netcdf writing
      """
      self.ncfile = nc.Dataset(fout, mode='w', format='NETCDF4')
      self.lat_dim = self.ncfile.createDimension('lat', self.ny)
      self.lon_dim = self.ncfile.createDimension('lon', self.nx)
      # Create variables to hold values for those referenced dimensions
      self.lat = self.ncfile.createVariable('lat', np.float32, ('lat','lon',))
      self.lat.units = 'degrees_north'
      self.lat.long_name = 'latitude'
      self.lat[:,:] = self.lats[:,:]

      self.lon = self.ncfile.createVariable('lon', np.float32, ('lat','lon',))
      self.lon.units = 'degrees_east'
      self.lon.long_name = 'longitude'
      self.lon[:,:] = self.lons[:,:]

      self.header(fout)


    def header(self, fout):
      """ ncout.header(fout) -- write netcdf header informtion """
      #Generic global header info:
      self.ncfile.title = fout
      self.ncfile.setncattr("institution","NOAA/NWS/NCEP")
      self.ncfile.setncattr("geospatial_lon_max","{:f}".format(self.lons.max() )  )
      self.ncfile.setncattr("geospatial_lon_min","{:f}".format(self.lons.min() )  )
      self.ncfile.setncattr("geospatial_lat_max","{:f}".format(self.lats.max() )  )
      self.ncfile.setncattr("geospatial_lat_min","{:f}".format(self.lats.min() )  )
      tmp = datetime.date.today()
      self.ncfile.setncattr("date_created",tmp.strftime("%Y-%m-%d") )

      #More specialized:
      self.ncfile.setncattr("contributor_name","Robert Grumbine")
      self.ncfile.setncattr("contributor_email","Robert.Grumbine@noaa.gov")
      self.ncfile.setncattr("creator_name","Robert Grumbine")
      self.ncfile.setncattr("creator_email","Robert.Grumbine@noaa.gov")
      #More specialized (kinds of things one might want):
      #self.ncfile.setncattr("source","NCEP GFS")
      #self.ncfile.setncattr("Conventions","conventions")
      #self.ncfile.setncattr("standard_name_vocabulary","names")
      #self.ncfile.setncattr("summary"," contribution to yopp sitemip")
      #self.ncfile.setncattr("keywords","YOPP, Polar, Supersite")

    def addvar(self, vname, dtype):
      """ ncout.addvar(varname, data_type) -- name and type a variable 
          for netcdf output """
      #debug print('dtype = ',dtype, flush=True)
      if (dtype == 'uint8'):
        fill = 255
      else:
        fill = -1

      try:
        tmp = self.ncfile.createVariable(vname, dtype, ( 'lat','lon'), fill_value=fill)
      except:
        return
      self.var.append(tmp)
      self.var[self.count].long_name = vname
      self.count += 1

    def encodevar(self, allvalues, vname):
      ''' ncout.encodevar(allvalues, vname) '''
      if (self.nx*self.ny != 0) :
        self.ncfile.variables[vname][:,:] = allvalues[:,:]

    def close(self):
      """ ncout.close() -- close netcdf file """
      self.ncfile.close()

#----------------------------------------------------------------------------------
xout = ncout(target_grid.nx, target_grid.ny)
xout.ncopen("hello.nc")

# will want to go to 4 byte float and int, default is 8
xout.addvar('csumx',csumx.dtype )
xout.addvar('csumx2',csumx2.dtype )
xout.addvar('tsumx',tsumx.dtype )
xout.addvar('tsumx2',tsumx2.dtype )
xout.addvar('count',gcount.dtype )

xout.encodevar(csumx,'csumx')
xout.encodevar(csumx2,'csumx2')
xout.encodevar(tsumx,'tsumx')
xout.encodevar(tsumx2,'tsumx2')
xout.encodevar(gcount,'count')

xout.close()
