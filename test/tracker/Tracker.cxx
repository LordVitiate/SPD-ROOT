
#include "Tracker.h"

#include "FairVolume.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoBuilder.h"
#include "FairGeoMedia.h"
#include "FairRun.h"
#include "FairRunSim.h"
#include "FairRuntimeDb.h"

#include "TRefArray.h"    
#include "TClonesArray.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "TGeoCompositeShape.h"
#include "TGeoTube.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"

#include "SpdCommonGeoMapper.h"
#include "TrackerPoint.h"
#include "SpdDetectorList.h"
#include "SpdStack.h"

#include <map>
#include <iostream>

#include <TMath.h>
using namespace TMath;

using namespace std;

using std::cout;
using std::endl;

ClassImp(Tracker)

//_____________________________________________________________________________
Tracker::Tracker() :
  SpdDetector("Tracker", kTRUE, kTestDetector),
  fTrackID(-1),
  fVolumeID1(-1),
  fVolumeID2(-1),
  fPos(),fMom(),fTime(-1.),
  fPosOut(),fMomOut(),fTimeOut(-1.),
  fLength(-1.),
  fSegmentLength(-1.),
  fELoss(-1),
  fNSteps(0),
  fMaxSimuStep(-1.),
  fPointCollection(0),
  fEnum(0),
  fGeoType(0),
  fBaseMaterial("vacuum"),
  fActiveMaterial("vacuum")
{
    SetDefaultGeometryParameters();
  
    SetParametersType("TrackerParSet");
  
    fNDataOut = 1;
    fOutDataPointObject = "TrackerPoint";
    
    SaveDetIdOption(0);
}

//_____________________________________________________________________________
Tracker::Tracker(const char* name, Bool_t active) :
  SpdDetector(name, active, kTestDetector),
  fTrackID(-1),
  fVolumeID1(-1),
  fVolumeID2(-1),
  fPos(),fMom(),fTime(-1.),
  fPosOut(),fMomOut(),fTimeOut(-1.),
  fLength(-1.),
  fSegmentLength(-1.),
  fELoss(-1),
  fNSteps(0),
  fMaxSimuStep(-1.),
  fPointCollection(0),
  fEnum(0),
  fGeoType(0),
  fBaseMaterial("air"),
  fActiveMaterial("air")
{  
    SetDefaultGeometryParameters();
  
    SetParametersType("TrackerParSet");
   
    fNDataOut = 1;
    fOutDataPointObject = "TrackerPoint";
    
    SaveDetIdOption(0);
}

//_____________________________________________________________________________
Tracker::~Tracker()
{
  if (fPointCollection) {
      fPointCollection->Delete();
      delete fPointCollection;
      fPointCollection = 0;
  }
}
//_____________________________________________________________________________________
void Tracker::Initialize()
{
  cout << "\n*******************************************************************************" << endl;
  cout << "************************* Tracker::Initialize *********************************" << endl;
  cout << "*******************************************************************************\n" << endl;
  
  // data collections
  fPointCollection = new TClonesArray(fOutDataPointObject);
  
  // Initialize module and fill parameters
  SpdDetector::Initialize(); 
 
  // SpdParSet* pars = GetParameters();
  // if (pars) pars->printParams();  
}

//_____________________________________________________________________________________
void Tracker::Reset()
{
  fPointCollection->Clear();
}

//_____________________________________________________________________________________
void Tracker::Register()
{
  if (fGeoMapper && fGeoMapper->GetGeoType() < 1) return;
    
  FairRootManager::Instance()->Register(fOutDataPointObject,"Tracker", 
                                        fPointCollection, kTRUE);
}

//_____________________________________________________________________________
void Tracker::BeginEvent() 
{
  fEnum++;
  
  //cout << "-I- <Tracker::BeginEvent> Event : " << fEnum << "\n";
}

//_____________________________________________________________________________________
Bool_t Tracker::ProcessHits(FairVolume* vol)
{
  //cout << "<Tracker::ProcessHits> " << gMC->CurrentVolName() << endl;
 
  //Set parameters at entrance of volume. Reset ELoss.
  if (gMC->IsTrackEntering()) {
      fNSteps = 0;
      fELoss  = 0.;
      fTime   = gMC->TrackTime() * 1.e9;  // ns -> s
      fLength = gMC->TrackLength();
      gMC->TrackPosition(fPos);
      gMC->TrackMomentum(fMom);
      
      if (fMaxSimuStep > 1e-9) gMC->SetMaxStep(fMaxSimuStep); //cm 
  }
  
  //cout << " <Tracker::ProcessHits> Track: " << gMC->GetStack()->GetCurrentTrackNumber() 
  //     << " volume: " << gMC->CurrentVolName() << endl;
       
  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();
  fNSteps++;
  
  // Create TrackerPoint at exit of active volume
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) 
  {
    
    if (!fSaveEmptyHits && fELoss == 0.) { return kFALSE; }
    
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    
    fTimeOut = gMC->TrackTime() * 1.e9;  // ns -> s
  
    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);
    
    fVolumeID1 = 0;
    fVolumeID2 = 0;
    
    TString vname = gMC->CurrentVolOffName(0);
    
    if (!vname.Contains("cave")) 
    {
        if (vname.Contains("Endcap")) {
            if (vname.Contains("Zminus")) fVolumeID1 = -2;
            else if (vname.Contains("Zplus")) fVolumeID1 = 2;
        }
        else if (vname.Contains("Barrel")){ 
            if (vname.Contains("Zminus")) fVolumeID1 = -1;
            else if (vname.Contains("Zplus")) fVolumeID1 = 1;
        }

        gMC->CurrentVolOffID(0,fVolumeID2);
    }
   
    fSegmentLength = gMC->TrackLength()-fLength;
  
    TGeoMedium* cmed = gGeoManager->GetMedium(gMC->CurrentMedium());
    
    TString path = gMC->CurrentVolPath();
    
    //cout << "path: " << gMC->CurrentVolPath() << "; VOLUME: " << vname 
    //     << "; DETECTOR(ID): " << kTestDetector << endl;
         
    //cout << "ID: "<< fVolumeID1 << "/" << fVolumeID2 << " eloss = " << fELoss*1e6 << " MeV " 
    //     << "  steps = " << fNSteps 
    //     << "  segment length = " << fSegmentLength << " cm " 
    //     << "  z = " << fPos.Z() << " cm " 
    //     << "  medium = " << cmed->GetName()
    //     << endl << endl;
            
    AddHit();
    
    //cout << "<Tracker::AddHit> " << fPointCollection->GetEntriesFast() << endl;
    
    SpdStack* stack = (SpdStack*)gMC->GetStack();
    stack->AddPoint(kTestDetector);
  } 
   
  return kTRUE;
}

//_____________________________________________________________________________________
void Tracker::AddHit() 
{
  static Bool_t addhit = (fOutDataPointObject == "TrackerPoint") ? kTRUE : kFALSE;
  
  if (!addhit) return;
  
  TString nodepath = gMC->CurrentVolPath();
  
  if (nodepath.BeginsWith("/cave_1")) 
      nodepath.ReplaceAll("/cave_1",""); 
  else return;
    
  if (fPointCollection) 
  {
      TClonesArray& clref = *fPointCollection;
      Int_t size = clref.GetEntriesFast();
  
      TrackerPoint* point = new(clref[size]) TrackerPoint(fTrackID,fDetId,
                             TVector3(fPos.X(),fPos.Y(),fPos.Z()),
                             TVector3(fMom.Px(),fMom.Py(),fMom.Pz()),
                             fTime, fLength, fELoss); 
      point->SetNode(nodepath);
      point->SetFirstNodeId(fVolumeID1);
      point->SetSecondNodeId(fVolumeID2);
      point->SetEventID(fEnum);
      point->SetSegmentLength(fSegmentLength);
      point->SetPositionOut(fPosOut.X(),fPosOut.Y(),fPosOut.Z());
      point->SetMomentumOut(fMomOut.Px(),fMomOut.Py(),fMomOut.Pz());
      point->SetTimeOut(fTimeOut);
  }
}

//_____________________________________________________________________________________
void Tracker::EndOfEvent()
{
   Reset();
}

//_____________________________________________________________________________________
void Tracker::FinishRun() 
{
   //SpdDetector::FinishRun();
   FillNodesTableIn(GetParameters());
}

//_____________________________________________________________________________________
void Tracker::ConstructGeometry()
{   
  // Create detector geometry
  
  fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();
  
  if (!fMasterVolume) {  
      cout << "-E- <Tracker::ConstructGeometry> No MASTER volume " << endl;
      return;
  }
  
       if (fGeoType == 1) ConstructGeometry_1();
  else if (fGeoType == 2) ConstructGeometry_2();
  else if (fGeoType == 3) ConstructGeometry_3();
  else {
      cout << "-F- <Tracker::ConstructGeometry> Bad geometry type: " << fGeoType << endl;
      exit(1);
  }
  
  cout << "\n*******************************************************************************" << endl;
  cout << "********************* Tracker::ConstructGeometry ******************************" << endl;
  cout << "********************* GEOMETRY TYPE (DETECTOR): " 
                       << fGeoType << " *****************************" << endl;
  cout << "*******************************************************************************\n" << endl;  
}

//________________________________________________________________________________
void Tracker::ConstructGeometry_1()
{  
   cout << "-I- <Tracker::ConstructGeometry_1> " << endl;
   
//    TGeoVolume* Target = BuildTarget();
//    
//    fMasterVolume->AddNode(Target,1);
//    
//    if (fNslices == 1) {
//        AddSensitiveVolume(Target);
//        return;
//    }
      
   TGeoVolume* Target = fMasterVolume;
  
   Double_t rmin, rmax, zmin, zmax;
   Double_t dr = Abs(fRmax-fRmin)/fNslices;
   Double_t dz = Abs(fZmax-fZmin)/fNslices;
   
   TGeoMedium* material = FindMedium(fActiveMaterial,"air");   
   TGeoVolume* vol;
   
   for (Int_t i(0); i<fNslices; i++) {
       
        rmin = fRmin + i*dr;
        zmin = fZmin + i*dz;
       
        vol = gGeoManager->MakeTube(Form("BarrelZplus%d",i+1),material,rmin,rmin+dr,0.5*zmin);
        
        vol->SetFillColor(kOrange+9);
        vol->SetLineColor(kOrange+9);
        vol->SetTransparency(90);
   
        Target->AddNode(vol,i+1,new TGeoTranslation(0,0,0.5*zmin));
        AddSensitiveVolume(vol);
        
        vol = gGeoManager->MakeTube(Form("BarrelZminus%d",i+1),material,rmin,rmin+dr,0.5*zmin);
        
        vol->SetFillColor(kOrange+9);
        vol->SetLineColor(kOrange+9);
        vol->SetTransparency(90);
   
        Target->AddNode(vol,i+1,new TGeoTranslation(0,0,-0.5*zmin));
        AddSensitiveVolume(vol);
   }
   
   for (Int_t i(0); i<fNslices; i++) {
       
        rmax = fRmin + (i+1)*dr;
        zmax = fZmin + i*dz;
        
        vol = gGeoManager->MakeTube(Form("EndcapZplus%d",i+1),material,0,rmax,0.5*dz);
        
        vol->SetFillColor(kRed-5);
        vol->SetLineColor(kRed-5);
        vol->SetTransparency(80);
   
        Target->AddNode(vol,i+1,new TGeoTranslation(0,0,zmax+0.5*dz));
        AddSensitiveVolume(vol);
        
        vol = gGeoManager->MakeTube(Form("EndcapZminus%d",i+1),material,0,rmax,0.5*dz);
        
        vol->SetFillColor(kRed-5);
        vol->SetLineColor(kRed-5);
        vol->SetTransparency(80);
   
        Target->AddNode(vol,i+1,new TGeoTranslation(0,0,-(zmax+0.5*dz)));
        AddSensitiveVolume(vol);
   }
}

//________________________________________________________________________________
void Tracker::ConstructGeometry_2()
{ 
   cout << "-I- <Tracker::ConstructGeometry_2> " << endl;
    
//    TGeoVolume* Target = BuildTarget();
//    
//    fMasterVolume->AddNode(Target,1);
//    
//    if (fNslices == 1) {
//        AddSensitiveVolume(Target);
//        return;
//    }
    
   TGeoVolume* Target = fMasterVolume;
   
   Double_t zshift;
   Double_t dz = Abs(fZmax-fZmin)/fNslices;
   
   TGeoMedium* material = FindMedium(fActiveMaterial,"aluminium"); 
   TGeoVolume* vol1 = gGeoManager->MakeTube("EndcapZplus",material,fRmin,fRmax,0.5*dz);
   TGeoVolume* vol2 = gGeoManager->MakeTube("EndcapZminus",material,fRmin,fRmax,0.5*dz);
 
   Int_t vcolor = kMagenta-8;
  
   vol1->SetFillColor(vcolor);
   vol1->SetLineColor(vcolor);
   vol1->SetTransparency(90);
        
   vol2->SetFillColor(vcolor);
   vol2->SetLineColor(vcolor);
   vol2->SetTransparency(90);
        
   for (Int_t i(0); i<fNslices; i++) {
       
        zshift = fZmin + (i+0.5)*dz;
        Target->AddNode(vol1,i+1,new TGeoTranslation(0,0, zshift));
        Target->AddNode(vol2,i+1,new TGeoTranslation(0,0,-zshift));
   }
   
   AddSensitiveVolume(vol1);
   AddSensitiveVolume(vol2);
}

//________________________________________________________________________________
void Tracker::ConstructGeometry_3()
{ 
   cout << "-I- <Tracker::ConstructGeometry_3> " << endl;
   
//    TGeoVolume* Target = BuildTarget();
//    
//    fMasterVolume->AddNode(Target,1);
//    
//    if (fNslices == 1) {
//        AddSensitiveVolume(Target);
//        return;
//    }
   
   TGeoVolume* Target = fMasterVolume;
   
   Double_t rmin;
   Double_t dr = Abs(fRmax-fRmin)/fNslices;
   Double_t dz = 0.5*Abs(fZmax-fZmin);
   
   TGeoMedium* material = FindMedium(fActiveMaterial,"air");   
   TGeoVolume* vol;
   
   for (Int_t i(0); i<fNslices; i++) {
       
        rmin = fRmin + i*dr;

        vol = gGeoManager->MakeTube(Form("BarrelZplus%d",i+1),material,rmin,rmin+dr,dz);
        
        vol->SetFillColor(kOrange+9);
        vol->SetLineColor(kOrange+9);
        vol->SetTransparency(90);
        
        Target->AddNode(vol,i+1,new TGeoTranslation(0,0,fZmax-dz));
        AddSensitiveVolume(vol);
        
        vol = gGeoManager->MakeTube(Form("BarrelZminus%d",i+1),material,rmin,rmin+dr,dz);
        
        vol->SetFillColor(kOrange+9);
        vol->SetLineColor(kOrange+9);
        vol->SetTransparency(90);
   
        Target->AddNode(vol,i+1,new TGeoTranslation(0,0,dz-fZmax));
        AddSensitiveVolume(vol);
   }
}

// //________________________________________________________________________________
// TGeoVolume* Tracker::BuildTarget()
// {
//    TGeoMedium* base_material = FindMedium(fBaseMaterial,"air");    
//    
//    TGeoVolume* Target = gGeoManager->MakeTube("TARGET",base_material,0,fRmax,fZmax);
//     
//    Target->SetFillColor(kOrange+8);
//    Target->SetLineColor(kOrange+8);
//    Target->SetTransparency(80);
//    
//    return Target;
// }

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
Bool_t Tracker::FillParsIn(SpdParSet* params)
{
   if (!params) return kFALSE;
   
   params->SetParameter("GeoType",fGeoType);
   
   params->SetParameter("BaseMaterial",fBaseMaterial);
   params->SetParameter("ActiveMaterial",fActiveMaterial);

   params->SetParameter("Nslices",fNslices);
   params->SetParameter("Rmin",fRmin);
   params->SetParameter("Rmax",fRmax);
   params->SetParameter("Zmin",fZmin);
   params->SetParameter("Zmax",fZmax);
   
   params->SetParameter("MaxSimuStep",fMaxSimuStep);
   
   Double_t dr(0.), dz(0.);
   
   if (fNslices > 1) 
   {
       dz = Abs(fZmax-fZmin)/fNslices;
       dr = Abs(fRmax-fRmin)/fNslices;
   }
   
   if (fGeoType == 1) {
       params->SetParameter("LayerWidth",dz,2);
       params->SetParameter("EndcapLayerWidth",dz,2);
       params->SetParameter("BarrelLayerWidth",dr,2);
   }
   else if (fGeoType == 2) {
       params->SetParameter("LayerWidth",dz,2);
   }
   else if (fGeoType == 3) {
       params->SetParameter("LayerWidth",dr,2);
   }

   return SpdDetector::FillParsIn(params);
}

//_____________________________________________________________________________
Bool_t Tracker::LoadParsFrom(SpdParSet* params) 
{
  if (!params) return kFALSE;
 
  if (!SpdDetector::LoadParsFrom(params)) return kFALSE;
   
  fOutDataPointObject = "unknown";

  if (fNDataOut < 1) return kTRUE;

  params->GetParameter("Detector/NOutData_1",fOutDataPointObject);
  
  params->GetParameter("GeoType",fGeoType);
   
  params->GetParameter("BaseMaterial",fBaseMaterial);
  params->GetParameter("ActiveMaterial",fActiveMaterial);

  params->GetParameter("Nslices",fNslices);
  params->GetParameter("Rmin",fRmin);
  params->GetParameter("Rmax",fRmax);
  params->GetParameter("Zmin",fZmin);
  params->GetParameter("Zmax",fZmax);
   
  params->GetParameter("MaxSimuStep",fMaxSimuStep);
  
  return kTRUE;
}

//_____________________________________________________________________________________
TString Tracker::GetDataOut(Int_t n) const 
{ 
  if (n < 0 || n >= fNDataOut) return "unknown";
  return fOutDataPointObject;
}


//_____________________________________________________________________________________
TClonesArray* Tracker::GetCollection(Int_t iColl) const 
{ 
  return (iColl == 0) ? fPointCollection : 0; 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
void Tracker::SetDefaultGeometryParameters(Int_t g)
{
    if (g == 1) {  // encaps+barrel geometry
       
       fGeoType = 1;
       
       fZmin = 118;
       fZmax = 318; 
       fRmin = 65; 
       fRmax = 160; 
       fNslices = 95;
       
       return;
    }
    
    if (g == 2) { // encaps-like geometry
      
       fGeoType = 2;
    
       fZmin = 118; 
       fZmax = 318;
       fRmin = 20; 
       fRmax = 160; 
       fNslices = 200;
    
       return;
    }
    
    if (g == 3) {  // barrel-like geometry
      
       fGeoType = 3;
    
       fZmin = 118; 
       fZmax = 318;
       fRmin = 20; 
       fRmax = 160; 
       fNslices = 200;
      
       return;
    }
    
    fGeoType = 1;
     
    fZmin = 118; // 236/2
    fZmax = 318; // 236/2. + 164  + 36; 
    fRmin = 65; 
    fRmax = 160;   
    fNslices = 1;
 
}

//_____________________________________________________________________________________
void Tracker::SetGeoParameters(Double_t Zmin, Double_t Zmax, Double_t Rmin, Double_t Rmax, Int_t N)
{
    if (Zmin < 0) Zmin = 0.;
    if (Zmax < 0) Zmax = 0.;
    if (Rmin < 0) Rmin = 0.;
    if (Rmax < 0) Rmax = 0.;   
    
    if (!(Rmax > Rmin) || !(Zmax > Zmin)) {
        cout << "-E- <Tracker::SetGeoParameters> Bad geometry parameters " << endl;
        fGeoType = 0;
        return;
    }
    
    SetSlicing(N);
   
    fRmin = Rmin; 
    fRmax = Rmax;
    fZmin = Zmin; 
    fZmax = Zmax; 
    
    fGeoType = 1;
}
    
//_____________________________________________________________________________________
void Tracker::SetGeoEndcapsParameters(Double_t Zmin, Double_t Zmax, 
                                      Double_t Rmin, Double_t Rmax, Int_t N)
{  
    SetGeoParameters(Zmin,Zmax,Rmin,Rmax,N);
    fGeoType = 2;
}
    
//_____________________________________________________________________________________
void Tracker::SetGeoBarrelParameters(Double_t Zmin, Double_t Zmax, 
                                     Double_t Rmin, Double_t Rmax, Int_t N)
{
    SetGeoParameters(Zmin,Zmax,Rmin,Rmax,N);
    fGeoType = 3;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
void Tracker::Print(Option_t*) const
{
   SpdDetector::Print("");
   
   TString divider('-',150);
  
   cout << "\n";
   cout << "\tGeometry type:        " << fGeoType << endl;
   cout << "\tBase material:        " << fBaseMaterial << endl;
   cout << "\tActive material:      " << fActiveMaterial << endl;
   cout << "\n";
   
   cout << "\tMax. step (simu):     " << fMaxSimuStep << " [cm] " << endl;
   
   cout << "\n";
   
   cout <<"\tZ (min,max):          " << fZmin << ", " << fZmax << " [cm] " << endl;
   cout <<"\tR (min,max):          " << fRmin << ", " << fRmax << " [cm] " << endl;
   
   cout << "\n";
      
   double dr = Abs(fRmax-fRmin);
   double dz = Abs(fZmax-fZmin);
   
   if (fNslices > 1) {
       dz /= fNslices;
       dr /= fNslices;
   }
   
   cout <<"\tN slices:             " << fNslices << endl;
   
   if (fGeoType == 1) {
       cout <<"\tdZ =                  " << dz << " [cm] " << endl;
       cout <<"\tdR =                  " << dr << " [cm] " << endl;
   }
   else if (fGeoType == 2) {
       cout <<"\tdZ =                  " << dz << " [cm] " << endl;
   }
   else if (fGeoType == 3) {
       cout <<"\tdR =                  " << dr << " [cm] " << endl;
   }
   
   cout << divider.Data() << "\n";
   
   cout << "\n";
}



