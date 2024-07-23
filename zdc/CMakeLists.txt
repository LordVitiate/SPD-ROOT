
#
# Create a library called "SpdZdc” 
#

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/spddata
${CMAKE_SOURCE_DIR}/spddata/params
${CMAKE_SOURCE_DIR}/spddata/mcdata
${CMAKE_SOURCE_DIR}/spddata/hits/zdc
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/common/geometry
${CMAKE_SOURCE_DIR}/common/region
${CMAKE_SOURCE_DIR}/spdgeometry
${CMAKE_SOURCE_DIR}/spdgeometry/caps
${CMAKE_SOURCE_DIR}/zdc
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

SpdZdcContFact.cxx

SpdZdc.cxx
SpdZdcParSet.cxx
SpdZdcPoint.cxx
#SpdZdcHitProducer.cxx
)

set(HEADERS

SpdZdcContFact.h

SpdZdc.h
SpdZdcParSet.h
SpdZdcPoint.h
#SpdZdcHitProducer.h
)

Set(LINKDEF SpdZdcLinkDef.h)
Set(LIBRARY_NAME SpdZdc)
Set(DEPENDENCIES Base SpdData SpdCommon SpdGeometry SpdField)

GENERATE_LIBRARY()
