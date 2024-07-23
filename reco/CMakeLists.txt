
# 
# Create a library called "SpdReco" 
# 
IF(GENFIT2_IS_DEFINED) 

set(INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${ROOT_INCLUDE_DIR} 
${ROOT_INCLUDE_DIR}/base/sim
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/reco
${CMAKE_SOURCE_DIR}/reco/vnt
${CMAKE_SOURCE_DIR}/reco/tof
${CMAKE_SOURCE_DIR}/reco/ecal
${CMAKE_SOURCE_DIR}/reco/rs
${CMAKE_SOURCE_DIR}/reco/bbc
${CMAKE_SOURCE_DIR}/reco/aeg
${CMAKE_SOURCE_DIR}/reco/zdc
${CMAKE_SOURCE_DIR}/reco/ideal
${CMAKE_SOURCE_DIR}/reco/tools
${CMAKE_SOURCE_DIR}/spddata/
${CMAKE_SOURCE_DIR}/spddata/mcdata
${CMAKE_SOURCE_DIR}/spddata/params
${CMAKE_SOURCE_DIR}/spddata/hits
${CMAKE_SOURCE_DIR}/spddata/hits/vnt
${CMAKE_SOURCE_DIR}/spddata/hits/tof
${CMAKE_SOURCE_DIR}/spddata/hits/ecal
${CMAKE_SOURCE_DIR}/spddata/hits/rs
${CMAKE_SOURCE_DIR}/spddata/hits/bbc
${CMAKE_SOURCE_DIR}/spddata/hits/aeg
${CMAKE_SOURCE_DIR}/spddata/hits/zdc
${CMAKE_SOURCE_DIR}/spddata/reco
${CMAKE_SOURCE_DIR}/spddata/reco/vnt
${CMAKE_SOURCE_DIR}/spddata/reco/tof
${CMAKE_SOURCE_DIR}/spddata/reco/ecal
${CMAKE_SOURCE_DIR}/spddata/reco/rs
${CMAKE_SOURCE_DIR}/spddata/reco/zdc
${CMAKE_SOURCE_DIR}/spddata/reco/bbc
${CMAKE_SOURCE_DIR}/spddata/reco/aeg
${CMAKE_SOURCE_DIR}/spddata/reco/kfdata
${CMAKE_SOURCE_DIR}/spddata/IdealTrackData
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/common/geometry
${CMAKE_SOURCE_DIR}/common/region
${CMAKE_SOURCE_DIR}/spdgeometry
${CMAKE_SOURCE_DIR}/spdgeometry/its
${CMAKE_SOURCE_DIR}/spdgeometry/tst
${CMAKE_SOURCE_DIR}/spdgeometry/tof
${CMAKE_SOURCE_DIR}/spdgeometry/ecalt
${CMAKE_SOURCE_DIR}/spdgeometry/rst
${CMAKE_SOURCE_DIR}/spdgeometry/caps
${CMAKE_SOURCE_DIR}/external/Eigen3
${CMAKE_SOURCE_DIR}/external/GenFit2
${CMAKE_SOURCE_DIR}/external/GenFit2/core/include
${CMAKE_SOURCE_DIR}/external/GenFit2/eventDisplay/include
${CMAKE_SOURCE_DIR}/external/GenFit2/fields/include
${CMAKE_SOURCE_DIR}/external/GenFit2/finitePlanes/include
${CMAKE_SOURCE_DIR}/external/GenFit2/fitters/include
${CMAKE_SOURCE_DIR}/external/GenFit2/measurements/include
${CMAKE_SOURCE_DIR}/external/GenFit2/trackReps/include
${CMAKE_SOURCE_DIR}/external/GenFit2/utilities/include
${CMAKE_SOURCE_DIR}/external/KFParticle/KFParticle
${CMAKE_SOURCE_DIR}/external/KFParticle/KFParticlePerformance
${CMAKE_SOURCE_DIR}/proc/tracking
${CMAKE_SOURCE_DIR}/proc/vertex
${CMAKE_SOURCE_DIR}/its
${CMAKE_SOURCE_DIR}/mvd
${CMAKE_SOURCE_DIR}/tst
${CMAKE_SOURCE_DIR}/tof
${CMAKE_SOURCE_DIR}/ecalt
${CMAKE_SOURCE_DIR}/rst
${CMAKE_SOURCE_DIR}/bbc
${CMAKE_SOURCE_DIR}/aeg
${CMAKE_SOURCE_DIR}/zdc
${CMAKE_SOURCE_DIR}/tst/barrel
${CMAKE_SOURCE_DIR}/tst/ecps
${CMAKE_SOURCE_DIR}/tof/barrel
${CMAKE_SOURCE_DIR}/tof/ecps
${CMAKE_SOURCE_DIR}/ecalt/barrel
${CMAKE_SOURCE_DIR}/ecalt/ecps
${CMAKE_SOURCE_DIR}/rst/barrel
${CMAKE_SOURCE_DIR}/rst/ecps
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS

SpdMCEventHelper.cxx
SpdMCEventMaker.cxx

tools/SpdMCDataIterator.cxx
tools/SpdMCDataReaderSimple.cxx

vnt/SpdMvdMCHitProducer.cxx
vnt/SpdItsMCHitProducer.cxx
vnt/SpdTsMCHitProducer.cxx
vnt/SpdMCTracksFinder.cxx
vnt/SpdMCVerticesFitter.cxx
vnt/SpdRCVerticesFinder.cxx
vnt/SpdRCKFpartV0Finder.cxx
vnt/SpdTrackFitterGF.cxx
vnt/SpdTrackPropagatorGF.cxx
vnt/SpdVertexCombiFinder.cxx
vnt/SpdMCTsParticleProducer.cxx
vnt/SpdRCTrackSeedsFinder.cxx
vnt/SpdRCTracksFinder.cxx
vnt/SpdRCTrackFitterGF.cxx

tof/SpdTofMCHitProducer.cxx
tof/SpdMCTofParticleProducer.cxx

ecal/SpdEcalCommon.cxx
ecal/SpdEcalMCHitProducer.cxx
ecal/SpdEcalRCMaker.cxx
ecal/SpdEcalClusterMCInfoMaker.cxx
ecal/SpdEcalPiGammaSeparator.cxx

rs/SpdRsMCHitProducer.cxx
rs/SpdRsMCClusterMaker.cxx

bbc/SpdBbcMCHitProducer.cxx
aeg/SpdAegMCHitProducer.cxx
aeg/SpdMCAegParticleProducer.cxx

zdc/SpdZdcMCHitProducer.cxx

ideal/SpdIdealTrackFinder.cxx
ideal/SpdIdealKalmanFitter.cxx
ideal/SpdIdealV0Finder.cxx

)

set(HEADERS

SpdMCEventHelper.h
SpdMCEventMaker.h

tools/SpdMCDataIterator.h
tools/SpdMCDataReaderSimple.h

vnt/SpdMvdMCHitProducer.h
vnt/SpdItsMCHitProducer.h
vnt/SpdTsMCHitProducer.h
vnt/SpdMCTracksFinder.h
vnt/SpdMCVerticesFitter.h
vnt/SpdRCVerticesFinder.h
vnt/SpdRCKFpartV0Finder.h
vnt/SpdTrackFitterGF.h
vnt/SpdTrackPropagatorGF.h
vnt/SpdVertexCombiFinder.h
vnt/SpdMCTsParticleProducer.h
vnt/SpdRCTrackSeedsFinder.h
vnt/SpdRCTracksFinder.h
vnt/SpdRCTrackFitterGF.h

tof/SpdTofMCHitProducer.h
tof/SpdMCTofParticleProducer.h

ecal/SpdEcalCommon.h
ecal/SpdEcalMCHitProducer.h
ecal/SpdEcalRCMaker.h
ecal/SpdEcalClusterMCInfoMaker.h
ecal/SpdEcalPiGammaSeparator.h
ecal/SpdEcalPiGammaSeparatorParameters.h

rs/SpdRsMCHitProducer.h
rs/SpdRsMCClusterMaker.h

bbc/SpdBbcMCHitProducer.h

aeg/SpdAegMCHitProducer.h
aeg/SpdMCAegParticleProducer.h


zdc/SpdZdcMCHitProducer.h

ideal/SpdIdealTrackFinder.h
ideal/SpdIdealKalmanFitter.h
ideal/SpdIdealV0Finder.h

)

ADD_DEFINITIONS(-DHomogeneousField)
#ADD_DEFINITIONS(-DNonhomogeneousField)

Set(HEADERS )
set(LINKDEF SpdRecoLinkDef.h)
set(LIBRARY_NAME SpdReco)

set(DEPENDENCIES Base SpdCommon SpdData SpdGeometry TrackingGF GenFit2 KFParticle SpdMvd SpdIts SpdTst SpdTof SpdZdc SpdBbc SpdAeg)

GENERATE_LIBRARY()

ENDIF()
