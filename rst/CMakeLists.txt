
#
# Create a library called "SpdRst” 
#

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/spddata
${CMAKE_SOURCE_DIR}/spddata/params
${CMAKE_SOURCE_DIR}/spddata/mcdata
${CMAKE_SOURCE_DIR}/spddata/hits
${CMAKE_SOURCE_DIR}/spddata/hits/rs
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/common/geometry
${CMAKE_SOURCE_DIR}/common/region
${CMAKE_SOURCE_DIR}/spdgeometry
${CMAKE_SOURCE_DIR}/spdgeometry/rst
${CMAKE_SOURCE_DIR}/rst
${CMAKE_SOURCE_DIR}/rst/barrel
${CMAKE_SOURCE_DIR}/rst/ecps
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

SpdRsTContFact.cxx

barrel/SpdRsTB.cxx
barrel/SpdRsTB2.cxx
barrel/SpdRsTB2woFEB.cxx
barrel/SpdRsTBParSet.cxx
barrel/SpdRsTBPoint.cxx
barrel/SpdRsTB2Point.cxx
barrel/SpdRsTBHitProducer.cxx
barrel/SpdRsTB2HitProducer.cxx

ecps/SpdRsTEC.cxx
ecps/SpdRsTEC2.cxx
ecps/SpdRsTEC2woFEB.cxx
ecps/SpdRsTECParSet.cxx
ecps/SpdRsTECPoint.cxx
ecps/SpdRsTEC2Point.cxx
ecps/SpdRsTECHitProducer.cxx
ecps/SpdRsTEC2HitProducer.cxx

)

Set(LINKDEF SpdRstLinkDef.h)
Set(LIBRARY_NAME SpdRst)
Set(DEPENDENCIES Base SpdData SpdCommon SpdGeometry SpdField)

GENERATE_LIBRARY()
