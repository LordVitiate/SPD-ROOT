
#
# Create a library called "SpdTst” 
#

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/spddata
${CMAKE_SOURCE_DIR}/spddata/params
${CMAKE_SOURCE_DIR}/spddata/mcdata
${CMAKE_SOURCE_DIR}/spddata/hits/vnt
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/common/geometry
${CMAKE_SOURCE_DIR}/common/region
${CMAKE_SOURCE_DIR}/spdgeometry
${CMAKE_SOURCE_DIR}/spdgeometry/tst
${CMAKE_SOURCE_DIR}/tst
${CMAKE_SOURCE_DIR}/tst/barrel
${CMAKE_SOURCE_DIR}/tst/ecps
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

SpdTsTContFact.cxx

barrel/SpdTsTB.cxx
barrel/SpdTsTBParSet.cxx
barrel/SpdTsTBPoint.cxx
barrel/SpdTsTBHitProducer.cxx

ecps/SpdTsTEC.cxx
ecps/SpdTsTECParSet.cxx
ecps/SpdTsTECPoint.cxx
ecps/SpdTsTECHitProducer.cxx

)

Set(LINKDEF SpdTstLinkDef.h)
Set(LIBRARY_NAME SpdTst)
Set(DEPENDENCIES Base SpdData SpdCommon SpdGeometry SpdField)

GENERATE_LIBRARY()
