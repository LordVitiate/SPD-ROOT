// $Id$
// Author: andre   2021/03/04

//_____________________________________________________________________________
//
// SpdEcalTEC2HitProducer
//_____________________________________________________________________________

#include "SpdEcalTEC2HitProducer.h"
#include "SpdEcalTX2HitProducerCommon.h"

#include <TMath.h>
#include <TClonesArray.h>
#include <TGeoManager.h>

#include "FairRuntimeDb.h"
#include "FairRun.h"

//#include "SpdEcalTECGeoMapper.h"
#include "SpdEcalTECParSet.h"
#include "SpdEcalTEC2GeoMapper.h"
#include "TGeoScaledShape.h"
#include "TGeoArb8.h"

#include "SpdEcalTEC2Point.h"
#include "SpdEcalTEC2Hit.h"

#include "SpdMCTrack.h"
#include "SpdGeoFactory.h"

#include <vector>
#include <iostream>
#include <map>

using namespace std;
namespace common = SpdEcalTX2HitProducerCommon;


ClassImp(SpdEcalTEC2HitProducer)

//_____________________________________________________________________________
SpdEcalTEC2HitProducer::SpdEcalTEC2HitProducer(TString params):
FairTask("Ideal EcalTEC hit Producer"),fEventHeader(0),
fParsName(params),fParameters(0),
fGeoMapper(0),fSaveDetIdOption(0),
fPointsArray(0),fTracksArray(0),fDigitsArray(0)
{

}

//_____________________________________________________________________________
SpdEcalTEC2HitProducer::~SpdEcalTEC2HitProducer() 
{

}

//_____________________________________________________________________________
void SpdEcalTEC2HitProducer::SetParameters() 
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
        
        SpdEcalTEC2GeoMapper* gmapper = dynamic_cast<SpdEcalTEC2GeoMapper*>(fGeoMapper);
        fAbsorberVolName = gmapper->GetAbsorberVolName();
        fScintVolName = gmapper->GetScintVolName();
    }
    
    fParameters->GetParameter("Detector/SaveDetIdOption",fSaveDetIdOption);
}

//_____________________________________________________________________________
InitStatus SpdEcalTEC2HitProducer::Init() 
{   
    // get RootManager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- <SpdEcalTEC2HitProducer::Init> "
             << "RootManager not instantiated!" << endl;
        return kFATAL;
    }

    // get Geometry parameters
    SetParameters();
    
    // get Event header
    fEventHeader = (SpdMCEventHeader*)ioman->GetObject("MCEventHeader.");
    if (!fEventHeader) {
        cout << "-E- <SpdEcalTEC2HitProducer::Init>" << "No HEADER object inside " << endl;
        return kERROR;
    }

    // get Points array
    fPointsArray = (TClonesArray*)ioman->GetObject("SpdEcalTEC2Point");
    if (!fPointsArray) {
        cout << "-W- <SpdEcalTEC2HitProducer::Init> " << "No EcalPoint array!" << endl;
        return kERROR;
    }
    
    // get Tracks array
    fTracksArray = (TClonesArray*)ioman->GetObject("SpdMCTrack");
    if (!fTracksArray) {
        cout << "-W- <SpdEcalTEC2HitProducer::Init> " << "No MCTrack array!" << endl;
        return kERROR;
    }

    // Create and register output array of Digits
    fDigitsArray = new TClonesArray("SpdEcalTEC2Hit",100);
    ioman->Register("SpdEcalTEC2Hit", "ECAL", fDigitsArray, kTRUE);
                           
    //ioman->Register("MCEventHeader.", "Event", fEventHeader, kTRUE);
    //ioman->Register("SpdMCTrack", "Track", fTracksArray, kTRUE);
    
    cout << "-I- <SpdEcalTEC2HitProducer:Init> Intialization successfull " << endl;
    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdEcalTEC2HitProducer::Finish() 
{
    cout << "\n-I- <SpdEcalTEC2HitProducer::Finish>" << endl;
}

//________________________________________________________________________________________
Double_t SpdEcalTEC2HitProducer::CellEnergyCalibration(Double_t e) {
    
    Double_t coeff = 0.269;
    if (e < 0.33) {
        coeff = 0.244638 + 0.0740879*e;
    }
    
    return 1./coeff;
}

//________________________________________________________________________________________
void SpdEcalTEC2HitProducer::Exec(Option_t* opt) 
{
    fDigitsArray->Delete();
    
    cout << "\n-I- <SpdEcalTEC2HitProducer::Exec> Started with " 
         << "tracks/points = ";
 
    if (fPointsArray && fPointsArray->GetEntriesFast() > 1) {
        cout << fTracksArray->GetEntriesFast() << "/"
             << fPointsArray->GetEntriesFast() << endl;
    }
    else {
        cout << "-W- <SpdEcalTEC2HitProducer::Exec> Empty array of points " 
             << endl;
    }
    
    if (!fDigitsArray) {
        Fatal("SpdEcalTEC2HitProducer::Exec", "No array of digits");
    }
    
    map<TString, SpdEcalTEC2Hit*> cellhits;

    for (Int_t i(0); i<fPointsArray->GetEntriesFast(); i++) {
          
         SpdEcalTEC2Point* pnt = (SpdEcalTEC2Point*)fPointsArray->At(i); 
         TString pathToCell = pnt->GetNodePathToCell(fAbsorberVolName, fScintVolName);
         
         if (cellhits.find(pathToCell) == cellhits.end()) {
            Int_t ne = fDigitsArray->GetEntriesFast();
            SpdEcalTEC2Hit* newhit = new ((*fDigitsArray)[ne]) SpdEcalTEC2Hit(pnt->GetDetectorID(), pnt->GetEndcapID(), pnt->GetModuleID(), pnt->GetCellID(), pathToCell);
            TVector3 pos = common::GetCellPosition(pathToCell);
            newhit->SetPosition(pos);
            cellhits[pathToCell] = newhit;
         }
         if (pnt->GetMaterialID() == fScintVolName) {
            cellhits[pathToCell]->AddEnergy(pnt->GetEnergyLoss());
         }
         cellhits[pathToCell]->AddMCEnergy(pnt->GetEnergyLoss());
                      
    }
         
         
    for (Int_t i(0); i<fDigitsArray->GetEntriesFast(); i++) {
        SpdEcalTEC2Hit* hit = (SpdEcalTEC2Hit*)fDigitsArray->At(i);
        hit->ScaleEnergy(CellEnergyCalibration(hit->GetEnergy()));
    }
    
    
    cout << "-I- <SpdEcalTEC2HitProducer::Exec> Hits/Points = " 
         << fDigitsArray->GetEntriesFast() 
         << "/" << fPointsArray->GetEntriesFast() << endl;
  
    

}




