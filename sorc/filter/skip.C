#include "ncepgrids.h"
#include <netcdf.h>

/* Write out netcdf */
#define NDIMS 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); fflush(stdout); }

template <class T>
void toflat(llgrid<T> &x, T *y) ;

void out_nc(int* ncid, int* varid, global_12th<double> &lat, global_12th<double> &lon, 
              global_12th<unsigned char> &land, global_12th<unsigned char> &posteriori, 
              global_12th<float> &dist) ;

int close_nc(int *ncid) {
  return nc_close(*ncid);
}

int open_nc(int *ncid, int *varid) {
  char fname[24];
  int rec_dimid, dimids[NDIMS];

  int varid_lat, varid_lon, varid_post, varid_dist, varid_land;
  int retcode;

  sprintf(fname,"seaice_fixed_fields.nc");
  retcode = nc_create(fname, NC_CLOBBER, ncid);
  if (retcode != 0) ERR(retcode);

  retcode = nc_def_dim(*ncid, "nlats",180*12, &rec_dimid);
  if (retcode != 0) ERR(retcode);
  dimids[0] = rec_dimid;
  retcode = nc_def_dim(*ncid, "nlons",360*12, &rec_dimid);
  if (retcode != 0) ERR(retcode);
  dimids[1] = rec_dimid;

// Header information
   //nc_put_att_int(*ncid, NC_GLOBAL, "platform_id", NC_INT, 1, platform);

// Field information
  nc_def_var(*ncid, "longitude", NC_DOUBLE, NDIMS, dimids, &varid_lon);
  nc_def_var(*ncid, "latitude", NC_DOUBLE, NDIMS, dimids, &varid_lat);

  retcode = nc_def_var(*ncid, "land", NC_SHORT, NDIMS, dimids, &varid_land);
  if (retcode != 0) ERR(retcode);

  nc_def_var(*ncid, "posteriori", NC_SHORT, NDIMS, dimids, &varid_post);
  if (retcode != 0) ERR(retcode);

  nc_def_var(*ncid, "distance_to_land", NC_FLOAT, NDIMS, dimids, &varid_dist);

  nc_enddef(*ncid);
  varid[0] = varid_lon;
  varid[1] = varid_lat;
  varid[2] = varid_land;
  varid[3] = varid_post;
  varid[4] = varid_dist;

  return 0;
}

int main(int argc, char *argv[]) {
  FILE *fland, *fpost, *fdist;
  global_12th<unsigned char> land, posteriori, skip;
  global_12th<float> dist;
  global_12th<double> lat, lon;
  ijpt loc;
  latpt ll;
  int nlats, nlons, ncid, varid[5];

  fland = fopen("seaice_gland5min", "r");
  fpost = fopen("seaice_posteriori_5min", "r");
  fdist = fopen("seaice_alldist.bin", "r");

  land.binin(fland);
  posteriori.binin(fpost);
  dist.binin(fdist);

  fclose(fland);
  fclose(fpost);
  fclose(fdist);
  printf("maxes: %f %f %f\n",(float) land.gridmax(), (float) posteriori.gridmax(), (float) dist.gridmax() );

  for (loc.j = 0 ; loc.j < skip.ypoints(); loc.j++) {
  for (loc.i = 0 ; loc.i < skip.xpoints(); loc.i++) {
    ll = skip.locate(loc);
    lat[loc] = (double) ll.lat;
    lon[loc] = (double) ll.lon;
  }
  }

// RG: Should make these args to the open
  nlats = skip.ypoints();
  nlons = skip.xpoints();
  open_nc(&ncid, &varid[0]);

  printf("about to call out_nc\n"); fflush(stdout);
  out_nc(&ncid, &varid[0], lat, lon, land, posteriori, dist);
  printf("returned from out_nc\n"); fflush(stdout);

  close_nc(&ncid);


  return 0;
}
template <class T>
void toflat(llgrid<T> &x, T *y) {
  int i, npts = x.xpoints()*x.ypoints();
  //printf("toflat, npts = %d\n",npts); fflush(stdout);
  //return;

  for (i = 0; i < npts; i++) {
    y[i] = x[i];
  }
  return;
}
void out_nc(int* ncid, int* varid, global_12th<double> &lat, global_12th<double> &lon, 
              global_12th<unsigned char> &land, global_12th<unsigned char> &posteriori, 
              global_12th<float> &dist) {

  int retcode, npts = lat.xpoints()*lat.ypoints();

  //printf("entered out_nc, npts = %d\n",npts); fflush(stdout);
  //printf("land[0] = %d %d\n",(int) land[0], (int) land[npts-1]);
  //printf("lat[0] = %e %e\n", lat[0], lat[npts-1]);
  //printf("dist[0] = %f %f\n", dist[0],dist[npts-1]);
  //fflush(stdout);
  printf("land, post min: %d %d\n",land.gridmin(), posteriori.gridmin() );
  printf("land, post max: %d %d\n",land.gridmax(), posteriori.gridmax() );
  fflush(stdout);

  unsigned char *tchar;
  short int *tsint;
  float  *tfloat;
  double *tdouble;
  tchar   = new unsigned char[npts];
  tfloat  = new float[npts];
  tsint   = new short int[npts];
  tdouble = new double[npts];

  tchar[0] = (unsigned char) 0;
  tfloat[0] = (float) 0.0;
  tdouble[0] = (double) 0.0;
  tsint[0] = (short int) 0.0;
  global_12th<short int> tmp;

  size_t start[NDIMS]; start[0] = 0, start[1] = 0;
  size_t count[NDIMS]; count[0] = lat.ypoints(); count[1] = lat.xpoints();

  toflat(lon, &tdouble[0]);
  retcode = nc_put_vara_double(*ncid, varid[0], start, count,  &tdouble[0]);
  toflat(lat, &tdouble[0]);
  retcode = nc_put_vara_double(*ncid, varid[1], start, count,  &tdouble[0]);

  conv(land, tmp);
  toflat(tmp, &tsint[0]);
  retcode = nc_put_vara_short(*ncid, varid[2], start, count, &tsint[0]);
  if (retcode != 0) ERR(retcode);

  conv(posteriori, tmp);
  toflat(tmp, &tsint[0]);
  retcode = nc_put_vara_short(*ncid, varid[3], start, count, &tsint[0]);
  if (retcode != 0) ERR(retcode);

  toflat(dist, &tfloat[0]);
  retcode = nc_put_vara_float(*ncid, varid[4], start, count, &tfloat[0]);

}
