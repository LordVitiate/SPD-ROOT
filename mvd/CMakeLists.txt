
#
# Create a library called "SpdMvd” 
#

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/spddata
${CMAKE_SOURCE_DIR}/spddata/params
${CMAKE_SOURCE_DIR}/spddata/mcdata
${CMAKE_SOURCE_DIR}/spddata/hits
${CMAKE_SOURCE_DIR}/spddata/hits/vnt
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/common/geometry
${CMAKE_SOURCE_DIR}/spdgeometry
${CMAKE_SOURCE_DIR}/spdgeometry/its
${CMAKE_SOURCE_DIR}/mvd
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

SpdMvdContFact.cxx

SpdMvd.cxx
SpdMvdParSet.cxx
SpdMvdPoint.cxx
#SpdMvdHitProducer.cxx
)

set(HEADERS
SpdMvdContFact.h
SpdMvd.h
SpdMvdParSet.h
SpdMvdPoint.h
#SpdMvdHitProducer.h
)

Set(LINKDEF SpdMvdLinkDef.h)
Set(LIBRARY_NAME SpdMvd)
Set(DEPENDENCIES Base SpdData SpdCommon SpdGeometry SpdField)

GENERATE_LIBRARY()
