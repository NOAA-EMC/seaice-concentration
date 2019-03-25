#include <cstdio>
using namespace std;

#include "icessmi.h"

int main(int argc, char *argv[]) {
  FILE *fin;
  int i;

// Header:
  int nfreqs, nobs;
  float *freqs;
  int   platform, *polar;

// points:
  char dtg[12];
  latpt ll;
  float *tb;
  float conc;
  int qc; 

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

  freqs = new float[nfreqs];
  tb    = new float[nfreqs];
  polar = new int[nfreqs];

  fread(freqs,     sizeof(float), nfreqs, fin);
  fread(polar,     sizeof(int)  , nfreqs, fin);
  for (i = 0 ; i < nfreqs; i++) {
    printf("%1d %f %d\n",i,freqs[i], polar[i]);
  } 

  // write out the observation points:
  for (i = 0; i < nobs; i++) {

    fread(dtg,   sizeof(char), 12, fin);
    fread(&ll,   sizeof(latpt), 1, fin);
    fread(tb,    sizeof(float), nfreqs, fin);
    fread(&conc, sizeof(float), 1, fin);
    fread(&qc,   sizeof(int),   1, fin);
   
    printf("%d %12s %7.3f %8.3f %6.2f %4.2f %1d\n",i, &dtg[0], ll.lat, ll.lon, tb[0], conc, qc);
    
  }


  return 0;
}
