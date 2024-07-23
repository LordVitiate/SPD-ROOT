
#
# Create a library called "SpdAeg” 
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
${CMAKE_SOURCE_DIR}/aeg
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

SpdAegContFact.cxx

SpdAeg.cxx
SpdAegParSet.cxx
SpdAegPoint.cxx
#SpdAegHitProducer.cxx
)

set(HEADERS

SpdAegContFact.h

SpdAeg.h
SpdAegParSet.h
SpdAegPoint.h
#SpdAegHitProducer.h
)

Set(LINKDEF SpdAegLinkDef.h)
Set(LIBRARY_NAME SpdAeg)
Set(DEPENDENCIES Base SpdData SpdCommon SpdGeometry SpdField)

GENERATE_LIBRARY()
