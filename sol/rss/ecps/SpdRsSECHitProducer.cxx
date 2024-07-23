// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdRsSECHitProducer
//_____________________________________________________________________________

#include "SpdRsSECHitProducer.h"

#include <TMath.h>
#include <TClonesArray.h>
#include <TGeoManager.h>

#include "FairRuntimeDb.h"
#include "FairRun.h"

//#include "SpdRsSECGeoMapper.h"
#include "SpdRsSECParSet.h"

#include "SpdRsSECPoint.h"
#include "SpdRsSECHit.h"

#include "SpdMCTrack.h"
#include "SpdGeoFactory.h"

#include <vector>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdRsSECHitProducer)

//_____________________________________________________________________________
SpdRsSECHitProducer::SpdRsSECHitProducer(TString params):
FairTask("Ideal RsSEC hit Producer"),fEventHeader(0),
fParsName(params),fParameters(0),
fGeoMapper(0),fSaveDetIdOption(0),
fPointsArray(0),fTracksArray(0),fDigitsArray(0)
{

}

//_____________________________________________________________________________
SpdRsSECHitProducer::~SpdRsSECHitProducer() 
{

}

//_____________________________________________________________________________
void SpdRsSECHitProducer::SetParameters() 
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
InitStatus SpdRsSECHitProducer::Init() 
{   
    // get RootManager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- <SpdRsSECHitProducer::Init> "
             << "RootManager not instantiated!" << endl;
        return kFATAL;
    }

    // get Geometry parameters
    SetParameters();
    
    // get Event header
    fEventHeader = (SpdMCEventHeader*)ioman->GetObject("MCEventHeader.");
   
    if (!fEventHeader) {
        cout << "-E- <SpdRsSECHitProducer::Init>" << "No HEADER object inside " << endl;
        return kERROR;
    }

    // get Points array
    fPointsArray = (TClonesArray*)ioman->GetObject("SpdRsSECPoint");
    
    if (!fPointsArray) {
        cout << "-W- <SpdRsSECHitProducer::Init> " << "No RsPoint array!" << endl;
        return kERROR;
    }
    
    // get Tracks array
    fTracksArray = (TClonesArray*)ioman->GetObject("SpdMCTrack");
    
    if (!fTracksArray) {
        cout << "-W- <SpdRsSECHitProducer::Init> " << "No MCTrack array!" << endl;
        return kERROR;
    }

    // Create and register output array of Digits
    fDigitsArray = new TClonesArray("SpdRsSECHit",100);
    ioman->Register("SpdRsSECHit", "ECAL", fDigitsArray, kTRUE);
                           
    cout << "-I- <SpdRsSECHitProducer:Init> Intialization successfull " << endl;
    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdRsSECHitProducer::Finish() 
{
    cout << "-I- <SpdRsSECHitProducer::Finish>" << endl;
}

//________________________________________________________________________________________
void SpdRsSECHitProducer::Exec(Option_t* opt) 
{
    cout << "-I- <SpdRsSECHitProducer::Exec> Started with " 
         << "tracks/points = ";
    
    if (fPointsArray && fPointsArray->GetEntriesFast() > 1) {
        cout << fTracksArray->GetEntriesFast() << "/"
             << fPointsArray->GetEntriesFast() << endl;
    }
    else {
        cout << "-W- <SpdRsSECHitProducer::Exec> Empty array of points " 
             << endl;
        return;
    }
    
    if (!fDigitsArray) {
        Fatal("SpdRsSECHitProducer::Exec", "No array of digits");
    }
    
    fDigitsArray->Delete();
    
    SpdRsSECHit* hit;  
    
    Int_t ne = fDigitsArray->GetEntriesFast();
    hit = new((*fDigitsArray)[ne]) SpdRsSECHit(0);
        
    Double_t total_dep = 0;
    
    SpdRsSECPoint* pnt = 0;
    for (Int_t i(0); i<fPointsArray->GetEntriesFast(); i++) {
          
         pnt = (SpdRsSECPoint*)fPointsArray->At(i); 
                 
         hit->AddPoint(i);
         hit->AddDeposit(pnt->GetEnergyLoss());  
             
         total_dep += pnt->GetEnergyLoss();
    }
     
    cout << "-I- <SpdRsSECHitProducer::Exec> Hits/Points = " 
         << fDigitsArray->GetEntriesFast() 
         << "/" << fPointsArray->GetEntriesFast() << endl; 
  
    cout << "-I- <SpdRsSECHitProducer::Exec> TOTAL DEPOSIT = " 
         << total_dep << endl;     
}




