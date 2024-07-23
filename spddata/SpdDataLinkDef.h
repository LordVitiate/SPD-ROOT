
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

//#pragma link C++ class DetectorId;

// mc
#pragma link C++ class SpdMCTrack+;
#pragma link C++ class SpdMCEventHeader+;
#pragma link C++ class SpdPrimGenData+;

// parameters
#pragma link C++ class SpdParameter+;
#pragma link C++ class SpdFieldPar+;
#pragma link C++ class SpdParSetContFact+;
#pragma link C++ class SpdPassiveGeoParSet+;
#pragma link C++ class SpdBaseParSet+;
#pragma link C++ class SpdParSet+;
#pragma link C++ class SpdPrimGenParSet+;
#pragma link C++ class SpdSetParSet+;

#pragma link C++ class SpdGeopathParser+;

// --- hits ---
#pragma link C++ class SpdHit+;
#pragma link C++ class SpdHitMCTruth+;

// ts
#pragma link C++ class SpdTsTBHit+;
#pragma link C++ class SpdTsTECHit+;

#pragma link C++ class SpdMCTrackHit+;
#pragma link C++ class SpdMCSiliconHit+;
#pragma link C++ class SpdMCStrawHit1D+;
#pragma link C++ class SpdMCStrawHit2D+;

// tof
#pragma link C++ class SpdTofMCHit+;

// ecal
#pragma link C++ class SpdEcalTBHit+;
#pragma link C++ class SpdEcalTECHit+;

#pragma link C++ class SpdEcalTB2Hit+;
#pragma link C++ class SpdEcalTEC2Hit+;

#pragma link C++ class SpdEcalHitMCTruth+;
#pragma link C++ class SpdEcalMCHit+;

// rs 
#pragma link C++ class SpdRsTBHit+;
#pragma link C++ class SpdRsTECHit+;

#pragma link C++ class SpdRsTB2Hit+;
#pragma link C++ class SpdRsTEC2Hit+;

#pragma link C++ class SpdRsHitMCTruth+;
#pragma link C++ class SpdRsMCHit+;

// aeg
#pragma link C++ class SpdAegMCHit+;

// bbc
#pragma link C++ class SpdBbcMCHit+;

// zdc
#pragma link C++ class SpdZdcMCHit+;

// --- reco ---

// event
#pragma link C++ class SpdMCEvent+;
#pragma link C++ class SpdMCParticle+;

#pragma link C++ class SpdMCParticleInfo+;

// vnt 
#pragma link C++ class SpdTrackMC+;
#pragma link C++ class SpdTrackState+;
#pragma link C++ class SpdTrackState2+;
#pragma link C++ class SpdTrackRCSeed+;
#pragma link C++ class SpdTrackRCSeedPar+;
#pragma link C++ class SpdTrackRC+;
#pragma link C++ class SpdTrackFitPar+;

#pragma link C++ class SpdTsParticle+;

#pragma link C++ class SpdVertexMC+;
#pragma link C++ class SpdVertexRC+;
#pragma link C++ class SpdVtxFitPar+;
#pragma link C++ class SpdPrimVertexFitPar+;
#pragma link C++ class SpdVertexFitPar+;
#pragma link C++ class SpdVertexFitPar2+;
#pragma link C++ class SpdVertexFitParDaughterTrack+;

// tof 
#pragma link C++ class SpdTofParticle+;

// aerogel

#pragma link C++ class SpdAegParticle+;


// ecal
#pragma link C++ class SpdEcalRCCluster+;
#pragma link C++ class SpdEcalRCParticle+;
#pragma link C++ class SpdEcalClusterMCInfo+;
#pragma link C++ class SpdEcalMCParticle+;

#pragma link C++ class SpdEcalTB2Cluster+;
#pragma link C++ class SpdEcalTB2RecoParticle+;
#pragma link C++ class SpdEcalTEC2Cluster+;
#pragma link C++ class SpdEcalTEC2RecoParticle+;

// rs 
#pragma link C++ class SpdRsMCCluster+;
#pragma link C++ class SpdRsMCParticle+;

// --- ideal --- 
#pragma link C++ class SpdIdealDataInfo+;
#pragma link C++ class SpdIdealSpacepointHit+;
#pragma link C++ class SpdIdealWirepointHit+;
#pragma link C++ class SpdIdealHit+;
#pragma link C++ class SpdIdealTrack+;
#pragma link C++ class SpdIdealTrackColl+;

#pragma link C++ class SpdKFSimpleRes+;
#pragma link C++ class SpdKFSimpleResTrack+;

// --- aux ---
#pragma link C++ class RWdata+;

#endif

