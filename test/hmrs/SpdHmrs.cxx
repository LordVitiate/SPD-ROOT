/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
#include "SpdHmrs.h"

#include "SpdHmrsPoint.h"
#include "SpdHmrsGeo.h"
#include "SpdHmrsGeoPar.h"

#include "FairVolume.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "SpdDetectorList.h"
#include "SpdStack.h"

#include "FairGeoBuilder.h"
#include "FairGeoMedia.h"
#include "FairGeoMedium.h"    

#include "TClonesArray.h"
#include "TVirtualMC.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "TGeoCompositeShape.h"
#include "TGeoTube.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"

#include <iostream>
using std::cout;
using std::endl;

SpdHmrs::SpdHmrs()
  : FairDetector("SpdHmrs", kTRUE, kSpdHmrs),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1),
    fSpdHmrsPointCollection(new TClonesArray("SpdHmrsPoint"))
{
}

SpdHmrs::SpdHmrs(const char* name, Bool_t active)
  : FairDetector(name, active, kSpdHmrs),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1),
    fSpdHmrsPointCollection(new TClonesArray("SpdHmrsPoint"))
{
}

SpdHmrs::~SpdHmrs()
{
  if (fSpdHmrsPointCollection) {
    fSpdHmrsPointCollection->Delete();
    delete fSpdHmrsPointCollection;
  }
}

void SpdHmrs::Initialize()
{
  FairDetector::Initialize();
  FairRuntimeDb* rtdb= FairRun::Instance()->GetRuntimeDb();
  SpdHmrsGeoPar* par=(SpdHmrsGeoPar*)(rtdb->getContainer("SpdHmrsGeoPar"));
}

Bool_t  SpdHmrs::ProcessHits(FairVolume* vol)
{
  /** This method is called from the MC stepping */

  //Set parameters at entrance of volume. Reset ELoss.
  if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPos);
    gMC->TrackMomentum(fMom);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Create SpdHmrsPoint at exit of active volume
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {
    fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    fVolumeID = vol->getMCid();
    if (fELoss == 0. ) { return kFALSE; }
    AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
           TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength,
           fELoss);

    // Increment number of SpdHmrs det points in TParticle
    SpdStack* stack = (SpdStack*) gMC->GetStack();
    stack->AddPoint(kSpdHmrs);
  }

  return kTRUE;
}

void SpdHmrs::EndOfEvent()
{

  fSpdHmrsPointCollection->Clear();

}



void SpdHmrs::Register()
{

  /** This will create a branch in the output tree called
      SpdHmrsPoint, setting the last parameter to kFALSE means:
      this collection will not be written to the file, it will exist
      only during the simulation.
  */

  FairRootManager::Instance()->Register("SpdHmrsPoint", "SpdHmrs",
                                        fSpdHmrsPointCollection, kTRUE);

}


TClonesArray* SpdHmrs::GetCollection(Int_t iColl) const
{
  if (iColl == 0) { return fSpdHmrsPointCollection; }
  else { return NULL; }
}

void SpdHmrs::Reset()
{
  fSpdHmrsPointCollection->Clear();
}

//_______________________________________________________________________________________
void SpdHmrs::ConstructGeometry()
{
    FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    FairGeoBuilder*  geobuild = geoLoad->getGeoBuilder(); 
    FairGeoMedia*       Media = geoFace->getMedia();
    
    TGeoVolume *top = gGeoManager->GetTopVolume();  

    int np = 3;  // number of slices (along z-axis)
    int ns = 8;  // number of angular sectors
    int nl = 12; // barrel thickness (number of layers)  
      
    TGeoVolume *yoke1[nl], *yoke2[nl-1];
    TGeoTranslation trans; 
    TGeoRotation    rot; 
      
    int nmed;
    
    nmed = geobuild->createMedium(Media->getMedium("iron"));
    TGeoMedium* Iron = gGeoManager->GetMedium(nmed);
       
    nmed = geobuild->createMedium(Media->getMedium("air"));
    TGeoMedium* Gas = gGeoManager->GetMedium(nmed); 
    
    //cout << "iron: " << Iron << " ";
    //if (Iron) cout << Iron->GetName(); 
    //cout << endl;
    
    //cout << "gas: " << Gas << " ";
    //if (Gas) cout << Gas->GetName(); 
    //cout << endl;
     
    double L = 196.;
    for (int j(0); j < nl-1; j++) {     
        yoke1[j] = gGeoManager->MakeBox(FName(Form("YOKE1_%d",j)),Iron, 3./2, L/2., 225./2);
        yoke2[j] = gGeoManager->MakeBox(FName(Form("YOKE2_%d",j)),Gas, 1./2, L/2., 220./2); 
        L += 5-j*0.2; 
        AddSensitiveVolume(yoke1[j]);
        AddSensitiveVolume(yoke2[j]);
        yoke1[j]->SetLineColor(kRed);
        yoke2[j]->SetLineColor(kBlue);
    }
    yoke1[nl-1] = gGeoManager->MakeBox(FName(Form("YOKE1_%d",nl-1)),Iron, 3./2, L/2., 225./2);
    AddSensitiveVolume(yoke1[nl-1]);
    yoke1[nl-1]->SetLineColor(kRed);
    
    double br1, br2, deg;  
    double ddeg = 360./ns;
    
    for (int i(0); i < np; i++) {
         br1 = 235.; 
         br2 = 237.4;
         for (int j(0); j < nl; j++) {                 
              deg = 0;          
              int k = 0;     
              do {
                 trans.SetTranslation(br1*TMath::Cos(deg), br1*TMath::Sin(deg), 227. - i*227.);
                 top->AddNode(yoke1[j], i*nl+k+1, new TGeoCombiTrans(trans,rot));
                 if (j < nl-1) {
                     trans.SetTranslation(br2*TMath::Cos(deg), br2*TMath::Sin(deg), 227. - i*228.);                  
                     top->AddNode(yoke2[j], i*nl+k+1, new TGeoCombiTrans(trans,rot));
                 }
                 k++;
                 rot.RotateZ(ddeg);
                 deg = ddeg*k*TMath::DegToRad();     
              } 
              while (k < ns);              
              br1 += 5;
              br2 += 5;
         }  
    }  
}

TString SpdHmrs::FName(TString name) 
{   
  TString mname = GetName();
  mname += "1_" + name;
  return mname; 
}

SpdHmrsPoint* SpdHmrs::AddHit(Int_t trackID, Int_t detID,
                                      TVector3 pos, TVector3 mom,
                                      Double_t time, Double_t length,
                                      Double_t eLoss)
{
  TClonesArray& clref = *fSpdHmrsPointCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) SpdHmrsPoint(trackID, detID, pos, mom,
         time, length, eLoss);
}

ClassImp(SpdHmrs)
