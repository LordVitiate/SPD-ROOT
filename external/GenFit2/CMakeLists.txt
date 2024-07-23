
#
# Create a library called "GenFit2" 
#

set(SYSTEM_INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}/external/Eigen3
${CMAKE_SOURCE_DIR}/external/GenFit2
${CMAKE_SOURCE_DIR}/external/GenFit2/core/include
${CMAKE_SOURCE_DIR}/external/GenFit2/eventDisplay/include
${CMAKE_SOURCE_DIR}/external/GenFit2/fields/include
${CMAKE_SOURCE_DIR}/external/GenFit2/finitePlanes/include
${CMAKE_SOURCE_DIR}/external/GenFit2/fitters/include
${CMAKE_SOURCE_DIR}/external/GenFit2/measurements/include
${CMAKE_SOURCE_DIR}/external/GenFit2/trackReps/include
${CMAKE_SOURCE_DIR}/external/GenFit2/utilities/include
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
#${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS

 ./eventDisplay/src/EventDisplay.cc
 
 ./core/src/AbsFinitePlane.cc
 ./core/src/AbsFitter.cc
 ./core/src/AbsFitterInfo.cc
 ./core/src/AbsMeasurement.cc
 ./core/src/AbsTrackRep.cc
 ./core/src/DetPlane.cc
 ./core/src/Exception.cc
 ./core/src/FieldManager.cc
 ./core/src/FitStatus.cc
 ./core/src/IO.cc
 ./core/src/Material.cc
 ./core/src/MeasuredStateOnPlane.cc
 ./core/src/MeasurementOnPlane.cc
 ./core/src/StateOnPlane.cc
 ./core/src/ThinScatterer.cc
 ./core/src/Tools.cc
 ./core/src/TrackCand.cc
 ./core/src/TrackCandHit.cc
 ./core/src/Track.cc
 ./core/src/TrackPoint.cc
 
 ./finitePlanes/src/RectangularFinitePlane.cc
 
 ./trackReps/src/MaterialEffects.cc
 ./trackReps/src/MplTrackRep.cc
 ./trackReps/src/RKTools.cc
 ./trackReps/src/RKTrackRep.cc
 ./trackReps/src/StepLimits.cc
 ./trackReps/src/TGeoMaterialInterface.cc
 
 ./measurements/src/FullMeasurement.cc
 ./measurements/src/HMatrixPhi.cc
 ./measurements/src/HMatrixU.cc
 ./measurements/src/HMatrixUnit.cc
 ./measurements/src/HMatrixUV.cc
 ./measurements/src/HMatrixV.cc
 ./measurements/src/PlanarMeasurement.cc
 ./measurements/src/ProlateSpacepointMeasurement.cc
 ./measurements/src/SpacepointMeasurement.cc
 ./measurements/src/WireMeasurement.cc
 ./measurements/src/WireMeasurementNew.cc
 ./measurements/src/WirePointMeasurement.cc
 ./measurements/src/WireTrackCandHit.cc
 
 ./fitters/src/AbsKalmanFitter.cc
 ./fitters/src/DAF.cc
 ./fitters/src/KalmanFitStatus.cc
 ./fitters/src/KalmanFittedStateOnPlane.cc
 ./fitters/src/KalmanFitter.cc
 ./fitters/src/KalmanFitterInfo.cc
 ./fitters/src/KalmanFitterRefTrack.cc
 ./fitters/src/ReferenceStateOnPlane.cc
 
 ./fields/src/ConstField.cc
 
 ./utilities/src/HelixTrackModel.cc
 ./utilities/src/MeasurementCreator.cc
 )
 
set(HEADERS
 
 ./eventDisplay/include/EventDisplay.h
  
 ./core/include/AbsBField.h
 ./core/include/AbsFinitePlane.h
 ./core/include/AbsFitter.h
 ./core/include/AbsFitterInfo.h
 ./core/include/AbsHMatrix.h
 ./core/include/AbsMeasurement.h
 ./core/include/AbsTrackRep.h
 ./core/include/DetPlane.h
 ./core/include/Exception.h
 ./core/include/FieldManager.h
 ./core/include/FitStatus.h
 ./core/include/IO.h
 ./core/include/Material.h
 ./core/include/MeasuredStateOnPlane.h
 ./core/include/MeasurementFactory.h
 ./core/include/MeasurementOnPlane.h
 ./core/include/MeasurementProducer.h
 ./core/include/SharedPlanePtr.h
 ./core/include/StateOnPlane.h
 ./core/include/ThinScatterer.h
 ./core/include/Tools.h
 ./core/include/TrackCand.h
 ./core/include/TrackCandHit.h
 ./core/include/Track.h
 ./core/include/TrackPoint.h
 
 ./finitePlanes/include/RectangularFinitePlane.h
 
 ./trackReps/include/AbsMaterialInterface.h
 ./trackReps/include/MaterialEffects.h
 ./trackReps/include/MplTrackRep.h
 ./trackReps/include/RKTools.h
 ./trackReps/include/RKTrackRep.h
 ./trackReps/include/StepLimits.h
 ./trackReps/include/TGeoMaterialInterface.h
 
 ./measurements/include/FullMeasurement.h
 ./measurements/include/HMatrixPhi.h
 ./measurements/include/HMatrixUV.h
 ./measurements/include/HMatrixU.h
 ./measurements/include/HMatrixUnit.h
 ./measurements/include/HMatrixV.h
 ./measurements/include/PlanarMeasurement.h
 ./measurements/include/ProlateSpacepointMeasurement.h
 ./measurements/include/SpacepointMeasurement.h
 ./measurements/include/WireMeasurement.h
 ./measurements/include/WireMeasurementNew.h
 ./measurements/include/WirePointMeasurement.h
 ./measurements/include/WireTrackCandHit.h
 
 ./fitters/include/AbsKalmanFitter.h
 ./fitters/include/DAF.h
 ./fitters/include/KalmanFitStatus.h
 ./fitters/include/KalmanFittedStateOnPlane.h
 ./fitters/include/KalmanFitter.h
 ./fitters/include/KalmanFitterInfo.h
 ./fitters/include/KalmanFitterRefTrack.h
 ./fitters/include/ReferenceStateOnPlane.h
 
 ./fields/include/ConstField.h

 ./utilities/include/HelixTrackModel.h
 ./utilities/include/MeasurementCreator.h
 ./utilities/include/mySpacepointDetectorHit.h
 ./utilities/include/mySpacepointMeasurement.h
)

Set(LINKDEF GenFit2LinkDef.h)
Set(LIBRARY_NAME GenFit2)
Set(DEPENDENCIES ${ROOT_LIBRARIES} Eve)

install(FILES

 ./core/include/Constants.h
 ./core/include/EigenMatrixTypedefs.h
 ./core/include/GFcore.h
 ./core/include/Units.h

 DESTINATION include
)

GENERATE_LIBRARY()



