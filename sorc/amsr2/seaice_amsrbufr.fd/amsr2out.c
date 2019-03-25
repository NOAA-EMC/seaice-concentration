#include <stdio.h>
#include "amsr2.h"
#include "f2c_files.h"


#ifdef IBM
void headerout
#elif LINUX
void headerout_
#else
void headerout_
#endif
(int *isad,int *myr,int *mon,int *mda,int *mhr,int *minu, 
                double *slat, double *slon, int *nchan) {
  amsr2head x;

  x.satid = *isad;

  x.date.year   = *myr;
  x.date.month  = *mon;
  x.date.day    = *mda;
  x.date.hour   = *mhr;
  x.date.minute = *minu;
  x.date.second = 0;

  x.clat   = *slat;
  x.clon   = *slon;
  x.nspots = *nchan;

  fwrite(&x, sizeof(x), 1, fout);

  return;
}
#ifdef IBM
void spotout
#elif LINUX
void spotout_
#else
void spotout_
#endif
(float *sccf, float *alfr, float *viirsq, float *anpo, float *btmp) {
  amsr2_spot x;
  /* printf("%f %f %f %f %f\n",*sccf, *alfr, *viirsq, *anpo, *btmp); */

  x.sccf = *sccf / 1.e8; /* tenths of GHz */
  x.alfr = *alfr;
  x.viirsq = *viirsq;
  x.anpo = *anpo;
  x.tmbr = *btmp;

  fwrite(&x, sizeof(x), 1, fout);

  return;
}
