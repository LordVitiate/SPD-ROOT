
add_subdirectory (clustering)

IF(GENFIT2_IS_DEFINED)
    MESSAGE(STATUS "PROC: GenFit2 is defined")
    add_subdirectory (tracking)
ELSE()
    MESSAGE(STATUS "PROC: the \"Tracking\" dictionary could not be built; \"GenFit2\" is required")   
ENDIF()

add_subdirectory (vertex)
