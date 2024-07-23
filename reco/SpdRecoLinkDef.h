
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class SpdMCEventHelper+;
#pragma link C++ class SpdMCEventMaker+;

// vnt
#pragma link C++ class SpdMvdMCHitProducer+;
#pragma link C++ class SpdItsMCHitProducer+;
#pragma link C++ class SpdTsMCHitProducer+;
#pragma link C++ class SpdMCTracksFinder+;
#pragma link C++ class SpdMCVerticesFitter+;
#pragma link C++ class SpdRCVerticesFinder+;
#pragma link C++ class SpdRCKFpartV0Finder+;
#pragma link C++ class SpdTrackFitterGF+;
#pragma link C++ class SpdTrackPropagatorGF+;
#pragma link C++ class SpdVertexCombiFinder+;
#pragma link C++ class SpdMCTsParticleProducer+;
#pragma link C++ class SpdRCTrackSeedsFinder+;
#pragma link C++ class SpdRCTracksFinder+;
#pragma link C++ class SpdRCTrackFitterGF+;

// tof
#pragma link C++ class SpdTofMCHitProducer+;
#pragma link C++ class SpdMCTofParticleProducer+;

// ecal 
#pragma link C++ class SpdEcalMCHitProducer+;
#pragma link C++ class SpdEcalRCMaker+;
#pragma link C++ class SpdEcalClusterMCInfoMaker+;
#pragma link C++ class SpdEcalPiGammaSeparator+;

// rs 
#pragma link C++ class SpdRsMCHitProducer+;
#pragma link C++ class SpdRsMCClusterMaker+;

// bbc
#pragma link C++ class SpdBbcMCHitProducer+;

// aeg
#pragma link C++ class SpdAegMCHitProducer+;
#pragma link C++ class SpdMCAegParticleProducer+;

// zdc
#pragma link C++ class SpdZdcMCHitProducer+;

// ideal
#pragma link C++ class SpdIdealTrackFinder+;
#pragma link C++ class SpdIdealKalmanFitter+;
#pragma link C++ class SpdIdealV0Finder+;

// tools
#pragma link C++ class SpdMCDataIterTask+;
#pragma link C++ class SpdMCDataIterator+;
#pragma link C++ class SpdMCDataReaderSimple+;

#endif
