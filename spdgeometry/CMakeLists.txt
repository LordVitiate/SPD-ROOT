
# 
# Create a library called "SpdGeometry" 
# 

set(INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${SIMPATH}/include/vmc
${CMAKE_SOURCE_DIR}/spddata
${CMAKE_SOURCE_DIR}/spddata/params
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/common/geometry
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/spdgeometry
${CMAKE_SOURCE_DIR}/spdgeometry/its
${CMAKE_SOURCE_DIR}/spdgeometry/tof
${CMAKE_SOURCE_DIR}/spdgeometry/tst
${CMAKE_SOURCE_DIR}/spdgeometry/ecalt
${CMAKE_SOURCE_DIR}/spdgeometry/rst
${CMAKE_SOURCE_DIR}/spdgeometry/caps
${CMAKE_SOURCE_DIR}/spdgeometry/sol
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS

SpdGeoFactory.cxx
SpdGeoLoader.cxx

./its/SpdItsVolPars.cxx
./its/SpdItsGeoMapperX.cxx
./its/SpdItsGeoBuilder.cxx

./its/SpdMvdGeoMapper.cxx

./tst/SpdTsBVolPars.cxx
./tst/SpdTsTBGeoMapper.cxx
./tst/SpdTsTBGeoBuilder.cxx
./tst/SpdTsECVolPars.cxx
./tst/SpdTsTECGeoMapper.cxx
./tst/SpdTsTECGeoBuilder.cxx

./tof/SpdTofBGeoMapper.cxx
./tof/SpdTofECGeoMapper.cxx

./ecalt/SpdEcalTBGeoMapper.cxx
./ecalt/SpdEcalTB2GeoMapper.cxx
./ecalt/SpdEcalBGeoMapper.cxx
./ecalt/SpdEcalTECGeoMapper.cxx
./ecalt/SpdEcalTEC2GeoMapper.cxx
./ecalt/SpdEcalECGeoMapper.cxx

./rst/SpdRsTBGeoMapper.cxx
./rst/SpdRsTB2GeoMapper.cxx
./rst/SpdRsTECGeoMapper.cxx
./rst/SpdRsTEC2GeoMapper.cxx

./caps/SpdZdcGeoMapper.cxx
./caps/SpdBbcGeoMapper.cxx
./caps/SpdAegGeoMapper.cxx

./sol/SpdTsSBGeoMapper.cxx
./sol/SpdTsSBGeoBuilder.cxx

./sol/SpdTsSECGeoMapper.cxx

./sol/SpdEcalSBGeoMapper.cxx
./sol/SpdEcalSECGeoMapper.cxx
./sol/SpdRsSBGeoMapper.cxx
./sol/SpdRsSECGeoMapper.cxx

)

set(HEADERS

SpdGeoFactory.h
SpdGeoLoader.h

its/SpdItsGeoBuilder.h
its/SpdItsGeoMapperX.h
its/SpdItsVolPars.h

its/SpdMvdGeoMapper.h

tst/SpdTsBVolPars.h
tst/SpdTsECVolPars.h
tst/SpdTsTBGeoBuilder.h
tst/SpdTsTBGeoMapper.h
tst/SpdTsTECGeoBuilder.h
tst/SpdTsTECGeoMapper.h

tof/SpdTofBGeoMapper.h
tof/SpdTofECGeoMapper.h

ecalt/SpdEcalTB2GeoMapper.h
ecalt/SpdEcalTBGeoMapper.h
ecalt/SpdEcalBGeoMapper.h
ecalt/SpdEcalTEC2GeoMapper.h
ecalt/SpdEcalTECGeoMapper.h
ecalt/SpdEcalECGeoMapper.h

rst/SpdRsTB2GeoMapper.h
rst/SpdRsTBGeoMapper.h
rst/SpdRsTEC2GeoMapper.h
rst/SpdRsTECGeoMapper.h

caps/SpdZdcGeoMapper.h
caps/SpdBbcGeoMapper.h
caps/SpdAegGeoMapper.h

sol/SpdEcalSBGeoMapper.h
sol/SpdEcalSECGeoMapper.h
sol/SpdRsSBGeoMapper.h
sol/SpdRsSECGeoMapper.h
sol/SpdTsSBGeoBuilder.h
sol/SpdTsSBGeoMapper.h
sol/SpdTsSECGeoMapper.h

)

set(LINKDEF  SpdGeometryLinkDef.h)
set(LIBRARY_NAME SpdGeometry)

set(DEPENDENCIES Base SpdCommon SpdData SpdPassive)

GENERATE_LIBRARY()
