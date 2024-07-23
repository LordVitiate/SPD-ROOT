// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdEcalSBHitProducer
//_____________________________________________________________________________

#include "SpdEcalSBHitProducer.h"

#include <TMath.h>
#include <TClonesArray.h>
#include <TGeoManager.h>

#include "FairRuntimeDb.h"
#include "FairRun.h"

//#include "SpdEcalSBGeoMapper.h"
#include "SpdEcalSBParSet.h"

#include "SpdEcalSBPoint.h"
#include "SpdEcalSBHit.h"

#include "SpdMCTrack.h"
#include "SpdGeoFactory.h"

#include <vector>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalSBHitProducer)

//_____________________________________________________________________________
SpdEcalSBHitProducer::SpdEcalSBHitProducer(TString params):
FairTask("Ideal EcalSB hit Producer"),fEventHeader(0),
fParsName(params),fParameters(0),
fGeoMapper(0),fSaveDetIdOption(0),
fPointsArray(0),fTracksArray(0),fDigitsArray(0)
{

}

//_____________________________________________________________________________
SpdEcalSBHitProducer::~SpdEcalSBHitProducer() 
{

}

//_____________________________________________________________________________
void SpdEcalSBHitProducer::SetParameters() 
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
InitStatus SpdEcalSBHitProducer::Init() 
{   
    // get RootManager
  
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- <SpdEcalSBHitProducer::Init> "
             << "RootManager not instantiated!" << endl;
        return kFATAL;
    }
 
    // get Geometry parameters
    SetParameters();
      
    // get Event header
    fEventHeader = (SpdMCEventHeader*)ioman->GetObject("MCEventHeader.");
   
    if (!fEventHeader) {
        cout << "-E- <SpdEcalSBHitProducer::Init>" << "No HEADER object inside " << endl;
        return kERROR;
    }

    // get Points array
    fPointsArray = (TClonesArray*)ioman->GetObject("SpdEcalSBPoint");
    
    if (!fPointsArray) {
        cout << "-W- <SpdEcalSBHitProducer::Init> " << "No EcalPoint array!" << endl;
        return kERROR;
    }
    
    // get Tracks array
    fTracksArray = (TClonesArray*)ioman->GetObject("SpdMCTrack");
    
    if (!fTracksArray) {
        cout << "-W- <SpdEcalSBHitProducer::Init> " << "No MCTrack array!" << endl;
        return kERROR;
    }

    // Create and register output array of Digits
    fDigitsArray = new TClonesArray("SpdEcalSBHit",100);
    ioman->Register("SpdEcalSBHit", "ECAL", fDigitsArray, kTRUE);
                           
    cout << "-I- <SpdEcalSBHitProducer:Init> Intialization successfull " << endl;
    
    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdEcalSBHitProducer::Finish() 
{
    cout << "\n-I- <SpdEcalSBHitProducer::Finish>" << endl;
}

//________________________________________________________________________________________
void SpdEcalSBHitProducer::Exec(Option_t* opt) 
{
    cout << "\n-I- <SpdEcalSBHitProducer::Exec> Started with " 
         << "tracks/points = ";
    
    if (fPointsArray && fPointsArray->GetEntriesFast() > 1) {
        cout << fTracksArray->GetEntriesFast() << "/"
             << fPointsArray->GetEntriesFast() << endl;
    }
    else {
        cout << "-W- <SpdEcalSBHitProducer::Exec> Empty array of points " 
             << endl;
        return;
    }
    
    if (!fDigitsArray) {
        Fatal("SpdEcalSBHitProducer::Exec", "No array of digits");
    }
    
    fDigitsArray->Delete();
    
    SpdEcalSBHit* hit;  
    
    Int_t ne = fDigitsArray->GetEntriesFast();
    hit = new((*fDigitsArray)[ne]) SpdEcalSBHit(0);
        
    Double_t total_dep = 0;
    
    SpdEcalSBPoint* pnt = 0;
    for (Int_t i(0); i<fPointsArray->GetEntriesFast(); i++) {
          
         pnt = (SpdEcalSBPoint*)fPointsArray->At(i); 
                 
         hit->AddPoint(i);
         hit->AddDeposit(pnt->GetEnergyLoss());  
             
         total_dep += pnt->GetEnergyLoss();
    }
     
    cout << "-I- <SpdEcalSBHitProducer::Exec> Hits/Points = " 
         << fDigitsArray->GetEntriesFast() 
         << "/" << fPointsArray->GetEntriesFast() << endl;
  
    cout << "-I- <SpdEcalSBHitProducer::Exec> TOTAL DEPOSIT = " 
         << total_dep << endl;     
}


