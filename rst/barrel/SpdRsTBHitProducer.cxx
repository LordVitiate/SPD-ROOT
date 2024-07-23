// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdRsTBHitProducer
//_____________________________________________________________________________

#include "SpdRsTBHitProducer.h"

#include <TMath.h>
#include <TClonesArray.h>
#include <TGeoManager.h>

#include "FairRuntimeDb.h"
#include "FairRun.h"

//#include "SpdRsTBGeoMapper.h"
#include "SpdRsTBParSet.h"

#include "SpdRsTBPoint.h"
#include "SpdRsTBHit.h"

#include "SpdMCTrack.h"
#include "SpdGeoFactory.h"

#include <vector>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdRsTBHitProducer)

//_____________________________________________________________________________
SpdRsTBHitProducer::SpdRsTBHitProducer(TString params):
FairTask("Ideal RsTB hit Producer"),fEventHeader(0),
fParsName(params),fParameters(0),
fGeoMapper(0),fSaveDetIdOption(0),
fPointsArray(0),fTracksArray(0),fDigitsArray(0)
{

}

//_____________________________________________________________________________
SpdRsTBHitProducer::~SpdRsTBHitProducer() 
{

}

//_____________________________________________________________________________
void SpdRsTBHitProducer::SetParameters() 
{   
    FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
    fParameters = (SpdParSet*)rtdb->getContainer(fParsName);
    ((FairParSet*)fParameters)->init();
  
    //fParameters->printParams();
    //fParameters->printByLabel(-1);
    //fParameters->printByLabel(-2);

    TString mapper;
    fParameters->GetParameter("Mapper",mapper); 
    fGeoMapper = SpdGeoFactory::Instance()->Mapper(mapper);
    
    if (fGeoMapper) {
       
        fGeoMapper->LoadParametersFrom(fParameters);
      
        fGeoMapper->InitGeometry();
        fGeoMapper->LockGeometry();
  
        fGeoMapper->Print("");
    }
    
    fParameters->GetParameter("Detector/SaveDetIdOption",fSaveDetIdOption);
}

//_____________________________________________________________________________
InitStatus SpdRsTBHitProducer::Init() 
{   
    // get RootManager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- <SpdRsTBHitProducer::Init> "
             << "RootManager not instantiated!" << endl;
        return kFATAL;
    }

    // get Geometry parameters
    SetParameters();
    
    // get Event header
    fEventHeader = (SpdMCEventHeader*)ioman->GetObject("MCEventHeader.");
   
    if (!fEventHeader) {
        cout << "-E- <SpdRsTBHitProducer::Init>" << "No HEADER object inside " << endl;
        return kERROR;
    }

    // get Points array
    fPointsArray = (TClonesArray*)ioman->GetObject("SpdRsTBPoint");
    
    if (!fPointsArray) {
        cout << "-W- <SpdRsTBHitProducer::Init> " << "No RsPoint array!" << endl;
        return kERROR;
    }
    
    // get Tracks array
    fTracksArray = (TClonesArray*)ioman->GetObject("SpdMCTrack");
    
    if (!fTracksArray) {
        cout << "-W- <SpdRsTBHitProducer::Init> " << "No MCTrack array!" << endl;
        return kERROR;
    }

    // Create and register output array of Digits
    fDigitsArray = new TClonesArray("SpdRsTBHit",100);
    ioman->Register("SpdRsTBHit", "ECAL", fDigitsArray, kTRUE);
                           
    cout << "-I- <SpdRsTBHitProducer:Init> Intialization successfull " << endl;
    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdRsTBHitProducer::Finish() 
{
    cout << "-I- <SpdRsTBHitProducer::Finish>" << endl;
}

//________________________________________________________________________________________
void SpdRsTBHitProducer::Exec(Option_t* opt) 
{
    cout << "-I- <SpdRsTBHitProducer::Exec> Started with " 
         << "tracks/points = ";
 
    if (fPointsArray && fPointsArray->GetEntriesFast() > 1) {
        cout << fTracksArray->GetEntriesFast() << "/"
             << fPointsArray->GetEntriesFast() << endl;
    }
    else {
        cout << "-W- <SpdRsTBHitProducer::Exec> Empty array of points " 
             << endl;
        return;
    }
    
    if (!fDigitsArray) {
        Fatal("SpdRsTBHitProducer::Exec", "No array of digits");
    }
    
    fDigitsArray->Delete();
    
    SpdRsTBHit* hit;  
    
    Int_t ne = fDigitsArray->GetEntriesFast();
    hit = new((*fDigitsArray)[ne]) SpdRsTBHit(0);
        
    Double_t total_dep = 0;
    
    SpdRsTBPoint* pnt = 0;
    for (Int_t i(0); i<fPointsArray->GetEntriesFast(); i++) {
          
         pnt = (SpdRsTBPoint*)fPointsArray->At(i); 
                 
         hit->AddPoint(i);
         hit->AddDeposit(pnt->GetEnergyLoss());  
             
         total_dep += pnt->GetEnergyLoss();
    }
     
    cout << "-I- <SpdRsTBHitProducer::Exec> Hits/Points = " 
         << fDigitsArray->GetEntriesFast() 
         << "/" << fPointsArray->GetEntriesFast() << endl; 
  
    cout << "-I- <SpdRsTBHitProducer::Exec> TOTAL DEPOSIT = " 
         << total_dep << endl;     
}




