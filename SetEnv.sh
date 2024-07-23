#!/bin/bash

export SIMPATH=/cvmfs/nica.jinr.ru/spd/centos7/fairsoft/jun19p1 
export FAIRROOTPATH=/cvmfs/nica.jinr.ru/spd/centos7/fairroot/v18.2.0

#export SIMPATH=/cvmfs/nica.jinr.ru/spd/centos7/fairsoft/nov20
#export FAIRROOTPATH=/cvmfs/nica.jinr.ru/spd/centos7/fairroot/v18.4.2

export PATH="${SIMPATH}/bin:$PATH"
export LD_LIBRARY_PATH="${SIMPATH}/lib:$LD_LIBRARY_PATH"

#export GEOMPATH="${VMCWORKDIR}/geometry"
#export MAGFPATH="${VMCWORKDIR}/input"

#echo actual path to geometry and materials: $GEOMPATH
#echo actual path to magnetic field maps: $MAGFPATH
