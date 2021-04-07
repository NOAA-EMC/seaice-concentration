#include <cstdio>
#include <cstdlib>

// Process netcdf L2 to binary L3
#include "netcdf.h"
/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

#include "ncepgrids.h"
#include "icessmis.h"
#include "icegrids.h"

void get_nc(char *fname, psgrid<float> &north, psgrid<float> &south);
void append(grid2<ssmis_tmp> &x, int index, float t19v, float t19h, float t22v, float t37v, float t37h, float t92v, float t92h, float conc) ;

template <class T>
void enter(mvector<T> &param, T *x) ;

void enter(grid2<float> &param, float *x) ;

int main(int argc, char *argv[]) {
  FILE *fout;
  northhigh<float> north;
  southhigh<float> south;
  northhigh<unsigned char> nconc;
  southhigh<unsigned char> sconc;

///////// NETCDF gets ///////////////////////////////////////////
  get_nc(argv[1], north, south);

//// write out the uchar maps:
  conv(north, nconc);
  conv(south, sconc);
  fout = fopen(argv[2], "w");
  nconc.binout(fout);
  fclose(fout);

  fout = fopen(argv[3], "w");
  sconc.binout(fout);
  fclose(fout);
  

  return 0;
}

void  get_nc(char *fname, psgrid<float> &north, psgrid<float> &south) {
// For netcdf:
  int i, ncid, varid, retval;
  int idp;
  char obname[90];
  size_t lenp;

// open netcdf file:
  retval = nc_open(fname, NC_NOWRITE, &ncid); if (retval != 0) ERR(retval);
  //debug printf("ncid, retval = %d %d\n",ncid, retval); fflush(stdout);

// find dimension:
  retval = nc_inq_dim(ncid, idp, obname, &lenp);
  if (retval != 0) ERR(retval);
  //debug printf("variable name and size: %s %ld\n",obname, lenp); fflush(stdout);

  float *fx;
  int *ix;
  double *doublex;
  fx = (float*) malloc(sizeof(float)*lenp);
  ix = (int*) malloc(sizeof(int)*lenp);
  doublex = (double*) malloc(sizeof(double)*lenp);

  mvector<double> dlat(lenp), dlon(lenp);
  mvector<int> satid(lenp), quality(lenp);
//  mvector<int> dtg1(lenp), dtg2(lenp);
  mvector<float> conc(lenp), land_flag(lenp);
  mvector<float> t19v(lenp), t19h(lenp), t22v(lenp), t37v(lenp), t37h(lenp), t92v(lenp), t92h(lenp);

// get locations:
  retval = nc_inq_varid(ncid, "latitude", &varid); if (retval != 0) ERR(retval);
  retval = nc_get_var_double(ncid, varid, doublex); if (retval != 0) ERR(retval); 
  enter(dlat, doublex);

  retval = nc_inq_varid(ncid, "longitude", &varid); if (retval != 0) ERR(retval);
  retval = nc_get_var_double(ncid, varid, doublex); if (retval != 0) ERR(retval); 
  enter(dlon, doublex);

  //debug printf("lat range: %e %e\n",dlat.maximum(), dlat.minimum() );
  //debug printf("lon range: %e %e\n",dlon.maximum(), dlon.minimum() );

// satid, quality, ignore dtg -- presumption of being taken care of already
  //retval = nc_inq_varid(ncid, "satid", &varid); if (retval != 0) ERR(retval);
  //retval = nc_get_var_int(ncid, varid, ix); if (retval != 0) ERR(retval); 
  //enter(satid, ix);
  retval = nc_inq_varid(ncid, "quality", &varid); if (retval != 0) ERR(retval);
  retval = nc_get_var_int(ncid, varid, ix); if (retval != 0) ERR(retval);
  enter(quality, ix);

// satellite Tb info:
  retval = nc_inq_varid(ncid, "tb_19V", &varid); if (retval != 0) ERR(retval);
  retval = nc_get_var_float(ncid, varid, fx); if (retval != 0) ERR(retval);
  enter(t19v, fx);
  retval = nc_inq_varid(ncid, "tb_19H", &varid); if (retval != 0) ERR(retval);
  retval = nc_get_var_float(ncid, varid, fx); if (retval != 0) ERR(retval);
  enter(t19h, fx);
  retval = nc_inq_varid(ncid, "tb_22V", &varid); if (retval != 0) ERR(retval);
  retval = nc_get_var_float(ncid, varid, fx); if (retval != 0) ERR(retval);
  enter(t22v, fx);
  retval = nc_inq_varid(ncid, "tb_37V", &varid); if (retval != 0) ERR(retval);
  retval = nc_get_var_float(ncid, varid, fx); if (retval != 0) ERR(retval);
  enter(t37v, fx);
  retval = nc_inq_varid(ncid, "tb_37H", &varid); if (retval != 0) ERR(retval);
  retval = nc_get_var_float(ncid, varid, fx); if (retval != 0) ERR(retval);
  enter(t37h, fx);
  retval = nc_inq_varid(ncid, "tb_92V", &varid); if (retval != 0) ERR(retval);
  retval = nc_get_var_float(ncid, varid, fx); if (retval != 0) ERR(retval);
  enter(t92v, fx);
  retval = nc_inq_varid(ncid, "tb_92H", &varid); if (retval != 0) ERR(retval);
  retval = nc_get_var_float(ncid, varid, fx); if (retval != 0) ERR(retval);
  enter(t92h, fx);
  //debug printf("t19v range: %e %e\n",t19v.maximum(), t19v.minimum() );

// concentration, land flag:
  retval = nc_inq_varid(ncid, "ice_concentration", &varid); if (retval != 0) ERR(retval); 
  retval = nc_get_var_float(ncid, varid, fx); if (retval != 0) ERR(retval);
  enter(conc, fx);
  retval = nc_inq_varid(ncid, "land_flag", &varid); if (retval != 0) ERR(retval);
  retval = nc_get_var_float(ncid, varid, fx); if (retval != 0) ERR(retval);
  enter(land_flag, fx);
  //debug printf("conc range: %e %e\n",conc.maximum(), conc.minimum() );

// close when all info has been read in:
  retval = nc_close(ncid); if (retval != 0) ERR(retval);
// release temporary vectors
  free(fx);
  free(ix);
  free(doublex);

// Now work on constructing L3: -----------------------------------------
  grid2<ssmis_tmp> ntmp(north.xpoints(), north.ypoints() );
  grid2<ssmis_tmp> stmp(south.xpoints(), south.ypoints() );

  grid2<int> ncount(north.xpoints(), north.ypoints() );
  grid2<int> scount(south.xpoints(), south.ypoints() );
  latpt ll;
  ijpt locn, locs;
  int index;
  ncount.set( 0);
  scount.set( 0);
  north.set( (float) 0.);
  south.set( (float) 0.);

  for (i = 0; i < lenp; i++) {
    ll.lat = dlat[i];
    ll.lon = dlon[i];
    // n.b.: location for wrong hemisphere returns as 0,0, which isn't helpful
    if (ll.lat > 0) {
      locn = north.locate(ll);
      if (north.in(locn)) {
        //debug printf("in north %f %f  %3d %3d quality %d conc %f land %f\n",
        //  ll.lat, ll.lon, locn.i, locn.j, quality[i], conc[i], land_flag[i]);
        if (quality[i] == 4 || quality[i] == 1) {
          index = locn.i+locn.j*ntmp.xpoints();
          append(ntmp, index, t19v[i], t19h[i], t22v[i], t37v[i], t37h[i], t92v[i], t92h[i], conc[i]);
          ncount[locn] += 1;
          north[locn]  += conc[i] * 100.;
        }
      }
    }
    else {
      locs = south.locate(ll);
      if (south.in(locs)) {
        //debug printf("in south %f %f  %3d %3d quality %d conc %f land %f\n",
        //  ll.lat, ll.lon, locs.i, locs.j, quality[i], conc[i], land_flag[i]);
        if (quality[i] == 4 || quality[i] == 1) {
          index = locs.i+locs.j*stmp.xpoints();
          append(stmp, index, t19v[i], t19h[i], t22v[i], t37v[i], t37h[i], t92v[i], t92h[i], conc[i]);
          scount[locs] += 1;
          south[locs]  += conc[i] * 100.;
        }
      }
    }
  }

  printf("ncount max %d %d %d\n",ncount.gridmax(), ncount.gridmin(), ncount.average() );
  printf("scount max %d %d %d\n",scount.gridmax(), scount.gridmin(), scount.average() );

  int undef = 0;
  for (locn.j = 0; locn.j < north.ypoints(); locn.j++) {
  for (locn.i = 0; locn.i < north.xpoints(); locn.i++) {
    if (ncount[locn] != 0) {
      north[locn] /= ncount[locn];
    }
    else {
      north[locn] = 224.;
      undef++;
    }
  }
  }
  for (locs.j = 0; locs.j < south.ypoints(); locs.j++) {
  for (locs.i = 0; locs.i < south.xpoints(); locs.i++) {
    if (scount[locs] != 0) {
      south[locs] /= scount[locs];
    }
    else {
      south[locs] = 224.;
      undef++;
    }
  }
  }
  printf("%d undefined points\n",undef);

  //debug palette<unsigned char> gg(19,65);
  //debug north.xpm("n.xpm",7,gg);
  //debug south.xpm("s.xpm",7,gg);


  return;
}
// --------------------------------------------------------------------
template <class T>
void enter(mvector<T> &param, T *x) {
  for (int i = 0; i < param.xpoints(); i++) {
    if (x[i] > 1e30) x[i] = 0;
    param[i] = x[i];
  }
  return;
}

void enter(grid2<float> &param, float *x) {
  ijpt loc;
  int nx = param.xpoints();
  for (loc.j = 0; loc.j < param.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < param.xpoints(); loc.i++) {
    if (x[loc.i+ nx*loc.j] > 1e30) x[loc.i+ nx*loc.j] = 0;
    param[loc] = x[loc.i+ nx*loc.j];
  }
  }

  return;
}
void append(grid2<ssmis_tmp> &x, int index, float t19v, float t19h, float t22v, float t37v, float t37h, float t92v, float t92h, float conc) {
  if (x[index].count == 0) {
    x[index].t19v = (unsigned int) (0.5+t19v*100);
    x[index].t19h = (unsigned int) (0.5+t19h*100);
    x[index].t22v = (unsigned int) (0.5+t22v*100);
    x[index].t37v = (unsigned int) (0.5+t37v*100);
    x[index].t37h = (unsigned int) (0.5+t37h*100);
    x[index].t92v = (unsigned int) (0.5+t92v*100);
    x[index].t92h = (unsigned int) (0.5+t92h*100);
    x[index].conc_bar = (int) (0.5+conc*100);
    x[index].count = 1;
  }
  else {
    x[index].conc_bar +=   (int) (0.5 + conc*100);
    x[index].t19v += (unsigned int) (0.5+t19v*100);
    x[index].t19h += (unsigned int) (0.5+t19h*100);
    x[index].t22v += (unsigned int) (0.5+t22v*100);
    x[index].t37v += (unsigned int) (0.5+t37v*100);
    x[index].t37h += (unsigned int) (0.5+t37h*100);
    x[index].t92v += (unsigned int) (0.5+t92v*100);
    x[index].t92h += (unsigned int) (0.5+t92h*100);
    x[index].count += 1;
  }

  return ;
}
