      PROGRAM ssmi_netcdfout
!$$$  MAIN PROGRAM DOCUMENTATION BLOCK
! 
! MAIN PROGRAM: SSMIBUFR    SELECTIVE BUFR -> BINARY TRANSLATION
!   PRGMMR: GRUMBINE         ORG: NP21        DATE: 97-09-05  
!
! ABSTRACT: READ IN BUFR MESSAGES, SELECT THOSE MESSAGES WHICH
!    CORRESPOND TO A SATELLITE OF INTEREST, AND WRITE THEM OUT
!    AS UNFORMATTED BINARY FOR LATER USE.
!
! PROGRAM HISTORY LOG:
!   97-08-15  Robert Grumbine     ORIGINAL AUTHOR
!   98-07-21  Robert Grumbine  Y2K and F90 fixes
!   99-12-28  Robert Grumbine  Transfer to IBM-SP, requires changes to type
!                                sizes imposed by BUFRLIB
! 2000-08-15  Robert Grumbine  High resolution ssmi channels added
! 2001-05-24  Robert Grumbine  Changes for running on LINUX as well
! 2014-10-10  Robert Grumbine  F90-friendliness: removing goto,
! un-numbered do loops
! 2021-01-11  Robert Grumbine  Write out netcdf files, including ice concentration
!
! USAGE:
!   INPUT FILES:
!     FTNF14 - BUFR SOURCE DATA TANK
!   OUTPUT FILES:
!     FTNF51 - UNFORMATTED DATA FROM DESIRED SATELLITE(S)
!
! SUBPROGRAMS CALLED:
!    UNIQUE: NONE
!    BUFRLIB: OPENBF, UFBINT, UFBREP AND THEIR DESCENDANTS
!
! EXIT STATES:
!    COND = 0 - SUCCESSFUL RUN
!
! ATTRIBUTES:
!   LANGUAGE: F-77, F-90
!   MACHINE: Any (?)
!
!$$$ 
!     Program core from Vera Gerald 3/25/97.  
!     Read in bufr files and write out unformatted binary for
!       use by other programs
!     Robert Grumbine 4 June 1997.

      IMPLICIT none

      CHARACTER(80)  XIDST,XLCST,BRTST,XL85ST, BR85ST
      CHARACTER(8) inout,subset
      REAL     XIDENT(9),xloc(4,64),BRT(2,7,64),temps(7)
      REAL(8)   XIDENT8(9), xloc8(4,64), BRT8(2,7,64), temps8(7)
      REAL     xloc85(4, 192), brt85(2, 2, 192)
      REAL(8)   xloc858(4, 192), brt858(2, 2, 192)
      INTEGER index, index2, index3
      INTEGER iy, id, im, imins, isad, iunt, jcnt, jret, iret
      INTEGER kret, k2ret, kwrit, k
      INTEGER kscan, posn, sftg
      REAL tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7
      REAL xlat, xlon 
      INTEGER isec, iscan, ihr, idate, icnt
      INTEGER  ireadsb, ireadmg
      INTEGER creturns, openout, shortout, longout, hiresout
!-----------------------------------------------------------------------
!
!  THE LIST OF SSMI DATA AS CONVERTED AND WRITTEN TO BUFR FOLLOWS:
!
!     ARRAY            MNEMONIC   DESCRIPTOR   DESCRIPTION
!     -----            --------   ----------   -----------
!
!     XIDENT(1)        SAID       001007       SATELLITE ID
!     XIDENT(2)        YEAR       004001       YEAR
!     XIDENT(3)        MNTH       004002       MONTH
!     XIDENT(4)        DAYS       004003       DAY
!     XIDENT(5)        HOUR       004004       HOUR
!     XIDENT(6)        MINU       004005       MINUTE
!     XIDENT(7)        SECO       004006       SECOND
!     XIDENT(8)        ORBN       005040       ORBIT NUMBER
!     XIDENT(9)        SCNN       005201       SCAN NUMBER
!
!     xloc(1,1:64)     CLAT       005002       LATITUDE
!     xloc(2,1:64)     CLON       006002       LONGITUDE
!     xloc(3,1:64)     SFTG       020217       SURFACE TAG
!     xloc(4,1:64)     POSN       005202       POSITION NUMBER
!
!     BRT(1,1:7,1:64)  CHNM       005042       CHANNEL NUMBER
!     BRT(2,1:7,1:64)  TMBR       012063       RADIANCE TEMPERATURE
!
!     xloc85(1,1:192)  CLAT85     005255       Lat
!     xloc85(2,1:192)  CLON85     005255       Lon
!     xloc85(3,1:192)  SFTG85     005255       Surface Tag
!     xloc85(4,1:192)  POSN85     005255       Position Number
!
!     brt85(1,1:2,1:192) CHNM85   005254       Channel number
!     brt85(2,1:2,1:192) TMBR85   012255       Radiance Temperature
!     
!
!-----------------------------------------------------------------------
!
      REAL hiresobs(4,3)
      INTEGER i, j, ount, count
      INTEGER F11, F13, F14, F15, SATNO
      PARAMETER (F11 = 244)
      PARAMETER (F13 = 246)
      PARAMETER (F14 = 247)
      PARAMETER (F15 = 248)
      DATA XIDST,XLCST,BRTST,XL85ST, BR85ST
     1/'SAID YEAR MNTH DAYS HOUR MINU SECO ORBN SCNN               ',
     2 'CLAT CLON SFTG POSN                                        ',
     3 'CHNM TMBR                                                  ',
     4 'CLAT85 CLON85 SFTG85 POSN85                                ',
     5 'CHNM85 TMBR85                                              '/
!c  where IUNT is the FORTRAN I/O UNIT NUMBER for the tank of
!c  SSM/I brightness data.
!------------- For netcdf -----------------------------
      INTEGER open_nc, ssmiout_nc
      INTEGER nwrite, ncid, platform, varid(15)
!-----------------------------------------------------------------------

      icnt = 0
      jcnt = 0
      iunt = 14
      ount = 51
      inout = 'IN'
      DO j = 1, 3 
      DO i = 1, 4
        hiresobs(i,j) = 0.0
      ENDDO
      ENDDO

      count = 0
      OPEN (iunt, FORM="UNFORMATTED", STATUS="OLD")
!old      OPEN (ount, FORM="UNFORMATTED", STATUS="NEW")
      CALL openbf(iunt,inout,iunt)

      platform = 248
      creturns = open_nc(ount, ncid, platform, varid)
      PRINT *,'open_nc returned = ',creturns
      PRINT *,'ncid = ',ncid
      PRINT *,'varid = ',varid


      DO WHILE (ireadmg(iunt,subset,idate).eq.0) 
         !CD PRINT *,'starting inner do while '
         DO WHILE (ireadsb(iunt).eq.0)

           count = count + 1
           !CD PRINT *,'scan line count ',count

           CALL UFBINT(IUNT,XIDENT8,9,  1,iret,XIDST)
           DO index = 1, 9
             !CD PRINT *,'first index = ',index
             XIDENT(index) = XIDENT8(index)
           ENDDO
           IF (XIDENT(1) .GT. F15 .OR. XIDENT(1) .LT. F11 ) CYCLE
           SATNO = XIDENT(1)
           CALL UFBINT(IUNT,xloc8  ,4, 64,iret,XLCST)
           DO index = 1, 4
             !CD PRINT *,'first index 1,4 = ',index
           DO index2 = 1, 64
             !CD PRINT *,'second index 1,64 = ',index2
             xloc(index, index2) = xloc8(index, index2)
           ENDDO
           ENDDO
           CALL UFBREP(IUNT,BRT8  ,2,448,jret,BRTST)
           DO index = 1, 2
           DO index2 = 1, 7
           DO index3 = 1, 64
             !CD PRINT *,'3fold loop b: ',index, index2, index3
             BRT(index, index2, index3)= BRT8(index, index2, index3)
           ENDDO
           ENDDO
           ENDDO
           CALL UFBINT(IUNT, xloc858, 4, 192, kret, XL85ST)
           DO index = 1, 4
           DO index2 = 1, 192
             !CD PRINT *,'2fold loop a: ',index, index2, index3
             xloc85(index, index2) = xloc858(index, index2)
           ENDDO
           ENDDO
           CALL UFBREP(IUNT, brt858, 2, 384, k2ret, BR85ST)
           DO index = 1, 2
           DO index2 = 1, 2
           DO index3 = 1, 192
             !CD PRINT *,'3fold loop 2: ',index, index2, index3
             brt85(index, index2, index3) =
     1            brt858(index, index2, index3)
           ENDDO
           ENDDO
           ENDDO

           icnt = icnt + 1
           IF (jret .GT. 1) THEN
             jcnt = jcnt + 1 

             iy = XIDENT(2)
             im = XIDENT(3)
             id = XIDENT(4)
             ihr = XIDENT(5)
             imins = XIDENT(6)
             isad = XIDENT(1)
             iscan = XIDENT(9)
             isec = XIDENT(7)
! work here for NC version RG:
!old             creturns = shortout(SATNO, iy,im,id,ihr,imins,isec,iscan)

            DO kscan = 1,iret
              !CD PRINT *,'kscan = ',kscan
              xlat = xloc(1,kscan)
              xlon = xloc(2,kscan)
              sftg = xloc(3,kscan)
              posn = xloc(4,kscan)
              tmp1 = brt(2,1,kscan)
              tmp2 = brt(2,2,kscan)
              tmp3 = brt(2,3,kscan)
              tmp4 = brt(2,4,kscan)
              tmp5 = brt(2,5,kscan)
              tmp6 = brt(2,6,kscan)
              tmp7 = brt(2,7,kscan)
              !CD PRINT *,'scan pt ',satno, kscan, xlat, xlon, tmp1, tmp7
! work here for NC version RG:
!old              creturns = longout(kscan,xlat,xlon,sftg,posn,
!old     1               tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7)

              creturns = ssmiout_nc(
     1          SATNO, iy,im,id,ihr,imins,isec,iscan, 
     2          kscan,xlat,xlon,sftg,posn, 
     3          tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7, 
     4          ncid, varid, nwrite)
              !CD PRINT *,'creturns = ',creturns 

! Skip hires as it team1 doesn't use high frequencies
!!             Now write out the hires info:
!              DO k = 1, 3
!                kwrit = (kscan-1)*3 + k
!                xlat = xloc85(1, kwrit)
!                xlon = xloc85(2, kwrit)
!                sftg = xloc85(3, kwrit)
!                posn = xloc85(4, kwrit)
!                tmp6 = brt85(2, 1, kwrit)
!                tmp7 = brt85(2, 2, kwrit)
!! work here for NC version RG:
!              creturns = ssmiout_nc(
!     1          SATNO, iy,im,id,ihr,imins,isec,iscan, 
!     2          kscan,xlat,xlon,sftg,posn, 
!     3          tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7, 
!     4          ncid, varid, nwrite)
!              !CD PRINT *,'creturns = ',creturns 
!
!old                creturns = hiresout(kwrit, xlat, xlon, sftg, posn, 
!old     1                              tmp6, tmp7)
!              ENDDO
            ENDDO

         ENDIF
       
         ENDDO
       ENDDO

      CALL close_nc(ncid)

      STOP
      END
