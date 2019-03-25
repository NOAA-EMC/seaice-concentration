#include <cstdio>

// Generalized program for intercomparing two sea ice concentration grids
// Incorporates earlier programs dating back to at least 17 Sep 1998
// Robert Grumbine 20 August 2010

#include "ncepgrids.h"

double ice_extent(GRIDTYPE<DATTYPE> &x, float minconc) ;

template<class T>
void forstats(metricgrid<T> &ntoday, metricgrid<T> &nyesterday, metricgrid<unsigned char> &nland) ;
template<class T>
void showall(metricgrid<T> &x, metricgrid<T> &y, metricgrid<unsigned char> &nland) ;

void flags(GRIDTYPE<DATTYPE> &ref, GRIDTYPE<DATTYPE> &newer, GRIDTYPE<unsigned char> &land) ;
void kml(GRIDTYPE<DATTYPE> &x, GRIDTYPE<DATTYPE> &y, FILE *fout);

int main(int argc, char *argv[]) {
  FILE *ftoday, *fyesterday, *fland, *kmlout;
  GRIDTYPE<DATTYPE> ntoday, nyesterday;
  GRIDTYPE<float>  ndelta;
  GRIDTYPE<unsigned char> nland;

  palette<unsigned char>  hpal(21), gg(19,65);
  unsigned char land_flag = 157;
  float tmp = 255;
  ijpt x;
  int i;

// Get today, yesterday, land grids:
  ftoday = fopen(argv[1], "r");
  if (ftoday == (FILE*) NULL) {
    printf("monitor Failed to open today input file %s\n",argv[1]);
    return 1;
  }
  ntoday.binin(ftoday);
  fclose(ftoday);
  if (ntoday.gridmax() < 3) ntoday *= 100.;

  fyesterday = fopen(argv[2], "r");
  if (fyesterday == (FILE*) NULL) {
    printf("monitor Failed to open yesterday input file %s\n",argv[2]);
    return 1;
  }
  nyesterday.binin(fyesterday);
  fclose(fyesterday);
  if (nyesterday.gridmax() < 3) nyesterday *= 100.;

  fland = fopen(argv[3], "r");
  if (fland == (FILE*) NULL) {
    printf("monitor Failed to open land input file %s\n",argv[3]);
    return 1;
  }
  nland.binin(fland);
  fclose(fland);

  kmlout = fopen(argv[5],"w");
  if (kmlout == (FILE*) NULL) {
    printf("Failed to open %s\n",argv[5]);
    return 1;
  }

// Display the input grids:
  char fname[90];
  //ntoday.xpm("today.xpm",7,gg);

  sprintf(fname,"today.xpm");
  ntoday.xpm(fname,7,gg);
  sprintf(fname,"yesterday.xpm");
  nyesterday.xpm(fname,7,gg);
  

// Now start intercomparing ice grids ///////////////////////////////////

   // Take a look at the flags and their changes:
   flags(nyesterday, ntoday, nland);

  // now, apply range bounding to skip the flagging:
  for (x.j = 0; x.j < ntoday.ypoints() ; x.j++) {
  for (x.i = 0; x.i < ntoday.xpoints() ; x.i++) {
    // Mask out land
    if (nland[x] > 99) {
      ntoday[x] = land_flag;
      nyesterday[x] = land_flag;
    }
    if (ntoday[x] > 100 && ntoday[x] <= 128) ntoday[x] = 100;
    if (ntoday[x] > 128) ntoday[x] = land_flag;
    if (ntoday[x] < 15 ) ntoday[x] = 0;
    if (nyesterday[x] > 100 && nyesterday[x] <= 128) nyesterday[x] = 100;
    if (nyesterday[x] > 128) nyesterday[x] = land_flag;
    if (nyesterday[x] < 15 ) nyesterday[x] = 0;
  }
  }
  // will want to do something about the points that one has an analysis 
  //   and the other doesn't


// Bulk Statistics:
  printf("max, min new  %5.1f %5.1f old %5.1f %5.1f\n", 
          (float) ntoday.gridmax(land_flag), (float) ntoday.gridmin(land_flag), 
          (float) nyesterday.gridmax(land_flag), (float) nyesterday.gridmin(land_flag) );
  printf("ice area   million km^2  new %7.3f old %7.3f\n",
           ntoday.integrate(land_flag) / 1.e12 / 100., 
           nyesterday.integrate(land_flag) / 1.e12 / 100.);
  printf("ice extent million km^2  new %7.3f old %7.3f\n",
           ice_extent(ntoday, land_flag) / 1.e12 , 
           ice_extent(nyesterday, land_flag) / 1.e12);

// Verbose intercomparisons -- kml out file:
  kml(ntoday, nyesterday, kmlout);
  fclose(kmlout);
  #ifdef VERBOSE
    latpt ll;
    for (x.j = 0; x.j < ntoday.ypoints() ; x.j++) {
    for (x.i = 0; x.i < ntoday.xpoints() ; x.i++) {
      if (ntoday[x] != nyesterday[x]) {
        ll = ntoday.locate(x);
        printf("differ %4d %4d  %7.3f %7.3f  %3d %3d  %4d\n",x.i, x.j,
              ll.lat, ll.lon, ntoday[x], nyesterday[x], ntoday[x] - nyesterday[x]);
      }
    }
    }
  #endif

  // delta grids:
  ndelta.set((float) 0.0);

  for (x.j = 0; x.j < ntoday.ypoints() ; x.j++) {
  for (x.i = 0; x.i < ntoday.xpoints() ; x.i++) {
     if (ntoday[x] < 128. && nyesterday[x] < 128. ) {
       ndelta[x] = ntoday[x] - nyesterday[x];
     }
     if (ntoday[x] > 128)     { nland[x] = land_flag; }
     if (nyesterday[x] > 128) { nland[x] = land_flag; }
  }
  }
  printf("ndelta.gridmax %5.1f %5.1f\n", (float) ndelta.gridmax(), 
                                         (float) ndelta.gridmin()  );
  printf("delta mean rms %f %f\n", ndelta.average(), ndelta.rms() );
  float var = (ndelta.rms()*ndelta.rms() - ndelta.average()*ndelta.average() );
  printf("delta var %f sd %f\n",var, sqrt(var));
  printf("delta area %9.3f (k km^2)\n",ndelta.integrate() / 1.e9/100.);

  // Set up palette:
  hpal.set_color(0, 255, 255, 255); // at zero, we've got bad/no data or land
  int itmp;
  itmp = hpal.ncol / 2;
  for (i = 1; i < hpal.ncol; i++) {
    if (i < itmp) { // less ice today than yesterday - reds
      hpal.set_color(i, 255 - 255 *(itmp - i)/(itmp + i), 0, 0);
    }
    else if ( i == itmp ) {
      hpal.set_color(i, 0, 0, 0);
    }
    else {
      // more ice, blue
      hpal.set_color(i, 0, 0, 255 - 255 *(i - itmp)/(itmp ) );
    }
  }
  printf("color division = %d\n",itmp);

  // distribute colors
  for (x.j = 0; x.j < ntoday.ypoints() ; x.j++) {
  for (x.i = 0; x.i < ntoday.xpoints() ; x.i++) {
     if (ndelta[x] == 0)  { // do nothing
         ndelta[x] = 0;
     }
     else if (ndelta[x] < 0) { // assign colors in 1-(itmp-1) 
         tmp = (float) ndelta[x] * (float) (itmp - 2) / 100.0;
         ndelta[x] = -tmp;
     }
     else {
         tmp = (float) ndelta[x] * (float) (itmp - 2) / 100.0;
         ndelta[x] = tmp;
     }

     if (nland[x] > 128)    ndelta[x] = itmp;
  }
  }
  ndelta.xpm(argv[4], 1, hpal);

  // if working on statistical analysis with respect to neighbors:
  //forstats(ntoday, nyesterday, nland);

  return 0;

}
template<class T>
void showall(metricgrid<T> &x, metricgrid<T> &y, metricgrid<unsigned char> &nland) {
  ijpt loc;
  latpt ll;
  for (loc.j = 0; loc.j < nland.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < nland.xpoints(); loc.i++) {
    ll = nland.locate(loc);
    printf("%4d %4d  %6.2f %7.2f  %3d %3d %3d  %4d\n",loc.i, loc.j,
        ll.lat, ll.lon, (int) x[loc], (int) y[loc], nland[loc], (int)( y[loc] - x[loc]));
  }
  }

  return ;
}

template<class T>
void forstats(metricgrid<T> &ntoday, metricgrid<T> &nyesterday, metricgrid<unsigned char> &nland) {
  ijpt xip1, xim1, xjp1, xjm1;
  ijpt x;
// yesterday/today statistical prediction by neighbors
  for (x.j = 0; x.j < ntoday.ypoints() ; x.j++) {
  for (x.i = 0; x.i < ntoday.xpoints() ; x.i++) {
    xjp1 = x; xjp1.j += 1;
    xjm1 = x; xjm1.j -= 1;
    xip1 = x; xip1.i += 1;
    xim1 = x; xim1.i -= 1;
    if ( ntoday[x] < 128 &&
         nyesterday[x] < 128 &&
         nyesterday[xip1] < 128 && 
         nyesterday[xim1] < 128 && 
         nyesterday[xjp1] < 128 && 
         nyesterday[xjm1] < 128 &&
         nland[x] < 100 &&
        (nyesterday[x] + nyesterday[xip1] + nyesterday[xim1] + nyesterday[xjp1] + nyesterday[xjm1]) > 0 ) {
     printf("%3.0f  %3.0f %3.0f %3.0f %3.0f %3.0f\n",(float) ntoday[x], 
         (float) nyesterday[x], (float) nyesterday[xip1], (float) nyesterday[xim1], 
         (float) nyesterday[xjp1], (float) nyesterday[xjm1]  );
    }
  }
  }

  return ;
}        
double ice_extent(GRIDTYPE<DATTYPE> &x, float flag) {
  GRIDTYPE<float> tmp;
  ijpt loc;
  float minconc = 0;
  for (loc.j = 0; loc.j < x.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < x.xpoints(); loc.i++) {
    if (x[loc] > minconc && x[loc] <= 100. && x[loc] != flag) {
      tmp[loc] = 1.0;
    }
    else {
      tmp[loc] = 0.0;
    }
  }
  }
  return tmp.integrate();
}
void flags(GRIDTYPE<DATTYPE> &ref, GRIDTYPE<DATTYPE> &newer, GRIDTYPE<unsigned char> &land) {
  GRIDTYPE<float> delta;
  ijpt loc;
  int weather = 0, nodata = 0, baddata = 0, total = 0;
  DATTYPE cutoff = 1.;

  for (loc.j = 0; loc.j < ref.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < ref.xpoints(); loc.i++) {
   delta[loc]  = (float) ref[loc];
   delta[loc] -= (float) newer[loc];
   if ((newer[loc] == (DATTYPE) NO_DATA && ref[loc] != (DATTYPE) NO_DATA) ||
       (newer[loc] != (DATTYPE) NO_DATA && ref[loc] == (DATTYPE) NO_DATA)) {
     nodata += 1;
     delta[loc] = 0;
   }
   if ((newer[loc] == (DATTYPE) WEATHER && ref[loc] != (DATTYPE) WEATHER )||
       (newer[loc] != (DATTYPE) WEATHER && ref[loc] == (DATTYPE) WEATHER)) {
     weather += 1;
     delta[loc] = 0;
   }
   if ((newer[loc] == (DATTYPE) BAD_DATA && ref[loc] != (DATTYPE) BAD_DATA) ||
       (newer[loc] != (DATTYPE) BAD_DATA && ref[loc] == (DATTYPE) BAD_DATA)) {
     baddata += 1;
     delta[loc] = 0;
     }
     if (land[loc] > (unsigned int) 128) {
       delta[loc] = 0;
     }
     if (fabs(delta[loc]) >= cutoff) {
       total += 1;
     }
  }
  }
  printf("delta max min avg rms = %f %f %f %f\n",delta.gridmax(), delta.gridmin(), delta.average(), delta.rms() );
  printf("total number of changes %6d\n",total);
  printf("      number of no-data %6d\n",nodata);
  printf("      number of weather %6d\n",weather);
  printf("      number of baddata %6d\n",baddata);

  return;
} 
void kml(GRIDTYPE<DATTYPE> &x, GRIDTYPE<DATTYPE> &y, FILE *fout) {
  ijpt loc;
  latpt ll;
  float lat, lon;
  DATTYPE delta_toler = 20;

// Header information:
  fprintf(fout, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  fprintf(fout, "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n");
  fprintf(fout, "<Document>\n");

  fprintf(fout, "<Folder>\n");
  fprintf(fout, "  <name>ice analysis differences over %d </name>\n", delta_toler);
  fprintf(fout, "  <LookAt>\n");
  fprintf(fout, "   <longitude>-85</longitude>\n");
  fprintf(fout, "    <latitude>45</latitude>\n");
  fprintf(fout, "    <range>6400000</range>\n");
  fprintf(fout, "  </LookAt>\n");

// point by point comparisons:
  for (loc.j = 0; loc.j < x.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < x.xpoints(); loc.i++) {
    if (fabs((float)x[loc] - (float)y[loc]) >= delta_toler ) {
      ll = x.locate(loc);
      lat = ll.lat;
      lon = ll.lon;
      if ( lon > 180 ) {
        lon -= 360.;
      }
      fprintf(fout, "<Placemark>\n");
      fprintf(fout, " <name> ");
      fprintf(fout, "%3d %3d %4d ",x[loc], y[loc], x[loc] - y[loc]);
      fprintf(fout, " </name>\n");
      fprintf(fout, "  <Point>");
      fprintf(fout, "  <coordinates>%8.3f, %8.3f, 0</coordinates>",lon, lat);
      fprintf(fout, "  </Point>\n");
      fprintf(fout, "</Placemark>\n");
    }
  }
  }

//Now close off the folder, document:
  fprintf(fout, "  </Folder>\n");

  fprintf(fout, "</Document>\n");
  fprintf(fout, "</kml>\n");

  return ;
}
