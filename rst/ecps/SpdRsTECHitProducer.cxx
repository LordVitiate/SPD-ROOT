// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdRsTECHitProducer
//_____________________________________________________________________________

#include "SpdRsTECHitProducer.h"

#include <TMath.h>
#include <TClonesArray.h>
#include <TGeoManager.h>

#include "FairRuntimeDb.h"
#include "FairRun.h"

//#include "SpdRsTECGeoMapper.h"
#include "SpdRsTECParSet.h"

#include "SpdRsTECPoint.h"
#include "SpdRsTECHit.h"

#include "SpdMCTrack.h"
#include "SpdGeoFactory.h"

#include <vector>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdRsTECHitProducer)

//_____________________________________________________________________________
SpdRsTECHitProducer::SpdRsTECHitProducer(TString params):
FairTask("Ideal RsTEC hit Producer"),fEventHeader(0),
fParsName(params),fParameters(0),
fGeoMapper(0),fSaveDetIdOption(0),
fPointsArray(0),fTracksArray(0),fDigitsArray(0)
{

}

//_____________________________________________________________________________
SpdRsTECHitProducer::~SpdRsTECHitProducer() 
{

}

//_____________________________________________________________________________
void SpdRsTECHitProducer::SetParameters() 
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
InitStatus SpdRsTECHitProducer::Init() 
{   
    // get RootManager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- <SpdRsTECHitProducer::Init> "
             << "RootManager not instantiated!" << endl;
        return kFATAL;
    }

    // get Geometry parameters
    SetParameters();
    
    // get Event header
    fEventHeader = (SpdMCEventHeader*)ioman->GetObject("MCEventHeader.");
   
    if (!fEventHeader) {
        cout << "-E- <SpdRsTECHitProducer::Init>" << "No HEADER object inside " << endl;
        return kERROR;
    }

    // get Points array
    fPointsArray = (TClonesArray*)ioman->GetObject("SpdRsTECPoint");
    
    if (!fPointsArray) {
        cout << "-W- <SpdRsTECHitProducer::Init> " << "No RsPoint array!" << endl;
        return kERROR;
    }
    
    // get Tracks array
    fTracksArray = (TClonesArray*)ioman->GetObject("SpdMCTrack");
    
    if (!fTracksArray) {
        cout << "-W- <SpdRsTECHitProducer::Init> " << "No MCTrack array!" << endl;
        return kERROR;
    }

    // Create and register output array of Digits
    fDigitsArray = new TClonesArray("SpdRsTECHit",100);
    ioman->Register("SpdRsTECHit", "ECAL", fDigitsArray, kTRUE);
                           
    cout << "-I- <SpdRsTECHitProducer:Init> Intialization successfull " << endl;
    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdRsTECHitProducer::Finish() 
{
    cout << "-I- <SpdRsTECHitProducer::Finish>" << endl;
}

//________________________________________________________________________________________
void SpdRsTECHitProducer::Exec(Option_t* opt) 
{
    cout << "-I- <SpdRsTECHitProducer::Exec> Started with " 
         << "tracks/points = ";
    
    if (fPointsArray && fPointsArray->GetEntriesFast() > 1) {
        cout << fTracksArray->GetEntriesFast() << "/"
             << fPointsArray->GetEntriesFast() << endl;
    }
    else {
        cout << "-W- <SpdRsTECHitProducer::Exec> Empty array of points " 
             << endl;
        return;
    }
    
    if (!fDigitsArray) {
        Fatal("SpdRsTECHitProducer::Exec", "No array of digits");
    }
    
    fDigitsArray->Delete();
    
    SpdRsTECHit* hit;  
    
    Int_t ne = fDigitsArray->GetEntriesFast();
    hit = new((*fDigitsArray)[ne]) SpdRsTECHit(0);
        
    Double_t total_dep = 0;
    
    SpdRsTECPoint* pnt = 0;
    for (Int_t i(0); i<fPointsArray->GetEntriesFast(); i++) {
          
         pnt = (SpdRsTECPoint*)fPointsArray->At(i); 
                 
         hit->AddPoint(i);
         hit->AddDeposit(pnt->GetEnergyLoss());  
             
         total_dep += pnt->GetEnergyLoss();
    }
     
    cout << "-I- <SpdRsTECHitProducer::Exec> Hits/Points = " 
         << fDigitsArray->GetEntriesFast() 
         << "/" << fPointsArray->GetEntriesFast() << endl; 
  
    cout << "-I- <SpdRsTECHitProducer::Exec> TOTAL DEPOSIT = " 
         << total_dep << endl;     
}




