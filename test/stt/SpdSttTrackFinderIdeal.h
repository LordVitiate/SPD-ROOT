

#ifndef __SPDSTTTRACKFINDERIDEAL_H__
#define __SPDSTTTRACKFINDERIDEAL_H__

#include "SpdSttTrackFinder.h"
#include "SpdMCTrack.h"

#include "TList.h"
#include "TClonesArray.h"

class SpdTrackCand; 
class SpdSttHit;
class FairMCPoint;
//class TClonesArray;


class SpdSttTrackFinderIdeal : public SpdSttTrackFinder
{

 public:
    void GetTrackletCircular(Double_t firstX, Double_t firstY, Double_t firstR, 
			     Double_t secondX, Double_t secondY, Double_t secondR, 
			     Double_t thirdX, Double_t thirdY, Double_t thirdR, 
			     Double_t *circleRadii, Double_t *circleCentersX, 
			     Double_t *circleCentersY) const;
    
    //void ZoomTrack(Double_t &dSeed, Double_t &phiSeed, Double_t &rSeed, SpdSttTrack *track);  // not implemented
    void GetTrack(Double_t &dSeed, Double_t &phiSeed, Double_t &rSeed, Double_t &zSeed, Double_t &tanLamSeed, Int_t mcTrackNo); 
  
  /** Default constructor **/
  SpdSttTrackFinderIdeal();


  /** Standard constructor **/
  SpdSttTrackFinderIdeal(Int_t verbose);


  /** Destructor **/
  virtual ~SpdSttTrackFinderIdeal();


  /** Initialisation **/
  virtual void Init();


  /** Track finding algorithm
   ** This just reads MC truth (MCTracks and MCPoints), creates
   ** one StsTrack for each MCTrack and attaches the hits according
   ** to the MCTrack of the corresponding MCPoint
   **
   *@param mHitArray   Array of MAPS hits
   *@param trackArray  Array of CbmStsTrack
   **
   *@value Number of tracks created
   **/
  virtual Int_t DoFind(TClonesArray* trackCandArray, TClonesArray* trackArray, TClonesArray* helixHitArray);

  virtual void AddHitCollection(TClonesArray* mHitArray, TClonesArray* mPointArray) 
  { fHitCollectionList.Add(mHitArray); fPointCollectionList.Add(mPointArray); }
  
  void plotAllStraws();
  
  Bool_t putStraw(Double_t xpos, Double_t ypos, Double_t radius);

 
  void SetTubeArray(TClonesArray *tubeArray) { fTubeArray = tubeArray; };
 
  void Finish() {};

private:

  /** Arrays of MC information **/
  TClonesArray *fTubeArray; 
  TClonesArray* fMCTrackArray;

  Bool_t rootoutput;

  TList fHitCollectionList;
  TList fPointCollectionList;
  SpdSttHit* GetHitFromCollections(Int_t hitCounter);
  FairMCPoint* GetPointFromCollections(Int_t hitCounter);

  ClassDef(SpdSttTrackFinderIdeal,1);
};


#endif
