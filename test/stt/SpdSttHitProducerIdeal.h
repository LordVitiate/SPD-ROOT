// $Id$
// Author: artur   2015/10/15

#ifndef __SPDSTTHITPRODUCERIDEAL_H__
#define __SPDSTTHITPRODUCERIDEAL_H__ 

#include "FairTask.h"
#include <TVector3.h>

#include "SpdSttGeoPar.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdSttHitProducerIdeal                                                     //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
 
class TClonesArray;

class SpdSttHitProducerIdeal : public FairTask {
  
public:
 
  SpdSttHitProducerIdeal();
  virtual ~SpdSttHitProducerIdeal();

  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  
  void SetParameters();
  
protected:

  /* Input */ 
  TClonesArray* fPoints;      //! SpdSttPoints 

  /* Output */
  TClonesArray* fHits;        //! SpdSttHits 
  TClonesArray* fHitsInfo;    //! SpdSttHitInfo
  TClonesArray* fHelixHits;   //! SpdSttHelixHit

  SpdSttGeoPar *fSttParameters;  
  
  virtual void ProcessHelixHits();
  
  ClassDef(SpdSttHitProducerIdeal,1);

};

#endif
