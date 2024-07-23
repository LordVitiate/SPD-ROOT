// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdEcalSECHitProducer
//_____________________________________________________________________________

#include "SpdEcalSECHitProducer.h"

#include <TMath.h>
#include <TClonesArray.h>
#include <TGeoManager.h>

#include "FairRuntimeDb.h"
#include "FairRun.h"

//#include "SpdEcalSECGeoMapper.h"
#include "SpdEcalSECParSet.h"

#include "SpdEcalSECPoint.h"
#include "SpdEcalSECHit.h"

#include "SpdMCTrack.h"
#include "SpdGeoFactory.h"

#include <vector>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalSECHitProducer)

//_____________________________________________________________________________
SpdEcalSECHitProducer::SpdEcalSECHitProducer(TString params):
FairTask("Ideal EcalSEC hit Producer"),fEventHeader(0),
fParsName(params),fParameters(0),
fGeoMapper(0),fSaveDetIdOption(0),
fPointsArray(0),fTracksArray(0),fDigitsArray(0)
{

}

//_____________________________________________________________________________
SpdEcalSECHitProducer::~SpdEcalSECHitProducer() 
{

}

//_____________________________________________________________________________
void SpdEcalSECHitProducer::SetParameters() 
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
InitStatus SpdEcalSECHitProducer::Init() 
{   
    // get RootManager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- <SpdEcalSECHitProducer::Init> "
             << "RootManager not instantiated!" << endl;
        return kFATAL;
    }

    // get Geometry parameters
    SetParameters();
    
    // get Event header
    fEventHeader = (SpdMCEventHeader*)ioman->GetObject("MCEventHeader.");
   
    if (!fEventHeader) {
        cout << "-E- <SpdEcalSECHitProducer::Init>" << "No HEADER object inside " << endl;
        return kERROR;
    }

    // get Points array
    fPointsArray = (TClonesArray*)ioman->GetObject("SpdEcalSECPoint");
    
    if (!fPointsArray) {
        cout << "-W- <SpdEcalSECHitProducer::Init> " << "No EcalPoint array!" << endl;
        return kERROR;
    }
    
    // get Tracks array
    fTracksArray = (TClonesArray*)ioman->GetObject("SpdMCTrack");
    
    if (!fTracksArray) {
        cout << "-W- <SpdEcalSECHitProducer::Init> " << "No MCTrack array!" << endl;
        return kERROR;
    }

    // Create and register output array of Digits
    fDigitsArray = new TClonesArray("SpdEcalSECHit",100);
    ioman->Register("SpdEcalSECHit", "ECAL", fDigitsArray, kTRUE);
                           
    cout << "-I- <SpdEcalSECHitProducer:Init> Intialization successfull " << endl;
    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdEcalSECHitProducer::Finish() 
{
    cout << "\n-I- <SpdEcalSECHitProducer::Finish>" << endl;
}

//________________________________________________________________________________________
void SpdEcalSECHitProducer::Exec(Option_t* opt) 
{
    cout << "\n-I- <SpdEcalSECHitProducer::Exec> Started with " 
         << "tracks/points = ";
 
    if (fPointsArray && fPointsArray->GetEntriesFast() > 1) {
        cout << fTracksArray->GetEntriesFast() << "/"
             << fPointsArray->GetEntriesFast() << endl;
    }
    else {
        cout << "-W- <SpdEcalSECHitProducer::Exec> Empty array of points " 
             << endl;
        return;
    }
    
    if (!fDigitsArray) {
        Fatal("SpdEcalSECHitProducer::Exec", "No array of digits");
    }
    
    fDigitsArray->Delete();
    
    SpdEcalSECHit* hit;  
    
    Int_t ne = fDigitsArray->GetEntriesFast();
    hit = new((*fDigitsArray)[ne]) SpdEcalSECHit(0);
        
    Double_t total_dep = 0;
    
    SpdEcalSECPoint* pnt = 0;
    for (Int_t i(0); i<fPointsArray->GetEntriesFast(); i++) {
          
         pnt = (SpdEcalSECPoint*)fPointsArray->At(i); 
                 
         hit->AddPoint(i);
         hit->AddDeposit(pnt->GetEnergyLoss());  
             
         total_dep += pnt->GetEnergyLoss();
    }
     
    cout << "-I- <SpdEcalSECHitProducer::Exec> Hits/Points = " 
         << fDigitsArray->GetEntriesFast() 
         << "/" << fPointsArray->GetEntriesFast() << endl;
  
    cout << "-I- <SpdEcalSECHitProducer::Exec> TOTAL DEPOSIT = " 
         << total_dep << endl;     
}




