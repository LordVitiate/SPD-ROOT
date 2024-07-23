// $Id$
// Author: artur   2015/10/15

#include "SpdSttHitProducerIdeal.h"

#include "SpdSttHit.h"
#include "SpdSttHitInfo.h"
#include "SpdSttHelixHit.h"
#include "SpdSttPoint.h"
#include "SpdSttGeoMapper.h"
#include "SpdSttGeoPar.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TObjArray.h"
#include "TClonesArray.h"

#include <iostream>
#include <map>

using std::cout;
using std::endl;

//_____________________________________________________________________________________
SpdSttHitProducerIdeal::SpdSttHitProducerIdeal() :
  FairTask("Ideal STT Hit Producer",0),
  fPoints(0),fHits(0),fHitsInfo(0),fHelixHits(0),
  fSttParameters(0)
{ 
}

//_____________________________________________________________________________________
SpdSttHitProducerIdeal::~SpdSttHitProducerIdeal() 
{ 
}

//_____________________________________________________________________________________
void SpdSttHitProducerIdeal::SetParameters() 
{

  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (SpdSttGeoPar*)rtdb->getContainer("SpdSttGeoPar");
  ((FairParSet*)fSttParameters)->init();

  //fSttParameters->printParams();

  SpdSttGeoMapper* mapper = SpdSttGeoMapper::Instance();
  fSttParameters->FillParameters(mapper->GetParameters());
  mapper->InitGeometry(fSttParameters->GetGeometryType());
  
  mapper->print(0);

}

//_____________________________________________________________________________________
InitStatus SpdSttHitProducerIdeal::Init() 
{
  FairRootManager* ioman = FairRootManager::Instance();

  if (!ioman) {
      cout << "<SpdSttHitProducerIdeal::Init> RootManager not instantiated " << endl;
      return kFATAL;
  }

  // Get input array
  fPoints = (TClonesArray*)ioman->GetObject("SpdSttPoint");

  if (!fPoints) {
      cout << "<SpdSttHitProducerIdeal::Init>  No SpdSttPoint data" << endl;
      return kERROR;
  }

  // Create and register output data
  
  fHits = new TClonesArray("SpdSttHit");
  ioman->Register("SpdSttHit", "SttHit", fHits,kTRUE);

  fHitsInfo = new TClonesArray("SpdSttHitInfo");
  ioman->Register("SpdSttHitInfo", "SttHitInfo", fHitsInfo,kTRUE);
  
  fHelixHits = new TClonesArray("SpdSttHelixHit");
  ioman->Register("SpdSttHelixHit", "SttHelixHit", fHelixHits,kTRUE);
  
  SetParameters();

  Int_t geotype = SpdSttGeoMapper::Instance()->GetGeoType();
  if (geotype < 1) {
      cout << "<SpdSttHitProducerIdeal::Init> Bad geomety type: " << geotype << endl;
      return kFATAL;
  }
  cout << "\n<SpdSttHitProducerIdeal::Init> Intialisation successfull " 
       << "; geometry type: " << geotype << endl;
  
  return kSUCCESS;
}

//_____________________________________________________________________________________
void SpdSttHitProducerIdeal::Exec(Option_t* opt) 
{
   if (!fHits)      Fatal("Exec", "No Hits");
   if (!fHitsInfo)  Fatal("Exec", "No HitsInfo data");
   if (!fHelixHits) Fatal("Exec", "No HelixHits data");
  
   fHits->Delete();
   fHitsInfo->Delete();
   fHelixHits->Delete();
  
   SpdSttPoint *point = 0;
   SpdSttStrawTube* tube = 0;

   Int_t fileId(0), eventId, trackID, detID, secID; // ID's 
   
   Double_t htime, htimeError;         // hit time and error 
   Double_t eloss;                     // deposit value  
   TVector3 pos, epos;                 // tube source (pos) and sizes (epos)
   Double_t x, y, z, slen, rlen;       // tube source (x,y,z) and tube size (l,r)
   Double_t Rdistance, RdistanceError; // radial distance to hit and error
   
   Int_t mergedHits = 0;

   Int_t nPoints = fPoints->GetEntriesFast();
    
   //cout << "<SpdSttHitProducerIdeal::Exec> points = " << fPoints->GetEntriesFast() << endl;
   //return;
   
   SpdSttGeoMapper* mapper = SpdSttGeoMapper::Instance();
   
   SpdSttHit* hit;
  
   std::map< std::pair<Int_t,Int_t>, SpdSttHelixHit*> hitlist;
   std::map< std::pair<Int_t,Int_t>, SpdSttHelixHit*>::iterator it;
   
   Int_t counter = 0;
   Int_t hcounter = 0;
   
   for (Int_t iPoint = 0; iPoint < nPoints; iPoint++)  {
     
        point = (SpdSttPoint*)fPoints->At(iPoint);
        if (!point) continue;
        
        detID = point->GetDetectorID(); // tube ID
        
        tube = mapper->GetTube(detID); 
        if (!tube) continue; 
        
        secID = point->GetSectionID();  // tube section ID
        
        slen = tube->GetSectionHalfLength(secID);
        if (slen <= 0.) continue;
        rlen = tube->GetTubeInRad();
        if (rlen <= 0.) continue;
        
        eventId = point->GetEventID();    // event number
        trackID = point->GetTrackID();    // track number
        
        Rdistance = point->GetRadialDistance();
        if (Rdistance > rlen) continue;
        
        RdistanceError = 0;
      
        htime = point->GetTime();
        htimeError = 0;
        
        eloss = point->GetEnergyLoss() * 1e6;
        
        tube->GetSectionSource(secID,x,y,z);
        
        pos.SetXYZ(x,y,z);
        epos.SetXYZ(rlen,rlen,slen);
        
        new ((*fHitsInfo)[counter]) SpdSttHitInfo(fileId, eventId, trackID, iPoint, kFALSE);
      
        hit = new ((*fHits)[counter]) SpdSttHit(detID, secID, iPoint, 
                                                pos, epos, 
                                                Rdistance, RdistanceError, 
                                                htime, htimeError,
                                                eloss);
     
        
        it = hitlist.find(std::pair<Int_t,Int_t>(detID,secID));
        if (it != hitlist.end()) it->second->AddDeposit(counter,eloss);
        else {
             hitlist.insert(std::make_pair< std::pair<Int_t,Int_t>, SpdSttHelixHit*> (
                                            std::pair<Int_t,Int_t>(detID,secID), 
                                            new ((*fHelixHits)[hcounter]) SpdSttHelixHit(counter,hit))                                       
                       );
             hcounter++;
        }
        counter++;
    }
    
    ProcessHelixHits();
    
    cout << "<SpdSttHitProducerIdeal::Exec> [hits, helix hits] = " 
         << " [" << counter  << "," << hcounter <<"] "
         << " hits has created from " << nPoints << " points " 
         << endl;
}

//_____________________________________________________________________________________
void SpdSttHitProducerIdeal::ProcessHelixHits() {
    //
  
    if (!fHits || !fHelixHits) return;
    
    Int_t hen = fHits->GetEntriesFast();
    if (hen < 1) return;
    
    Int_t hhen = fHelixHits->GetEntriesFast();
    if (hhen < 1) return;
    
    SpdSttHit* hit;
    SpdSttHelixHit* hhit;
   
    Double_t R = 0;
    Int_t nR = 0;
    
    Bool_t dump = 0;
    
    for (Int_t i(0); i<hhen; i++) {
         nR = 0;
         R = 0;
         hhit = (SpdSttHelixHit*)fHelixHits->At(i);
         if (dump) cout << i << " ";  // print
         if (hhit) { 
             for (Int_t j(0); j<hhit->NHits(); j++) {
                  hit = (SpdSttHit*)fHits->At(hhit->HitID(j));
                  if (hit) {
                      if (dump) hit->Print();  // print
                      R += hit->Radial();
                      nR++;
                  }
             }
             if (nR > 0) hhit->SetRadials(R/nR,0);
             if (dump) hhit->Print(); // print
         }
        
         
         if (dump) cout << endl; // print
    }
    
}


ClassImp(SpdSttHitProducerIdeal)




