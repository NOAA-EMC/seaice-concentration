import os
import sys

import numpy as np
import netCDF4 as nc

obs = nc.Dataset(sys.argv[1])
lat = obs.variables['lat'][:,:]
lon = obs.variables['lon'][:,:]
mean = obs.variables['csumx'][:,:]
sdev = obs.variables['csumx2'][:,:]
tmean = obs.variables['tsumx'][:,:]
tsdev = obs.variables['tsumx2'][:,:]
count = obs.variables['count'][:,:]
#scount = count/cos(lat)

try:
  title_tag = sys.argv[2]
except:
  title_tag = "ref"

try:
  markersize = float(sys.argv[3])
except:
  markersize = 1

print("mean",mean.max(), mean.min() )
print("sdev",sdev.max(), sdev.min() )
print("tmean",tmean.max(), tmean.min() )
print("tsdev",tsdev.max(), tsdev.min() )
print("count",count.max(), count.min() )
tlats = lat[count > 0]
tlons = lon[count > 0]
tconc = mean[count > 0]
print("tlats",tlats.max(), tlats.min(), len(tlats) )
print("tlons",tlons.max(), tlons.min() )
print("tconc",tconc.max(), tconc.min() )

#debug: exit(0)

# i-j plot of points ----------------------------------
import matplotlib
import matplotlib.pyplot as plt
matplotlib.use('Agg') #batch mode

# lat-lon plot of error points ---------------------------------
import cartopy.crs as ccrs
import cartopy.feature as cfeature

#proj = ccrs.LambertConformal(central_longitude=-170., central_latitude = 60., cutoff=25.)
proj = ccrs.PlateCarree()
proj = ccrs.SouthPolarStereo(central_longitude=-60)
proj = ccrs.NorthPolarStereo(central_longitude=-80)
proj = ccrs.NorthPolarStereo(central_longitude=-170)

ax = plt.axes(projection = proj)
fig = plt.figure(figsize = (8,6))
ax = fig.add_subplot(1,1,1,projection = proj)
plt.title(title_tag)

# AA
#xlocs = list(range(-180,181,30))
#ylocs = list(range(-90, -30, 5))
#ax.set_extent([-180,180,-90,-30], crs=ccrs.PlateCarree())
# Arctic
xlocs = list(range(-180,181,30))
ylocs = list(range(30,90,5))
ax.set_extent([-180,180,35,90], crs=ccrs.PlateCarree())
#Bering sea-ish
#ax.set_extent([-180,-90,35,90], crs=ccrs.PlateCarree())

ax.gridlines(crs=ccrs.PlateCarree(), xlocs=xlocs, ylocs=ylocs )
# not on hera: ax.coastlines()
ax.add_feature(cfeature.GSHHSFeature(levels=[1,2], scale="c") )


#plt.scatter(tlons, tlats, transform=ccrs.PlateCarree(), s = markersize, alpha = alpha)

tlats = lat[mean > 0 ]
tlons = lon[mean > 0 ]
print(len(tlats),"points > 0 concentration")
plt.scatter(tlons, tlats, transform=ccrs.PlateCarree(), s = markersize, color='blue')

tlats = lat[mean >= 80]
tlons = lon[mean >= 80]
print(len(tlats),"points >= 80 concentration")
plt.scatter(tlons, tlats, transform=ccrs.PlateCarree(), s = markersize, color = 'red')

#cs = ax.pcolormesh(lon, lat, mean, cmap=colors, transform=ccrs.PlateCarree() )
#cs = ax.pcolormesh(lon, lat, mean, transform=ccrs.PlateCarree() )
#cb = plt.colorbar(cs, extend='both', orientation='horizontal', shrink=0.5, pad=.04)
#cbarlabel = '%s' % ("hello1")
#cb.set_label(cbarlabel, fontsize=12)

plt.savefig("ll_"+title_tag+".png")
plt.close()
