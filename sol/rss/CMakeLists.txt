
#
# Create a library called "SpdRss” 
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
${CMAKE_SOURCE_DIR}/sol/rss
${CMAKE_SOURCE_DIR}/sol/rss/barrel
${CMAKE_SOURCE_DIR}/sol/rss/ecps
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

SpdRsSContFact.cxx

barrel/SpdRsSB.cxx
barrel/SpdRsSBParSet.cxx
barrel/SpdRsSBPoint.cxx
barrel/SpdRsSBHit.cxx
barrel/SpdRsSBHitProducer.cxx

ecps/SpdRsSEC.cxx
ecps/SpdRsSECParSet.cxx
ecps/SpdRsSECPoint.cxx
ecps/SpdRsSECHit.cxx
ecps/SpdRsSECHitProducer.cxx

)

Set(LINKDEF SpdRssLinkDef.h)
Set(LIBRARY_NAME SpdRss)
Set(DEPENDENCIES Base SpdData SpdCommon SpdField)

GENERATE_LIBRARY()
