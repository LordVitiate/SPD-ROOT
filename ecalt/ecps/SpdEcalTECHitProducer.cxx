// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdEcalTECHitProducer
//_____________________________________________________________________________

#include "SpdEcalTECHitProducer.h"

#include <TMath.h>
#include <TClonesArray.h>
#include <TGeoManager.h>

#include "FairRuntimeDb.h"
#include "FairRun.h"

//#include "SpdEcalTECGeoMapper.h"
#include "SpdEcalTECParSet.h"

#include "SpdEcalTECPoint.h"
#include "SpdEcalTECHit.h"

#include "SpdMCTrack.h"
#include "SpdGeoFactory.h"

#include <vector>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalTECHitProducer)

//_____________________________________________________________________________
SpdEcalTECHitProducer::SpdEcalTECHitProducer(TString params):
FairTask("Ideal EcalTEC hit Producer"),fEventHeader(0),
fParsName(params),fParameters(0),
fGeoMapper(0),fSaveDetIdOption(0),
fPointsArray(0),fTracksArray(0),fDigitsArray(0)
{

}

//_____________________________________________________________________________
SpdEcalTECHitProducer::~SpdEcalTECHitProducer() 
{

}

//_____________________________________________________________________________
void SpdEcalTECHitProducer::SetParameters() 
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
InitStatus SpdEcalTECHitProducer::Init() 
{   
    // get RootManager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- <SpdEcalTECHitProducer::Init> "
             << "RootManager not instantiated!" << endl;
        return kFATAL;
    }

    // get Geometry parameters
    SetParameters();
    
    // get Event header
    fEventHeader = (SpdMCEventHeader*)ioman->GetObject("MCEventHeader.");
   
    if (!fEventHeader) {
        cout << "-E- <SpdEcalTECHitProducer::Init>" << "No HEADER object inside " << endl;
        return kERROR;
    }

    // get Points array
    fPointsArray = (TClonesArray*)ioman->GetObject("SpdEcalTECPoint");
    
    if (!fPointsArray) {
        cout << "-W- <SpdEcalTECHitProducer::Init> " << "No EcalPoint array!" << endl;
        return kERROR;
    }
    
    // get Tracks array
    fTracksArray = (TClonesArray*)ioman->GetObject("SpdMCTrack");
    
    if (!fTracksArray) {
        cout << "-W- <SpdEcalTECHitProducer::Init> " << "No MCTrack array!" << endl;
        return kERROR;
    }

    // Create and register output array of Digits
    fDigitsArray = new TClonesArray("SpdEcalTECHit",100);
    ioman->Register("SpdEcalTECHit", "ECAL", fDigitsArray, kTRUE);
                           
    cout << "-I- <SpdEcalTECHitProducer:Init> Intialization successfull " << endl;
    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdEcalTECHitProducer::Finish() 
{
    cout << "\n-I- <SpdEcalTECHitProducer::Finish>" << endl;
}

//________________________________________________________________________________________
void SpdEcalTECHitProducer::Exec(Option_t* opt) 
{
    cout << "\n-I- <SpdEcalTECHitProducer::Exec> Started with " 
         << "tracks/points = ";
 
    if (fPointsArray && fPointsArray->GetEntriesFast() > 1) {
        cout << fTracksArray->GetEntriesFast() << "/"
             << fPointsArray->GetEntriesFast() << endl;
    }
    else {
        cout << "-W- <SpdEcalTECHitProducer::Exec> Empty array of points " 
             << endl;
        return;
    }
    
    if (!fDigitsArray) {
        Fatal("SpdEcalTECHitProducer::Exec", "No array of digits");
    }
    
    fDigitsArray->Delete();
    
    SpdEcalTECHit* hit;  
    
    Int_t ne = fDigitsArray->GetEntriesFast();
    hit = new((*fDigitsArray)[ne]) SpdEcalTECHit(0);
        
    Double_t total_dep = 0;
    
    SpdEcalTECPoint* pnt = 0;
    for (Int_t i(0); i<fPointsArray->GetEntriesFast(); i++) {
          
         pnt = (SpdEcalTECPoint*)fPointsArray->At(i); 
                 
         hit->AddPoint(i);
         hit->AddDeposit(pnt->GetEnergyLoss());  
             
         total_dep += pnt->GetEnergyLoss();
    }
     
    cout << "-I- <SpdEcalTECHitProducer::Exec> Hits/Points = " 
         << fDigitsArray->GetEntriesFast() 
         << "/" << fPointsArray->GetEntriesFast() << endl;
  
    cout << "-I- <SpdEcalTECHitProducer::Exec> TOTAL DEPOSIT = " 
         << total_dep << endl;     
}




