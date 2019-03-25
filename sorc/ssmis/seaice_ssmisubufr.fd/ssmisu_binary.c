#include <stdio.h>
#include "ssmisu.h"

FILE *fout;

#ifdef IBM
int openout(int *unit) {
#elif LINUX
int openout_(int *unit) {
#else
int openout_(int *unit) {
#endif
  char fname[12];
  sprintf(fname,"fort.%02d",*unit);
  fout = fopen(fname, "w");
  if (fout == (FILE *) NULL) {
    printf("Failed to open output unit %d\n",*unit);
    return 1;
  }
  return 0;
}

/* ssmisout */
#ifdef IBM
void ssmisout(double *hdr, double *ident, double *chan) {
#elif LINUX
void ssmisout_(double *hdr, double *ident, double *chan) {
#else
void ssmisout_(double *hdr, double *ident, double *chan) {
#endif

  ssmisupt x;
  int bad = 0;
  int k, j;

  x.year   = (short int)     hdr[1];
  x.month  = (unsigned char) hdr[2];
  x.day    = (unsigned char) hdr[3];
  x.hour   = (unsigned char) hdr[4];
  x.minute = (unsigned char) hdr[5];
  x.second = (unsigned char) hdr[6];
  x.satid = (short int) ident[0];
  
  x.clat = hdr[7];
  x.clon = hdr[8];
  if (x.clat > 90 || x.clon > 360 || x.clon < -360) bad += 1;

  k = 0;
  for (j = 0; j < 24; j++) {
    /* We're only extracting a few channels, hence looking for j */
    if (j == 7 || (j >= 11 && j <= 17) ) {
      x.obs[k].tmbr    = chan[j*4 + 1];
      /* VERBOSE printf("%2d %6.2f\n",j, x.obs[j].tmbr); */
      if (x.obs[k].tmbr > 350) bad += 1;
      k += 1;
    }
  }


  if (bad == 0) {
    fwrite(&x, sizeof(ssmisupt), 1, fout);
  }
  #ifdef VERBOSE2
    printf("%4d %2d %2d %2d %2d %2d %3d  ",x.year, x.month, x.day, x.hour, x.minute, x.second, x.satid);
    printf("%6.2f %7.2f  ",x.clat, x.clon);
    for (i = 0; i < k; i++) {
      printf("%6.2f ",x.obs[i].tmbr);
    }
    printf("\n");
  #endif

  return;
}
