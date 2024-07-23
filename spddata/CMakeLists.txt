
#
# Create a library called "SpdData" 
#

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${SIMPATH}/include/vmc
${CMAKE_SOURCE_DIR}/spddata
${CMAKE_SOURCE_DIR}/spddata/params
${CMAKE_SOURCE_DIR}/spddata/mcdata
${CMAKE_SOURCE_DIR}/spddata/hits
${CMAKE_SOURCE_DIR}/spddata/hits/rs
${CMAKE_SOURCE_DIR}/spddata/hits/ecal
${CMAKE_SOURCE_DIR}/spddata/hits/tof
${CMAKE_SOURCE_DIR}/spddata/hits/vnt
${CMAKE_SOURCE_DIR}/spddata/hits/bbc
${CMAKE_SOURCE_DIR}/spddata/hits/aeg
${CMAKE_SOURCE_DIR}/spddata/hits/zdc
${CMAKE_SOURCE_DIR}/spddata/reco
${CMAKE_SOURCE_DIR}/spddata/reco/rs
${CMAKE_SOURCE_DIR}/spddata/reco/ecal
${CMAKE_SOURCE_DIR}/spddata/reco/tof
${CMAKE_SOURCE_DIR}/spddata/reco/vnt
${CMAKE_SOURCE_DIR}/spddata/reco/bbc
${CMAKE_SOURCE_DIR}/spddata/reco/aeg
${CMAKE_SOURCE_DIR}/spddata/reco/zdc
${CMAKE_SOURCE_DIR}/spddata/reco/kfdata
${CMAKE_SOURCE_DIR}/spddata/IdealTrackData
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS

hits/SpdHit.cxx
hits/SpdHitMCTruth.cxx

hits/vnt/SpdMCTrackHit.cxx
hits/vnt/SpdMCSiliconHit.cxx
hits/vnt/SpdMCStrawHit1D.cxx
hits/vnt/SpdMCStrawHit2D.cxx

hits/vnt/SpdTsTBHit.cxx
hits/vnt/SpdTsTECHit.cxx

hits/tof/SpdTofMCHit.cxx

hits/ecal/SpdEcalTBHit.cxx
hits/ecal/SpdEcalTECHit.cxx
hits/ecal/SpdEcalTB2Hit.cxx
hits/ecal/SpdEcalTEC2Hit.cxx
hits/ecal/SpdEcalMCHit.cxx

hits/rs/SpdRsMCHit.cxx
hits/rs/SpdRsTID.cxx
hits/rs/SpdRsTBHit.cxx
hits/rs/SpdRsTB2Hit.cxx
hits/rs/SpdRsTECHit.cxx
hits/rs/SpdRsTEC2Hit.cxx

hits/bbc/SpdBbcMCHit.cxx
hits/aeg/SpdAegMCHit.cxx
hits/zdc/SpdZdcMCHit.cxx

mcdata/SpdMCEventHeader.cxx 
mcdata/SpdMCTrack.cxx
mcdata/SpdPrimGenData.cxx

params/SpdParameter.cxx 
params/SpdFieldPar.cxx
params/SpdPassiveGeoParSet.cxx
params/SpdBaseParSet.cxx
params/SpdParSet.cxx
params/SpdPrimGenParSet.cxx
params/SpdParSetContFact.cxx
params/SpdSetParSet.cxx

IdealTrackData/SpdIdealDataInfo.cxx
IdealTrackData/SpdIdealHit.cxx
IdealTrackData/SpdIdealSpacepointHit.cxx
IdealTrackData/SpdIdealWirepointHit.cxx
IdealTrackData/SpdIdealTrack.cxx
IdealTrackData/SpdIdealTrackColl.cxx

reco/SpdMCEvent.cxx
reco/SpdMCParticle.cxx

reco/vnt/SpdVertexMC.cxx
reco/vnt/SpdTrackMC.cxx
reco/vnt/SpdTrackState.cxx
reco/vnt/SpdTrackState2.cxx
reco/vnt/SpdTrackFitPar.cxx
reco/vnt/SpdVertexFitPar.cxx
reco/vnt/SpdTrackRCSeed.cxx
reco/vnt/SpdTrackRCSeedPar.cxx
reco/vnt/SpdTrackRC.cxx
reco/vnt/SpdVertexRC.cxx
reco/vnt/SpdTsParticle.cxx

reco/tof/SpdTofParticle.cxx

reco/aeg/SpdAegParticle.cxx


reco/ecal/SpdEcalTB2Cluster.cxx
reco/ecal/SpdEcalTB2RecoParticle.cxx
reco/ecal/SpdEcalTEC2Cluster.cxx
reco/ecal/SpdEcalTEC2RecoParticle.cxx
reco/ecal/SpdEcalRCCluster.cxx
reco/ecal/SpdEcalRCParticle.cxx
reco/ecal/SpdEcalClusterMCInfo.cxx
reco/ecal/SpdEcalMCParticle.cxx

reco/rs/SpdRsMCCluster.cxx
reco/rs/SpdRsMCParticle.cxx

reco/kfdata/SpdKFSimpleRes.cxx

RWdata.cxx
SpdGeopathParser.cxx

)

Set(HEADERS 

hits/SpdHit.h
hits/SpdHitMCTruth.h

hits/vnt/SpdMCTrackHit.h
hits/vnt/SpdMCSiliconHit.h
hits/vnt/SpdMCStrawHit1D.h
hits/vnt/SpdMCStrawHit2D.h

hits/vnt/SpdTsTBHit.h
hits/vnt/SpdTsTECHit.h

hits/tof/SpdTofMCHit.h

hits/ecal/SpdEcalTBHit.h
hits/ecal/SpdEcalTECHit.h
hits/ecal/SpdEcalTB2Hit.h
hits/ecal/SpdEcalTEC2Hit.h
hits/ecal/SpdEcalMCHit.h

hits/rs/SpdRsMCHit.h
hits/rs/SpdRsTID.h
hits/rs/SpdRsTBHit.h
hits/rs/SpdRsTB2Hit.h
hits/rs/SpdRsTECHit.h
hits/rs/SpdRsTEC2Hit.h

hits/bbc/SpdBbcMCHit.h
hits/aeg/SpdAegMCHit.h
hits/zdc/SpdZdcMCHit.h

mcdata/SpdMCEventHeader.h
mcdata/SpdMCTrack.h
mcdata/SpdPrimGenData.h

params/SpdBaseParSet.h
params/SpdFieldPar.h
params/SpdParameter.h
params/SpdParSetContFact.h
params/SpdParSet.h
params/SpdPassiveGeoParSet.h
params/SpdPrimGenParSet.h
params/SpdSetParSet.h

IdealTrackData/SpdIdealDataInfo.h
IdealTrackData/SpdIdealHit.h
IdealTrackData/SpdIdealSpacepointHit.h
IdealTrackData/SpdIdealTrackColl.h
IdealTrackData/SpdIdealTrack.h
IdealTrackData/SpdIdealWirepointHit.h

reco/SpdMCEvent.h
reco/SpdMCParticle.h

reco/vnt/SpdVertexMC.h
reco/vnt/SpdTrackMC.h
reco/vnt/SpdTrackState.h
reco/vnt/SpdTrackState2.h
reco/vnt/SpdTrackFitPar.h
reco/vnt/SpdVertexFitPar.h
reco/vnt/SpdTrackRCSeed.h
reco/vnt/SpdTrackRCSeedPar.h
reco/vnt/SpdTrackRC.h
reco/vnt/SpdVertexRC.h
reco/vnt/SpdTsParticle.h

reco/tof/SpdTofParticle.h

reco/aeg/SpdAegParticle.h

reco/ecal/SpdEcalTB2Cluster.h
reco/ecal/SpdEcalTB2RecoParticle.h
reco/ecal/SpdEcalTEC2Cluster.h
reco/ecal/SpdEcalTEC2RecoParticle.h
reco/ecal/SpdEcalRCCluster.h
reco/ecal/SpdEcalRCParticle.h
reco/ecal/SpdEcalClusterMCInfo.h
reco/ecal/SpdEcalMCParticle.h

reco/rs/SpdRsMCCluster.h
reco/rs/SpdRsMCParticle.h

reco/kfdata/SpdKFSimpleRes.h

RWdata.h
SpdGeopathParser.h

SpdDetectorList.h
)

Set(LINKDEF SpdDataLinkDef.h)
Set(LIBRARY_NAME SpdData)
Set(DEPENDENCIES Base)

GENERATE_LIBRARY()



