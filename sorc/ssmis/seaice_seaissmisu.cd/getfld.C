int ssmis_getfld(ssmis *ice, int npts, unsigned char *cfld, float *ffld, int sel) {
/* Extract a desired field (specified by sel) from a full ssmis map
   (ice) and copy to both a character array (cfld) and a floating
   point array (ffdl) for some other routine to use.
   Tb Floats are scaled into degrees Kelvin, and have a 0.01 degree precision.
   Tb Chars are linearly rescaled according to o = (i-50)/2, where i is 
     the floating number input, and o is the output, with 2 degree precision
     starting from 50 Kelvin.
   Ice concentrations are 1% precision, floating or character.
   Robert Grumbine 11 October 1994.
*/

  int i, limit;

  limit = npts;

  switch (sel) {
  case SSMIS_T19V :
    for (i = 0; i < limit; i++) {
      ffld[i] = (float) ice[i].t19v/100.;
      cfld[i] = (unsigned char) (0.5 + (ffld[i] - 50.) / 2. );
    }
    break;
  case SSMIS_T19H :
    for (i = 0; i < limit; i++) {
      ffld[i] = (float) ice[i].t19h/100.;
      cfld[i] = (unsigned char) (0.5 + (ffld[i] - 50.) / 2. );
    }
    break;
  case SSMIS_T22V :
    for (i = 0; i < limit; i++) {
      ffld[i] = (float) ice[i].t22v/100.;
      cfld[i] = (unsigned char) (0.5 + (ffld[i] - 50.) / 2. );
    }
    break;
  case SSMIS_T37V :
    for (i = 0; i < limit; i++) {
      ffld[i] = (float) ice[i].t37v/100.;
      cfld[i] = (unsigned char) (0.5 + (ffld[i] - 50.) / 2. );
    }
    break;
  case SSMIS_T37H :
    for (i = 0; i < limit; i++) {
      ffld[i] = (float) ice[i].t37h/100.;
      cfld[i] = (unsigned char) (0.5 + (ffld[i] - 50.) / 2. );
    }
    break;
  case SSMIS_T92V :
    for (i = 0; i < limit; i++) {
      ffld[i] = (float) ice[i].t92v/100.;
      cfld[i] = (unsigned char) (0.5 + (ffld[i] - 50.) / 2. );
    }
    break;
  case SSMIS_T92H :
    for (i = 0; i < limit; i++) {
      ffld[i] = (float) ice[i].t92h/100.;
      cfld[i] = (unsigned char) (0.5 + (ffld[i] - 50.) / 2. );
    }
    break;
  case SSMIS_T150H :
    for (i = 0; i < limit; i++) {
      ffld[i] = (float) ice[i].t150h/100.;
      cfld[i] = (unsigned char) (0.5 + (ffld[i] - 50.) / 2. );
    }
    break;
  case SSMIS_CONC_BAR :
    for (i = 0; i < limit; i++) {
      ffld[i] = (float) ice[i].conc_bar/100.;
      cfld[i] = (unsigned char) (0.5 + ice[i].conc_bar) ;
    }
    break;
  case SSMIS_BAR_CONC :
    for (i = 0; i < limit; i++) {
      ffld[i] = (float) ice[i].bar_conc/100.;
      cfld[i] = (unsigned char) ice[i].bar_conc ;
    }
    break;
  case SSMIS_COUNT :
    for (i = 0; i < limit; i++) {
      ffld[i] = (float) ice[i].count;
      cfld[i] = (unsigned char) ice[i].count;
    }
    break;
  case SSMIS_WEATHER_COUNT :
    for (i = 0; i < limit; i++) {
      ffld[i] = (float) ice[i].count;
      cfld[i] = (unsigned char) ice[i].count;
    }
    break;
  case SSMIS_HIRES_CONC :
    for (i = 0; i < limit; i++) {
      ffld[i] = (float) ice[i].hires_conc/100.;
      cfld[i] = (unsigned char) ice[i].hires_conc ;
    }
    break;
  default :
    return -1;
  }

  return 0;

}

