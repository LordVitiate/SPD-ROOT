
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class SpdRootFileSink+;
#pragma link C++ class SpdTask+;
#pragma link C++ class SpdParticlesAndProcesses+;

// simulations (base)
#pragma link C++ class SpdRunSim+;
#pragma link C++ class SpdStack+;
#pragma link C++ class SpdModulesMap+;
#pragma link C++ class SpdCommonGeoMapper+;
#pragma link C++ class SpdPassiveModule+;
#pragma link C++ class SpdDetector+;
#pragma link C++ class SpdDetector::DetVolume+;

// analysis (base)
#pragma link C++ class SpdRunAna+;

// geometry
#pragma link C++ class SpdGeoVolPars+;
#pragma link C++ class SpdGeoBuilder+;
#pragma link C++ class SpdGeoMapper+;
#pragma link C++ class SpdGeoTableRec+;
#pragma link C++ class SpdGeoTable+;
#pragma link C++ class SpdNodesIdTable+;

#pragma link C++ class SpdGeoVVolume+;
#pragma link C++ class SpdGeoVVolumeBox2D+;

// region
#pragma link C++ class SpdRegion+;
#pragma link C++ class SpdBoxRegion+;
#pragma link C++ class SpdTubeRegion+;
#pragma link C++ class SpdPhysicalRegion+;

// checks
#pragma link C++ class SpdGeoScanner+;
#pragma link C++ class SpdGeoScanData+;

#endif
