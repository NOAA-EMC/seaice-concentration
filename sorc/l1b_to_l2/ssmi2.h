/* Additional elements for l1b to l2 */

/* define field numbers */
  #define SSMI_T19V 0
  #define SSMI_T19H 1
  #define SSMI_T22V 2
  #define SSMI_T37V 3
  #define SSMI_T37H 4
  #define SSMI_T85V 5
  #define SSMI_T85H 6
  #define SSMI_CONC_BAR 7
  #define SSMI_BAR_CONC 8
  #define SSMI_COUNT    9
  #define SSMI_HIRES_CONC 10
  #define SSMI_WEATHER_COUNT 11
  #define SSMI_OLD_CONC 12

typedef struct {
  short int year;
  unsigned char month, day, hour, minute, second;
  short int satid;
  double clat, clon;
  double tb[7];
} ssmipt;

extern void zero_bufr(ssmipt *b);
extern int process_bufr(ssmipt *b);
