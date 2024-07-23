// $Id$
// Author: andre/artur   2020/07/31

#include "SpdEcalTEC2.h"

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
#include "TGeoPgon.h"
#include "TGeoCompositeShape.h"
#include "TGeoTube.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TMatrixD.h"

#include "SpdEcalTEC2.h"
#include "SpdEcalTEC2Point.h"
#include "SpdEcalTEC2GeoMapper.h"
#include "SpdCommonGeoMapper.h"
#include "SpdDetectorList.h"
#include "SpdStack.h"
#include "SpdParSet.h"
#include "SpdGeoFactory.h"

#include <map>
#include <iostream>
#include <vector>

using namespace std;

using std::cout;
using std::endl;

ClassImp(SpdEcalTEC2)

//_____________________________________________________________________________________
SpdEcalTEC2::SpdEcalTEC2(): 

  SpdDetector("Ecal endcaps (tor)", kSpdEcalTEC, kTRUE),
  fTrackID(-1),
  fDetID(-1),fEndcapID(-1),fModuleID(-1), fCellID(-1), fLayerID(-1), fMatID(""),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1),
  fEnableWritingPointsScint(kTRUE),
  fEnableWritingPointsAll(kFALSE),
  fPointCollection(0),
  fEndcap(0),
  fModule(0),
  fCell(0)
{
    SetParametersType("EcalTECParSet");
   
    fNDataOut = 1;
    fOutDataPointObject = "SpdEcalTEC2Point"; 
}

//_____________________________________________________________________________________
SpdEcalTEC2::SpdEcalTEC2(const char* name, Bool_t active):

  SpdDetector(name, kSpdEcalTEC, active),
  fTrackID(-1),
  fDetID(-1),fEndcapID(-1),fModuleID(-1), fCellID(-1), fLayerID(-1), fMatID(""),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1),
  fEnableWritingPointsScint(kTRUE),
  fEnableWritingPointsAll(kFALSE),
  fPointCollection(0),
  fEndcap(0),
  fModule(0),
  fCell(0)
{
    SetParametersType("EcalTECParSet");
   
    fNDataOut = 1;
    fOutDataPointObject = "SpdEcalTEC2Point"; 
}

//_____________________________________________________________________________________
SpdEcalTEC2::~SpdEcalTEC2()
{
  if (fPointCollection) {
      fPointCollection->Delete();
      delete fPointCollection;
      fPointCollection = 0;
  }
}

//_____________________________________________________________________________________
void SpdEcalTEC2::Initialize()
{
  cout << "\n*******************************************************************************" << endl;
  cout << "************************* SpdEcalTEC2::Initialize *****************************" << endl;
  cout << "*******************************************************************************\n" << endl;
  
  // data collections
  fPointCollection = new TClonesArray(fOutDataPointObject);
  
  // Initialize module and fill parameters
  SpdDetector::Initialize(); 
 
  // SpdParSet* pars = GetParameters();
  // if (pars) pars->printParams();
}

//_____________________________________________________________________________________
void SpdEcalTEC2::Reset()
{
  fPointCollection->Delete();
  //fPointCollection->Clear();
}

//_____________________________________________________________________________________
void SpdEcalTEC2::Register()
{
  if (!fGeoMapper) return;
  
  if (!fEnableWritingPointsScint) return;
  
  FairRootManager::Instance()->Register(fOutDataPointObject,"SpdEcalTEC2", 
                                        fPointCollection, kTRUE);
}

//_____________________________________________________________________________________
Bool_t SpdEcalTEC2::ProcessHits(FairVolume* vol)
{
  //cout << "<SpdEcalTEC2::ProcessHits> " << endl;
  
  //Set parameters at entrance of volume. Reset ELoss.
  if (gMC->IsTrackEntering()) {
      //cout << "track entering\n";
      fELoss  = 0.;
      fTime   = gMC->TrackTime() * 1.e9;  // ns -> s
      fLength = gMC->TrackLength();
      gMC->TrackPosition(fPos);
      gMC->TrackMomentum(fMom);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Create SpdEcalTEC2Point at exit of active volume
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) 
  {
    
    if (!fSaveEmptyHits && fELoss == 0.) { return kFALSE; }
    
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    
    fNodePath = gMC->CurrentVolPath();
    SpdGeopathParser parser;
    parser.ParsePath(fNodePath);
    
    //cave: 1, endcap: 2, module:3, cell:4, layer:5, lead/scint: name[5]
    fDetID =  kSpdEcalTEC;
    fEndcapID = parser.Num(2, true)*2 - 3; //-1: negative Z, +1: positive Z
    fModuleID = parser.Num(3, true);
    fCellID = parser.Num(4, true);
    fLayerID = parser.Num(5, true);
    fMatID = parser.Name(5, true);
    
    //cout << " <<< ENDCAP >>> " << fMatID << " " << gMC->CurrentVolPath() << endl;
    
    assert(fMatID == fAbsorberVolName || fMatID == fScintVolName);
    
    if (fEnableWritingPointsAll) AddHit();
    else if (fEnableWritingPointsScint && fMatID == fScintVolName) AddHit();
   
    SpdStack* stack = (SpdStack*)gMC->GetStack();
    stack->AddPoint(kSpdEcalTEC);
  } 
   
  return kTRUE;
}

//_____________________________________________________________________________________
void SpdEcalTEC2::AddHit() 
{
  static Bool_t addhit = (fOutDataPointObject == "SpdEcalTEC2Point") ? kTRUE : kFALSE;
  
  if (!addhit) return;
  
  TClonesArray& clref = *fPointCollection;
  Int_t size = clref.GetEntriesFast();
  
  //cout << "SpdEcalTEC2::AddHit: Adding hit: " << fTrackID << " " << fDetID << " " << fEndcapID << " " << fVolumeID3 << " " << 
  //                                 fPos.X() << " " << fPos.Y() << " " << fPos.Z() << " " << 
  //                                 fMom.Px() << " " << fMom.Py() << " " << fMom.Pz() << " " << 
  //                                 fTime << " " << fLength << " " << fELoss << " path = " << fNodePath <<"\n";
  
  new(clref[size]) SpdEcalTEC2Point(fTrackID, 
                                    fDetID, fEndcapID, fModuleID, fCellID, fLayerID, 
                                    fMatID,
                                    TVector3(fPos.X(),fPos.Y(),fPos.Z()),
                                    TVector3(fMom.Px(),fMom.Py(),fMom.Pz()),
                                    fTime, fLength, fELoss, fNodePath);                                 
}

//_____________________________________________________________________________________
void SpdEcalTEC2::EndOfEvent()
{
   Reset();
}

//_____________________________________________________________________________________
void SpdEcalTEC2::FinishRun() 
{
   //SpdDetector::FinishRun();
   FillNodesTableIn(GetParameters());
}

//_____________________________________________________________________________________
void SpdEcalTEC2::ConstructGeometry()
{   
   fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();

   if (!fMasterVolume) {  
       cout << "-E- <SpdIts::ConstructGeometry> No MASTER volume " << endl;
       return;
   }
   
   if (!GetMapper()) return;
   
   if (!fGeoMapper->InitGeometry()) return;

   //fGeoMapper->Print("");
   
   SpdEcalTEC2GeoMapper* mapper = dynamic_cast<SpdEcalTEC2GeoMapper*>(fGeoMapper);
   
   fAbsorberVolName = mapper->GetAbsorberVolName();
   fScintVolName    = mapper->GetScintVolName();

   Int_t geo_type = fGeoMapper->GetGeoType();
   
   BuildGeometry();
   
   fGeoMapper->LockGeometry(); 
   
   cout << "\n*******************************************************************************" << endl;
   cout << "********************** SpdEcalTEC2::ConstructGeometry *************************" << endl;
   cout << "********************** GEOMETRY TYPE: " << geo_type 
        << " ***************************************" << endl;
   cout << "*******************************************************************************\n" << endl;
}

//________________________________________________________________________________
void SpdEcalTEC2::BuildGeometry()
{ 
   BuildEndcap();
   
   SpdEcalTEC2GeoMapper* mapper = dynamic_cast<SpdEcalTEC2GeoMapper*>(fGeoMapper);
   if (!mapper) return;

   Double_t layer1SizeZ = mapper->GetLayer1SizeZ();
   Double_t layer2SizeZ = mapper->GetLayer2SizeZ();
   Int_t nLayers = mapper->GetNLayers();
   Double_t endcapThickness = nLayers*(layer1SizeZ + layer2SizeZ);
   Double_t zshift = mapper->GetEndcapMinDist() + 0.5*endcapThickness;
   
   fMasterVolume->AddNode(fEndcap, 1, new TGeoTranslation(0, 0,-zshift));
   fMasterVolume->AddNode(fEndcap, 2, new TGeoTranslation(0, 0, zshift));
}

//________________________________________________________________________________
void SpdEcalTEC2::BuildEndcap()
{
   BuildModule();
   
   SpdEcalTEC2GeoMapper* mapper = dynamic_cast<SpdEcalTEC2GeoMapper*>(fGeoMapper);
   if (!mapper) return;
   
   Double_t endCapHoleRadius = mapper->GetEndcapSize() - mapper->GetEndcapWidth();
   Double_t layerEndcapSize = mapper->GetCellSize();
   Double_t cellClearance = mapper->GetCellClearance();
   Double_t moduleClearance = mapper->GetModuleClearance();
   Double_t endcapSize = mapper->GetEndcapSize();
   TString baseMedium = mapper->GetBaseMedium();

   Double_t layer1SizeZ = mapper->GetLayer1SizeZ();
   Double_t layer2SizeZ = mapper->GetLayer2SizeZ();
   Int_t nLayers = mapper->GetNLayers();
   Double_t endcapThickness = nLayers*(layer1SizeZ + layer2SizeZ);
   
   TGeoMedium* Air = FindMedium(baseMedium, "");
   
   fEndcap = gGeoManager->MakePgon(mapper->AddPrefix("Endcap"), Air, 22.5, 360, 8, 2);
   
   TGeoPgon* pgonOuter = (TGeoPgon*)(fEndcap->GetShape());
   pgonOuter->DefineSection(0, -endcapThickness/2, endCapHoleRadius, endcapSize);
   pgonOuter->DefineSection(1,  endcapThickness/2, endCapHoleRadius, endcapSize);
   
   Double_t cellSize = 2*layerEndcapSize + cellClearance;
   Double_t nCellsXY = 1;
   Double_t totalXFilled = cellSize;
   
   while (totalXFilled < 2*endcapSize) {
      totalXFilled += cellSize + moduleClearance;
      nCellsXY += 1;
   }
   
   nCellsXY -= 1;
   totalXFilled -= (cellSize + moduleClearance);
   
   if (nCellsXY >= 1000) {
       cout << "-E- <SpdEcalTEC2::ConstructGeometry_1> "
            << "Number of modules >= 1000. Decrease cell size or remake the module naming scheme." << endl;
       exit(1);
   }
   
   Double_t gridLeftX   = -totalXFilled/2;
   Double_t gridBottomY = -totalXFilled/2;
   
   TGeoTranslation* currentModuleTrans;
   
   Double_t corner[3] = {0,0,0};
   
   Int_t nx = 0;
   Int_t ny = 0;

   for (Int_t ix = 0; ix < nCellsXY; ++ix) {
        for (Int_t iy = 0; iy < nCellsXY; ++iy) {
        
             corner[0] = gridLeftX   + ix*(cellSize + moduleClearance);
             corner[1] = gridBottomY + iy*(cellSize + moduleClearance);
         
             if (!fEndcap->Contains(corner)) continue; 
             corner[1] += cellSize;
             if (!fEndcap->Contains(corner)) continue; 
             corner[0] += cellSize;
             if (!fEndcap->Contains(corner)) continue; 
             corner[1] -= cellSize;
             if (!fEndcap->Contains(corner)) continue; 
             corner[0] -= cellSize;
              
             currentModuleTrans = new TGeoTranslation(corner[0] + cellSize/2, corner[1] + cellSize/2, corner[2]); 
                       
             fEndcap->AddNode(fModule, (iy+1)*1000 + ix+1, currentModuleTrans);
             if (ix+1 > nx) nx = ix+1;
             if (iy+1 > ny) ny = iy+1;
       }
   }
   
   mapper->SetNModulesX(nx);
   mapper->SetNModulesY(nx);

   fEndcap->SetLineColor(kBlue);
   fEndcap->SetFillColor(kBlue);
   fEndcap->SetTransparency(50);
}

//________________________________________________________________________________
void SpdEcalTEC2::BuildModule() 
{
   BuildCell();
   
   SpdEcalTEC2GeoMapper* mapper = dynamic_cast<SpdEcalTEC2GeoMapper*>(fGeoMapper);
   if (!mapper) return; 
   
   TString baseMedium = mapper->GetBaseMedium();

   Double_t layer1SizeZ = mapper->GetLayer1SizeZ();
   Double_t layer2SizeZ = mapper->GetLayer2SizeZ();
   Int_t nLayers = mapper->GetNLayers();
   Double_t endcapThickness = nLayers*(layer1SizeZ + layer2SizeZ);
 
   TGeoMedium* Air = FindMedium(baseMedium, "");
 
   Double_t halfsizeModule = mapper->GetCellSize() + 0.5*mapper->GetCellClearance();
   Double_t shift = 0.5*mapper->GetCellSize() + 0.5*mapper->GetCellClearance();
      
   fModule = gGeoManager->MakeBox("EcalECModule", Air, halfsizeModule, halfsizeModule, 0.5*endcapThickness); 
          
   fModule->AddNode(fCell, 1, new TGeoTranslation(-shift,  shift, 0));
   fModule->AddNode(fCell, 2, new TGeoTranslation( shift,  shift, 0));
   fModule->AddNode(fCell, 3, new TGeoTranslation(-shift, -shift, 0));
   fModule->AddNode(fCell, 4, new TGeoTranslation( shift, -shift, 0));
    
   fModule->SetFillColor(kBlue);
   fModule->SetLineColor(kBlue);
   fModule->SetTransparency(50);
}

//_____________________________________________________________________________________
void SpdEcalTEC2::BuildCell()
{
   SpdEcalTEC2GeoMapper* mapper = dynamic_cast<SpdEcalTEC2GeoMapper*>(fGeoMapper);
   if (!mapper) return; 

   Int_t nLayers = mapper->GetNLayers();
   TString baseMedium = mapper->GetBaseMedium();
   TString absorberMedium = mapper->GetAbsorberMedium();
   TString scintMedium = mapper->GetScintMedium();
   Double_t layerEndcapSize = mapper->GetCellSize();
   Double_t layer1SizeZ = mapper->GetLayer1SizeZ();
   Double_t layer2SizeZ = mapper->GetLayer2SizeZ();
   Double_t endcapThickness = nLayers*(layer1SizeZ + layer2SizeZ);
   TGeoMedium* Air   = FindMedium(baseMedium, "");
   TGeoMedium* Lead  = FindMedium(absorberMedium, baseMedium);
   TGeoMedium* Scint = FindMedium(scintMedium, baseMedium);
   
   // construct active volumes
   TGeoVolume* mat1layer = gGeoManager->MakeBox(fAbsorberVolName,   Lead, 
                                                layerEndcapSize/2, layerEndcapSize/2, layer1SizeZ/2); 
   TGeoVolume* mat2layer = gGeoManager->MakeBox(fScintVolName, Scint, 
                                                layerEndcapSize/2, layerEndcapSize/2, layer2SizeZ/2); 
   
   if (fEnableWritingPointsScint) AddSensitiveVolume(mat2layer);  // ATTENTION
   if (fEnableWritingPointsAll)   AddSensitiveVolume(mat1layer);  // ATTENTION
   
   fCell = gGeoManager->MakeBox("EcalECCell", Lead, layerEndcapSize/2, layerEndcapSize/2, endcapThickness/2); 
   
 //  Int_t nLayers = floor(endcapThickness/(layer1SizeZ + layer2SizeZ));
   Double_t currZcenter;

   for (Int_t ilayer = 0; ilayer < nLayers; ++ilayer) {
        currZcenter = (layer1SizeZ + layer2SizeZ) * (-nLayers/2. + 0.5 + ilayer);
        Double_t center1 = currZcenter - (layer1SizeZ+layer2SizeZ)/2 + layer1SizeZ/2;
        Double_t center2 = currZcenter + (layer1SizeZ+layer2SizeZ)/2 - layer2SizeZ/2;
        fCell->AddNode(mat1layer, ilayer+1, new TGeoTranslation(0, 0, center1));
        fCell->AddNode(mat2layer, ilayer+1, new TGeoTranslation(0, 0, center2));
   }
      
//   // construct layer
//   TGeoVolume* layer = gGeoManager->MakeBox("EcalECLayer", Air, 
//                       layerEndcapSize/2, layerEndcapSize/2, (layer1SizeZ+layer2SizeZ)/2); 
//   
//   layer->AddNode(mat1layer, 1, new TGeoTranslation(0, 0, -(layer1SizeZ+layer2SizeZ)/2 + layer1SizeZ/2));
//   layer->AddNode(mat2layer, 1, new TGeoTranslation(0, 0,  (layer1SizeZ+layer2SizeZ)/2 - layer2SizeZ/2));
//
//   // construct cell
//   fCell = gGeoManager->MakeBox("EcalECCell", Lead, layerEndcapSize/2, layerEndcapSize/2, endcapThickness/2); 
//   
//   Int_t nLayers = floor(endcapThickness/(layer1SizeZ + layer2SizeZ));
//   Double_t currZcenter;
//
//   for (Int_t ilayer = 0; ilayer < nLayers; ++ilayer) {
//        currZcenter = (layer1SizeZ + layer2SizeZ) * (-nLayers/2. + 0.5 + ilayer);
//        fCell->AddNode(layer, ilayer+1, new TGeoTranslation(0, 0, currZcenter));
//   }
//      
//   layer->SetFillColor(kAzure+1); 
//   layer->SetLineColor(kAzure+1); 
//   layer->SetTransparency(50);
//   
//
   mat1layer->SetFillColor(kBlue+4);
   mat1layer->SetLineColor(kBlue+4);
   mat1layer->SetTransparency(50);
   
   mat2layer->SetFillColor(kCyan);
   mat2layer->SetLineColor(kCyan);
   mat2layer->SetTransparency(50);
   
   fCell->SetFillColor(kAzure+1);
   fCell->SetLineColor(kAzure+1);
   fCell->SetTransparency(50);
}
 
//_____________________________________________________________________________________
SpdGeoMapper* SpdEcalTEC2::GetMapper() 
{
   if (fGeoMapper) return fGeoMapper;
   
   SpdGeoFactory* factory = SpdGeoFactory::Instance();
 
   // search for mapper
   fGeoMapper = factory->SearchForMapper("SpdEcalTEC2GeoMapper");
   
   // create default mapper
   if (!fGeoMapper) fGeoMapper = factory->Mapper("SpdEcalTEC2GeoMapper");
    
   return fGeoMapper;
}

//_____________________________________________________________________________
Bool_t SpdEcalTEC2::LoadParsFrom(SpdParSet* params) 
{
   if (!params) return kFALSE;
 
   if (!SpdDetector::LoadParsFrom(params)) return kFALSE;
   
   TString mapper;
   params->GetParameter("Mapper",mapper); 
   fGeoMapper = SpdGeoFactory::Instance()->Mapper(mapper);  
   if (fGeoMapper) fGeoMapper->LoadParametersFrom(params);
    
   fOutDataPointObject = "unknown";

   if (fNDataOut < 1) return kTRUE;

   params->GetParameter("Detector/NOutData_1",fOutDataPointObject);
   
   return kTRUE;
}

//_____________________________________________________________________________________
TString SpdEcalTEC2::GetDataOut(Int_t n) const 
{ 
   if (n < 0 || n >= fNDataOut) return "unknown";
   return fOutDataPointObject;
}

//_____________________________________________________________________________________
TClonesArray* SpdEcalTEC2::GetCollection(Int_t iColl) const 
{ 
   return (iColl == 0) ? fPointCollection : 0; 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Double_t SpdEcalTEC2::GetCapacity() const 
{ 
   // return (fEndcap) ? fEndcap->GetShape()->Capacity() : 0;
   return 0;
}
   
//_____________________________________________________________________________________
Double_t SpdEcalTEC2::GetMass() const 
{ 
   return (fEndcap) ? GetCapacity()*GetDensity() : 0;
}

//_____________________________________________________________________________________
Double_t SpdEcalTEC2::GetDensity() const 
{   
   return (fEndcap) ? fEndcap->GetMaterial()->GetDensity() : 0; 
} 

//_____________________________________________________________________________
void SpdEcalTEC2::Print(Option_t*) const
{
   SpdDetector::Print("");
   
   if (!fGeoMapper) return;
   
   TString divider('-',150);
   
   cout << "\n";

   fGeoMapper->Print("");

   cout << "\n";
   cout <<"\tCapacity (total):    " << GetCapacity()*1e-6 << " [m^3] " << endl;
   cout <<"\tMass (total):        " << GetMass()*1e-3 << " [kg] " << endl;
   cout <<"\tDensity (averaged):  " << GetDensity() << " [g/cm^3] " << endl;
   cout << "\n";
   
   cout << "\n" << divider.Data() << "\n";       
   printf("%6s %4s %15s %14s %14s %14s \n\n","Type","N","Material","Dens [g/cm^3]","Volume [m^3]","Mass [kg]");       
   
   if (!fEndcap) {        
       cout << "\n";        
       return;    
   }

   printf("%6d %4d %15s %14.6f %14.6f %14.6f \n",            
           1, 1, fEndcap->GetMaterial()->GetName(),            
           GetDensity(), GetCapacity()*1e-6, GetMass()*1e-3);     
   printf("%6d %4d %15s %14.6f %14.6f %14.6f \n",            
           1, 2, fEndcap->GetMaterial()->GetName(),            
           GetDensity(), GetCapacity()*1e-6, GetMass()*1e-3);     
   
   cout << divider.Data() << "\n";       
   
   printf("%6s %35s %14.3e %14.3e \n","TOTAL:","",2*GetCapacity()*1e-6,2*GetMass()*1e-3);        
   
   cout << divider.Data() << "\n";       
   
   cout << "\n";
}

