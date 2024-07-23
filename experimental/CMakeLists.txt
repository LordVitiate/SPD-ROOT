set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/common/geometry
${CMAKE_SOURCE_DIR}/common/region
${CMAKE_SOURCE_DIR}/spddata
${CMAKE_SOURCE_DIR}/spddata/hits
${CMAKE_SOURCE_DIR}/spddata/params
${CMAKE_SOURCE_DIR}/spddata/hits/rs
${CMAKE_SOURCE_DIR}/spddata/reco/vnt
${CMAKE_SOURCE_DIR}/reco/vnt
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/external/GenFit2
${CMAKE_SOURCE_DIR}/external/GenFit2/core/include
${CMAKE_SOURCE_DIR}/external/GenFit2/eventDisplay/include
${CMAKE_SOURCE_DIR}/external/GenFit2/fields/include
${CMAKE_SOURCE_DIR}/external/GenFit2/finitePlanes/include
${CMAKE_SOURCE_DIR}/external/GenFit2/fitters/include
${CMAKE_SOURCE_DIR}/external/GenFit2/measurements/include
${CMAKE_SOURCE_DIR}/external/GenFit2/trackReps/include
${CMAKE_SOURCE_DIR}/external/GenFit2/utilities/include
${CMAKE_SOURCE_DIR}/external/Eigen3
${CMAKE_SOURCE_DIR}/proc/tracking
${CMAKE_SOURCE_DIR}/experimental
${CMAKE_SOURCE_DIR}/experimental/rstrkfinder
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}

) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
rstrkfinder/RsGFMeasurementMaker.cxx
rstrkfinder/RsTrackFitterGF.cxx
rstrkfinder/RsPlaneHolder.cxx
rstrkfinder/RsTrackFinder.cxx
rstrkfinder/RsRecTrack.cxx
)

#set(HEADERS
#rstrkfinder/RsGFMeasurementMaker.h
#rstrkfinder/RsTrackFitterGF.h
#rstrkfinder/RsPlaneHolder.h
#rstrkfinder/RsTrackFinder.h
#rstrkfinder/RsRecTrack.h
#)

Set(LINKDEF SpdExperimentalLinkDef.h)
Set(LIBRARY_NAME Experim)
Set(DEPENDENCIES Base TrackingGF GenFit2 SpdField SpdCommon SpdGeometry)

GENERATE_LIBRARY()
