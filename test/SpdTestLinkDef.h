#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// barrel
#pragma link C++ class SpdBarrel+;
#pragma link C++ class SpdBarrelGeoPar+;
#pragma link C++ class SpdBarrelContFact+;

// simple
#pragma link C++ class TestDetector+;
#pragma link C++ class SpdSmplGeoRoot+;
//#pragma link C++ class SpdCaveGeoTest+;

// hmrs
#pragma link C++ class SpdHmrsContFact;
#pragma link C++ class SpdHmrsGeo;
#pragma link C++ class SpdHmrsGeoPar;
#pragma link C++ class SpdHmrs;
#pragma link C++ class SpdHmrsPoint+;

// sts
#pragma link C++ class SpdStsContFact;
#pragma link C++ class SpdSts;
#pragma link C++ class SpdStsGeo;
#pragma link C++ class SpdStsGeoPar+;
#pragma link C++ class SpdStsPoint+;

// stt
#pragma link C++ class SpdStt+;
#pragma link C++ class SpdSttContFact+;
#pragma link C++ class SpdSttHitProducerIdeal+;
#pragma link C++ class SpdSttTrackFinder+;
// #pragma link C++ class SpdSttFindTracks+;
// #pragma link C++ class SpdSttTrackFinderIdeal+;
// #pragma link C++ class SpdSttTrackMatch+;
// #pragma link C++ class SpdSttMatchTracks+;
#pragma link C++ class SpdSttHelixHit+;
#pragma link C++ class SpdSttHit+;    
#pragma link C++ class SpdSttHitInfo+; 
#pragma link C++ class SpdSttPoint+;
#pragma link C++ class SpdSttTrack+;
#pragma link C++ class SpdSttGeoPar;
#pragma link C++ class SpdSttStrawTube;
#pragma link C++ class SpdSttStrawTubeA;
#pragma link C++ class SpdSttGeoMapper+; 

// eclacell
#pragma link C++ class EcalCell;
#pragma link C++ class EcalCellEventHeader;
#pragma link C++ class EcalCellHits;
#pragma link C++ class EcalCellPoint;
#pragma link C++ class EcalCellContFact;

// tracker
#pragma link C++ class Tracker;
#pragma link C++ class TrackerPoint;
#pragma link C++ class TrackerContFact;

#endif
