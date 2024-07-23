# 
# Create a library called "SpdGen" 
# 

set(INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
${PYTHIA8_INCLUDE_DIR}
${SIMPATH}/include/vmc
${CMAKE_SOURCE_DIR}/FtfEvtGen/
${CMAKE_SOURCE_DIR}/FtfEvtGen/include
${CMAKE_SOURCE_DIR}/spdgenerators
${CMAKE_SOURCE_DIR}/spddata
${CMAKE_SOURCE_DIR}/spddata/params
${CMAKE_SOURCE_DIR}/spddata/mcdata
${CMAKE_SOURCE_DIR}/common
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib
${GEANT4_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS

SpdGenerator.cxx
SpdPrimGenFactory.cxx

SpdDecayer.cxx

SpdPythia6Decayer.cxx
SpdPythia6.cxx
SpdPythia6Generator.cxx

SpdPythia8.cxx
SpdPythia8Generator.cxx
SpdPythia8Decayer.cxx

SpdUrqmdGenerator.cxx
SpdFtfGenerator.cxx
SpdIsotropicGenerator.cxx

SpdParticleGenerator.cxx
SpdMultiParticleGenerator.cxx
SpdEvtBaseGenerator.cxx

SpdPrimaryGenerator.cxx

SpdUniformPtCostGenerator.cxx
)

set(HEADERS

SpdGenerator.h
SpdPrimGenFactory.h

SpdDecayer.h

SpdPythia6.h
SpdPythia6Generator.h
SpdPythia6Decayer.h

SpdPythia8.h
SpdPythia8Generator.h
SpdPythia8Decayer.h

SpdUrqmdGenerator.h
SpdFtfGenerator.h
SpdIsotropicGenerator.h

SpdParticleGenerator.h
SpdMultiParticleGenerator.h
SpdEvtBaseGenerator.h

SpdPrimaryGenerator.h

SpdUniformPtCostGenerator.h
)

set(LINKDEF  GenLinkDef.h)
set(LIBRARY_NAME SpdGen)

#set(DEPENDENCIES Base VMCLibrary ParBase SpdData SpdCommon pythia8 pythia6 EGPythia6)
set(DEPENDENCIES Base ParBase SpdData SpdCommon pythia8 Pythia6 EGPythia6 EGPythia8)
#set(DEPENDENCIES Base ParBase SpdData SpdCommon EGPythia6 EGPythia8)

GENERATE_LIBRARY()
