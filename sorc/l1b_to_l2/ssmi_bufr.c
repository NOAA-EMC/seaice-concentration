#include <stdio.h>

#include "ssmi2.h"

/* For error qc 16 March 2004 */
int err_19_polar ;
int err_37_polar ;
int err_85_polar ;
int err_lat      ;
int err_lon      ;

/* Returns number of errors found */

int process_bufr(ssmipt *b) {
/* Perform some bounds checking for the long data */
/* Order of checking is related to the frequency with which
   a given field is erroneous */
/* Note that typically if one number is wrong, so are several others */
/* Robert Grumbine 10 February 1994 */
/* Process the bufr data records, which will eventually include the short 
    data as well. 
   Only processing is to check for qc purposes.
*/

  int nerr = 0;
/* tb tests are in the new 'filter' that also comments about land types */

/* Polarization tests: */ 
    if ( b->tb[SSMI_T19H] > b->tb[SSMI_T19V]) {
      nerr += 1;
      err_19_polar += 1;
    }
    if ( b->tb[SSMI_T37H] > b->tb[SSMI_T37V]) {
      nerr += 1;
      err_37_polar += 1;
    }
    if ( b->tb[SSMI_T85H] > b->tb[SSMI_T85V]) {
      nerr += 1;
      err_85_polar += 1;
    }

  
/* Location and surface type tests: */
    if ( b->clat > 180.) {
       nerr += 1;
       err_lat += 1;
    }
    if ( b->clon > 360.) {
       nerr += 1;
       err_lon += 1;
    }

    if (nerr != 0) {
      /* printf("19polar, 37polar, 85 polar, lat, lon %d %d %d %d %d\n", 
          err_19_polar , err_37_polar , err_85_polar , err_lat, err_lon);
      */
      zero_bufr(b);
    }

  
  return nerr;
  
}

void zero_bufr(ssmipt *b) {

   b->clat     = 0;
   b->clon     = 0;
   b->tb[SSMI_T19V]         = 0;
   b->tb[SSMI_T19H]         = 0;
   b->tb[SSMI_T22V]         = 0;
   b->tb[SSMI_T37V]         = 0;
   b->tb[SSMI_T37H]         = 0;
   b->tb[SSMI_T85V]         = 0;
   b->tb[SSMI_T85H]         = 0;

  return;
}
