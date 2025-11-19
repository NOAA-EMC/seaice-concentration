// skel, filter, ssmisu_algorithm.c
typedef int bool;

#define OCEAN      0
#define LAND     157
#define BAD_DATA 166
#define WEATHER  177
#define COAST    195
#define MIXED    195
#define NO_DATA  224

#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); fflush(stdout); }

#define false (1==0)
#define true (1==1)

#define MAX_CONC 127
#define MAX_ICE  128
#define MIN_CONC  15

#define NDIMS 1
#define NFREQS 8
#define SSMIS_ANTENNA 0
#define SSMIS_GR22LIM 0.045
#define SSMIS_GR37LIM 0.05
#define SSMIS_T150H 0
#define SSMIS_T19H 1
#define SSMIS_T19V 2
#define SSMIS_T22V 3
#define SSMIS_T37H 4
#define SSMIS_T37V 5
#define SSMIS_T92H 7
#define SSMIS_T92V 6
