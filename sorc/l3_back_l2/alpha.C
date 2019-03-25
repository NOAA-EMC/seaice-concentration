//Copy/paste of Bob's reread.C
//Currently a bit of a hack job: Reads binary file and output to netcdf.
//Currently built this way:
//gcc bin2nc.C -I$NETCDF/include -L$NETCDF/lib -lnetcdf -o bin2nc
#include <cstdio>
#include <netcdf.h>

#define FILE_NAME "sea-ice-con.nc"
#define NDIMS 1

struct Loc //Replaces latpt, couldn't find icessmi.h
{
  float lat;
  float lon;
};

int main(int argc, char *argv[]) {
  FILE *fin;
  int i;
  // Header:
  int nfreqs, nobs;
  int qc;
  int   platform;//, *polar;
  char dtg[12];
  Loc ll;
  float conc;

  //Netcdf stuff
  int ncid;      // Netcdf file handle
  int rec_dimid; // Id for unlimited dim
  int varid_sic;
  int varid_lon;
  int varid_lat;
  int dimids[NDIMS];
  size_t start[NDIMS], count[NDIMS];

  nc_create(FILE_NAME, NC_CLOBBER, &ncid);
  nc_def_dim(ncid, "nobs", NC_UNLIMITED , &rec_dimid);
  dimids[0] = rec_dimid;
  nc_def_var(ncid, "lon", NC_FLOAT, NDIMS, dimids, &varid_lon);
  nc_def_var(ncid, "lat", NC_FLOAT, NDIMS, dimids, &varid_lat);
  nc_def_var(ncid, "icec", NC_FLOAT, NDIMS, dimids, &varid_sic);
  nc_enddef(ncid);

  ///////////////  Read in //////////////////////////////
  fin = fopen(argv[1], "r");
  if (fin == (FILE*) NULL) {
    printf("failed to open input file %s\n",argv[1]);
    return 1;
  }
  /////////////////////////////////////////////////////////
  //read in header info:
  fread(&nobs,     sizeof(int)  ,     1 , fin);
  fread(&platform, sizeof(int)  ,     1 , fin);
  fread(&nfreqs  , sizeof(int)  ,     1 , fin);
  printf("nobs %d platform %d nfreqs %d\n",nobs, platform, nfreqs); fflush(stdout);

  float freqs[nfreqs];
  float tb[nfreqs];
  int polar[nfreqs];
  fread(freqs,     sizeof(float), nfreqs, fin);
  fread(polar,     sizeof(int)  , nfreqs, fin);
  for (i = 0 ; i < nfreqs; i++) {
    printf("%1d %f %d\n",i,freqs[i], polar[i]);
  }
  // write out the observation points:
  int cnt = 0;
  for (int i = 0; i < nobs; i++) {
    fread(dtg,   sizeof(char), 12, fin);
    fread(&ll,   sizeof(Loc), 1, fin);
    fread(tb,    sizeof(float), nfreqs, fin);
    fread(&conc, sizeof(float), 1, fin);
    fread(&qc,   sizeof(int),   1, fin);

    // Write netcdf file
    if (qc == 1) {
      start[0] = cnt;
      count[0] = 1;
      nc_put_vara_float(ncid, varid_lon, start, count, &ll.lon);
      nc_put_vara_float(ncid, varid_lat, start, count, &ll.lat);
      nc_put_vara_float(ncid, varid_sic, start, count, &conc);
      cnt++;
    }
  }

  //Finalize netcdf
  nc_close(ncid);
  return 0;
}
