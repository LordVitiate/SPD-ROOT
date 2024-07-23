// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdRsSBHitProducer
//_____________________________________________________________________________

#include "SpdRsSBHitProducer.h"

#include <TMath.h>
#include <TClonesArray.h>
#include <TGeoManager.h>

#include "FairRuntimeDb.h"
#include "FairRun.h"

//#include "SpdRsSBGeoMapper.h"
#include "SpdRsSBParSet.h"

#include "SpdRsSBPoint.h"
#include "SpdRsSBHit.h"

#include "SpdMCTrack.h"
#include "SpdGeoFactory.h"

#include <vector>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdRsSBHitProducer)

//_____________________________________________________________________________
SpdRsSBHitProducer::SpdRsSBHitProducer(TString params):
FairTask("Ideal RsSB hit Producer"),fEventHeader(0),
fParsName(params),fParameters(0),
fGeoMapper(0),fSaveDetIdOption(0),
fPointsArray(0),fTracksArray(0),fDigitsArray(0)
{

}

//_____________________________________________________________________________
SpdRsSBHitProducer::~SpdRsSBHitProducer() 
{

}

//_____________________________________________________________________________
void SpdRsSBHitProducer::SetParameters() 
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
InitStatus SpdRsSBHitProducer::Init() 
{   
    // get RootManager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- <SpdRsSBHitProducer::Init> "
             << "RootManager not instantiated!" << endl;
        return kFATAL;
    }

    // get Geometry parameters
    SetParameters();
    
    // get Event header
    fEventHeader = (SpdMCEventHeader*)ioman->GetObject("MCEventHeader.");
   
    if (!fEventHeader) {
        cout << "-E- <SpdRsSBHitProducer::Init>" << "No HEADER object inside " << endl;
        return kERROR;
    }

    // get Points array
    fPointsArray = (TClonesArray*)ioman->GetObject("SpdRsSBPoint");
    
    if (!fPointsArray) {
        cout << "-W- <SpdRsSBHitProducer::Init> " << "No RsPoint array!" << endl;
        return kERROR;
    }
    
    // get Tracks array
    fTracksArray = (TClonesArray*)ioman->GetObject("SpdMCTrack");
    
    if (!fTracksArray) {
        cout << "-W- <SpdRsSBHitProducer::Init> " << "No MCTrack array!" << endl;
        return kERROR;
    }

    // Create and register output array of Digits
    fDigitsArray = new TClonesArray("SpdRsSBHit",100);
    ioman->Register("SpdRsSBHit", "ECAL", fDigitsArray, kTRUE);
                           
    cout << "-I- <SpdRsSBHitProducer:Init> Intialization successfull " << endl;
    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdRsSBHitProducer::Finish() 
{
    cout << "-I- <SpdRsSBHitProducer::Finish>" << endl;
}

//________________________________________________________________________________________
void SpdRsSBHitProducer::Exec(Option_t* opt) 
{
    cout << "-I- <SpdRsSBHitProducer::Exec> Started with " 
         << "tracks/points = ";
 
    if (fPointsArray && fPointsArray->GetEntriesFast() > 1) {
        cout << fTracksArray->GetEntriesFast() << "/"
             << fPointsArray->GetEntriesFast() << endl;
    }
    else {
        cout << "-W- <SpdRsSBHitProducer::Exec> Empty array of points " 
             << endl;
        return;
    }
    
    if (!fDigitsArray) {
        Fatal("SpdRsSBHitProducer::Exec", "No array of digits");
    }
    
    fDigitsArray->Delete();
    
    SpdRsSBHit* hit;  
    
    Int_t ne = fDigitsArray->GetEntriesFast();
    hit = new((*fDigitsArray)[ne]) SpdRsSBHit(0);
        
    Double_t total_dep = 0;
    
    SpdRsSBPoint* pnt = 0;
    for (Int_t i(0); i<fPointsArray->GetEntriesFast(); i++) {
          
         pnt = (SpdRsSBPoint*)fPointsArray->At(i); 
                 
         hit->AddPoint(i);
         hit->AddDeposit(pnt->GetEnergyLoss());  
             
         total_dep += pnt->GetEnergyLoss();
    }
     
    cout << "-I- <SpdRsSBHitProducer::Exec> Hits/Points = " 
         << fDigitsArray->GetEntriesFast() 
         << "/" << fPointsArray->GetEntriesFast() << endl; 
  
    cout << "-I- <SpdRsSBHitProducer::Exec> TOTAL DEPOSIT = " 
         << total_dep << endl;     
}




