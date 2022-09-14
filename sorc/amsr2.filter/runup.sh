yy=2003
for mm in 01 02 03 04 05 06 07 08 09 10 11 12
do
  ym=${yy}${mm}
  tag=${ym}01

  ln -sf /Volumes/Data/dcom.ssmi85/ssmi85.$tag        fort.14 
  ln -sf /Volumes/Data/qdoi/$ym/avhrr-only-v2.$tag.nc avhrr-only.nc
  time ./ssmi_tol2 > out.$tag
  time python3 version3.py > outp.$tag
  mv round1 round1.$tag
  mv l2out.f248.51.nc l2out.ssmi.${tag}.nc
done

