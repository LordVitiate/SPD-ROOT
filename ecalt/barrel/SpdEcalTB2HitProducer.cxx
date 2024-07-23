// $Id$
// Author: andre   2021/03/04

//_____________________________________________________________________________
//
// SpdEcalTB2HitProducer
//_____________________________________________________________________________

#include "SpdEcalTB2HitProducer.h"
#include "SpdEcalTX2HitProducerCommon.h"

#include <TMath.h>
#include <TClonesArray.h>
#include <TGeoManager.h>

#include "FairRuntimeDb.h"
#include "FairRun.h"

//#include "SpdEcalTBGeoMapper.h"
#include "SpdEcalTBParSet.h"
#include "TGeoScaledShape.h"
#include "TGeoArb8.h"

#include "SpdEcalTB2Point.h"
#include "SpdEcalTB2GeoMapper.h"
#include "SpdEcalTEC2GeoMapper.h"
#include "SpdEcalTB2Hit.h"

#include "SpdMCTrack.h"
#include "SpdGeoFactory.h"

#include <vector>
#include <iostream>
#include <map>
#include <unordered_map>

using namespace std;
namespace common = SpdEcalTX2HitProducerCommon;

ClassImp(SpdEcalTB2HitProducer)

//_____________________________________________________________________________
SpdEcalTB2HitProducer::SpdEcalTB2HitProducer(TString params):
FairTask("Ideal EcalTB hit Producer"),fEventHeader(0),
fParsName(params),fParameters(0),
fGeoMapper(0),fSaveDetIdOption(0),
fPointsArray(0),fTracksArray(0), fTracksFilteredArray(0), fDigitsArray(0)
{

}

//_____________________________________________________________________________
SpdEcalTB2HitProducer::~SpdEcalTB2HitProducer() 
{

}

//_____________________________________________________________________________
void SpdEcalTB2HitProducer::SetParameters() 
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
        
        SpdEcalTB2GeoMapper* gmapper = dynamic_cast<SpdEcalTB2GeoMapper*>(fGeoMapper);
        fAbsorberVolName = gmapper->GetAbsorberVolName();
        fScintVolName = gmapper->GetScintVolName();
    }
    else {
        cout << "-F- <SpdEcalTB2HitProducer::SetParameters: mapper not found" << endl;
        exit(1);
    }
    
    fParameters->GetParameter("Detector/SaveDetIdOption",fSaveDetIdOption);
}



//_____________________________________________________________________________
InitStatus SpdEcalTB2HitProducer::Init() 
{   
    // get RootManager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- <SpdEcalTB2HitProducer::Init> "
             << "RootManager not instantiated!" << endl;
        return kFATAL;
    }
 
    // get Geometry parameters
    SetParameters();
      
    // get Event header
    fEventHeader = (SpdMCEventHeader*)ioman->GetObject("MCEventHeader.");
    if (!fEventHeader) {
        cout << "-E- <SpdEcalTB2HitProducer::Init>" << "No HEADER object inside " << endl;
        return kERROR;
    }

    // get Points array
    fPointsArray = (TClonesArray*)ioman->GetObject("SpdEcalTB2Point");
    if (!fPointsArray) {
        cout << "-W- <SpdEcalTB2HitProducer::Init> " << "No EcalPoint array!" << endl;
        return kERROR;
    }
    
    // get Tracks array
    fTracksArray = (TClonesArray*)ioman->GetObject("SpdMCTrack");
    if (!fTracksArray) {
        cout << "-W- <SpdEcalTB2HitProducer::Init> " << "No MCTrack array!" << endl;
        return kERROR;
    }

    // Create and register output array of Digits
    fDigitsArray = new TClonesArray("SpdEcalTB2Hit",100);
    ioman->Register("SpdEcalTB2Hit", "ECAL", fDigitsArray, kTRUE);
    fTracksFilteredArray = new TClonesArray("SpdMCTrack",100);
    ioman->Register("SpdMCTrackFilt", "Track", fTracksFilteredArray, kTRUE);
                           
    //ioman->Register("MCEventHeader.", "Event", fEventHeader, kTRUE);
    //ioman->Register("SpdMCTrack", "Track", fTracksArray, kTRUE);
    
    cout << "-I- <SpdEcalTB2HitProducer:Init> Intialization successfull " << endl;
    
    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdEcalTB2HitProducer::Finish() 
{
    cout << "\n-I- <SpdEcalTB2HitProducer::Finish>" << endl;
}


//________________________________________________________________________________________
Double_t SpdEcalTB2HitProducer::CellEnergyCalibration(Double_t e) {
    
    Double_t coeff = 0.269;
    if (e < 0.33) {
        coeff = 0.244638 + 0.0740879*e;
    }
    return 1./coeff;
}

//________________________________________________________________________________________
void SpdEcalTB2HitProducer::WriteFilteredTracks() {

    if (!fTracksFilteredArray) {
        Fatal("SpdEcalTB2HitProducer::Exec", "No output array of filtered tracks");
    }
    fTracksFilteredArray->Delete();
    
    Double_t barrelWidth = SpdEcalTB2GeoMapper::Instance()->GetBarrelWidth();
    Double_t barrelOuterSize = SpdEcalTB2GeoMapper::Instance()->GetBarrelOuterSize();
    Double_t fBarrelRadius = barrelOuterSize - barrelWidth;
    Double_t fECMinDist = SpdEcalTEC2GeoMapper::Instance()->GetEndcapMinDist();
    TString fVolEC = "ECALENDCAP";
    TString fVolBarrel = "EcalBasket";

    //write a filtered array of SpdMCTracks
    Int_t ntracks = fTracksArray->GetEntriesFast();
    for (Int_t itr = 0; itr < ntracks; ++itr) {
        SpdMCTrack* tr = (SpdMCTrack*)fTracksArray->At(itr);

        //skip tracks which were produced inside ECAL 
        TString volName = tr->GetStartNodeName();
        if (volName.BeginsWith(fVolEC) || volName.BeginsWith(fVolBarrel)) {
            continue;
        }
        
        //track starts outside of ECAL: skip!
        TVector3 startVtx = tr->GetStartVertex();
        if (startVtx.X()*startVtx.X() + startVtx.Y()*startVtx.Y() > fBarrelRadius*fBarrelRadius) {
            continue;
        }
        if (fabs(startVtx.Z()) > fECMinDist) {
            continue;
        }
        Int_t ntr = fTracksFilteredArray->GetEntriesFast();
        tr->SetSecondMotherId(itr);
        SpdMCTrack* newtrack = new ((*fTracksFilteredArray)[ntr]) SpdMCTrack(*tr);
        
    }

}

//________________________________________________________________________________________
void SpdEcalTB2HitProducer::Exec(Option_t* opt) 
{
    WriteFilteredTracks();

    fDigitsArray->Delete();
    cout << "\n-I- <SpdEcalTB2HitProducer::Exec> Started with " 
         << "tracks/points = ";
    
    if (fPointsArray && fPointsArray->GetEntriesFast() > 1) {
        cout << fTracksArray->GetEntriesFast() << "/"
             << fPointsArray->GetEntriesFast() << endl;
    }
    else {
        cout << "-W- <SpdEcalTB2HitProducer::Exec> Empty array of points " 
             << endl;
    }
    
    if (!fDigitsArray) {
        Fatal("SpdEcalTB2HitProducer::Exec", "No array of digits");
    }

    map<TString, SpdEcalTB2Hit*> cellhits;
        
    for (Int_t i(0); i<fPointsArray->GetEntriesFast(); i++) {
          
         SpdEcalTB2Point* pnt = (SpdEcalTB2Point*)fPointsArray->At(i); 
         TString pathToCell = pnt->GetNodePathToCell(fAbsorberVolName, fScintVolName);
         
         if (cellhits.find(pathToCell) == cellhits.end()) {
            Int_t ne = fDigitsArray->GetEntriesFast();
            SpdEcalTB2Hit* newhit = new ((*fDigitsArray)[ne]) SpdEcalTB2Hit(pnt->GetDetectorID(), pnt->GetBasketID(), pnt->GetModuleZID(), pnt->GetModulePhiID(), pnt->GetCellID(), pathToCell);
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
        SpdEcalTB2Hit* hit = (SpdEcalTB2Hit*)fDigitsArray->At(i);
        hit->ScaleEnergy(CellEnergyCalibration(hit->GetEnergy()));
    }
     
    cout << "-I- <SpdEcalTB2HitProducer::Exec> Hits/Points = " 
         << fDigitsArray->GetEntriesFast() 
         << "/" << fPointsArray->GetEntriesFast() << endl;
         
}







