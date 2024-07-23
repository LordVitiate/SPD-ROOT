// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdTsSECHitProducer
//_____________________________________________________________________________

#include "SpdTsSECHitProducer.h"

#include <TMath.h>
#include <TClonesArray.h>
#include <TGeoManager.h>

#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairRun.h"

//#include "SpdTsSECGeoMapper.h"
#include "SpdTsSECParSet.h"

#include "SpdTsSECPoint.h"
#include "SpdTsSECHit.h"

#include "SpdMCTrack.h"
#include "SpdGeoFactory.h"

#include <vector>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdTsSECHitProducer)

//_____________________________________________________________________________
SpdTsSECHitProducer::SpdTsSECHitProducer(TString params):
FairTask("Ideal TsSEC hit Producer"),fEventHeader(0),
fParsName(params),fParameters(0),
fGeoMapper(0),fSaveDetIdOption(0),
fPointsArray(0),fTracksArray(0),fDigitsArray(0)
{

}

//_____________________________________________________________________________
SpdTsSECHitProducer::~SpdTsSECHitProducer() 
{

}

//_____________________________________________________________________________
void SpdTsSECHitProducer::SetParameters() 
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
InitStatus SpdTsSECHitProducer::Init() 
{   
    // get RootManager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- <SpdTsSECHitProducer::Init> "
             << "RootManager not instantiated!" << endl;
        return kFATAL;
    }

    // get Geometry parameters
    SetParameters();
    
    // get Event header
    fEventHeader = (SpdMCEventHeader*)ioman->GetObject("MCEventHeader.");
   
    if (!fEventHeader) {
        cout << "-E- <SpdTsSECHitProducer::Init>" << "No HEADER object inside " << endl;
        return kERROR;
    }

    // get Points array
    fPointsArray = (TClonesArray*)ioman->GetObject("SpdTsSECPoint");
    
    if (!fPointsArray) {
        cout << "-W- <SpdTsSECHitProducer::Init> " << "No TsPoint array!" << endl;
        return kERROR;
    }
    
    // get Tracks array
    fTracksArray = (TClonesArray*)ioman->GetObject("SpdMCTrack");
    
    if (!fTracksArray) {
        cout << "-W- <SpdTsSECHitProducer::Init> " << "No MCTrack array!" << endl;
        return kERROR;
    }

    // Create and register output array of Digits
    fDigitsArray = new TClonesArray("SpdTsSECHit",100);
    ioman->Register("SpdTsSECHit", "ECAL", fDigitsArray, kTRUE);
                           
    cout << "-I- <SpdTsSECHitProducer:Init> Intialization successfull " << endl;
    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdTsSECHitProducer::Finish() 
{
    cout << "-I- <SpdTsSECHitProducer::Finish>" << endl;
}

//________________________________________________________________________________________
void SpdTsSECHitProducer::Exec(Option_t* opt) 
{
    cout << "-I- <SpdTsSECHitProducer::Exec> Started with " 
         << "tracks/points = ";
    
    if (fPointsArray && fPointsArray->GetEntriesFast() > 1) {
        cout << fTracksArray->GetEntriesFast() << "/"
             << fPointsArray->GetEntriesFast() << endl;
    }
    else {
        cout << "-W- <SpdTsSECHitProducer::Exec> Empty array of points " 
             << endl;
        return;
    }
    
    if (!fDigitsArray) {
        Fatal("SpdTsSECHitProducer::Exec", "No array of digits");
    }
    
    fDigitsArray->Delete();
    
    SpdTsSECHit* hit;  
    
    Int_t ne = fDigitsArray->GetEntriesFast();
    hit = new((*fDigitsArray)[ne]) SpdTsSECHit(0);
        
    Double_t total_dep = 0;
    
    SpdTsSECPoint* pnt = 0;
    for (Int_t i(0); i<fPointsArray->GetEntriesFast(); i++) {
          
         pnt = (SpdTsSECPoint*)fPointsArray->At(i); 
                 
         hit->AddPoint(i);
         hit->AddDeposit(pnt->GetEnergyLoss());  
             
         total_dep += pnt->GetEnergyLoss();
    }
     
    cout << "-I- <SpdTsSECHitProducer::Exec> Hits/Points = " 
         << fDigitsArray->GetEntriesFast() 
         << "/" << fPointsArray->GetEntriesFast() << endl; 
  
    cout << "-I- <SpdTsSECHitProducer::Exec> TOTAL DEPOSIT = " 
         << total_dep << endl;     
}




