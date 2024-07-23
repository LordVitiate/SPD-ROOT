
#
# Create a library called "SpdIts” 
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
${CMAKE_SOURCE_DIR}/its
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

SpdItsContFact.cxx

SpdIts.cxx
SpdItsParSet.cxx
SpdItsPoint.cxx
#SpdItsHitProducer.cxx
)

set(HEADERS
SpdItsContFact.h
SpdIts.h
SpdItsParSet.h
SpdItsPoint.h
#SpdItsHitProducer.h
)

Set(LINKDEF SpdItsLinkDef.h)
Set(LIBRARY_NAME SpdIts)
Set(DEPENDENCIES Base SpdData SpdCommon SpdGeometry SpdField)

GENERATE_LIBRARY()
