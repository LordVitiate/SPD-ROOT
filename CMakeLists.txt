# The name of our project is “SPDROOT”.  CMakeLists files in this project can
# refer to the root source directory of the project as ${SPDROOT_SOURCE_DIR} 
# or as ${CMAKE_SOURCE_DIR} and to the root binary directory of the project as 
# ${SPDROOT_BINARY_DIR} or ${CMAKE_BINARY_DIR}.
# This difference is important for the base classes which are in FAIRROOT 
# and the experiment part.

# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

# Set name of our project to “SPDROOT". Has to be done
# after check of cmake version since this is a new feature                  
PROJECT(SPDROOT)

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX /opt/spdroot/ CACHE PATH "SpdRoot install prefix" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

#set(CMAKE_VERBOSE_MAKEFILE true)
include(CMakeParseArguments)

cmake_policy(SET CMP0010 OLD)
#In case you need Fortran 
#ENABLE_LANGUAGE(Fortran)

# Check for needed environment variables
IF(NOT DEFINED ENV{FAIRROOTPATH})
  MESSAGE(FATAL_ERROR "You did not define the environment variable FAIRROOTPATH which is needed to find FairRoot. Please set this variable and execute cmake again.")
ENDIF(NOT DEFINED ENV{FAIRROOTPATH})

IF(NOT DEFINED ENV{SIMPATH})
  MESSAGE(FATAL_ERROR "You did not define the environment variable SIMPATH which is nedded to find the external packages. Please set this variable and execute cmake again.") 
ENDIF(NOT DEFINED ENV{SIMPATH})

SET(SIMPATH $ENV{SIMPATH})
SET(FAIRROOTPATH $ENV{FAIRROOTPATH})

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ 
# is checked
#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules"  ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules"  ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules_old"  ${CMAKE_MODULE_PATH})

Set(CheckSrcDir "${FAIRROOTPATH}/share/fairbase/cmake/checks")

find_package(FairRoot)

# Load some basic macros which are needed later on
include(FairMacros)
include(WriteConfigFile)
include(CTest)
include(CheckCompiler)
#include(CheckFortran)

#Check the compiler and set the compile and link flags
If(NOT CMAKE_BUILD_TYPE)
  Message("Set BuildType DEBUG")
  set(CMAKE_BUILD_TYPE Debug)
EndIf(NOT CMAKE_BUILD_TYPE)

Check_Compiler()

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(INCLUDE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/include")
Set(VMCWORKDIR ${CMAKE_INSTALL_PREFIX})
Option(USE_PATH_INFO "Information from PATH and LD_LIBRARY_PATH are used." OFF)
If(USE_PATH_INFO)
  Set(PATH "$PATH")
  If (APPLE)
    Set(LD_LIBRARY_PATH $ENV{DYLD_LIBRARY_PATH})
  Else (APPLE)
    Set(LD_LIBRARY_PATH $ENV{LD_LIBRARY_PATH})
  EndIf (APPLE)
Else(USE_PATH_INFO)
  STRING(REGEX MATCHALL "[^:]+" PATH $ENV{PATH})
EndIf(USE_PATH_INFO)

# Check if the user wants to build the project in the source
# directory
CHECK_OUT_OF_SOURCE_BUILD()

# Check if we are on an UNIX system. If not stop with an error
# message
IF(NOT UNIX)
  MESSAGE(FATAL_ERROR "You're not on an UNIX system. The project was up to now only tested on UNIX systems, so we break here. If you want to go on please edit the CMakeLists.txt in the source directory.")
ENDIF(NOT UNIX)  

# Check if the external packages are installed into a separate install
# directory
CHECK_EXTERNAL_PACKAGE_INSTALL_DIR()

# searches for needed packages
# REQUIRED means that cmake will stop if this packages are not found
# For example the framework can run without GEANT3, but ROOT is
# mandatory

find_package2(PUBLIC ROOT  VERSION 6.10.00  REQUIRED)
find_package2(PUBLIC Pythia6)
find_package2(PUBLIC Pythia8)

find_package2(PUBLIC Geant3)
if(Geant3_FOUND)
  Set(Geant3_LIBRARY_DIR ${Geant3_DIR}/${Geant3_CMAKE_INSTALL_LIBDIR})
  FIND_PATH(Geant3_SYSTEM_DIR NAMES data PATHS
           ${Geant3_DIR}/../../share/geant3
           NO_DEFAULT_PATH)
endif()

Message("-- Looking for Geant4 ...")

find_package2(PUBLIC Geant4)

Find_Package2(PUBLIC VGM)

find_package2(PUBLIC Geant4VMC)

Set(Geant4_INCLUDE_DIRS "$ENV{SIMPATH}/include/Geant4")
Set(Geant4VMC_INCLUDE_DIRS "$ENV{SIMPATH}/include/geant4vmc")
Set(Geant4VMC_LIBRARY_DIR "$ENV{SIMPATH}/lib")

Set(Boost_NO_SYSTEM_PATHS TRUE)
Set(Boost_NO_BOOST_CMAKE TRUE)
If(${ROOT_LIBRARY_DIR} MATCHES /lib/root)
  set(BOOST_ROOT ${SIMPATH})
  set(GSL_DIR ${SIMPATH})
Else(${ROOT_LIBRARY_DIR} MATCHES /lib/root)
  set(BOOST_ROOT ${SIMPATH}/basics/boost)
  set(GSL_DIR ${SIMPATH}/basics/gsl)
EndIf(${ROOT_LIBRARY_DIR} MATCHES /lib/root)
Message("-- Looking for Boost ...")
# If an older version of boost is found both of the variables below are
# cached and in a second cmake run, a good boost version is found even 
# if the version is to old. 
# To overcome this problem both variables are cleared before checking
# for boost.
Unset(Boost_INCLUDE_DIR CACHE)
Unset(Boost_LIBRARY_DIRS CACHE)
find_package(Boost 1.41)
If (Boost_FOUND)
  Set(Boost_Avail 1)
Else (Boost_FOUND)
  Set(Boost_Avail 0)
EndIf (Boost_FOUND)

# set a variable which should be used in all CMakeLists.txt
# Defines all basic include directories from fairbase
SetBasicVariables()

Message(":: SYSTEM_INCLUDE_DIR  : ${SYSTEM_INCLUDE_DIRECTORIES}")
Message(":: BASE_INCLUDE_DIR    : ${BASE_INCLUDE_DIRECTORIES}")
Message(":: CMAKE_SOURCE_DIR    : ${CMAKE_SOURCE_DIR}")
Message(":: CMAKE_CURRENT_BINARY_DIR    : ${CMAKE_CURRENT_BINARY_DIR}")
Message(":: CMAKE_INSTALL_PREFIX    : ${CMAKE_INSTALL_PREFIX}")

# Set the library version in the main CMakeLists.txt
SET(FAIRROOT_MAJOR_VERSION 0)
SET(FAIRROOT_MINOR_VERSION 0)
SET(FAIRROOT_PATCH_VERSION 0)
SET(FAIRROOT_VERSION "${FAIRROOT_MAJOR_VERSION}.${FAIRROOT_MINOR_VERSION}.${FAIRROOT_PATCH_VERSION}")
SET(FAIRROOT_LIBRARY_PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES}
    VERSION "${FAIRROOT_VERSION}"
    SOVERSION "${FAIRROOT_MAJOR_VERSION}"
    SUFFIX ".so"
)

#Generate_Version_Info()

SET(_LIBDIR ${CMAKE_BINARY_DIR}/lib)
SET(LD_LIBRARY_PATH  ${_LIBDIR} ${SIMPATH}/lib ${LD_LIBRARY_PATH})
SET(PATH ${CMAKE_SOURCE_DIR} ${CMAKE_INSTALL_PREFIX}/bin ${PATH}) 

# Check if the compiler support specific C++11 features
# Up to now this is only a check since the code does not use
# any of the features of the new standard
include(CheckCXX11Features)

IF(HAS_CXX11_SHAREDPOINTER)
  Add_Definitions(-DHAS_SHAREDPOINTER)
ENDIF(HAS_CXX11_SHAREDPOINTER)

# Recurse into the given subdirectories.  This does not actually
# cause another cmake executable to run.  The same process will walk through
# the project's entire directory structure.

add_subdirectory (external)
add_subdirectory (spddata)
add_subdirectory (spdgenerators)
add_subdirectory (spdgeometry)
add_subdirectory (common)
add_subdirectory (field)  
add_subdirectory (passive)
add_subdirectory (zdc)
add_subdirectory (bbc)
add_subdirectory (aeg)
add_subdirectory (its)
add_subdirectory (mvd)
add_subdirectory (tof)
add_subdirectory (tst)
add_subdirectory (ecalt)
add_subdirectory (rst)
add_subdirectory (proc)
add_subdirectory (reco)
add_subdirectory (spddisplay)
add_subdirectory (test)
#add_subdirectory (sol)
add_subdirectory (experimental)

################################################################################
if(PROJECT_PACKAGE_DEPENDENCIES)
  message(STATUS "  ")
  message(STATUS "  ${Cyan}DEPENDENCY FOUND     VERSION                   PREFIX${CR}")
  foreach(dep IN LISTS PROJECT_PACKAGE_DEPENDENCIES)
    if(${dep}_VERSION)
      if(${dep} STREQUAL DDS)
        set(version_str "${BGreen}${${dep}_MAJOR_VERSION}.${${dep}_MINOR_VERSION}${CR}")
      elseif(${dep} STREQUAL Boost)
        if(Boost_VERSION_MAJOR)
          set(version_str "${BGreen}${${dep}_VERSION_MAJOR}.${${dep}_VERSION_MINOR}${CR}")
        else()
          set(version_str "${BGreen}${${dep}_MAJOR_VERSION}.${${dep}_MINOR_VERSION}${CR}")
        endif()
      else()
        set(version_str "${BGreen}${${dep}_VERSION}${CR}")
      endif()
    else()
      set(version_str "${BYellow}unknown${CR}")
    endif()
    if(PROJECT_${dep}_VERSION)
      set(version_req_str " (>= ${PROJECT_${dep}_VERSION})")
    endif()
    pad(${dep} 20 " " dep_padded)
    if(DISABLE_COLOR)
      pad("${version_str}${version_req_str}" 25 " " version_padded)
    else()
      pad("${version_str}${version_req_str}" 25 " " version_padded COLOR 1)
    endif()
    set(prefix ${${dep}_PREFIX})
    if(${dep} STREQUAL Boost)
      if(TARGET Boost::headers)
        get_target_property(boost_include Boost::headers INTERFACE_INCLUDE_DIRECTORIES)
      else()
        get_target_property(boost_include Boost::boost INTERFACE_INCLUDE_DIRECTORIES)
      endif()
      get_filename_component(prefix ${boost_include}/.. ABSOLUTE)
    elseif(${dep} STREQUAL Protobuf)
      get_filename_component(prefix ${Protobuf_INCLUDE_DIRS}/.. ABSOLUTE)
    elseif(${dep} STREQUAL msgpack)
      get_target_property(msgpack_include msgpackc INTERFACE_INCLUDE_DIRECTORIES)
      get_filename_component(prefix ${msgpack_include}/.. ABSOLUTE)
    elseif(${dep} STREQUAL Pythia6)
      get_filename_component(prefix ${PYTHIA6_LIBRARY_DIR}/.. ABSOLUTE)
    elseif(${dep} STREQUAL Pythia8)
      get_filename_component(prefix ${PYTHIA8_LIB_DIR}/.. ABSOLUTE)
    elseif(${dep} STREQUAL FairLogger)
      if(FairLogger_PREFIX)
        set(prefix ${FairLogger_PREFIX})
      else()
        set(prefix ${FairLogger_ROOT})
      endif()
    elseif(${dep} STREQUAL FairMQ)
      if(FairMQ_PREFIX)
        set(prefix ${FairMQ_PREFIX})
      else()
        set(prefix ${FairMQ_ROOT})
      endif()
    elseif(${dep} STREQUAL yaml-cpp)
      get_filename_component(prefix ${YAML_CPP_INCLUDE_DIR}/.. ABSOLUTE)
    elseif(${dep} STREQUAL Geant4VMC)
      string(REPLACE ":" ";" geant4vmc_include ${Geant4VMC_INCLUDE_DIRS})
      list(GET geant4vmc_include 0 geant4vmc_include)
      get_filename_component(prefix ${geant4vmc_include}/../.. ABSOLUTE)
    elseif(${dep} STREQUAL Geant3)
      string(REPLACE ":" ";" geant3_include ${Geant3_INCLUDE_DIRS})
      list(GET geant3_include 0 geant3_include)
      get_filename_component(prefix ${geant3_include}/../.. ABSOLUTE)
    elseif(${dep} STREQUAL Geant4)
      list(GET Geant4_INCLUDE_DIRS 0 geant4_include)
      get_filename_component(prefix ${geant4_include}/../.. ABSOLUTE)
    elseif(${dep} STREQUAL VGM)
      string(REPLACE ":" ";" vgm_include ${VGM_INCLUDE_DIRS})
      list(GET vgm_include 0 vgm_include)
      get_filename_component(prefix ${vgm_include}/.. ABSOLUTE)
    elseif(${dep} STREQUAL ROOT)
      set(prefix ${ROOT_INSTALL_DIR})
    elseif(${dep} STREQUAL IWYU)
      get_filename_component(prefix ${IWYU_BINARY}/.. ABSOLUTE)
    elseif(${dep} STREQUAL yaml-cpp)
      get_filename_component(prefix ${YAML_CPP_INCLUDE_DIR}/.. ABSOLUTE)
    endif()

    message(STATUS "  ${BWhite}${dep_padded}${CR}${version_padded}${prefix}")

    if(${dep} STREQUAL Geant3)
      message(STATUS "                                          G3SYS: ${Geant3_SYSTEM_DIR}")
    endif()
    if(${dep} STREQUAL Geant4)
      foreach(dataset IN LISTS Geant4_DATASETS)
        pad(${Geant4_DATASET_${dataset}_ENVVAR} 20 " " envvar_padded LEFT)
        message(STATUS "                          ${envvar_padded}: ${Geant4_DATASET_${dataset}_PATH}")
      endforeach()
    endif()

    unset(version_str)
    unset(version_padded)
    unset(version_req_str)
  endforeach()
endif()
message(STATUS "  ")

Option( BUILD_DOXYGEN
        "Whether to generate automatic documentation with Doxygen"
        OFF )
        
if(BUILD_DOXYGEN)
    find_package( Doxygen )
    set( DOXY_DOC_OUT ${CMAKE_CURRENT_BINARY_DIR}/doc )
    configure_file( doc/doxyfile.1.8.8.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile )
    add_custom_target( html-doc 
                    doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating automatic documentation into ${DOXY_DOC_OUT}" )
endif(BUILD_DOXYGEN)

WRITE_CONFIG_FILE(config.sh)
WRITE_CONFIG_FILE(config.sh_install)

file(APPEND ${CMAKE_BINARY_DIR}/config.sh "export VMCWORKDIR=${CMAKE_SOURCE_DIR}\n")
file(APPEND ${CMAKE_BINARY_DIR}/config.sh "export SPDROOTPATH=${CMAKE_SOURCE_DIR}\n")
file(APPEND ${CMAKE_BINARY_DIR}/config.sh "export GEOMPATH=${CMAKE_SOURCE_DIR}/geometry\n")
file(APPEND ${CMAKE_BINARY_DIR}/config.sh "export MAGFPATH=${CMAKE_SOURCE_DIR}/input\n")
file(APPEND ${CMAKE_BINARY_DIR}/config.sh "export ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/include:${ROOT_INCLUDE_PATH}\n")

file(APPEND ${CMAKE_BINARY_DIR}/config.sh_install "export SPDROOTPATH=${VMCWORKDIR}\n")
file(APPEND ${CMAKE_BINARY_DIR}/config.sh_install "export GEOMPATH=${VMCWORKDIR}/share/geometry\n")
file(APPEND ${CMAKE_BINARY_DIR}/config.sh_install "export MAGFPATH=${VMCWORKDIR}/share/input\n")
file(APPEND ${CMAKE_BINARY_DIR}/config.sh_install "export ROOT_INCLUDE_PATH=${VMCWORKDIR}/include:${ROOT_INCLUDE_PATH}\n")

Install(FILES ${CMAKE_BINARY_DIR}/config.sh_install
        DESTINATION bin
        RENAME SetEnv.sh
       )

Install(FILES spdroot.py
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ
        DESTINATION bin
	)

Install(FILES macro/rootlogon.C macro/rootlogoff.C
          DESTINATION macro/
          )

Install(DIRECTORY config
          DESTINATION .
          )

Install(DIRECTORY gconfig
          DESTINATION .
          )

Install(FILES macro/SimuQslPy6.C macro/SimuQslPy8.C
          DESTINATION share/examples/
          )

Install(DIRECTORY external/Eigen3/Eigen
	  DESTINATION include/
	  )

Install(DIRECTORY macro/run macro/primgen macro/analysis 
          DESTINATION share/examples
          PATTERN ".svn" EXCLUDE)

Install(DIRECTORY macro/examples/K0decay macro/examples/chic macro/examples/ecal macro/examples/jpsi-mumu
          DESTINATION share/examples/fullchain/
          PATTERN ".svn" EXCLUDE)

Install(DIRECTORY geometry
          DESTINATION share/
          PATTERN ".svn" EXCLUDE)

Install(DIRECTORY input
          DESTINATION share/
          PATTERN ".svn" EXCLUDE)

Install(FILES spd_check_system.sh
          DESTINATION ./
	  RENAME check_system.sh
                )

Install(FILES spd_check_system.csh
          DESTINATION ./
	  RENAME check_system.csh
                )

#Install(FILES ${CMAKE_BINARY_DIR}/check_system.sh
#          DESTINATION .
#                )

#Install(FILES ${CMAKE_BINARY_DIR}/check_system.csh
#          DESTINATION .
#                )

 
configure_file(${CMAKE_SOURCE_DIR}/CTestCustom.cmake 
               ${CMAKE_BINARY_DIR}/CTestCustom.cmake
              )

# Summary ######################################################################
if(CMAKE_CXX_FLAGS)
  message(STATUS "  ")
  message(STATUS "  ${Cyan}GLOBAL CXX FLAGS${CR}  ${BGreen}${CMAKE_CXX_FLAGS}${CR}")
endif()
if(CMAKE_CONFIGURATION_TYPES)
  message(STATUS "  ")
  message(STATUS "  ${Cyan}BUILD TYPE         CXX FLAGS${CR}")
  string(TOUPPER "${CMAKE_BUILD_TYPE}" selected_type)
  foreach(type IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(TOUPPER "${type}" type_upper)
    if(type_upper STREQUAL selected_type)
      pad("${type}" 18 " " type_padded)
      message(STATUS "${BGreen}* ${type_padded}${CMAKE_CXX_FLAGS_${type_upper}}${CR}")
    else()
      pad("${type}" 18 " " type_padded)
      message(STATUS "  ${BWhite}${type_padded}${CR}${CMAKE_CXX_FLAGS_${type_upper}}")
    endif()
    unset(type_padded)
    unset(type_upper)
  endforeach()
  message(STATUS "  ")
  message(STATUS "  (Change the build type with ${BMagenta}-DCMAKE_BUILD_TYPE=...${CR})")
endif()

