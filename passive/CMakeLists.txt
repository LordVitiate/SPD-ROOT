
#
# Create a library called "SpdPassive" 
#

set(INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${ROOT_INCLUDE_DIR} 
${SIMPATH}/include/vmc
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/spddata
${CMAKE_SOURCE_DIR}/spddata/params
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/passive
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}

)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
SpdCave.cxx           
SpdGeoCave.cxx  
SpdGeoFrame.cxx 
SpdPipe.cxx 
SpdTorBasket.cxx   
SpdSolMagnet.cxx  
SpdSolMagnet2.cxx 
SpdTorMagnet.cxx  
SpdHybMagnet.cxx  
)

Set(HEADERS 
SpdCave.h
SpdGeoCave.h
SpdGeoFrame.h
SpdHybMagnet.h
SpdPipe.h
SpdSolMagnet.h
SpdSolMagnet2.h 
SpdTorBasket.h
SpdTorMagnet.h
)

Set(LINKDEF PassiveLinkDef.h)
Set(LIBRARY_NAME SpdPassive)
Set(DEPENDENCIES Base GeoBase ParBase SpdCommon SpdData)

GENERATE_LIBRARY()
