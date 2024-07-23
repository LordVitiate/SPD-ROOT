
#
# Create a library called "SpdEcals” 
#

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/spddata
${CMAKE_SOURCE_DIR}/spddata/params
${CMAKE_SOURCE_DIR}/spddata/mcdata
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/common/geometry
${CMAKE_SOURCE_DIR}/common/region
${CMAKE_SOURCE_DIR}/spdgeometry
${CMAKE_SOURCE_DIR}/spdgeometry/sol
${CMAKE_SOURCE_DIR}/sol/ecals
${CMAKE_SOURCE_DIR}/sol/ecals/barrel
${CMAKE_SOURCE_DIR}/sol/ecals/ecps
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

SpdEcalSContFact.cxx
SpdEcalSPointAn.cxx

barrel/SpdEcalSB.cxx
barrel/SpdEcalSBParSet.cxx
barrel/SpdEcalSBPoint.cxx
barrel/SpdEcalSBHit.cxx
barrel/SpdEcalSBHitProducer.cxx

ecps/SpdEcalSEC.cxx
ecps/SpdEcalSECParSet.cxx
ecps/SpdEcalSECPoint.cxx
ecps/SpdEcalSECHit.cxx
ecps/SpdEcalSECHitProducer.cxx

)

set(HEADERS
barrel/SpdEcalSBGeoPar.h
barrel/SpdEcalSB.h
barrel/SpdEcalSBHit.h
barrel/SpdEcalSBHitProducer.h
barrel/SpdEcalSBParSet.h
barrel/SpdEcalSBPoint.h
ecps/SpdEcalSECGeoPar.h
ecps/SpdEcalSEC.h
ecps/SpdEcalSECHit.h
ecps/SpdEcalSECHitProducer.h
ecps/SpdEcalSECParSet.h
ecps/SpdEcalSECPoint.h
SpdEcalSContFact.h
SpdEcalSPointAn.h
)

Set(LINKDEF SpdEcalsLinkDef.h)
Set(LIBRARY_NAME SpdEcals)
Set(DEPENDENCIES Base SpdData SpdCommon SpdField)

GENERATE_LIBRARY()
