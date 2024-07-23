// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdEcalSBPointAn
//_____________________________________________________________________________

#include "SpdEcalSPointAn.h"

#include <TMath.h>
#include <TClonesArray.h>
#include <TGeoManager.h>

#include "FairRuntimeDb.h"
#include "FairRun.h"

#include "SpdFieldCreator.h"
#include "SpdFieldPar.h"
#include "SpdField.h"

//#include "SpdEcalSBGeoMapper.h"
//#include "SpdEcalSECGeoMapper.h"

#include "SpdEcalSBParSet.h"
#include "SpdEcalSECParSet.h"

#include "SpdMCTrack.h"
#include "SpdGeoFactory.h"

#include "SpdEcalSBPoint.h"
#include "SpdEcalSBHit.h"
#include "SpdEcalSECPoint.h"
#include "SpdEcalSECHit.h"

#include <vector>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalSPointAn)

//_____________________________________________________________________________
SpdEcalSPointAn::SpdEcalSPointAn():FairTask("EcalS Point Analyser"),
fGeoMapperB(0),fGeoMapperEC(0),fField(0),
fParsNameB("EcalSBParSet"),fParsNameEC("EcalSECParSet"),
fParametersB(0),fParametersEC(0),
fSaveDetIdOptionB(0),fSaveDetIdOptionEC(0),
fEventHeader(0),fTracksArray(0),
fPointsArrayB(0),fDigitsArrayB(0),
fPointsArrayEC(0),fDigitsArrayEC(0)
{

}

//_____________________________________________________________________________
SpdEcalSPointAn::~SpdEcalSPointAn() 
{

}

//_____________________________________________________________________________
void SpdEcalSPointAn::SetParameters() 
{   
    FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
    
    TString mapper;
    
    /* ----- read barrel parameters  ----- */
     
    fParametersB = (SpdParSet*)rtdb->getContainer(fParsNameB);
    ((FairParSet*)fParametersB)->init();
  
    //fParametersB->printParams();
    //fParametersB->printByLabel(-1);
    //fParametersB->printByLabel(-2);

    fParametersB->GetParameter("Mapper",mapper); 
    fGeoMapperB = SpdGeoFactory::Instance()->Mapper(mapper);
    
    if (fGeoMapperB) {
     
        fGeoMapperB->LoadParametersFrom(fParametersB);
        
        fGeoMapperB->InitGeometry();
        fGeoMapperB->LockGeometry();
  
        fGeoMapperB->Print("");
    }
    
    fParametersB->GetParameter("Detector/SaveDetIdOption",fSaveDetIdOptionB);
    
    /*  ----- read endcaps parameters  ----- */
     
    fParametersEC = (SpdParSet*)rtdb->getContainer(fParsNameEC);
    ((FairParSet*)fParametersEC)->init();
  
    //fParametersEC->printParams();
    //fParametersEC->printByLabel(-1);
    //fParametersEC->printByLabel(-2);
    
    fParametersEC->GetParameter("Mapper",mapper); 
    fGeoMapperEC = SpdGeoFactory::Instance()->Mapper(mapper);
    
    if (fGeoMapperEC) {
   
        fGeoMapperEC->LoadParametersFrom(fParametersEC);
   
        fGeoMapperEC->InitGeometry();
        fGeoMapperEC->LockGeometry();
  
        fGeoMapperEC->Print("");
    }
    
    fParametersEC->GetParameter("Detector/SaveDetIdOption",fSaveDetIdOptionEC);
}

//_____________________________________________________________________________
void SpdEcalSPointAn::SetField() 
{         
    if (FairFieldFactory::Instance()) {
        fField = (SpdField*)FairRun::Instance()->GetField();
        return;
    }
    
    FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();  
    SpdFieldCreator* fieldCreator = new SpdFieldCreator();    
    SpdFieldPar *fpars = (SpdFieldPar*)rtdb->getContainer("SpdFieldPar");
    
    if (fpars) {
       ((FairParSet*)fpars)->init();
        fField = (SpdField*)fieldCreator->createFairField(fpars);
        //((SpdFieldPar*)fpars)->print(1);
    }
    
    //if (fField) fField->Print(""); 
}

//_____________________________________________________________________________
InitStatus SpdEcalSPointAn::Init() 
{   
    // get RootManager
  
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- <SpdEcalSPointAn::Init> "
             << "RootManager not instantiated!" << endl;
        return kFATAL;
    }
 
    // get Geometry parameters
    SetParameters();
    
    // get Field parameters
    SetField();
      
    // get Event header
    fEventHeader = (SpdMCEventHeader*)ioman->GetObject("MCEventHeader.");
  
    if (!fEventHeader) {
        cout << "-W- <SpdEcalSPointAn::Init>" << "No HEADER object inside " << endl;
        //return kERROR;
    }

    // get Points array (barrel)
    fPointsArrayB = (TClonesArray*)ioman->GetObject("SpdEcalSBPoint");
    
    if (!fPointsArrayB) {
        cout << "-W- <SpdEcalSPointAn::Init> " << "No EcalSBPoint array!" << endl;
        return kERROR;
    }
    
    // get Points array (endcaps)
    fPointsArrayEC = (TClonesArray*)ioman->GetObject("SpdEcalSECPoint");
    
    if (!fPointsArrayEC) {
        cout << "-W- <SpdEcalSPointAn::Init> " << "No EcalSECPoint array!" << endl;
        return kERROR;
    }
    
    // get Tracks array
    fTracksArray = (TClonesArray*)ioman->GetObject("SpdMCTrack");
    
    if (!fTracksArray) {
        cout << "-W- <SpdEcalSPointAn::Init> " << "No MCTrack array!" << endl;
        return kERROR;
    }
    
    // Create and register output array of Digits
    fDigitsArrayB = new TClonesArray("SpdEcalSBHit",100);
    ioman->Register("SpdEcalSBHit", "ECAL", fDigitsArrayB, kTRUE);
    
    fDigitsArrayEC = new TClonesArray("SpdEcalSECHit",100);
    ioman->Register("SpdEcalSECHit", "ECAL", fDigitsArrayEC, kTRUE);
                           
    cout << "-I- <SpdEcalSPointAn:Init> Intialization successfull " << endl;
    
    PrintInput();
    
    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdEcalSPointAn::PrintInput()
{
    cout << "\n\n-I- <SpdEcalSPointAn::PrintInput> " << endl;
    
    if (fEventHeader) fEventHeader->Print(1);
    cout << "\n";
    
    if (fGeoMapperB)  fGeoMapperB->Print("");
    cout << "\n";
    
    if (fGeoMapperEC) fGeoMapperEC->Print("");
    cout << "\n";
    
    if (fField) fField->Print(""); 
}

//________________________________________________________________________________________
void SpdEcalSPointAn::Finish() 
{
    cout << "\n-I- <SpdEcalSPointAn::Finish>" << endl;
}

//________________________________________________________________________________________
void SpdEcalSPointAn::Exec(Option_t* opt) 
{
    if (!fTracksArray) return;
    
    cout << "\n";
    cout << "-I- <SpdEcalSPointAn::Exec> Started with: " 
         << "tracks = " << fTracksArray->GetEntriesFast() << "; points = ";
    
    if (fPointsArrayB && fPointsArrayB->GetEntriesFast() > 1) {
        cout << " /BARREL: " << fPointsArrayB->GetEntriesFast();
    }
    else {
        cout << "\n";
        cout << "-W- <SpdEcalSPointAn::Exec> Empty array of points (B)" 
             << endl;
        return;
    }
    
    if (fPointsArrayEC && fPointsArrayEC->GetEntriesFast() > 1) {
        cout << " /ENDCAPS: "<< fPointsArrayEC->GetEntriesFast() << endl;
    }
    else {
        cout << "\n";
        cout << "-W- <SpdEcalSPointAn::Exec> Empty array of points (EC)" 
             << endl;
        return;
    }
    cout << "\n";
    
    if (!fDigitsArrayB) {
        Fatal("SpdEcalSPointAn::Exec", "No array of digits (B)");
    }
    
    if (!fDigitsArrayEC) {
        Fatal("SpdEcalSPointAn::Exec", "No array of digits (EC)");
    }
    
    fDigitsArrayB->Delete();
    fDigitsArrayEC->Delete();
    
    
    SpdMCTrack* track;
    Int_t detpoints[NSPDDETECTORS];
    memset(detpoints,0,NSPDDETECTORS*sizeof(Int_t));
    
    Int_t mid;
    for (Int_t i(0); i<fTracksArray->GetEntriesFast(); i++) {
         track = (SpdMCTrack*)fTracksArray->At(i);
         if (!track) continue;
         for (Int_t j(0); j<NSPDDETECTORS; j++) {
              detpoints[j] += track->GetNPoints(DetectorId(j)); 
         }
         mid = track->GetMotherId();
    }
    
    cout << "\n";
    for (Int_t j(0); j<NSPDDETECTORS; j++) 
         printf("# Detector : %4d  Points : %8d \n",j,detpoints[j]);
    cout << "\n";
    
    Double_t total_dep(0), dep;
    
    Int_t ne;
    
    //---------------------------------------------
    
    SpdEcalSBHit* hitb;  
    
    ne = fDigitsArrayB->GetEntriesFast();
    hitb = new((*fDigitsArrayB)[ne]) SpdEcalSBHit(0);
    
    dep = 0;
    
    SpdEcalSBPoint* pntb = 0;
    for (Int_t i(0); i<fPointsArrayB->GetEntriesFast(); i++) {
          
         pntb = (SpdEcalSBPoint*)fPointsArrayB->At(i); 
                 
         hitb->AddPoint(i);
         hitb->AddDeposit(pntb->GetEnergyLoss());  
             
         dep += pntb->GetEnergyLoss();
    }
    
    total_dep += dep; 
    
    cout << "-I- <SpdEcalSPointAn::Exec> BARREL: Hits/Points = " 
         << fDigitsArrayB->GetEntriesFast() 
         << "/" << fPointsArrayB->GetEntriesFast() 
         << "; deposit = " << dep << endl; 
         
    //---------------------------------------------
    
    SpdEcalSECHit* hitec;  
    
    ne = fDigitsArrayEC->GetEntriesFast();
    hitec = new((*fDigitsArrayEC)[ne]) SpdEcalSECHit(0);
       
    dep = 0;
     
    SpdEcalSECPoint* pntec = 0;
    for (Int_t i(0); i<fPointsArrayEC->GetEntriesFast(); i++) {
          
         pntec = (SpdEcalSECPoint*)fPointsArrayEC->At(i); 
                 
         hitec->AddPoint(i);
         hitec->AddDeposit(pntec->GetEnergyLoss());  
             
         dep += pntec->GetEnergyLoss();
    }
    
    total_dep += dep; 
         
    cout << "-I- <SpdEcalSPointAn::Exec> ENDCAPS: Hits/Points = " 
         << fDigitsArrayEC->GetEntriesFast() 
         << "/" << fPointsArrayEC->GetEntriesFast() 
         << "; deposit = " << dep << endl; 
    
    //---------------------------------------------
         
    cout << "-I- <SpdEcalSPointAn::Exec> sum. deposit = " << total_dep << endl;              
  
}


