      PROGRAM psgrib
!$$$  MAIN PROGRAM DOCUMENTATION BLOCK
!                .      .    .                                       .
! MAIN PROGRAM: PSGNORTH     ENGRIB POLAR STEREOGRAPHIC DATA
!   PRGMMR: GRUMBINE         ORG: NP21        DATE: 2000-02-07
!
! ABSTRACT: READ IN 1 BYTE VALUES FROM A GRID SPECIFIED BY THE
!    ICEGRID.INC FILE AND WRITE THEM OUT AS A GRIB FIELD.
!
! PROGRAM HISTORY LOG:
!    97-06-24 ROBERT GRUMBINE
!    98-01-28 Robert Grumbine  -- Add generation of WMO bulletin
!    98-07-21 Robert Grumbine  -- Y2K and F90
!    98-11-18 Robert Grumbine    Drop W3FQ02 in favor of utcdat
!  1999-09-02 Robert Grumbine    IBM SP Conversion: change WRYTE to bacio
!  2006-01-23 Boi    Vuong       REPLACED THE ROUTINE WMOOUT WITH MAKWMO
!                                AND MKFLDSEP TO ADD FIELD SEPARATOR FOR TOC
!
! USAGE:
!    INPUT FILES:
!     FTNF06 - STANDARD INPUT - DATE FLAG FOR GRIB
!     FTNF11 - CONCENTRATION GRID TO ENGRIB
!    OUTPUT FILES:
!     FTNF51 - GRIBBED CONCENTRATION FILE
!     FTNF52 - WMO-Encoded file
!
!  SUBPROGRAMS CALLED:
!    UNIQUE: GRIBIT, MAKWMO , MKFLDSEP
!    LIBRARY: 
!      W3LIB - WRYTE, W3FI72, GTBITS, utcdat, W3FI92, W3AI19
!      BACIO
!
!  EXIT STATES:
!    COND = 0 - SUCCESSFUL RUN
!
!  REMARKS:
!
!  ATTRIBUTES:
!    LANGUAGE: FORTRAN 77
!    MACHINE: Any
!
!$$$
!     Engrib character data from polar stereographic grids.  
!     Robert Grumbine 4 June 1997.  

      IMPLICIT none
!     Includes for bacio
      INCLUDE "locale.inc"
      INCLUDE "clib.inc"

!     Variables for Gribbing
      INCLUDE "icegrid.inc"
      REAL map(LP, MP)
      LOGICAL lbm(LP, MP)
      REAL pi, xlat1, xlon1, xlat2, xlon2
      INTEGER griblen, LENHEAD
      PARAMETER (griblen = (100 + 28 + LP*MP*(8+1)) / 8 )
      CHARACTER conc(LP, MP), grib( griblen )
      PARAMETER (LENHEAD=21)

      INTEGER lgrib, ierr, iret
      INTEGER IOPT,INSIZE,LENOUT
      
!     Variables for WMO encoding
      INTEGER wmounit
      CHARACTER(1)  CSEP(80)
      CHARACTER(1) HEADER(21)

      CHARACTER(6) BULHEAD
      CHARACTER(4) KW
      PARAMETER (wmounit = 52 )
      PARAMETER (KW      = "KWBM")
      PARAMETER (BULHEAD = "OENA88")

!     Local Utility variables
      INTEGER i, j, cen, yy, mmm, dd
      INTEGER fdes, newpos, nactual, start, bacio
      CHARACTER(7) fname
      INTEGER MOVA2I

      CALL W3TAGB('PSGNORTH',2006,0023,0054,'NP21')

      READ (*,*) cen
      READ (*,*) yy
      READ (*,*) mmm
      READ (*,*) dd
      yy = yy + 100 * cen

      IOPT   = 2
      INSIZE = 19
      iret = 0
      newpos = 0
      start = 0
      i = LP*MP
      WRITE (fname,9001) 
 9001 FORMAT ("fort.11")

      ierr = bacio(BAOPEN_RONLY + BAREAD + BACLOSE, start, newpos, 
     1              SIZEOF_CHARACTER, i, nactual, fdes, fname, conc) 
      IF (ierr .NE. 0) THEN
        PRINT *,'bacio ierr = ',ierr
        STOP "error from bacio read "
      ENDIF

      DO 1000 j = 1, MP
        DO 1100 i = 1, LP
          map(i,j) = FLOAT(MOVA2I(conc(i,j))) / 100.
 1100   CONTINUE
 1000 CONTINUE

      CALL mapxy(xorig, yorig, xlat1, xlon1, 
     1            slat, slon, sgn, eccen2, rearth)
      CALL mapxy(xorig+L*dx, yorig+M*dy, xlat2, xlon2, 
     1            slat, slon, sgn, eccen2, rearth)

      pi = ABS(ACOS(-1.))
      xlat1 = xlat1*pi/180.
      xlon1 = xlon1*pi/180.
      xlat2 = xlat2*pi/180.
      xlon2 = xlon2*pi/180.

      CALL gribit(map, lbm, 5, LP, MP, 8, 0.0, 
     1            28, 1, 7, 120, 0, 91, 102, 0, 0, 
     2            yy, mmm, dd, 0, 1, 0, 0, 10, 0, 0, 2, 
     3     xlat1, xlon1, xlat2, xlon2, dx, dy, -90.-slon, sgn, gridno,
     4     grib, lgrib, ierr) 

  
      IF (ierr .EQ. 0) THEN
        WRITE (fname, 9002) 
 9002   FORMAT("fort.51")
        ierr = bacio(BAOPEN_WONLY + BAWRITE + BACLOSE, start, newpos, 
     1                SIZEOF_CHARACTER, lgrib, nactual, fdes, fname, 
     2                grib) 
        IF (WMO) THEN
          WRITE (fname, 9020)
 9020     FORMAT("fort.52")
          CALL BAOPENW(WMOUNIT,FNAME,IRET)
          IF ( IRET .NE. 0 ) THEN
            WRITE(6,FMT='("ERROR OPENING OUTPUT GRIB FILE:",A8)') FNAME
            WRITE(6,FMT='(" BAOPENW ERROR = ",I5)') IRET
            STOP 20
          ENDIF
!
!       MAKE FLAG FIELD SEPARATOR BLOCK
!
          CALL MKFLDSEP(CSEP,IOPT,INSIZE,LGRIB+LENHEAD,LENOUT)
!
!         MAKE WMO HEADER
!
          CALL MAKWMO (BULHEAD,DD,0,KW,HEADER)
!
!       WRITE OUT SEPARATOR BLOCK, ABBREVIATED WMO HEADING,
!
          CALL WRYTE(WMOUNIT,LENOUT,CSEP)
          CALL WRYTE(WMOUNIT,LENHEAD,HEADER)
          CALL WRYTE(WMOUNIT,LGRIB,GRIB)

        ENDIF
       ELSE
        PRINT *,'Error ',ierr,' constructing grib message in psgrib'
      ENDIF
     
      CALL W3TAGE('PSGNORTH')

      STOP
      END
