
#
# Create a library called "VertexKF" 
# 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/proc/
${CMAKE_SOURCE_DIR}/proc/vertex
${CMAKE_SOURCE_DIR}/external/KFParticle/
${CMAKE_SOURCE_DIR}/external/KFParticle/KFParticle
${CMAKE_SOURCE_DIR}/external/KFParticle/KFParticlePerformance
${CMAKE_SOURCE_DIR}/spddata/
${CMAKE_SOURCE_DIR}/spddata/IdealTrackData
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
SpdKFParticleTest.cxx
)

set(HEADERS
SpdKFParticleTest.h
)

ADD_DEFINITIONS(-DHomogeneousField)
#ADD_DEFINITIONS(-DNonhomogeneousField)

Set(LINKDEF VertexKFLinkDef.h)
Set(LIBRARY_NAME VertexKF)
Set(DEPENDENCIES Base)

GENERATE_LIBRARY()

