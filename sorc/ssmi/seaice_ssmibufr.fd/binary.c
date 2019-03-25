#include <stdio.h>

/* Perform the unformatted binary output for ssmibufr */
/* Robert Grumbine 12 Mar 2004 */

FILE *fout;

#ifdef IBM
int openout(int *unit) {
#elif LINUX
int openout_(int *unit) {
#else
int openout_(int *unit) {
#endif
  char fname[80];
  sprintf(fname,"fort.%02d\0",*unit);
  fout = fopen(fname, "w");
  if (fout == (FILE *) NULL) {
    printf("Failed to open output unit %d\n",*unit);
    return 1;
  }
  return 0;
}

#ifdef IBM
int shortout(int *satno, int *iy, int *im, int *id, int *ihr, int *imins, int *isec, int *iscan) {
#elif LINUX
int shortout_(int *satno, int *iy, int *im, int *id, int *ihr, int *imins, int *isec, int *iscan) {
#else
int shortout_(int *satno, int *iy, int *im, int *id, int *ihr, int *imins, int *isec, int *iscan) {
#endif
  fwrite(satno, sizeof(int), 1, fout);
  fwrite(iy, sizeof(int), 1, fout);
  fwrite(im, sizeof(int), 1, fout);
  fwrite(id, sizeof(int), 1, fout);
  fwrite(ihr, sizeof(int), 1, fout);
  fwrite(imins, sizeof(int), 1, fout);
  fwrite(isec, sizeof(int), 1, fout);
  fwrite(iscan, sizeof(int), 1, fout);
  return 0;
}

#ifdef IBM
int longout(int *kscan, float *xlat, float *xlon, int *sftg, int *posn, float *tmp1, float *tmp2, float *tmp3, float *tmp4, float *tmp5, float *tmp6, float *tmp7) {
#elif LINUX
int longout_(int *kscan, float *xlat, float *xlon, int *sftg, int *posn, float *tmp1, float *tmp2, float *tmp3, float *tmp4, float *tmp5, float *tmp6, float *tmp7) {
#else
int longout_(int *kscan, float *xlat, float *xlon, int *sftg, int *posn, float *tmp1, float *tmp2, float *tmp3, float *tmp4, float *tmp5, float *tmp6, float *tmp7) {
#endif

  fwrite(kscan, sizeof(int), 1, fout);
  fwrite(xlat, sizeof(float), 1, fout);
  fwrite(xlon, sizeof(float), 1, fout);
  fwrite(sftg, sizeof(int), 1, fout);
  fwrite(posn, sizeof(int), 1, fout);
  fwrite(tmp1, sizeof(float), 1, fout);
  fwrite(tmp2, sizeof(float), 1, fout);
  fwrite(tmp3, sizeof(float), 1, fout);
  fwrite(tmp4, sizeof(float), 1, fout);
  fwrite(tmp5, sizeof(float), 1, fout);
  fwrite(tmp6, sizeof(float), 1, fout);
  fwrite(tmp7, sizeof(float), 1, fout);
  return 0;
} 

#ifdef IBM
int hiresout(int *kwrit, float *xlat, float *xlon, int *sftg, int*posn, float *tmp1, float*tmp2) {
#else
int hiresout_(int *kwrit, float *xlat, float *xlon, int *sftg, int*posn, float *tmp1, float*tmp2) {
#endif
  fwrite(kwrit, sizeof(int), 1, fout);
  fwrite(xlat, sizeof(float), 1, fout);
  fwrite(xlon, sizeof(float), 1, fout);
  fwrite(sftg, sizeof(int), 1, fout);
  fwrite(posn, sizeof(int), 1, fout);
  fwrite(tmp1, sizeof(float), 1, fout);
  fwrite(tmp2, sizeof(float), 1, fout);
  
  return 0;
}
