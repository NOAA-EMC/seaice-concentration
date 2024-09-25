#! /usr/bin/env bash
set -eux

readonly HOMEseaice_analysis=$(cd "$(dirname "$(readlink -f -n "${BASH_SOURCE[0]}" )" )/.." && pwd -P)

source "${HOMEseaice_analysis}/ush/detect_machine.sh"
echo "Machine: ${MACHINE_ID}"

RT_COMPILER="intel"
echo "Compiler: ${RT_COMPILER}"

#load modules
source "${HOMEseaice_analysis}/ush/module-setup.sh"
source ${HOMEseaice_analysis}/versions/build.${MACHINE_ID}.ver

set +x
source ${HOMEseaice_analysis}/ush/load_modules.sh
set -x

echo ${FC}
echo ${CXX}
# ---------------------------------------------

export MMAB_VER=""
export MMAB_BASE=${HOMEseaice_analysis}/sorc/mmablib/${MMAB_VER}
export MMAB_INC=$MMAB_BASE/include
export MMAB_SRC=$MMAB_BASE/sorc
export MMAB_LIBF4=$MMAB_BASE/libombf_4.a

if [ ! -d ${HOMEseaice_analysis}/sorc/mmablib ] ; then
	cd ${HOMEseaice_analysis}/sorc/
	git clone --recursive -b operations https://github.com/rgrumbine/mmablib
fi
if [ ! -f ${HOMEseaice_analysis}/sorc/mmablib/libombf_4.a ] ; then
	cd ${HOMEseaice_analysis}/sorc/mmablib
	make
	cd ..
fi
# ---------------------------------------------

exit 99

cd ${HOMEseaice_analysis}/sorc/

#for d in general amsr2 ssmis avhrr l1b_to_l2 l2_to_l3
for d in general
do
  cp ${HOMEseaice_analysis}/sorc/makeall.mk $d
  cd $d
  ./makeall.sh
  cd ..
done

if [ ! -d ../exec ] ; then
  mkdir ../exec
fi
./toexec cp

#clean up
rm */makeall.mk
