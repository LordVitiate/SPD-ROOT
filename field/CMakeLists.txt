
#
# Create a library called "SpdField” 
#

set(INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES} 
${ROOT_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/common/region
${CMAKE_SOURCE_DIR}/spddata/params
${CMAKE_SOURCE_DIR}/field 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}

)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
SpdField.cxx
SpdConstField.cxx   
SpdRadialField.cxx   
SpdMultiField.cxx
SpdFieldCreator.cxx 
SpdFieldMap.cxx
SpdFieldMapData.cxx
SpdFieldMap1_8.cxx
SpdFieldMapData1_8.cxx
SpdAxialFieldMap.cxx
SpdAxialFieldMapData.cxx
SpdFieldContFact.cxx
)

set(HEADERS
SpdAxialFieldMapData.h
SpdAxialFieldMap.h
SpdConstField.h
SpdFieldContFact.h
SpdFieldCreator.h
SpdField.h
SpdFieldMapData.h
SpdFieldMap.h
SpdFieldMapData1_8.h
SpdFieldMap1_8.h
SpdMultiField.h
SpdRadialField.h
)

set(LINKDEF FieldLinkDef.h)
Set(LIBRARY_NAME SpdField)
Set(DEPENDENCIES Base SpdData SpdCommon)

GENERATE_LIBRARY()

