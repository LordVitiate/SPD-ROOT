
#
# Create a library called "SpdTof” 
#

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/spddata
${CMAKE_SOURCE_DIR}/spddata/params
${CMAKE_SOURCE_DIR}/spddata/mcdata
${CMAKE_SOURCE_DIR}/spddata/hits/tof
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/common/geometry
${CMAKE_SOURCE_DIR}/common/region
${CMAKE_SOURCE_DIR}/spdgeometry
${CMAKE_SOURCE_DIR}/spdgeometry/tof
${CMAKE_SOURCE_DIR}/tof
${CMAKE_SOURCE_DIR}/tof/barrel
${CMAKE_SOURCE_DIR}/tof/ecps
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

SpdTofContFact.cxx

barrel/SpdTofB.cxx
barrel/SpdTofBParSet.cxx
barrel/SpdTofBPoint.cxx
#barrel/SpdTofBHitProducer.cxx

ecps/SpdTofEC.cxx
ecps/SpdTofECParSet.cxx
ecps/SpdTofECPoint.cxx
#ecps/SpdTofECHitProducer.cxx
)

set(HEADERS

SpdTofContFact.h

barrel/SpdTofB.h
barrel/SpdTofBParSet.h
barrel/SpdTofBPoint.h
#barrel/SpdTofBHitProducer.h

ecps/SpdTofEC.h
ecps/SpdTofECParSet.h
ecps/SpdTofECPoint.h
#ecps/SpdTofECHitProducer.h
)

Set(LINKDEF SpdTofLinkDef.h)
Set(LIBRARY_NAME SpdTof)
Set(DEPENDENCIES Base SpdData SpdCommon SpdGeometry SpdField)

GENERATE_LIBRARY()
