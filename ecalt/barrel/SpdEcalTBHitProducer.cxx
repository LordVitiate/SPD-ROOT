// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdEcalTBHitProducer
//_____________________________________________________________________________

#include "SpdEcalTBHitProducer.h"

#include <TMath.h>
#include <TClonesArray.h>
#include <TGeoManager.h>

#include "FairRuntimeDb.h"
#include "FairRun.h"

//#include "SpdEcalTBGeoMapper.h"
#include "SpdEcalTBParSet.h"

#include "SpdEcalTBPoint.h"
#include "SpdEcalTBHit.h"

#include "SpdMCTrack.h"
#include "SpdGeoFactory.h"

#include <vector>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalTBHitProducer)

//_____________________________________________________________________________
SpdEcalTBHitProducer::SpdEcalTBHitProducer(TString params):
FairTask("Ideal EcalTB hit Producer"),fEventHeader(0),
fParsName(params),fParameters(0),
fGeoMapper(0),fSaveDetIdOption(0),
fPointsArray(0),fTracksArray(0),fDigitsArray(0)
{

}

//_____________________________________________________________________________
SpdEcalTBHitProducer::~SpdEcalTBHitProducer() 
{

}

//_____________________________________________________________________________
void SpdEcalTBHitProducer::SetParameters() 
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
InitStatus SpdEcalTBHitProducer::Init() 
{   
    // get RootManager
  
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- <SpdEcalTBHitProducer::Init> "
             << "RootManager not instantiated!" << endl;
        return kFATAL;
    }
 
    // get Geometry parameters
    SetParameters();
      
    // get Event header
    fEventHeader = (SpdMCEventHeader*)ioman->GetObject("MCEventHeader.");
   
    if (!fEventHeader) {
        cout << "-E- <SpdEcalTBHitProducer::Init>" << "No HEADER object inside " << endl;
        return kERROR;
    }

    // get Points array
    fPointsArray = (TClonesArray*)ioman->GetObject("SpdEcalTBPoint");
    
    if (!fPointsArray) {
        cout << "-W- <SpdEcalTBHitProducer::Init> " << "No EcalPoint array!" << endl;
        return kERROR;
    }
    
    // get Tracks array
    fTracksArray = (TClonesArray*)ioman->GetObject("SpdMCTrack");
    
    if (!fTracksArray) {
        cout << "-W- <SpdEcalTBHitProducer::Init> " << "No MCTrack array!" << endl;
        return kERROR;
    }

    // Create and register output array of Digits
    fDigitsArray = new TClonesArray("SpdEcalTBHit",100);
    ioman->Register("SpdEcalTBHit", "ECAL", fDigitsArray, kTRUE);
                           
    cout << "-I- <SpdEcalTBHitProducer:Init> Intialization successfull " << endl;
    
    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdEcalTBHitProducer::Finish() 
{
    cout << "\n-I- <SpdEcalTBHitProducer::Finish>" << endl;
}

//________________________________________________________________________________________
void SpdEcalTBHitProducer::Exec(Option_t* opt) 
{
    cout << "\n-I- <SpdEcalTBHitProducer::Exec> Started with " 
         << "tracks/points = ";
    
    if (fPointsArray && fPointsArray->GetEntriesFast() > 1) {
        cout << fTracksArray->GetEntriesFast() << "/"
             << fPointsArray->GetEntriesFast() << endl;
    }
    else {
        cout << "-W- <SpdEcalTBHitProducer::Exec> Empty array of points " 
             << endl;
        return;
    }
    
    if (!fDigitsArray) {
        Fatal("SpdEcalTBHitProducer::Exec", "No array of digits");
    }
    
    fDigitsArray->Delete();
    
    SpdEcalTBHit* hit;  
    
    Int_t ne = fDigitsArray->GetEntriesFast();
    hit = new((*fDigitsArray)[ne]) SpdEcalTBHit(0);
        
    Double_t total_dep = 0;
    
    SpdEcalTBPoint* pnt = 0;
    for (Int_t i(0); i<fPointsArray->GetEntriesFast(); i++) {
          
         pnt = (SpdEcalTBPoint*)fPointsArray->At(i); 
                 
         hit->AddPoint(i);
         hit->AddDeposit(pnt->GetEnergyLoss());  
             
         total_dep += pnt->GetEnergyLoss();
    }
     
    cout << "-I- <SpdEcalTBHitProducer::Exec> Hits/Points = " 
         << fDigitsArray->GetEntriesFast() 
         << "/" << fPointsArray->GetEntriesFast() << endl;
  
    cout << "-I- <SpdEcalTBHitProducer::Exec> TOTAL DEPOSIT = " 
         << total_dep << endl;     
}


