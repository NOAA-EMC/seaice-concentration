#include "ncepgrids.h"

// Apply an a posteriori file (derived from SST flags in this case)
//    to the hemispheric analysis grids and write out the filtered
//    versions
// Robert Grumbine 13 August 2010

// Process netcdf a posteriori file 28 Jul 2025
#include "netcdf.h"
/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

void enter(grid2<short int> &param, short int *x) ;

int get_nc(char *fname, global_12th<short int> &flags);


float posteriori(psgrid<unsigned char> &x, global_12th<short int> &flags, psgrid<unsigned char> &y);

int main(int argc, char *argv[]) {
  FILE *inflag, *innh, *insh, *outnh, *outsh;
  global_12th<short int> flags;
  NORTH<unsigned char> nh, nhout;
  SOUTH<unsigned char> sh, shout;
  float area = 0.0;

  //inflag = fopen(argv[1], "r");
  //flags.binin(inflag); fclose(inflag);
  // read posteriori.nc
  get_nc(argv[1],flags);
  printf("returned from get_nc\n"); fflush(stdout);

  innh   = fopen(argv[2], "r");
  insh   = fopen(argv[3], "r");
  outnh  = fopen(argv[4], "w");
  outsh  = fopen(argv[5], "w");

  nh.binin(innh); fclose(innh);
  sh.binin(insh); fclose(insh);
   
  area  = posteriori(nh, flags, nhout);
  area += posteriori(sh, flags, shout);
  printf("%f thousand km^2 caught in a posteriori filter \n",area / 1.e9 );
  nhout.binout(outnh); fclose(outnh);
  shout.binout(outsh); fclose(outsh);

  return 0;
}
float posteriori(psgrid<unsigned char> &x, global_12th<short int> &flags, psgrid<unsigned char> &y) {
  float area = 0;
  ijpt iloc, floc;
  latpt ll;

  y = x;

  for (iloc.j = 0; iloc.j < x.ypoints(); iloc.j++) {
  for (iloc.i = 0; iloc.i < x.xpoints(); iloc.i++) {
    ll = x.locate(iloc);
    floc = flags.locate(ll);
    if (flags[floc] == 2 || flags[floc] == NO_DATA) {
      x[iloc] = 0;
    }
    else if (flags[floc] == 158 || flags[floc] == 159 || flags[floc] == 160 \
                           || flags[floc] == 161 || flags[floc] == 162 \
                           || flags[floc] == 163 || flags[floc] == 164 ) {
      if (x[iloc] >= MIN_CONC && x[iloc] <= MAX_CONC) {
           area += x.cellarea(iloc); 
           x[iloc] = 1;
           y[iloc] = 0;
      }
      else {
        x[iloc] = 0;
      }
    }
    else if (flags[floc] == 170 || flags[floc] == 171 || flags[floc] == 172) {
      if (x[iloc] >= MIN_CONC && x[iloc] <= MAX_CONC) {
           area += x.cellarea(iloc); 
           x[iloc] = 1;
           y[iloc] = 0;
      }
      else {
        x[iloc] = 0;
      }
    }
    else {
      x[iloc] = 0;
    }
  }
  }

  return area;
}
int get_nc(char *fname, global_12th<short int> &posteriori) {
//lat, lon, posteriori, mask,
  int i, ncid, varid, retval;
  int idp = 0;
  char obname[90];
  size_t lenp;

  retval = nc_open(fname, NC_NOWRITE, &ncid); if (retval != 0) ERR(retval);

  //retval = nc_inq_dim(ncid, idp, obname, &lenp);
  //if (retval != 0) ERR(retval);
  //printf("var name and size %s %ld\n",obname, lenp); fflush(stdout);
  //if (lenp == 0) {
  //  printf("no data for posteriori file file %s, exiting\n",fname);
  //  return 1;
  //}

  short int *ix;
  ix = (short int *) malloc(sizeof(short int) * posteriori.xpoints()*posteriori.ypoints()  );
 
  retval = nc_inq_varid(ncid, "posteriori", &varid); 
  if (retval != 0) ERR(retval);

  retval = nc_get_var_short(ncid, varid, ix);
  if (retval != 0) ERR(retval);
  enter(posteriori, ix);


  return 0;
}
void enter(grid2<short int> &param, short int *x) {
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
