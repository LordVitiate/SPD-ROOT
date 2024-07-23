
#
# Create a library called "SpdDisplay” 
#

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/spddata
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/spddisplay
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}

) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
#Put here your sourcefiles

SpdEventManager.cxx
SpdFairEventManager.cxx
SpdEveEventViewer.cxx
SpdMCTracks.cxx

)

set(HEADERS
SpdEveEventViewer.h
SpdEventManager.h
SpdFairEventManager.h
SpdMCTracks.h
)

Set(LINKDEF SpdDisplayLinkDef.h)
Set(LIBRARY_NAME SpdDisplay)
Set(DEPENDENCIES Base EventDisplay SpdData SpdField)

GENERATE_LIBRARY()


