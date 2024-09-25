#! /usr/bin/env bash

# Load our modules:
module use "${HOMEseaice_analysis}/modulefiles"

case "${MACHINE_ID}" in
  "wcoss2" | "hera" | "orion" | "hercules" | "gaea" | "jet" | "s4" | "noaacloud")
    module load "module_base.${MACHINE_ID}"
    ;;
  *)
    echo "WARNING: UNKNOWN PLATFORM"
    ;;
esac

module list

