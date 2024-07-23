
#
# Create a library called "Clustering" 
# 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
#put here all directories where header files are located
${CMAKE_SOURCE_DIR}/proc/clustering
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
ClusterElement.cxx
Cluster.cxx
ClusterCollection.cxx
ClManager.cxx
)

set(HEADERS
ClManager.h
ClusterCollection.h
ClusterElement.h
Cluster.h
)

Set(LINKDEF ClusterLinkDef.h)
Set(LIBRARY_NAME Clustering)
Set(DEPENDENCIES Base)

GENERATE_LIBRARY()
