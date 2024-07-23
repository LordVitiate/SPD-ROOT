
#
# Create a library called "SpdEcalt” 
#

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/spddata
${CMAKE_SOURCE_DIR}/spddata/params
${CMAKE_SOURCE_DIR}/spddata/mcdata
${CMAKE_SOURCE_DIR}/spddata/hits
${CMAKE_SOURCE_DIR}/spddata/hits/ecal
${CMAKE_SOURCE_DIR}/spddata/reco/ecal
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/common/geometry
${CMAKE_SOURCE_DIR}/common/region
${CMAKE_SOURCE_DIR}/spdgeometry
${CMAKE_SOURCE_DIR}/spdgeometry/ecalt
${CMAKE_SOURCE_DIR}/ecalt
${CMAKE_SOURCE_DIR}/ecalt/barrel
${CMAKE_SOURCE_DIR}/ecalt/ecps
${CMAKE_SOURCE_DIR}/field
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}

) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS

SpdEcalTContFact.cxx
SpdEcalTPointAn.cxx

SpdEcalTX2HitProducerCommon.cxx

barrel/SpdEcalTBParSet.cxx

barrel/SpdEcalTB.cxx
barrel/SpdEcalTBPoint.cxx
barrel/SpdEcalTBHitProducer.cxx

barrel/SpdEcalTB2.cxx
barrel/SpdEcalTB2Point.cxx
barrel/SpdEcalTB2HitProducer.cxx
barrel/SpdEcalTB2Reconstruction.cxx

barrel/SpdEcalB.cxx
barrel/SpdEcalBPoint.cxx


ecps/SpdEcalTECParSet.cxx

ecps/SpdEcalTEC.cxx
ecps/SpdEcalTECPoint.cxx
ecps/SpdEcalTECHitProducer.cxx

ecps/SpdEcalTEC2.cxx
ecps/SpdEcalTEC2Point.cxx
ecps/SpdEcalTEC2HitProducer.cxx
ecps/SpdEcalTEC2Reconstruction.cxx

ecps/SpdEcalEC.cxx
ecps/SpdEcalECPoint.cxx

)

set(HEADERS

SpdEcalTX2HitProducerCommon.h

barrel/SpdEcalTBParSet.h

barrel/SpdEcalB.h
barrel/SpdEcalBPoint.h

barrel/SpdEcalTB2.h
barrel/SpdEcalTB2Point.h
barrel/SpdEcalTB2HitProducer.h
barrel/SpdEcalTB2Reconstruction.h

barrel/SpdEcalTB.h
barrel/SpdEcalTBPoint.h
barrel/SpdEcalTBHitProducer.h


ecps/SpdEcalTECParSet.h

ecps/SpdEcalEC.h
ecps/SpdEcalECPoint.h

ecps/SpdEcalTEC2.h
ecps/SpdEcalTEC2Point.h
ecps/SpdEcalTEC2HitProducer.h
ecps/SpdEcalTEC2Reconstruction.h

ecps/SpdEcalTEC.h
ecps/SpdEcalTECPoint.h
ecps/SpdEcalTECHitProducer.h

SpdEcalTContFact.h
SpdEcalTPointAn.h
)

Set(LINKDEF SpdEcaltLinkDef.h)
Set(LIBRARY_NAME SpdEcalt)
Set(DEPENDENCIES Base SpdData SpdCommon SpdGeometry SpdField)

GENERATE_LIBRARY()
