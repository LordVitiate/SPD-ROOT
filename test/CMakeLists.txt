
# 
# Create a library called "SpdTest" 
# 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/spddata
${CMAKE_SOURCE_DIR}/spddata/params
${CMAKE_SOURCE_DIR}/spddata/mcdata
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/common/geometry
${CMAKE_SOURCE_DIR}/spdgeometry
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/test
${CMAKE_SOURCE_DIR}/test/barrel
${CMAKE_SOURCE_DIR}/test/simple
${CMAKE_SOURCE_DIR}/test/hmrs
${CMAKE_SOURCE_DIR}/test/sts
${CMAKE_SOURCE_DIR}/test/stt
${CMAKE_SOURCE_DIR}/test/ecalcell
${CMAKE_SOURCE_DIR}/test/tracker
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}

) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS

barrel/SpdBarrel.cxx
barrel/SpdBarrelGeoPar.cxx
barrel/SpdBarrelContFact.cxx

simple/TestDetector.cxx
simple/SpdSmplGeoRoot.cxx
#simple/SpdCaveGeoTest.cxx

hmrs/SpdHmrs.cxx
hmrs/SpdHmrsContFact.cxx
hmrs/SpdHmrsGeo.cxx
hmrs/SpdHmrsGeoPar.cxx
hmrs/SpdHmrsPoint.cxx

sts/SpdSts.cxx
sts/SpdStsContFact.cxx
sts/SpdStsGeo.cxx
sts/SpdStsGeoPar.cxx
sts/SpdStsPoint.cxx

stt/SpdStt.cxx
stt/SpdSttContFact.cxx
stt/SpdSttHitProducerIdeal.cxx
stt/SpdSttTrackFinder.cxx
#stt/SpdSttFindTracks.cxx
#stt/SpdSttTrackFinderIdeal.cxx
#stt/SpdSttTrackMatch.cxx
#stt/SpdSttMatchTracks.cxx
stt/SpdSttPoint.cxx  
stt/SpdSttHit.cxx  
stt/SpdSttHitInfo.cxx  
stt/SpdSttHelixHit.cxx
stt/SpdSttTrack.cxx
stt/SpdSttGeoPar.cxx
stt/SpdSttGeoMapper.cxx
stt/SpdSttStrawTube.cxx
stt/SpdSttStrawTubeA.cxx

ecalcell/EcalCell.cxx
ecalcell/EcalCellEventHeader.cxx
ecalcell/EcalCellHits.cxx
ecalcell/EcalCellPoint.cxx
ecalcell/EcalCellContFact.cxx

tracker/Tracker.cxx;
tracker/TrackerPoint.cxx;
tracker/TrackerContFact.cxx;

)

Set(LINKDEF SpdTestLinkDef.h)
Set(LIBRARY_NAME SpdTest)
Set(DEPENDENCIES Base SpdCommon SpdData SpdGeometry SpdField)

GENERATE_LIBRARY()



