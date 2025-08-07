#include "ncepgrids.h"

// Program to read in climatology for the day and write out
//   an estimated sea ice field.
// Use count >= 15, then condavg for concentration field
// Robert Grumbine 10 March 2014
// Update to new climatology: netcdf, 18 years (2007-2024)
// Robert Grumbine 7 August 2025

#include "netcdf.h"
/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}
int get_nc(char *fname, psgrid<float> &count, psgrid<float> &condavg) ;
void enter(grid2<float> &param, float *x) ;

int main(int argc, char *argv[]) {
  osisaf_north<float> nhcount, nhcondavg;
  osisaf_south<float> shcount, shcondavg;
  GRIDTYPE<float> high, countmp, avgtmp;
  FILE *fin, *fout;
  ijpt loc;
  latpt ll;


  get_nc(argv[1], nhcount, nhcondavg);
  get_nc(argv[2], shcount, shcondavg);
  printf("nh %f %f  %f %f\n",nhcount.gridmax(), nhcount.gridmin(), nhcondavg.gridmax(), nhcondavg.gridmin() );
  printf("sh %f %f  %f %f\n",shcount.gridmax(), shcount.gridmin(), shcondavg.gridmax(), shcondavg.gridmin() );

  float landval = nhcount.gridmin();
  float nonval = landval;

  countmp.fromall(nhcount, landval, nonval);
  avgtmp.fromall(nhcondavg, landval, nonval);
  for (loc.j = 0; loc.j < high.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < high.xpoints(); loc.i++) {
    ll = high.locate(loc);
    if (ll.lat > 0) {
      if (countmp[loc] >= 9) {
	high[loc] = avgtmp[loc]; 
      }
      else {
        high[loc] = 0.0;
      }
    }
  }
  } 

  countmp.fromall(shcount, landval, nonval);
  avgtmp.fromall(shcondavg, landval, nonval);
  for (loc.j = 0; loc.j < high.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < high.xpoints(); loc.i++) {
    ll = high.locate(loc);
    if (ll.lat < 0) {
      if (countmp[loc] >= 9) {
	high[loc] = avgtmp[loc]; 
      }
      else {
        high[loc] = 0.0;
      }
    }
  }
  }

  fout = fopen(argv[3],"w");
  high.binout(fout);
  fclose(fout);

  GRIDTYPE<unsigned char> x;
  fout = fopen("char","w");
  for (loc.j = 0; loc.j < high.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < high.xpoints(); loc.i++) {
    x[loc] = (unsigned char) 100.*high[loc];
  }
  }
  x.binout(fout);
  fclose(fout);

  return 0;
}
int get_nc(char *fname, psgrid<float> &count, psgrid<float> &condavg) {
  int i, ncid, varid, retval;
  int idp = 0;
  char obname[90];
  size_t lenp;

  retval = nc_open(fname, NC_NOWRITE, &ncid); if (retval != 0) ERR(retval);
  
  float *fx;
  fx = (float *) malloc(sizeof(float)*count.xpoints() * count.ypoints() );

  retval = nc_inq_varid(ncid, "count", &varid);
  if (retval != 0) ERR(retval);
  retval = nc_get_var_float(ncid, varid, fx);
  if (retval != 0) ERR(retval);
  enter(count, fx);
    
  retval = nc_inq_varid(ncid, "conditional_average", &varid);
  if (retval != 0) ERR(retval);
  retval = nc_get_var_float(ncid, varid, fx);
  if (retval != 0) ERR(retval);
  enter(condavg, fx);
  if (condavg.gridmax() >= 1.58) condavg /= 100.;
    
  nc_close(ncid);
  
  return 0;
}
void enter(grid2<float> &param, float *x) {
  ijpt loc;
  int nx = param.xpoints();
  for (loc.j = 0; loc.j < param.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < param.xpoints(); loc.i++) {
    // flag value is negative
    if (x[loc.i+ nx*loc.j] < 0 ) x[loc.i+ nx*loc.j] = 0;
    param[loc] = x[loc.i+ nx*loc.j];
  } 
  }
  
  return;
} 
