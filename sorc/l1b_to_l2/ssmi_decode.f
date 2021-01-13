      PROGRAM ssmi
! ---------------------------------------------------------
!
!
! Program to decode the ssmi tank
! .. and to write out the netcdf L2 information
! .. including sea ice concentration

      IMPLICIT none
      CHARACTER(80)  XIDST,XLCST,BRTST,XL85ST, BR85ST
      CHARACTER(8) inout,subset
      REAL     XIDENT(9),xloc(4,64),BRT(2,7,64),temps(7)
      REAL*8   XIDENT8(9), xloc8(4,64), BRT8(2,7,64), temps8(7)
      REAL     xloc85(4, 192), brt85(2, 2, 192)
      REAL*8   xloc858(4, 192), brt858(2, 2, 192)
      INTEGER index, index2, index3
      INTEGER iy, id, im, imins, isad, iunt, jcnt, jret, iret
      INTEGER kret, k2ret, kwrit, k
      INTEGER kscan, posn, sftg
      REAL tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7
      REAL xlat, xlon
      INTEGER isec, iscan, ihr, idate, icnt
      INTEGER  ireadsb, ireadmg
      INTEGER creturns, openout, shortout, longout, hiresout

! For ssmi, DMSP F11 to 15
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


!ssmis      REAL bmiss
!ssmis      PARAMETER (bmiss=10.0E6)
!ssmis      CHARACTER(8) SUBSET
!ssmis      REAL*8 hdr(9),ident(3), ssmischn(4,24)
!ssmis
!ssmis      INTEGER ichan, jj, ii, isiid, ntank, lubfi
!ssmis      INTEGER iorbn, islnm, isaid, iret, JT, IDATE
!ssmis      INTEGER mhr, mins, secs, mda, myr, mmo
!ssmis      INTEGER icount, err, freq
!ssmis      REAL xlat, xlon
      INTEGER ntank
!ssmis
!------------- For netcdf -----------------------------
      INTEGER outunit, textout
      INTEGER open_nc
!RG: 13 will change for ssmi
      INTEGER nwrite, ncid, platform, varid(13)
!------------------------------------------------------

      CALL W3TAGB('SSMI_Decode',0095,0333,0077,'NP11   ')

      CALL DATELEN(10)

      lubfi  = 11
      icount = 0
      ntank  = 1

      freq = 10 * 1000 !Text output frequency
      textout = 52

      outunit  = 51
!      platform = 285
!platform = 246,7,8
      platform = 248 !F15
      nwrite   = 0
      creturns = open_nc(outunit, ncid, platform, varid)
      PRINT *,'open_nc returned = ',creturns
      PRINT *,'ncid = ',ncid
      PRINT *,'varid = ',varid


      DO JT = 1, ntank
!-----------------------------------------------------------------------
!                       DECODE BUFR FILE
!-----------------------------------------------------------------------
!
!  OPEN THE BUFR INPUT FILE
!  ------------------------
!
      CALL OPENBF(lubfi,'IN',lubfi)
      CALL READMG(lubfi,SUBSET,IDATE,IRET)
      IF(IRET.NE.0) GOTO 100
      PRINT*,'READING DATA FOR ',IDATE

!  READ A SUBSET - READ A MESSAGE WHEN NO SUBSETS - END WHEN NO MESSAGES
!  ---------------------------------------------------------------------
  150 CONTINUE
      CALL READSB(lubfi,IRET)
!          print*,' readsb iret ',iret
      IF(IRET.NE.0) THEN
         CALL READMG(lubfi,SUBSET,IDATE,IRET)
!          print*,' readms iret ',iret
         IF(IRET.NE.0) GOTO 100
         GOTO 150
      ENDIF

!  -----------------------------------------------------------------
!  CALL UFBINT, UFBREP TO GET THE DATA
!     MNEMONICS are embedded in CALLS
!  -----------------------------------------------

      call ufbint(lubfi,hdr,9,1,iret,
     *            'SAID YEAR MNTH DAYS HOUR MINU SECO CLAT CLON')
!     WRITE(6,*) 'IRET= ',IRET
      call ufbint(lubfi,ident,3,1,iret,'SAID ORBN SLNM')

      isaid = NINT(ident(1))
      iorbn = NINT(ident(2))
      islnm = NINT(ident(3))

      isiid = NINT(hdr(1))
      myr   = NINT(hdr(2))
      mmo   = NINT(hdr(3))
      mda   = NINT(hdr(4))
      mhr   = NINT(hdr(5))
      mins  = NINT(hdr(6))
      secs  = NINT(hdr(7))
      xlat  = hdr(8)
      xlon  = hdr(9)

!    "SSMISCHN"24  =  24 btmps
!    each one is "CHNM  TMBR  WTCA   CTCA "
!      TCA are target calibrations, warm and cold

      call ufbseq(lubfi,ssmischn,4,24,iret,'SSMISCHN')

               icount = icount + 1

      if(mod(icount,freq) .eq. 0) then

        PRINT *,'nwrite = ',nwrite
        write(textout,50)isiid,myr,mmo,mda,mhr,mins,secs, xlat,
     *    xlon,isaid,islnm,iorbn
   50   format(1x,i3,1x,i4,1x,5i2,1x,2f12.5,1x,2i6,1x,i8)

        do ii = 1,4
         do jj = 1,24
          if(ssmischn(ii,jj).ge.bmiss)ssmischn(ii,jj)= -999.99
         enddo
        enddo

        do jj = 1, 24
          if(ssmischn(1,jj).le.-999.99) then
              ichan = -999
            else
             ichan = NINT(ssmischn(1,jj))
          endif 
          write(textout,502)ichan,ssmischn(2,jj)
        enddo
  502   format('chan # ',i4,' tmbr ',f8.2)

      endif

! Output in binary all steps' info:
      !CALL ssmisout(hdr, ident, ssmischn)
      IF (isaid .EQ. platform) THEN
        CALL ssmisout_nc(hdr, ident, ssmischn, ncid, varid, nwrite)
      ENDIF

!
!     get next report
!
      GO TO 150
  100 CONTINUE

         call   closbf(lubfi)

          if(icount.lt.1) then
           WRITE(6,*) 'READMG ERROR W/ BUFR MSG:IRET=',IRET
           err = 23
!            call exit(23)
            STOP
          endif

        WRITE(6,*) 'AFTER WOOLENS INTERFACE:  icount= ',icount

      WRITE(6,617)
  617 FORMAT(1x,'  TIMES AND TOTAL NUMBER OF btmps OVER THE AREA')

      ENDDO

!.................
      CALL close_nc(ncid)
      PRINT *,'nwrite = ',nwrite

      WRITE(6,692)icount
  692 FORMAT(1x,'END OF JOB','count= ',i8)

        CALL W3TAGE('DCODNET3')

      END
