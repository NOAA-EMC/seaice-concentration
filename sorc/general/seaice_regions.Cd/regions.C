#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
using namespace std;

#include "ncepgrids.h"  
#include "params.h"

#define MAXSTR 9000

//Jan 30 1998
//Generalized 10 November 2003 for both hemispheres and taking data and map files
//  as arguments
//Robert Grumbine

int bound(int *x, int imin, int imax) ;
int inparse(fijpt *center, int *range, int *mag, GRIDTYPE <unsigned char> b, 
            FILE *cgilog); 

int main(int argc, char *argv[]) {
  GRIDTYPE<unsigned char> b, nland;
  psgrid<unsigned char> d, dland;
  grid2<unsigned char>  e, eland;
  palette<unsigned char> gg(19,65);
  FILE *nin, *cgilog;
  int i, j, range;
  int x1, y1, x2, y2, mag;
  ijpt ij1, ij2;
  float lat, lon, frange;
  latpt ll;
  fijpt center;
  char *len[5];
//  char *sublen[5];
  char *null, delim;

  mag = 0;
  printf("entered the program\n"); fflush(stdout);

  cgilog = fopen("cgilog","a");
  if (cgilog == (FILE *) NULL) {
    printf("failed to open the log file\n");
    return 1;
  }

  printf("about to enter the main branch\n"); fflush(stdout);
// Section to handle the arguments, read in the basic data, and
//   compute the derived constants.
{

  if (argc == 3) {
    fprintf(cgilog, "invoked with stdin as input \n");
    inparse(&center, &range, &mag, b, cgilog);
    fprintf(cgilog, "returned from inparse\n"); fflush(cgilog);
  }
  else if (argc == 4) {
    fprintf(cgilog, "invoked with a single argument, presumed to be an imagemap\n");
    frange = 550.;
    range = (int) (frange*1000. / b.deltax() + 0.5);
    bound(&range, 1, min(b.xpoints(), b.ypoints() )/3 );
    if (strlen(argv[1]) > MAXSTR/2) {
      printf("Error in input, your value is _far_ too long\n");
      return -1;
    }
    else {
      for (i = 0; i < 1; i++) {
         len[i]    = (char *) malloc(MAXSTR*sizeof(char) );
         //sublen[i] = (char *) malloc(MAXSTR*sizeof(char) );
      }
      null = NULL;
      delim = ',';
      len[0] = strtok(argv[1], &delim);
      len[1] = strtok(null, &delim);
      i = (int) atoi(len[0]);
      j = (int) atoi(len[1]);
      fprintf(cgilog,"Center point at %d %d\n", i, j); fflush(cgilog);
      // Note the following transforms because the upper left corner is
      //  1,1 in imagemaps
      center.i = i - 1;
      center.j = b.ypoints()  - j;
      if (center.i < 0 || center.i >= b.xpoints() ||
          center.j < 0 || center.j >= b.ypoints() ) {
        fprintf(cgilog,"Point out of range\n");
        return -2;
      }
    }  // End of imagemap case
  }

    
    else if (argc < 6) {
      fprintf(cgilog, "Error, insufficient arguments\n");
      return -1;
    }
    else {
      fprintf(cgilog,"Working with command line\n"); fflush(cgilog);
      i = strlen(argv[1]);
      if ( i > 12) { fprintf(cgilog,"Latitude too long\n"); return -1;}
      lat = atof(argv[1]);
      if (fabs(lat) > 90. || fabs(lat) < 30. ) {
        fprintf(cgilog, "Latitude out of range\n");
        return -3;
      }
    
      if (strlen(argv[2]) > 12) { 
         fprintf(cgilog,"Longitude too long\n"); 
         return -1;
      }
      lon = atof(argv[2]);
      if (fabs(lon) > 360.) {
        fprintf(cgilog, "Longitude out of range\n");
        return -4;
      }
    
      if (strlen(argv[3]) > 12) {
        fprintf(cgilog, "Range string too long\n"); 
        return -1;
      }
      frange = atof(argv[3]);
      range = (int) (frange*1000. / b.deltax() + 0.5);
      bound(&range, 1, min(b.xpoints(), b.ypoints() )/3 );
  
      fprintf(cgilog,"arg list = %f %f, %d \n", lat, lon, range);

    if (argc <= 6) {
      mag = 0;
    }
    else {
      mag = atoi(argv[4]);
      bound(&mag, 1, 1600);
    }

    ll.lat = lat;
    ll.lon = lon;
    center = b.locate(ll); 
    if (center.i < 0 || center.i >= b.xpoints() ||
        center.j < 0 || center.j >= b.ypoints() ) {
      printf("Point out of range\n");
      return -2;
    }
  }   // End of the >= 4 argument clause

  fprintf(cgilog, "done parsing arguments\n"); fflush(cgilog);
  nin = fopen(argv[argc-2], "r");
  if (nin != (FILE *) NULL ) {
    //b.read(b, nin);
    b.binin(nin);
    //printf("b size is %d %d\n",b.xpoints(), b.ypoints() ); fflush(stdout);
  }
  else {
    printf("Failed to open data file %s\n", argv[argc-2]);
    return 2; 
  }
  fclose(nin);


  x1 = (int) (0.5 + center.i - range);
  x2 = (int) (0.5 + center.i + range);
  y1 = (int) (0.5 + center.j - range);
  y2 = (int) (0.5 + center.j + range);
  bound(&x1, 0, b.xpoints() - 1);
  bound(&x2, 0, b.xpoints() - 1);
  bound(&y1, 0, b.ypoints() - 1);
  bound(&y2, 0, b.ypoints() - 1);
  //printf("mag currently = %d\n",mag); fflush(stdout);

  if (mag == 0) {
    mag = min(640 / (x2 - x1 + 1), 480 / (y2 - y1 + 1) );
    fprintf(cgilog,"Setting magnification to %d\n", mag);
    fflush(cgilog);
  }
  else {
    if (mag * (x2 - x1 + 1) > 1600 || mag * (y2 - y1 +1 ) > 1200) {
      mag = min(1600 / (x2 - x1 + 1) , 1200 / (y2 - y1 + 1) );
      fprintf(cgilog,"Resetting Selected magnification to %d\n", mag);
      fflush(cgilog);
    }
  }
} // end of arghand subsection

////////////////////////////////////////////////////////////////////////

  nin = fopen(argv[argc-1],"r");
  if (nin == NULL ) {
    printf("Failed to open the land file\n");
    return 3;
  }
  //nland.read(nland,nin);
  nland.binin(nin);
  //printf("nland size %d %d\n",nland.xpoints(), nland.ypoints() ); fflush(stdout);
  fclose(nin);


  fprintf(cgilog,"x1, y1, x2, y2 %3d %3d %3d %3d \n",x1, y1, x2, y2); 
  fflush(cgilog);
  ij1.i = x1; ij1.j = y1;
  ij2.i = x2; ij2.j = y2;
  //printf("about to try subsetting b\n"); fflush(stdout);
  // orig b.subset(d, ij1, ij2);
  d.subset(b, ij1, ij2);
  //printf("about to try subsetting nland\n"); fflush(stdout);
  // nland.subset(dland, ij1, ij2);
  dland.subset(nland, ij1, ij2);
  printf("d size is %d %d\n",d.xpoints(), d.ypoints() ); fflush(stdout);
  printf("b size is %d %d\n",b.xpoints(), b.ypoints() ); fflush(stdout);

  e     = d.magnify(mag);
  eland = dland.magnify(mag);
  printf("about to remap e, size is %d %d\n",e.xpoints(), e.ypoints() ); fflush(stdout);
  {
    ijpt loc;
    for (loc.j = 0; loc.j < e.ypoints() ; loc.j++) {
    for (loc.i = 0; loc.i < e.xpoints() ; loc.i++) {
       if (eland[loc] == LAND) {
         e[loc] = 0;
       }
       else if (eland[loc] == COAST) {
         e[loc] = 1;
       }
       else if (e[loc] == NO_DATA) {
         e[loc] = 2;
       }
       else if (e[loc] == WEATHER) {
         e[loc] = 3;
       }
       else {
         e[loc] = 4 + min(100,e[loc])/7; 
       }
    }
    }
  }
  printf("about to output the graphic e size is %d %d\n",e.xpoints(), e.ypoints() );
  char fname[5];
  sprintf(fname, "e.xpm");
  //e.xpm("e.xpm", 1, eland, gg);
  e.xpm(fname, 1, eland, gg);
////////////////////////////////////////////////////////////////////////

  return 0;
}

int bound(int *x, int imin, int imax) {
  if (imin > imax) { return -1; }
  if (*x < imin) *x = imin;
  if (*x > imax) *x = imax;
  return 0;
} 

int inparse(fijpt *center, int *range, int *mag, GRIDTYPE<unsigned char> b, FILE *cgilog) 
{
  char *len[5], *sublen[5];
  char stin[MAXSTR];
  char **endptr = NULL;
  char *null, delim;
  int i, j;
  latpt ll;
  float lat, lon, frange;
  fijpt tcen;
  
  fgets(&stin[0], MAXSTR-1, stdin);
  if (strlen(stin) > MAXSTR/2) {
    printf("string ridiculously long\n");
    return -1;
  }
    
  for (i = 0; i < 5; i++) {
     len[i] = (char *) malloc(MAXSTR*sizeof(char) );
     sublen[i] = (char *) malloc(MAXSTR*sizeof(char) );
  }

  null = NULL;
  delim='&';
  len[0] = strtok(&stin[0], &delim);
  if (len[0] != NULL) {
    for (i = 1; i < 5; i++) {
      len[i] = strtok(null, &delim);
      if (len[i] == NULL) break;
    }
  }
  for (j = 0; j < i; j++) {
  }


  delim = '=';

  i = strlen(len[0]);
  if ( i > 24) { printf("Lat string too long\n"); return -1;}
  sublen[0] = strtok(len[0], &delim);
  sublen[1] = strtok(null, &delim);
  lat = strtod(sublen[1], endptr);
  if (fabs(lat) > 90. ) {
    printf("Latitude out of range\n");
    return -3;
  }

  if (strlen(len[1]) > 24) { printf("Lon too long\n"); return -1;}
  sublen[0] = strtok(len[1], &delim);
  sublen[1] = strtok(null, &delim);
  lon = strtod(sublen[1], endptr);
  if (fabs(lon) > 360.) {
    printf("Longitude out of range\n");
    return -4;
  }

  if (strlen(len[2]) > 24) {printf("Range string too long\n"); return -1;}
  sublen[0] = strtok(len[2], &delim);
  sublen[1] = strtok(null, &delim);
  frange = strtod(sublen[1], endptr);
  *range = (int) (frange*1000. / b.deltax() + 0.5);
  bound(range, 1, min(b.xpoints(), b.ypoints() )/3 );

  if (strlen(len[3]) > 24) { printf("Mag too long\n"); return -1;}
  sublen[0] = strtok(len[3], &delim);
  sublen[1] = strtok(null, &delim);
  *mag = (int) strtol(sublen[1], endptr, 0);


  ll.lat = lat;
  ll.lon = lon;
  tcen = b.locate(ll); 
  if (tcen.i < 0 || tcen.i >= b.xpoints() ||
      tcen.j < 0 || tcen.j >= b.ypoints() ) {
    printf("Point out of range\n");
    return -2;
  }

  center->i = tcen.i;
  center->j = tcen.j;
  fprintf(cgilog,"Leaving inparse, center.i,j = %f %f\n", center->i, center->j);
  fflush(cgilog);

  return 0;
}
