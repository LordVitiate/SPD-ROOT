// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdEcalTBPointAn
//_____________________________________________________________________________

#include "SpdEcalTPointAn.h"

#include <TMath.h>
#include <TClonesArray.h>
#include <TGeoManager.h>

#include "FairRuntimeDb.h"
#include "FairRun.h"

#include "SpdFieldCreator.h"
#include "SpdFieldPar.h"
#include "SpdField.h"

//#include "SpdEcalTBGeoMapper.h"
//#include "SpdEcalTECGeoMapper.h"

#include "SpdEcalTBParSet.h"
#include "SpdEcalTECParSet.h"

#include "SpdMCTrack.h"
#include "SpdGeoFactory.h"

#include "SpdEcalTBPoint.h"
#include "SpdEcalTBHit.h"
#include "SpdEcalTECPoint.h"
#include "SpdEcalTECHit.h"

#include <vector>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalTPointAn)

//_____________________________________________________________________________
SpdEcalTPointAn::SpdEcalTPointAn():FairTask("EcalT Point Analyser"),
fGeoMapperB(0),fGeoMapperEC(0),fField(0),
fParsNameB("EcalTBParSet"),fParsNameEC("EcalTECParSet"),
fParametersB(0),fParametersEC(0),
fSaveDetIdOptionB(0),fSaveDetIdOptionEC(0),
fEventHeader(0),fTracksArray(0),
fPointsArrayB(0),fDigitsArrayB(0),
fPointsArrayEC(0),fDigitsArrayEC(0)
{

}

//_____________________________________________________________________________
SpdEcalTPointAn::~SpdEcalTPointAn() 
{

}

//_____________________________________________________________________________
void SpdEcalTPointAn::SetParameters() 
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
void SpdEcalTPointAn::SetField() 
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
InitStatus SpdEcalTPointAn::Init() 
{   
    // get RootManager
  
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- <SpdEcalTPointAn::Init> "
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
        cout << "-W- <SpdEcalTPointAn::Init>" << "No HEADER object inside " << endl;
        //return kERROR;
    }

    // get Points array (barrel)
    fPointsArrayB = (TClonesArray*)ioman->GetObject("SpdEcalTBPoint");
    
    if (!fPointsArrayB) {
        cout << "-W- <SpdEcalTPointAn::Init> " << "No EcalTBPoint array!" << endl;
        return kERROR;
    }
    
    // get Points array (endcaps)
    fPointsArrayEC = (TClonesArray*)ioman->GetObject("SpdEcalTECPoint");
    
    if (!fPointsArrayEC) {
        cout << "-W- <SpdEcalTPointAn::Init> " << "No EcalTECPoint array!" << endl;
        return kERROR;
    }
    
    // get Tracks array
    fTracksArray = (TClonesArray*)ioman->GetObject("SpdMCTrack");
    
    if (!fTracksArray) {
        cout << "-W- <SpdEcalTPointAn::Init> " << "No MCTrack array!" << endl;
        return kERROR;
    }
    
    // Create and register output array of Digits
    fDigitsArrayB = new TClonesArray("SpdEcalTBHit",100);
    ioman->Register("SpdEcalTBHit", "ECAL", fDigitsArrayB, kTRUE);
    
    fDigitsArrayEC = new TClonesArray("SpdEcalTECHit",100);
    ioman->Register("SpdEcalTECHit", "ECAL", fDigitsArrayEC, kTRUE);
                           
    cout << "-I- <SpdEcalTPointAn:Init> Intialization successfull " << endl;
    
    PrintInput();
    
    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdEcalTPointAn::PrintInput()
{
    cout << "\n\n-I- <SpdEcalTPointAn::PrintInput> " << endl;
    
    if (fEventHeader) fEventHeader->Print(1);
    cout << "\n";
    
    if (fGeoMapperB)  fGeoMapperB->Print("");
    cout << "\n";
    
    if (fGeoMapperEC) fGeoMapperEC->Print("");
    cout << "\n";
    
    if (fField) fField->Print(""); 
}

//________________________________________________________________________________________
void SpdEcalTPointAn::Finish() 
{
    cout << "\n-I- <SpdEcalTPointAn::Finish>" << endl;
}

//________________________________________________________________________________________
void SpdEcalTPointAn::Exec(Option_t* opt) 
{
    if (!fTracksArray) return;
    
    cout << "\n";
    cout << "-I- <SpdEcalTPointAn::Exec> Started with: " 
         << "tracks = " << fTracksArray->GetEntriesFast() << "; points = ";
    
    if (fPointsArrayB && fPointsArrayB->GetEntriesFast() > 1) {
        cout << " /BARREL: " << fPointsArrayB->GetEntriesFast();
    }
    else {
        cout << "\n";
        cout << "-W- <SpdEcalTPointAn::Exec> Empty array of points (B)" 
             << endl;
        return;
    }
    
    if (fPointsArrayEC && fPointsArrayEC->GetEntriesFast() > 1) {
        cout << " /ENDCAPS: "<< fPointsArrayEC->GetEntriesFast() << endl;
    }
    else {
        cout << "\n";
        cout << "-W- <SpdEcalTPointAn::Exec> Empty array of points (EC)" 
             << endl;
        return;
    }
    cout << "\n";
    
    if (!fDigitsArrayB) {
        Fatal("SpdEcalTPointAn::Exec", "No array of digits (B)");
    }
    
    if (!fDigitsArrayEC) {
        Fatal("SpdEcalTPointAn::Exec", "No array of digits (EC)");
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
    
    SpdEcalTBHit* hitb;  
    
    ne = fDigitsArrayB->GetEntriesFast();
    hitb = new((*fDigitsArrayB)[ne]) SpdEcalTBHit(0);
    
    dep = 0;
    
    SpdEcalTBPoint* pntb = 0;
    for (Int_t i(0); i<fPointsArrayB->GetEntriesFast(); i++) {
          
         pntb = (SpdEcalTBPoint*)fPointsArrayB->At(i); 
                 
         hitb->AddPoint(i);
         hitb->AddDeposit(pntb->GetEnergyLoss());  
             
         dep += pntb->GetEnergyLoss();
    }
    
    total_dep += dep; 
    
    cout << "-I- <SpdEcalTPointAn::Exec> BARREL: Hits/Points = " 
         << fDigitsArrayB->GetEntriesFast() 
         << "/" << fPointsArrayB->GetEntriesFast() 
         << "; deposit = " << dep << endl; 
         
    //---------------------------------------------
    
    SpdEcalTECHit* hitec;  
    
    ne = fDigitsArrayEC->GetEntriesFast();
    hitec = new((*fDigitsArrayEC)[ne]) SpdEcalTECHit(0);
       
    dep = 0;
     
    SpdEcalTECPoint* pntec = 0;
    for (Int_t i(0); i<fPointsArrayEC->GetEntriesFast(); i++) {
          
         pntec = (SpdEcalTECPoint*)fPointsArrayEC->At(i); 
                 
         hitec->AddPoint(i);
         hitec->AddDeposit(pntec->GetEnergyLoss());  
             
         dep += pntec->GetEnergyLoss();
    }
    
    total_dep += dep; 
         
    cout << "-I- <SpdEcalTPointAn::Exec> ENDCAPS: Hits/Points = " 
         << fDigitsArrayEC->GetEntriesFast() 
         << "/" << fPointsArrayEC->GetEntriesFast() 
         << "; deposit = " << dep << endl; 
    
    //---------------------------------------------
         
    cout << "-I- <SpdEcalTPointAn::Exec> sum. deposit = " << total_dep << endl;              
  
}


