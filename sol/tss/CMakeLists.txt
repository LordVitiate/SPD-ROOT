
#
# Create a library called "SpdTss” 
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
${CMAKE_SOURCE_DIR}/sol/tss
${CMAKE_SOURCE_DIR}/sol/tss/barrel
${CMAKE_SOURCE_DIR}/sol/tss/ecps
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

SpdTsSContFact.cxx

barrel/SpdTsSB.cxx
barrel/SpdTsSBParSet.cxx
barrel/SpdTsSBPoint.cxx
barrel/SpdTsSBHit.cxx
barrel/SpdTsSBHitProducer.cxx

./ecps/SpdTsSEC.cxx
./ecps/SpdTsSECParSet.cxx
./ecps/SpdTsSECPoint.cxx
./ecps/SpdTsSECHit.cxx
./ecps/SpdTsSECHitProducer.cxx

)

Set(LINKDEF SpdTssLinkDef.h)
Set(LIBRARY_NAME SpdTss)
Set(DEPENDENCIES Base SpdData SpdCommon SpdGeometry SpdField)

GENERATE_LIBRARY()
