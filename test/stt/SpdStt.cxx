/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
#include "SpdStt.h"

#include "FairVolume.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairRunSim.h"
#include "FairRuntimeDb.h"

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
#include "TRefArray.h"

#include "SpdSttGeoPar.h"
#include "SpdDetectorList.h"
#include "SpdStack.h"
#include "SpdSttGeoMapper.h"
#include "SpdMCEventHeader.h"
#include "SpdSttPoint.h"

#include <iostream>

using std::cout;
using std::endl;

//_____________________________________________________________________________________
SpdStt::SpdStt() : FairDetector("SpdStt", kTRUE, kSpdStt),
 fEventNum(-1),
 fTrackID(-1),
 fVolumeID(-1),
 fSectionID(-1),
 fPosIn(),
 fPosOut(),
 fMomIn(),
 fMomOut(),
 fMass(0),
 fTime(-1.),
 fLength(-1.),
 fELoss(-1),
 fTubeVolName("undefined"),
 fTubeSecName("undefined"),
 fSpdSttPointCollection(new TClonesArray("SpdSttPoint"))
{
}

//_____________________________________________________________________________________
SpdStt::SpdStt(const char* name, Bool_t active): FairDetector(name, active, kSpdStt),
 fEventNum(-1),
 fTrackID(-1),
 fVolumeID(-1),
 fSectionID(-1),
 fPosIn(),
 fPosOut(),
 fMomIn(),
 fMomOut(),
 fMass(0),
 fTime(-1.),
 fLength(-1.),
 fELoss(-1),
 fTubeVolName("undefined"),
 fTubeSecName("undefined"),
 fSpdSttPointCollection(new TClonesArray("SpdSttPoint"))
{
}

//_____________________________________________________________________________________
SpdStt::~SpdStt() 
{
  if (fSpdSttPointCollection) {
      fSpdSttPointCollection->Delete();
      delete fSpdSttPointCollection;
      fSpdSttPointCollection = 0;
  }
}

//_____________________________________________________________________________________
void SpdStt::Initialize()
{
  // Initialize detector and fill geometry parameters
  
  cout << "\n*******************************************************************************" << endl;
  cout << "************************* SpdStt::Initialize **********************************" << endl;
  cout << "*******************************************************************************\n" << endl;
  
  FairDetector::Initialize();
  FillGeometryParameters();
  
  SpdSttGeoPar* pars = GetParameters();
  pars->printParams();
}

//_____________________________________________________________________________________
void SpdStt::Reset()
{
   //  Has to be called after each event to reset the containers  
   fSpdSttPointCollection->Clear();
}

//_____________________________________________________________________________________
void SpdStt::Register()
{
   FairRootManager::Instance()->Register("SpdSttPoint", "SpdStt", fSpdSttPointCollection, kTRUE);
}

//_____________________________________________________________________________________
Bool_t SpdStt::ProcessHits(FairVolume* vol)
{
  
  //cout << "<SpdStt::ProcessHits> " << endl;
  
  if (gMC->IsTrackEntering()) {
      fELoss  = 0.;
      fTime   = gMC->TrackTime() * 1.0e09;
      fLength = gMC->TrackLength();
      gMC->TrackPosition(fPosIn);
      gMC->TrackMomentum(fMomIn);
  }

  fELoss += gMC->Edep();

  if (gMC->IsTrackExiting()    ||
      gMC->IsTrackStop()       ||
      gMC->IsTrackDisappeared()) 
  {  
       if (fELoss == 0.) return kFALSE;  
       if (gMC->TrackCharge() == 0) return kFALSE; 
       
       TString vname = gMC->CurrentVolName();
       
       if (vname == fTubeSecName) {
         
           gMC->CurrentVolOffID(0,fSectionID);     
                 
           vname = gMC->CurrentVolPath();
           Int_t geolevel = vname.CountChar('/');
        
           fVolumeID  = -1;
                      
           for (Int_t i(0); i<geolevel; i++) {
                vname = gMC->CurrentVolOffName(i);     
                if (vname == fTubeVolName) {
                    gMC->CurrentVolOffID(i,fVolumeID);    
                    break;
                }
           }
           
           if (fVolumeID == -1) return kFALSE;
           if (fSectionID == fVolumeID) fSectionID = 0;
           
           SpdStack* stack = (SpdStack*)gMC->GetStack();
           
           gMC->TrackPosition(fPosOut);
           gMC->TrackMomentum(fMomOut);
                
           fEventNum = gMC->CurrentEvent();   
           fTrackID  = stack->GetCurrentTrackNumber();
           fMass = gMC->TrackMass();
              
         /*  cout << "<SpdStt::ProcessHits> " << fEventNum << ": " << gMC->CurrentVolPath() 
                << " track = " << fTrackID 
                << " section = " <<  fSectionID  
                << " tube = " <<  fVolumeID << endl;  */  
           
           // Add point to collection
           AddHit();
             
           // Increment number of SpdStt det points in TParticle     
           stack->AddPoint(kSpdStt);   
       }
  }

  return kTRUE;
}

//_____________________________________________________________________________________
SpdSttPoint* SpdStt::AddHit() 
{
   Double_t lpos[3], gpos[3];
   
   // local in-position
   gpos[0] = fPosIn.X();
   gpos[1] = fPosIn.Y();
   gpos[2] = fPosIn.Z();
        
   gMC->Gmtod(gpos,lpos,1);
   
   //cout << "GPOS = " << gpos[0] << " " << gpos[1] << " " << gpos[2] << endl; 
   //cout << "LPOS = " << lpos[0] << " " << lpos[1] << " " << lpos[2] << endl; 
   
   TVector3 LocPosIn(lpos[0],lpos[1],lpos[2]);
     
   // local out-position
   gpos[0] = fPosOut.X();
   gpos[1] = fPosOut.Y();
   gpos[2] = fPosOut.Z();
        
   gMC->Gmtod(gpos,lpos,1);
   
   TVector3 LocPosOut(lpos[0],lpos[1],lpos[2]);
         
   // global position
   TVector3 Pos(0.5*(fPosIn.X()+fPosOut.X()),
                0.5*(fPosIn.Y()+fPosOut.Y()),
                0.5*(fPosIn.Z()+fPosOut.Z()));     
   
   TVector3 MomIn(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz());
   TVector3 MomOut(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz());
     
   TClonesArray& clref = *fSpdSttPointCollection;
   Int_t size = clref.GetEntriesFast(); 
 
   SpdSttPoint* point = new(clref[size]) SpdSttPoint(fTrackID, fVolumeID, fSectionID,
                                                     Pos, LocPosIn, LocPosOut,  
                                                     MomIn, MomOut,                                      
                                                     fTime, fLength, fELoss, fMass);
   
   point->SetEventID(UInt_t(fEventNum));
   return point;
}

//_____________________________________________________________________________________
void SpdStt::EndOfEvent()
{
   SpdMCEventHeader* header = (SpdMCEventHeader*)FairRunSim::Instance()->GetMCEventHeader();
   header->Print();
   
   Reset();

   FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
   SpdSttGeoPar* par = (SpdSttGeoPar*)(rtdb->getContainer("SpdSttGeoPar"));
}

//_____________________________________________________________________________________
void SpdStt::ConstructGeometry()
{ 
    // Create the detector geometry
  
    SpdSttGeoMapper* mapper = SpdSttGeoMapper::Instance();
    
    if (mapper->GetGeoType() < 1) {
        cout << "<SpdStt::ConstructGeometry> Set default geometry: 1 " << endl;
        mapper->InitGeometry(1);    
    }
  
    cout << "\n*******************************************************************************" << endl;
    cout << "********************** SpdStt::ConstructGeometry ******************************" << endl;
    cout << "********************** GEOMETRY TYPE: " << mapper->GetGeoType() 
         << " ***************************************" << endl;
    cout << "*******************************************************************************\n" << endl;
    
    mapper->print(0);
 
    FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    FairGeoBuilder*  geobuild = geoLoad->getGeoBuilder(); 
    FairGeoMedia*       Media = geoFace->getMedia();
    
    TGeoVolume *top = gGeoManager->GetTopVolume();
    
    int nmed;
    
    TGeoMedium* Air = gGeoManager->GetMedium("air");
    if (!Air) {
        nmed = geobuild->createMedium(Media->getMedium("air"));
        Air = gGeoManager->GetMedium(nmed);
    }
    
    nmed = geobuild->createMedium(Media->getMedium("arco27030"));
    TGeoMedium* Gas = gGeoManager->GetMedium(nmed);
      
    nmed = geobuild->createMedium(Media->getMedium("gold"));
    TGeoMedium* Gold = gGeoManager->GetMedium(nmed);
   
    nmed = geobuild->createMedium(Media->getMedium("tungsten"));
    TGeoMedium* Tung = gGeoManager->GetMedium(nmed);
    
    nmed = geobuild->createMedium(Media->getMedium("mylar"));
    TGeoMedium* Mylar = gGeoManager->GetMedium(nmed);
    
    SpdSttStrawTube* tube = mapper->GetTube();
     
    SpdParameter* par;

    Double_t Rmax, Rmin;
    
    par = mapper->GetParameter("barrel_rmin");
    par->Value(Rmin);
    
    par = mapper->GetParameter("barrel_rmax");
    par->Value(Rmax);
     
    Double_t rmax = tube->GetTubeOutRad();
    Double_t rmin = tube->GetTubeInRad();
    Double_t hlen = tube->GetTubeHalfLength();
    
    Double_t rwire1 = 0.009;
    Double_t rwire2 = 0.010;
    
    SpdSttGeoPar* pars = GetParameters();
    
    if (pars) {  
        pars->SetParameter("wire_rmin",rwire1);
        pars->SetParameter("wire_rmax",rwire2);
    }
  
    Int_t nsections = tube->GetNSections();
        
    fTubeVolName = FName("StrawSTT");
    fTubeSecName = fTubeVolName;
  
    /* __________ STRAW TUBE  __________ */
      
    TGeoVolume* vol = gGeoManager->MakeTubs(fTubeVolName, Gas, 0, rmax, hlen, 0, 360); 
    AddSensitiveVolume(vol);
    vol->SetLineColor(kRed);
    
    TGeoVolume* vol1 = gGeoManager->MakeTubs(FName("Straw_mylar"), Mylar, rmin, rmax, hlen, 0, 360); 
    AddSensitiveVolume(vol1);
    vol1->SetLineColor(kBlue);
    
    vol->AddNode(vol1, 1, new TGeoTranslation());  

    if (nsections  != 1) {
        fTubeSecName = FName("StrawSectionSTT");
        TGeoVolume* vsec;
        Double_t z, hl;
        for (Int_t i(0); i < nsections; i++) {  
             z = tube->GetSectionSource(i);
             hl = tube->GetSectionHalfLength(i);
             vsec = gGeoManager->MakeTubs(fTubeSecName, Gas, 0, rmin, hl, 0, 360); 
             AddSensitiveVolume(vsec);
             vol->AddNode(vsec,i, new TGeoTranslation(0,0,z));  
        }              
    }
    
    // wire 
    TGeoVolume* vol3 = gGeoManager->MakeTubs(FName("Straw_wire_tung"), Tung, 0., rwire1, hlen, 0, 360); 
    AddSensitiveVolume(vol3);
    vol3->SetLineColor(kBlack);
   
    TGeoVolume* vol2 = gGeoManager->MakeTubs(FName("Straw_wire_gold"), Gold, 0., rwire2, hlen, 0, 360); 
    AddSensitiveVolume(vol2);
    vol2->SetLineColor(kOrange);

    vol2->AddNode(vol3, 1, new TGeoTranslation());  
    vol->AddNode(vol2, 1, new TGeoTranslation()); 
    
  
    //top->AddNode(vol, 1, new TGeoTranslation()); 
    //return;
    
    
    /* __________ BARREL  __________ */
    
    TGeoVolume* volx = gGeoManager->MakeTubs(FName("AssembleStrawSTT"),Air, Rmin, Rmax, hlen, 0, 360); 
    volx->SetLineColor(kBlue);
    AddSensitiveVolume(volx);
    
//     TGeoVolume* vol_in = gGeoManager->MakeTubs(FName(Form("Block_in")),Air, Rmin-0.1, Rmin, hlen, 0, 360);   
//     vol_in->SetLineColor(kBlack);  
//     top->AddNode(vol_in, 1, new TGeoTranslation()); 
//     
//     TGeoVolume* vol_out = gGeoManager->MakeTubs(FName(Form("Block_out")),Air, Rmax, Rmax+0.1, hlen, 0, 360);   
//     vol_out->SetLineColor(kBlack);  
//     top->AddNode(vol_out, 1, new TGeoTranslation());
    
    
    /* __________ BUILD GEOMETRY  __________ */
    
    Double_t x, y, z;
    Int_t nt = mapper->GetTotalTubes(); 
    
    for (Int_t i(1); i<=nt; i++) {
         tube = mapper->GetTube(i);
         tube->GetSource(x,y,z);
         volx->AddNode(vol, i, new TGeoTranslation(x,y,z));     
    }
    
    top->AddNode(volx, 1, new TGeoTranslation()); 
}

//_____________________________________________________________________________________
TString SpdStt::FName(TString name) 
{   
    TString mname = GetName();
    mname += "1_" + name;
    return mname; 
}

//_____________________________________________________________________________________
SpdSttGeoPar* SpdStt::GetParameters() 
{
    FairRun* run = FairRun::Instance();
    if (!run) return 0;
    FairRuntimeDb* rtdb = run->GetRuntimeDb(); 
    if (!rtdb) return 0;
    SpdSttGeoPar* pars = (SpdSttGeoPar*)(rtdb->getContainer("SpdSttGeoPar"));  
    return pars;
}

//_____________________________________________________________________________________
void SpdStt::FillGeometryParameters()
{
    SpdSttGeoMapper* mapper = SpdSttGeoMapper::Instance();
    SpdSttGeoPar* pars = GetParameters();
    
    pars->SetGeometryType(mapper->GetGeoType()); 
    pars->SetParameters(mapper->GetParameters());
    
    pars->setChanged();
    pars->setInputVersion(FairRun::Instance()->GetRunId(),1);
}


ClassImp(SpdStt)


