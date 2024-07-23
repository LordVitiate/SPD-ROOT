
#
# Create a library called "TrackingGF" 
# 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/proc/tracking
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/external/Eigen3
${CMAKE_SOURCE_DIR}/external/GenFit2/core/include
${CMAKE_SOURCE_DIR}/external/GenFit2/eventDisplay/include
${CMAKE_SOURCE_DIR}/external/GenFit2/fields/include
${CMAKE_SOURCE_DIR}/external/GenFit2/finitePlanes/include
${CMAKE_SOURCE_DIR}/external/GenFit2/fitters/include
${CMAKE_SOURCE_DIR}/external/GenFit2/measurements/include
${CMAKE_SOURCE_DIR}/external/GenFit2/trackReps/include
${CMAKE_SOURCE_DIR}/external/GenFit2/utilities/include
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/common/region
${CMAKE_SOURCE_DIR}/common/geometry
${CMAKE_SOURCE_DIR}/spddata
${CMAKE_SOURCE_DIR}/spddata/params
${CMAKE_SOURCE_DIR}/spddata/IdealTrackData
${CMAKE_SOURCE_DIR}/spddata/hits
${CMAKE_SOURCE_DIR}/spddata/hits/vnt
${CMAKE_SOURCE_DIR}/spdgeometry
${CMAKE_SOURCE_DIR}/spdgeometry/its
${CMAKE_SOURCE_DIR}/spdgeometry/tst
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
SpdGFTrackW.cxx
SpdGFMagneticField.cxx
SpdGFMeasurementCreator.cxx
SpdGFHitCreator.cxx
SpdRKTrackRep.cxx
SpdKalmanFitterRefTrack.cxx
)

set(HEADERS
SpdGFBase.h
SpdGFMagneticField.h
SpdGFMeasurementCreator.h
SpdGFHitCreator.h
SpdGFTrackW.h
SpdRKTrackRep.h
SpdKalmanFitterRefTrack.h
)

Set(LINKDEF TrackingGFLinkDef.h)
Set(LIBRARY_NAME TrackingGF)
Set(DEPENDENCIES Base GenFit2 SpdData SpdField SpdCommon SpdGeometry)

GENERATE_LIBRARY()

