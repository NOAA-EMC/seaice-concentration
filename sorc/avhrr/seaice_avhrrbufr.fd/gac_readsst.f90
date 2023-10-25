PROGRAM bufr_readsst
!CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
!  Original program by Bert Katz X/X/2005
!  Modified 11 Dec 2008 by Robert Grumbine
!                                                                      C
!     IN THE SCRIPT THAT RUNS THIS PROGRAM, ONE MUST ISSUE THE         C
!     COMMANDS:                                                        C
!                                                                      C
!     export XLFRTEOPTS="unit_vars=yes"                                C
!     export XLFUNIT_11="NAME OF INPUT FILE (INCLUDING FULL PATH)"     C
!                                                                      C
!CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
      !RG IMPLICIT NONE

      CHARACTER(80)  IDST1,IDST2,IDCHN

      DATA IDST1  /'YEAR MNTH DAYS HOUR MINU SECO SAID CLATH CLONH'/
      DATA IDST2  /'SAZA SOZA FOVN CLAVR '/
      DATA IDCHN  /'INCN ALBD TMBR '/

      REAL(8) XDATA(18),XCHAN(3,5)
      DOUBLE PRECISION dst(18), chan(3,5)
      CHARACTER(8)  SUBSET
      INTEGER outunit, creturns
      INTEGER lat
      PARAMETER (lat = 8)

      DATA IUNT /11/

      outunit = 51
      creturns = openout(outunit)

      CALL OPENBF(IUNT,'IN',IUNT)

      CALL READMG(IUNT,SUBSET,IDATE,IERR)
      IF(IERR.NE.0) GO TO 8888

      IREC = 0

10    CONTINUE

      CALL READSB(IUNT,IERR)
      IF(IERR.NE.0) THEN
        CALL READMG(IUNT,SUBSET,IDATE,IERR)
        IF(IERR.NE.0) GO TO 8888
      ELSE
        CALL UFBINT(IUNT,XDATA( 1),9,1,IRET,IDST1)
        CALL UFBINT(IUNT,XDATA(10),4,1,IRET,IDST2)
        CALL UFBREP(IUNT,XCHAN    ,3,5,IRET,IDCHN)
        IREC = IREC + 1
        do lll = 1, 9
          dst(lll)   = XDATA(lll)
          dst(lll+9) = XDATA(lll+9)
        enddo
        chan = XCHAN

        IF (MOD(IREC,100000) .EQ. 0) THEN
          PRINT *,'irec = ',IREC/100000,'e5'
        ENDIF
!Since the concern is polar, trim off the low-latitude data
        IF (ABS(XDATA(lat)) .GT. 30) THEN
          CALL gacout(dst, chan)
        ENDIF

      ENDIF
      GO TO 10

!  I/O ERROR EXIT
!  --------------

 8888 CONTINUE
      PRINT *,'IERR = ',IERR
      WRITE(6,1001) IREC
 1001 FORMAT(' EOF ENCOUNTERED IN SST INPUT FILE AFTER RECORD NO. ',I8)
      STOP

!  NORMAL EXIT
!  -----------

END
