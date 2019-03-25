#include <stdio.h>
#include "avhrr.h"

FILE *_gac_fout;

#ifdef IBM
int openout(int *unit) {
#elif LINUX
int openout_(int *unit) {
#else
int openout_(int *unit) {
#endif
  char fname[12];
  sprintf(fname,"fort.%02d",*unit);
  _gac_fout = fopen(fname, "w");
  if (_gac_fout == (FILE *) NULL) {
    printf("Failed to open output unit %d\n",*unit);
    return 1;
  }
  return 0;
}

#ifdef IBM
void gacout(double *dst, double *chan) {
#elif LINUX
void gacout_(double *dst, double *chan) {
#else
void gacout_(double *dst, double *chan) {
#endif
  int j;

  gacpt x;
  x.year   = (short int)     dst[0];
  x.month  = (unsigned char) dst[1];
  x.day    = (unsigned char) dst[2];
  x.hour   = (unsigned char) dst[3];
  x.minute = (unsigned char) dst[4];
  x.second = (unsigned char) dst[5];
  x.satid = (short int) dst[6];
  
  x.clat = dst[7];
  x.clon = dst[8];

  x.saza = dst[9];
  x.soza = dst[10];

  x.fovn = dst[11];
  x.clavr = dst[12];

  for (j = 0; j < 5; j++) {
    x.obs[j].channel = (unsigned char) chan[j*3 + 0];
    if (chan[j*3 + 1] > 1e6) chan[j*3 + 1] = 255;
    x.obs[j].albedo = (unsigned char) chan[j*3 + 1];
    if (chan[j*3 + 2] > 1e6) chan[j*3 + 2] = 999.0;
    x.obs[j].tmbr = chan[j*3 + 2];
  }

  /* if (x.year != 2008) {
     printf("year = %d dst.0 = %e ",x.year, dst[0]);
   }
  */
  fwrite(&x, sizeof(gacpt), 1, _gac_fout);

  return;
}
