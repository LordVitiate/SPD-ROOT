
# 
# Create a library called "SpdGen" 
# 

set(INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIR}
${ROOT_INCLUDE_DIR} 
${Geant4_INCLUDE_DIRS}
${Geant4VMC_INCLUDE_DIRS}
${CMAKE_SOURCE_DIR}/spddata/
${CMAKE_SOURCE_DIR}/spddata/mcdata
${CMAKE_SOURCE_DIR}/spddata/params
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/common/geometry
${CMAKE_SOURCE_DIR}/common/region
${CMAKE_SOURCE_DIR}/common/checks
)

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${ROOT_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${Geant4_INCLUDE_DIRS}
  ${Geant4VMC_INCLUDE_DIRS}
)

include_directories( ${INCLUDE_DIRECTORIES})

include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS

SpdRootFileSink.cxx
SpdTask.cxx

SpdRunSim.cxx
SpdStack.cxx 
SpdModulesMap.cxx
SpdPassiveModule.cxx
SpdDetector.cxx
SpdCommonGeoMapper.cxx

SpdRunAna.cxx
SpdParticlesAndProcesses.cxx

geometry/SpdGeoVolPars.cxx
geometry/SpdGeoTable.cxx
geometry/SpdNodesIdTable.cxx
geometry/SpdGeoMapper.cxx
geometry/SpdGeoBuilder.cxx
geometry/SpdGeoVVolume.cxx

region/SpdRegion.cxx

checks/SpdGeoScanner.cxx
)

set(HEADERS
checks/SpdGeoScanner.h
geometry/SpdGeoBuilder.h
geometry/SpdGeoMapper.h
geometry/SpdGeoTable.h
geometry/SpdGeoVolPars.h
geometry/SpdGeoVVolume.h
geometry/SpdNodesIdTable.h
region/SpdRegion.h

SpdRootFileSink.h
SpdCommonGeoMapper.h
SpdDetector.h
SpdGlobals.h
SpdModulesMap.h
SpdPassiveModule.h
SpdTask.h
SpdRunAna.h
SpdRunSim.h
SpdStack.h
SpdParticlesAndProcesses.h
)

set(LINKDEF  SpdCommonLinkDef.h)
set(LIBRARY_NAME SpdCommon)

set(DEPENDENCIES Base SpdData ${Geant4VMC_LIBRARIES} ${Geant4_LIBRARIES} FairTools)

GENERATE_LIBRARY()

