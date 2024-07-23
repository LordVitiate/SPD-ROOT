
IF(${ROOT_VERSION_NUMBER} LESS 60000)
    MESSAGE(STATUS "GenFit2 requires ROOT version > 6.00.00")
ELSE()
    MESSAGE(STATUS "GenFit2 will be built")
    add_subdirectory (GenFit2)
    SET(GENFIT2_IS_DEFINED true CACHE BOOL "GENFIT2_IS_DEFINED" FORCE)
ENDIF()

add_subdirectory (KFParticle)

