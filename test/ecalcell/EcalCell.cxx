
#include "EcalCell.h"

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

#include "EcalCellHits.h"
#include "EcalCellEventHeader.h"
#include "EcalCellPoint.h"
#include "SpdDetectorList.h"
#include "SpdStack.h"
#include "SpdParSet.h"

#include <map>
#include <iostream>

using namespace std;

using std::cout;
using std::endl;

ClassImp(EcalCell)

//_____________________________________________________________________________
EcalCell::EcalCell() :
  FairDetector("EcalCell", kTRUE, kTestDetector),
  fTrackID(-1),
  fVolumeID(-1),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1),
  fEventHeader(0),
  fPoints(0),
  fHits(0),
  fEnum(0),
  fTotalEloss(0.),
  fGeoType(0),
  fNLayers(10),
  fNXYPads(10),
  fLayerSizeXY(10.),
  fLayer1SizeZ(0.5),
  fLayer2SizeZ(0.5)
{
    fNDataOut = 2;
    
    fOutPointObject = "EcalCellPoint";
    fOutHitsObject  = "EcalCellHits.";
    
    SetGeometryType(0); 
}

//_____________________________________________________________________________
EcalCell::EcalCell(const char* name, Bool_t active) :
  FairDetector(name, active, kTestDetector),
  fTrackID(-1),
  fVolumeID(-1),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1),
  fEventHeader(0),
  fPoints(0),
  fHits(0),
  fEnum(0),
  fTotalEloss(0.),
  fGeoType(0),
  fNLayers(10),
  fNXYPads(10),
  fLayerSizeXY(10.),
  fLayer1SizeZ(0.5),
  fLayer2SizeZ(0.5)
{  
    fNDataOut = 2;
    
    fOutPointObject = "EcalCellPoint";
    fOutHitsObject  = "EcalCellHits.";   
    
    SetGeometryType(0); 
}

//_____________________________________________________________________________
EcalCell::~EcalCell()
{
  if (fPoints) {
      fPoints->Delete();
      delete fPoints;
  }
  if (fHits) delete fHits;
}

//_____________________________________________________________________________
void EcalCell::Initialize()
{
  fEventHeader = (EcalCellEventHeader*)FairRunSim::Instance()->GetMCEventHeader();

  fPoints = new TClonesArray("EcalCellPoint");
  fHits = new EcalCellHits();

  FairDetector::Initialize();
  
  FillParameters();
}

//_____________________________________________________________________________
void EcalCell::Reset()
{
  if (fPoints) fPoints->Clear();
}

//_____________________________________________________________________________
void EcalCell::Register()
{
  FairRootManager::Instance()->Register(fOutPointObject, "EcalCell", fPoints, kTRUE);
  FairRootManager::Instance()->Register(fOutHitsObject, "EcalCell", fHits, kTRUE);
}

//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************

//_____________________________________________________________________________
void EcalCell::BeginEvent() 
{
  fEnum++;
  
  cout << "-I- <EcalCell::BeginEvent> Event : " << fEnum << "\n";
  
  if (fHits) {
      fHits->Clear();
      fHits->SetEventId(fEnum);      
      fHits->SetNZ(fNLayers); 
      fHits->SetNXY(fNXYPads); 
  }
  
  if (fEventHeader) fEventHeader->Clear();
  
  fTotalEloss = 0;
}

//_____________________________________________________________________________
Bool_t EcalCell::ProcessHits(FairVolume* vol)
{
  if (gMC->IsTrackEntering()) {
      fELoss  = 0.;
      fTime   = gMC->TrackTime() * 1.0e09;
      fLength = gMC->TrackLength();
      gMC->TrackPosition(fPos);
      gMC->TrackMomentum(fMom);
  }
 
  fELoss += gMC->Edep();
  
  //cout <<"VOL NAME: " <<  gMC->CurrentVolPath() << " " << gMC->CurrentVolName() << endl;
  
  if ( 
       gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   
     ) 
  {   
     if (fELoss == 0.) return kFALSE; 
     
     fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
     
     TString path = gMC->CurrentVolPath();
    
     //if (path.Contains("scinpad")) 
         fTotalEloss += fELoss;
     
     AddHit();         
  }

  return kTRUE;
}

//_____________________________________________________________________________
void EcalCell::AddHit() 
{  
  TString nodepath = gMC->CurrentVolPath();
  
  if (!nodepath.Contains("scinpad")) return;
  
  if (nodepath.BeginsWith("/cave_1/TARGET_1")) 
      nodepath.ReplaceAll("/cave_1/TARGET_1",""); 
  else return;
    
  if (fHits) {
      fHits->AddEdep(nodepath,fELoss);
  }
  
  if (fPoints) {
      TClonesArray& clref = *fPoints;
      Int_t size = clref.GetEntriesFast();
  
      EcalCellPoint* point = new(clref[size]) EcalCellPoint(fTrackID,fDetId,
                             TVector3(fPos.X(),fPos.Y(),fPos.Z()),
                             TVector3(fMom.Px(),fMom.Py(),fMom.Pz()),
                             fTime, fLength, fELoss); 
      point->SetNode(nodepath);
      point->SetEventID(fEnum);
      
      TParticle* part = gMC->GetStack()->GetCurrentTrack();
      if (part) point->SetPdg(part->GetPdgCode());
  }
}

//_____________________________________________________________________________
void EcalCell::PostTrack()
{
  if (!fEventHeader) return;
    
  SpdStack* stack = (SpdStack*)gMC->GetStack();
  
  // check primary particle
  if (stack->GetCurrentTrackNumber() != 0) return;
  TParticle* part = stack->GetCurrentTrack();
  if (part->GetFirstMother() != -1) return;
  
  fEventHeader->SetParticleName(part->GetName()); 
  fEventHeader->SetPdgNum(part->GetPdgCode());
  fEventHeader->SetMass(part->GetMass());
  fEventHeader->SetStartPoint(part->Vx(),part->Vy(),part->Vz());
  fEventHeader->SetMomentum(part->Px(),part->Py(),part->Pz());
    
  //PrintMCProcesses();
  //part->Print();
 
  TString path = gMC->CurrentVolPath();
  Int_t   process = GetCurrentStepProcess();
  Int_t   nsec = gMC->NSecondaries();
  Double_t x,y,z;
  gMC->TrackPosition(x,y,z);
   
  fEventHeader->SetPrimLastProcessID(process);
  fEventHeader->SetPrimLastND(nsec);
  fEventHeader->SetPrimLastVolPath(path);
  fEventHeader->SetPrimLastPoint(x,y,z);
  
  Double_t radlen(0), intlen(0), depth(0), length(0);
  
  GetIntRadLen(part,x,y,z,radlen,intlen,depth,length);
  
  fEventHeader->SetPrimLastIntLen(intlen);
  fEventHeader->SetPrimLastRadLen(radlen);
  fEventHeader->SetPrimLastDepth(depth);
  fEventHeader->SetPrimLastDist(length);
  
//   cout << "Process = " << process << " " << TMCProcessName[process] 
//        << " " << path << " x,y,z = " << x << " " << y << " " << z << " "
//        << " nsec = " << nsec << endl;
}

//_____________________________________________________________________________
void EcalCell::FinishEvent() 
{
  if (fPoints) cout << "-I- <EcalCell::FinishEvent> Points: " << fPoints->GetEntriesFast() << endl;  
  if (fHits) fHits->Print(1);
  if (fEventHeader) fEventHeader->SetTotalEloss(fTotalEloss);
}

//_____________________________________________________________________________
void EcalCell::EndOfEvent()
{
  if (fEventHeader) fEventHeader->Print(1);
  
  Reset();
}

//_____________________________________________________________________________
void EcalCell::FinishRun()
{
  cout << endl;
  cout << "************************************************************"<<endl;
  cout << "* <EcalCell::FinishRun> " << endl;
  cout << "* Events : " << fEnum << "\n";
  cout << "************************************************************"<<endl;
  cout << endl;
}

//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************

//_____________________________________________________________________________
void EcalCell::FillParameters() 
{
   FairRun* run = FairRun::Instance();
   if (!run) return;
  
   FairRuntimeDb* rtdb = run->GetRuntimeDb(); 
   if (!rtdb) return;
  
   SpdParSet* params = (SpdParSet*)(rtdb->getContainer("EcalCellParSet"));
   
   params->SetParameter("EcalCellGeoType",fGeoType,1);
   
   params->SetParameter("Detector/Id",fDetId,-2);     
   params->SetParameter("Detector/NOutData",fNDataOut,-2);
   
   for (Int_t i(0); i<fNDataOut; i++) {
        params->SetParameter(Form("OutData_%d",i+1),GetDataOut(i),-1); 
   }
   
   params->SetParameter("NLayers",fNLayers,1);
   params->SetParameter("NXYPads",fNXYPads,1); 
      
   params->setChanged();
   params->setInputVersion(FairRun::Instance()->GetRunId(),1);
}

//_____________________________________________________________________________________
TString EcalCell::GetDataOut(Int_t n) const 
{ 
  if (n < 0 || n >= fNDataOut) return "unknown";
  
  switch (n) {
      case 0 : return fOutHitsObject;
      case 1 : return fOutPointObject;
      default: return "unknown";
  }
  
  return "unknown";
}

//_____________________________________________________________________________
TClonesArray* EcalCell::GetCollection(Int_t iColl) const
{
  return (iColl == 0) ? fPoints : 0;
}

//_____________________________________________________________________________
Int_t EcalCell::GetCurrentStepProcess() 
{
   static TArrayI processes;
   Int_t nproc = gMC->StepProcesses(processes);
   if (nproc > 0) return processes[nproc-1]; 
   else return kPNoProcess;
}

//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************

//_____________________________________________________________________________
void EcalCell::ConstructGeometry()
{    
 
//  cout << "\n\n\n -I- <EcalCell::ConstructGeometry> " << endl;
//     cout << fNLayers << endl;
//     cout << fNXYPads  << endl;    
//     cout << fLayerSizeXY << endl; 
//     cout << fLayer1SizeZ  << endl;
//     cout << fLayer2SizeZ  << endl; 
  
    Double_t lzsize = fLayer1SizeZ + fLayer2SizeZ;
    Double_t tzsize = fNLayers * lzsize;
   
    Int_t nmed;
   
    TGeoMedium* Air = gGeoManager->GetMedium("air");
    if (!Air) {
        nmed = CreateMedium("air");
        Air = gGeoManager->GetMedium(nmed);
        if (!Air) Air = DefaultMedium();
    }
   
    TGeoMedium* Lead = gGeoManager->GetMedium("lead");
    if (!Lead) {
        nmed = CreateMedium("lead");
        Lead = gGeoManager->GetMedium(nmed);
        if (!Lead) Lead = DefaultMedium();
    }
    
    TGeoMedium* Scint = gGeoManager->GetMedium("FscScint");
    if (!Scint) {
        nmed = CreateMedium("FscScint");
        Scint = gGeoManager->GetMedium(nmed);
        if (!Scint) Scint = DefaultMedium();
    }
    
//     if (Air)   cout << Air->GetName() << endl;
//     if (Lead)  cout << Lead->GetName() << endl;
//     if (Scint) cout << Scint->GetName() << endl;
    
    TGeoVolume* TopVol = gGeoManager->GetTopVolume(); 
  
    TString volname;
    
    TGeoVolume* vol;
    Double_t zz;
    
    // create target box
   

    TGeoVolume* target = gGeoManager->MakeBox("TARGET",Lead,
                                              0.5*fLayerSizeXY, 0.5*fLayerSizeXY, 0.5*tzsize);
    target->SetFillColor(kCyan);
    target->SetLineColor(kCyan);
    target->SetTransparency(80);
    
    // create layer box
    
    TGeoVolume* layer = gGeoManager->MakeBox("layer",Air,
                                              0.5*fLayerSizeXY, 0.5*fLayerSizeXY, 0.5*lzsize);
    layer->SetFillColor(kGreen);
    layer->SetLineColor(kGreen);
    layer->SetTransparency(70);
    
    // create double-layer (lead+scint) box
   
    TGeoVolume* blead = 0;
    
    if (fLayer1SizeZ > 0) {
        blead = gGeoManager->MakeBox("leadbox",Lead,
                                     0.5*fLayerSizeXY, 0.5*fLayerSizeXY, 0.5*fLayer1SizeZ);  
        blead->SetFillColor(kBlue);
        blead->SetLineColor(kBlue);
        blead->SetTransparency(60);
    }
    
    TGeoVolume* bscin = gGeoManager->MakeBox("scinbox",Scint,
                                              0.5*fLayerSizeXY, 0.5*fLayerSizeXY, 0.5*fLayer2SizeZ); 
    bscin->SetFillColor(kPink);
    bscin->SetLineColor(kPink);
    bscin->SetTransparency(50);
    
    // slice scin plane
   
    Double_t xypadsize =  fLayerSizeXY/fNXYPads;
     
    TGeoVolume* scinpad = gGeoManager->MakeBox("scinpad",Scint,
                                               0.5*xypadsize, 0.5*xypadsize, 0.5*fLayer2SizeZ); 
    scinpad->SetFillColor(kPink);
    scinpad->SetLineColor(kPink);
    scinpad->SetTransparency(50);
    
    //ATTENTION!!!ATTENTION!!!ATTENTION                                        
    if (scinpad) AddSensitiveVolume(scinpad);
    //ATTENTION!!!ATTENTION!!!ATTENTION
    
    Int_t n(0);
    Double_t yy;
  
    Double_t xx = -0.5 * (fLayerSizeXY + xypadsize); 
    for (Int_t i(0); i<fNXYPads; i++) {
         xx += xypadsize;
         yy = -0.5 * ( fLayerSizeXY + xypadsize);
         for (Int_t j(0); j<fNXYPads; j++) {
              n++;
              yy += xypadsize;
              bscin->AddNode(scinpad,n,new TGeoTranslation(xx,yy,0));
         }
    }
    
    // add sublayers to the main layer
    
    if (blead) {
        zz = -0.5*lzsize + 0.5*fLayer1SizeZ;
        layer->AddNode(blead,1,new TGeoTranslation(0,0,zz));
    }
    
    zz =  0.5*lzsize - 0.5*fLayer2SizeZ;
    layer->AddNode(bscin,1,new TGeoTranslation(0,0,zz));
    
    //ATTENTION!!!ATTENTION!!!ATTENTION
    if (bscin) AddSensitiveVolume(bscin);
    if (blead) AddSensitiveVolume(blead);
    //ATTENTION!!!ATTENTION!!!ATTENTION
        
    // add layers to the target
    
    zz = -0.5*(tzsize + lzsize);
    
    for (Int_t i(0); i<fNLayers; i++) {
         zz += lzsize;
         target->AddNode(layer,i+1,new TGeoTranslation(0,0,zz)); 
    }
    
    // add target to TOP volume
    
    TopVol->AddNode(target,1,new TGeoTranslation(0,0,0)); 
    
    //------------------------------------------------
    // forward target
    
//     TGeoMedium* Iron = gGeoManager->GetMedium("lead");
//     if (!Iron) {
//         nmed = CreateMedium("lead");
//         Iron = gGeoManager->GetMedium(nmed);
//         //if (!Iron) Iron = DefaultMedium();
//     }
//     
//     TGeoVolume* fwtarget = gGeoManager->MakeBox("fwtarget",Iron, 5, 5, 0.5);
//     
//     fwtarget->SetFillColor(kRed);
//     fwtarget->SetLineColor(kRed);
//     //fwtarget->SetTransparency(50);
//     
//     AddSensitiveVolume(fwtarget);
//     
//     TopVol->AddNode(fwtarget,1,new TGeoTranslation(0,0,-150.)); 
}

//_____________________________________________________________________________
void EcalCell::SetGeometryType(Int_t type)
{
   fGeoType = type;
    
   if (type == 1) 
   {
      fNLayers      = 500;
      fNXYPads      = 105;
      fLayerSizeXY  = 315; // cm
      fLayer1SizeZ  = 0.1; // cm
      fLayer2SizeZ  = 0.9; // cm
      
      return;
   }
   
   else if (type == 2) 
   {
      fNLayers      = 100;
      fNXYPads      = 20;
      fLayerSizeXY  = 100;  // cm
      fLayer1SizeZ  = 0.5;  // cm
      fLayer2SizeZ  = 4.5;  // cm
      
      return;
   }

    // default
 
    fGeoType = 0;
 
    fNLayers      = 50;
    fNXYPads      = 9;
    fLayerSizeXY  = 300;  // cm
    fLayer1SizeZ  = 5.;   // cm
    fLayer2SizeZ  = 5.;   // cm
}

//_____________________________________________________________________________
void EcalCell::GetIntRadLen(TParticle* part, Double_t x, Double_t y, Double_t z,
                            Double_t& radlen, Double_t& intlen, 
                            Double_t& depth, Double_t& length)
{
   radlen = 0;
   intlen = 0;
   depth  = 0;
   length = 0;
   
   if (!gGeoManager) return;
   if (!part) return;
   
   TGeoNavigator* ntor = gGeoManager->GetCurrentNavigator();
   if (!ntor) return;
   
   TGeoNode *cnode, *next;
  
   ntor->SetCurrentPoint(x,y,z);
   
   cnode = ntor->SearchNode();
   
   if (!cnode) {
        cout << "<EcalCell::GetIntRadLen>  Point (" << x << ", " << y << ", " << z << ")"
             << " is out of boundaries " << endl;
        return;      
   }
   else {
        //cout << "<EcalCell::GetIntRadLen> START POINT: (" << x << ", " << y << ", " << z << ") " << endl;
        //cout << "<EcalCell::GetIntRadLen> START NODE: " << cnode->GetName() << " PATH: " << ntor->GetPath() << endl;     
   }
   
   TVector3 Dir(-part->Px(),-part->Py(),-part->Pz());
   Dir.SetMag(1);
   
   TGeoMaterial* mat;
   TString mname, nname;
   Double_t step;
   
   //------------------------------------------------------
   // 0.1 mm (Pb)   -> 1.134 g/cm^2
   // 0.9 mm (Scin) -> 0.9288 g/cm^2
   //                + 2.063 g/cm^2
   //------------------------------------------------------
   
   while (true) {
      
      ntor->SetCurrentDirection(Dir.X(),Dir.Y(),Dir.Z());
     
      next = ntor->FindNextBoundaryAndStep(); // *** !!! *** 
     
      nname = cnode->GetName();
      mname = cnode->GetMedium()->GetName();
      
      mat = cnode->GetMedium()->GetMaterial();
      step = ntor->GetStep();
      
      // mat->GetRadLen() returns radiation length in cm !
      if (mat->GetRadLen() > 0) radlen += step/mat->GetRadLen();
      if (mat->GetIntLen() > 0) intlen += step/mat->GetIntLen();
      
      depth += step*mat->GetDensity();
      
      //cout << depth << " " << step << " " << mname << " add: " <<  step*mat->GetDensity() << endl;
      //cout << length << endl;
      
      //cout << "matradlen = " << mat->GetRadLen() << "matintlen =  " << mat->GetIntLen() << endl;
      //cout << "medium: " << mname << " node: " << nname << " step = " << step 
      //     << " radlen =  " << radlen << " intlen = " << intlen << "\n\n";    
     
      if (nname == "cave_1") break;
      
      length += step;
       
      if (!next) break;  
      
      cnode = next;
   }
   
}

//_____________________________________________________________________________
Int_t EcalCell::CreateMedium(const char* name) 
{
  FairGeoLoader* geoLoad = FairGeoLoader::Instance();
  if (!geoLoad) return 0;
  
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  if (!geoFace) return 0;
  
  FairGeoBuilder* geobuild = geoLoad->getGeoBuilder(); 
  if (!geobuild) return 0;
      
  FairGeoMedia* Media = geoFace->getMedia();
  if (!Media) return 0;
  
  return geobuild->createMedium(Media->getMedium(name));
}

//_____________________________________________________________________________
TGeoMedium* EcalCell::DefaultMedium()
{
  return 0;   
}

//_____________________________________________________________________________
void EcalCell::PrintMCProcesses() 
{
   // Print data from TMCProcess.h
   Info("PrintMCProcesses","Total number of declared processes: %d",kMaxMCProcess);
   for (Int_t i(0); i<kMaxMCProcess; i++) printf("%-5d  %s\n",i,TMCProcessName[i]); 
} 
