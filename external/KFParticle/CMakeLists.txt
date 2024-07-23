
# 
# Create a library called "KFParticle" 
# 

set(INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/external/KFParticle
${CMAKE_SOURCE_DIR}/external/KFParticle/KFParticle
${CMAKE_SOURCE_DIR}/external/KFParticle/KFParticlePerformance
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set (SRCS
  KFParticle/KFParticleTopoReconstructor.cxx
  KFParticle/KFVertex.cxx
  KFParticle/KFPTrack.cxx
  KFParticle/KFPTrackVector.cxx
  KFParticle/KFPVertex.cxx
  KFParticle/KFParticlePVReconstructor.cxx
  KFParticle/KFParticleDatabase.cxx
  KFParticle/KFParticleBase.cxx
  KFParticle/KFParticleBaseSIMD.cxx
  KFParticle/KFParticle.cxx
  KFParticle/KFParticleSIMD.cxx
  KFParticle/KFParticleFinder.cxx
  KFParticle/KFPEmcCluster.cxx
  KFParticlePerformance/KFMCParticle.cxx
  KFParticlePerformance/KFMCVertex.cxx
  KFParticlePerformance/KFParticlePerformanceBase.cxx
  KFParticlePerformance/KFTopoPerformance.cxx
  KFParticlePerformance/KFPartEfficiencies.cxx
)

set(HEADERS
KFParticle/KFParticleBase.h
KFParticle/KFParticleBaseSIMD.h
KFParticle/KFParticleDatabase.h
KFParticle/KFParticleDef.h
KFParticle/KFParticleField.h
KFParticle/KFParticleFinder.h
KFParticle/KFParticle.h
KFParticle/KFParticlePVReconstructor.h
KFParticle/KFParticleSIMD.h
KFParticle/KFParticleTopoReconstructor.h
KFParticle/KFPEmcCluster.h
KFParticle/KFPInputData.h
KFParticle/KFPSimdAllocator.h
KFParticle/KFPTrack.h
KFParticle/KFPTrackVector.h
KFParticle/KFPVertex.h
KFParticle/KFVertex.h 
KFParticlePerformance/KFMCCounter.h
KFParticlePerformance/KFMCParticle.h
KFParticlePerformance/KFMCTrack.h
KFParticlePerformance/KFMCVertex.h
KFParticlePerformance/KFPVEfficiencies.h
KFParticlePerformance/KFPartEfficiencies.h
KFParticlePerformance/KFPartMatch.h
KFParticlePerformance/KFParticlePerformanceBase.h
KFParticlePerformance/KFTopoPerformance.h
)

ADD_DEFINITIONS(-DHomogeneousField)
#ADD_DEFINITIONS(-DNonhomogeneousField)

set(LINKDEF KFParticleLinkDef.h)
set(LIBRARY_NAME KFParticle)

set(DEPENDENCIES Vc)

GENERATE_LIBRARY()



