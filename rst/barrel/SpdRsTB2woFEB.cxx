// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdRsTB2woFEB
//_____________________________________________________________________________

#include "SpdRsTB2woFEB.h"

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
#include "TGeoArb8.h"

#include "TMath.h"

#include "SpdRsTB2Point.h"
#include "SpdRsTB2GeoMapper.h"

#include "SpdDetectorList.h"
#include "SpdStack.h"
#include "SpdMCEventHeader.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include <iostream>

using namespace std;
using namespace TMath;

ClassImp(SpdRsTB2woFEB)

//_____________________________________________________________________________________
SpdRsTB2woFEB::SpdRsTB2woFEB(): 

  SpdDetector("RS barrel (qsl)", kSpdRsTB, kTRUE),
  fTrackID(-1),
  fVolumeID(-1),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1),
  fPointCollection(0),
  fModule(0)
{
  SetParametersType("RsTBParSet");
   
  fNDataOut = 1;
  fOutDataPointObject = "SpdRsTB2Point"; 
  
  fBaseColor = kGreen+2;   // (air)
  fAbsorbColor = kGreen+4; // (iron)
}

//_____________________________________________________________________________________
SpdRsTB2woFEB::SpdRsTB2woFEB(const char* name, Bool_t active):

  SpdDetector(name, kSpdRsTB, active),
  fTrackID(-1),
  fVolumeID(-1),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1),
  fPointCollection(0),
  fModule(0)
{
  SetParametersType("RsTBParSet");
   
  fNDataOut = 1;
  fOutDataPointObject = "SpdRsTB2Point";
  
  fBaseColor = kGreen+2;   // (air)
  fAbsorbColor = kGreen+4; // (iron)
}

//_____________________________________________________________________________________
SpdRsTB2woFEB::~SpdRsTB2woFEB()
{
  if (fPointCollection) {
      fPointCollection->Delete();
      delete fPointCollection;
      fPointCollection = 0;
  }
}

//_____________________________________________________________________________________
void SpdRsTB2woFEB::Initialize()
{
  cout << "\n*******************************************************************************" << endl;
  cout << "************************* SpdRsTB2woFEB::Initialize ********************************" << endl;
  cout << "*******************************************************************************\n" << endl;
  
  // data collections
  fPointCollection = new TClonesArray(fOutDataPointObject);
  
  // Initialize module and fill parameters
  SpdDetector::Initialize(); 
 
  // SpdParSet* pars = GetParameters();
  // if (pars) pars->printParams();  

}

//_____________________________________________________________________________________
void SpdRsTB2woFEB::Reset()
{
  fPointCollection->Clear();
}

//_____________________________________________________________________________________
void SpdRsTB2woFEB::Register()
{
  if (fGeoMapper && fGeoMapper->GetGeoType() < 1) return;
    
  FairRootManager::Instance()->Register(fOutDataPointObject,"SpdRsTB2woFEB", 
                                        fPointCollection, kTRUE);
}

//_____________________________________________________________________________________
Bool_t SpdRsTB2woFEB::ProcessHits(FairVolume* vol)
{
  
  //cout << "<SpdRsTB2woFEB::ProcessHits> " << gMC->CurrentVolPath() << endl;
  
  //Set parameters at entrance of volume. Reset ELoss.
  if (gMC->IsTrackEntering()) {
      fELoss  = 0.;
      fTime   = gMC->TrackTime() * 1.e9;  // ns -> s
      fLength = gMC->TrackLength();
      gMC->TrackPosition(fPos);
      gMC->TrackMomentum(fMom);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Create SpdRsTB2Point at exit of active volume
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) 
  {
    
    if (!fSaveEmptyHits && fELoss == 0.) { return kFALSE; }
    
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    fSegmentLength = gMC->TrackLength() - fLength;
    fTimeOut   = gMC->TrackTime() * 1.e9;  // ns -> s
    
    fVolumeID   = kSpdRsTB;
    fVolumePath = gMC->CurrentVolPath();
    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);
        
    //cout << "path = " << fVolumePath << endl;
   
    AddHit();
    
    //cout << "<SpdRsTB2woFEB::AddHit> " << fPointCollection->GetEntriesFast() << endl;
    
    SpdStack* stack = (SpdStack*)gMC->GetStack();
    stack->AddPoint(kSpdRsTB);
  } 
   
  return kTRUE;
}

//_____________________________________________________________________________________
void SpdRsTB2woFEB::AddHit() 
{
  static Bool_t addhit = (fOutDataPointObject == "SpdRsTB2Point") ? kTRUE : kFALSE;
  
  if (!addhit) return;
  
  TClonesArray& clref = *fPointCollection;
  Int_t size = clref.GetEntriesFast();
  
  new(clref[size]) SpdRsTB2Point(fTrackID,fVolumeID,fVolumePath,
                                 TVector3(fPos.X(),fPos.Y(),fPos.Z()),
                                 TVector3(fMom.Px(),fMom.Py(),fMom.Pz()),
                                 TVector3(fPosOut.X(),fPosOut.Y(),fPosOut.Z()),
                                 TVector3(fMomOut.Px(),fMomOut.Py(),fMomOut.Pz()),
                                 fTime, fTimeOut,
                                 fLength, fSegmentLength, fELoss);
}

//_____________________________________________________________________________________
void SpdRsTB2woFEB::EndOfEvent()
{
   Reset();
}

//_____________________________________________________________________________________
void SpdRsTB2woFEB::FinishRun() 
{
   //SpdDetector::FinishRun();
   FillNodesTableIn(GetParameters());
}

//_____________________________________________________________________________________
void SpdRsTB2woFEB::ConstructGeometry()
{   
  // Create detector geometry
  
  fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();
  
  if (!fMasterVolume) {  
      cout << "-E- <SpdTsTEC::ConstructGeometry> No MASTER volume " << endl;
      return;
  }
  
  if (!GetMapper()) return;
  
  if (!fGeoMapper->InitGeometry()) return;
      
  //fGeoMapper->Print("");
  
  Int_t geo_type = fGeoMapper->GetGeoType();

  ConstructDetector();
  
  fGeoMapper->LockGeometry();
      
  cout << "\n*******************************************************************************" << endl;
  cout << "********************** SpdRsTB2woFEB::ConstructGeometry ****************************" << endl;
  cout << "********************** GEOMETRY TYPE: " << geo_type 
       << " ***************************************" << endl;
  cout << "*******************************************************************************\n" << endl;
}

//_____________________________________________________________________________________
void SpdRsTB2woFEB::ConstructDetector()
{  
  SpdParameter* par;
  
  SpdRsTB2GeoMapper* mapper = dynamic_cast<SpdRsTB2GeoMapper*>(fGeoMapper);
  if (!mapper) return;
  
  Double_t mlen   = mapper->GetModuleLength();
  
  Double_t hmin   = mapper->GetModuleHmin();
  Double_t hmax   = mapper->GetModuleHmax();;
  Double_t lmin   = mapper->GetModuleLmin();
  Double_t lmax   = mapper->GetModuleLmax(); 
  
  fModule = BuildModule("SideSVol", mapper->GetBaseMaterial(),
                        0.5*mlen, hmin, hmax, 0.5*lmin, 0.5*lmax);
  
  if (!fModule) {
      cout << "-E- <SpdRsTB2woFEB::ConstructGeometry> "
           << "Volume " << fModule->GetName() << " cannot be created " << endl;
      return;
  }
    
  fModule->SetFillColor(kGreen);
  fModule->SetLineColor(kGreen);
  fModule->SetTransparency(30);  
  
  /*-----------------------------------------------------------*/
  TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
  rotNoRot->RegisterYourself();
    
  Double_t msize   = mapper->GetModuleSize();
  Double_t mwidth  = mapper->GetModuleWidth();
  
  Int_t nLayers = mapper->GetNLayers();
  
  // Add bottom layer (absrober(6.0cm) + mdt detectors(3.5cm))
  fModule->AddNode(CreateBottomLayer(msize, mwidth, mlen), 1, new TGeoCombiTrans("",  0, 0, 0, rotNoRot));
  
  // Add regular layers (3.0cm absorber + 3.5cm mdt detectors)
  for (Int_t i(2); i < nLayers+1; ++i) {   
       fModule->AddNode(CreateRegularLayer(i, msize, mwidth, mlen), i, new TGeoCombiTrans("", 0, 0, 0, rotNoRot));
  }
  
  // Add top absorber (6.0cm)
  fModule->AddNode(CreateTopLayer(nLayers+1, msize, mwidth, mlen), nLayers+1, new TGeoCombiTrans("", 0, 0, 0, rotNoRot));
  
  /*-----------------------------------------------------------*/

  TGeoTranslation trans(0,0,0); 
  TGeoRotation rot;
  
  Double_t angle = mapper->GetSecAngle();
  Int_t nsectors = mapper->GetNSectors();
  
  for (int i(1); i<=nsectors; i++) {
      
       fMasterVolume->AddNode(fModule,i,new TGeoCombiTrans(trans,rot));   
       rot.RotateZ(angle); 
  }
}

//________________________________________________________________________________
TGeoVolume* SpdRsTB2woFEB::BuildModule(const char* name, TString mat, Double_t length, 
                                  Double_t hmin, Double_t hmax, Double_t lmin, Double_t lmax)
{
  //cout << "-I- <SpdRsTB2woFEB::BuildModule> " << length << "/" 
  //     << hmin << "/" << hmax  << "/" <<  lmin << "/" << lmax << endl;
  
  Double_t v[16];
  
  v[0] =  lmin; v[1] = hmin;
  v[2] = -lmin; v[3] = hmin;
  v[4] = -lmax; v[5] = hmax;
  v[6] =  lmax; v[7] = hmax;
  
  v[8]  =  lmin; v[9]  = hmin;
  v[10] = -lmin; v[11] = hmin;
  v[12] = -lmax; v[13] = hmax;
  v[14] =  lmax; v[15] = hmax;

  TGeoMedium* material = FindMedium(mat,"air");
  
  if (!material) {
      cout << "-E- <SpdRsTB2woFEB::BuildModule> "
            << "Unknown material: " << mat << endl;
      return 0;
  }
  
  TGeoVolume* vol = gGeoManager->MakeArb8(name,material,length,v);
 
  //AddSensitiveVolume(vol); //ATTENTION FIXME ATTENTION
  
  return vol;
}

//________________________________________________________________________________
TGeoVolume* SpdRsTB2woFEB::CreateBottomLayer(Double_t rsSize, Double_t rsThickness, Double_t rsLength) 
{
  //----------------------------------------------------------------------------
  // Creates trapezoid representing bottom layer of the RS Barrel part
  // (6.0cm thick iron absorber and a layer of MDT detectors)
  // Parameters: 1. Size of RS from collision point (rsSize);
  //             2. Thickness of RS rsThickness.
  // Returns: TGeoVolume object of the layer with absorber and MDTs.
  //----------------------------------------------------------------------------
    
  SpdRsTB2GeoMapper* mapper = dynamic_cast<SpdRsTB2GeoMapper*>(fGeoMapper);
  if (!mapper) {
      cout << "-E- <SpdRsTB2woFEB::CreateBottomLayer> Unknown mapper: " << mapper->ClassName() << endl;
      return 0;
  }
  
  Double_t layerThickness = 6.0+3.5; //cm
  Double_t widthEnv = 8.5;           //cm xAxis
  Double_t thicknessEnv = 1.64;      //cm zAxis

  // Materials
  TString mat;
  
  mat = mapper->GetAbsorbMaterial();
  TGeoMedium   *piron = FindMedium(mat,"");
  
  mat = mapper->GetBaseMaterial();
  TGeoMedium   *pair  = FindMedium(mat,"");
  
  // Matrices
  TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
  rotNoRot->RegisterYourself();

  Double_t lmin = (rsSize - rsThickness)*(sqrt(2.0)-1.0); // dimensions (bottom base)
  Double_t lmax = (rsSize - rsThickness + layerThickness)*(sqrt(2.0)-1.0); // dimensions (top base)
  Double_t hmin = (rsSize - rsThickness); 
  Double_t hmax = (rsSize - rsThickness + layerThickness);

//   cout << "lmin: " << lmin << endl;
//   cout << "lmax: " << lmax << endl;
//   cout << "hmin: " << hmin << endl;
//   cout << "hmax: " << hmax << endl;

  Double_t v_bottom[16];
  
  v_bottom[0] =  lmin; v_bottom[1] = hmin;
  v_bottom[2] = -lmin; v_bottom[3] = hmin;
  v_bottom[4] = -lmax; v_bottom[5] = hmax;
  v_bottom[6] =  lmax; v_bottom[7] = hmax;
  
  v_bottom[8]  =  lmin; v_bottom[9]  = hmin;
  v_bottom[10] = -lmin; v_bottom[11] = hmin;
  v_bottom[12] = -lmax; v_bottom[13] = hmax;
  v_bottom[14] =  lmax; v_bottom[15] = hmax;

  TGeoArb8* Layer_SideS_Shape = new TGeoArb8("Layer_SideS_Shape", rsLength/2, v_bottom);

  Double_t Layer_Sector_Abs_dx = lmin;
  Double_t Layer_Sector_Abs_dy = 6./2;
  Double_t Layer_Sector_Abs_dz = rsLength/2;

  //cout << "Layer_SideS_Abs_Base_Shape: "
  //     << 2*Layer_Sector_Abs_dx << ", " << 2*Layer_Sector_Abs_dy << ", " << 2*Layer_Sector_Abs_dz << endl;

  TGeoBBox* Layer_SideS_Abs_Base_Shape = new TGeoBBox("Layer_SideS_Abs_Base_Shape", 
                                                      Layer_Sector_Abs_dx, 
                                                      Layer_Sector_Abs_dy, 
                                                      Layer_Sector_Abs_dz);
                                                          
  TGeoVolume* Layer_SideS_Vol = new TGeoVolume("LayerSideSVol", Layer_SideS_Shape, pair);
  TGeoVolume* Layer_SideS_Abs_Base_Vol = new TGeoVolume("LayerSideSAbsBaseVol", Layer_SideS_Abs_Base_Shape, piron);

  Layer_SideS_Vol->AddNode(Layer_SideS_Abs_Base_Vol, 1, new TGeoCombiTrans("", 0, hmin+Layer_Sector_Abs_dy, 0, rotNoRot));

  Double_t Layer_Envelope_startX = -1.0*lmin;
  Double_t Layer_Envelope_shiftX = Layer_Envelope_startX + widthEnv/2;
  Double_t Layer_length          = 2*abs(Layer_Envelope_startX);
  
  Int_t    Nenvelopes = Int_t(Layer_length/widthEnv);
  
  //cout << "Layer #1 length: " << Layer_length << endl;
  //cout << "Number of MDT detector in Layer #1: " << Nenvelopes << endl;

  for (UInt_t i(0); i < Nenvelopes; i++) 
  {
      Layer_SideS_Vol->AddNode(CreateMDT(rsLength-0.2), i+1, new TGeoCombiTrans("", Layer_Envelope_shiftX, hmin+6+3.5/2, 0, rotNoRot)); 
      Layer_Envelope_shiftX = Layer_Envelope_shiftX + widthEnv;
  }
  
  Layer_SideS_Abs_Base_Vol->SetFillColor(fAbsorbColor);
  Layer_SideS_Abs_Base_Vol->SetLineColor(fAbsorbColor);
  Layer_SideS_Abs_Base_Vol->SetTransparency(30);  
  
  Layer_SideS_Vol->SetFillColor(fBaseColor);
  Layer_SideS_Vol->SetLineColor(fBaseColor);
  Layer_SideS_Vol->SetTransparency(30);  
  
  return Layer_SideS_Vol;
}

//________________________________________________________________________________
TGeoVolume* SpdRsTB2woFEB::CreateTopLayer(Int_t layerN, Double_t rsSize, Double_t rsThickness, Double_t rsLength) 
{
  //----------------------------------------------------------------------------  
  //
  // Creates trapezoid representing regular layer of the RS Barrel part
  // (3.0cm thick iron absorber and a layer of MDT detectors)
  // Parameters: 1. Number of layer in a sector
  //             2. Size of RS from collision point (rsSize);
  //             3. Thickness of RS rsThickness.
  // Returns: TGeoVolume object of the layer with absorber and MDTs.
  //----------------------------------------------------------------------------  
  
  SpdRsTB2GeoMapper* mapper = dynamic_cast<SpdRsTB2GeoMapper*>(fGeoMapper);
  if (!mapper) {
      cout << "-E- <SpdRsTB2woFEB::CreateTopLayer> Unknown mapper: " << mapper->ClassName() << endl;
      return 0;
  }
  
  Double_t layerThickness = 3.0+3.0; // cm
  Double_t Sector_dy = rsLength/2.0; // cm
  Double_t widthEnv = 8.5;           // cm xAxis
  Double_t thicknessEnv = 1.64;      // cm zAxis
  
  // Materials
  TString mat;
  
  mat = mapper->GetAbsorbMaterial();
  TGeoMedium   *piron = FindMedium(mat,"");  
  
  mat = mapper->GetBaseMaterial();
  TGeoMedium   *pair  = FindMedium(mat,"");

  // Matrices
  TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
  rotNoRot->RegisterYourself();
  
  Double_t lmin = (rsSize - rsThickness + 9.5 + (layerN-1)*layerThickness)*(sqrt(2.0)-1.0); // dimensions (bottom base)
  Double_t lmax = (rsSize - rsThickness + 9.5 + (layerN)*layerThickness)*(sqrt(2.0)-1.0);   // dimensions (top base)
  Double_t hmin = (rsSize - rsThickness + 9.5 + (layerN-2)*6.5); 
  Double_t hmax = (rsSize - rsThickness + 9.5 + (layerN-2)*6.5 + layerThickness);

  //cout << "Layer #" << layerN << " dimensions (bottom base, lmin): [" << -lmin << "," << lmin << "]" << endl;
  //cout << "Layer #" << layerN << " dimensions (bottom base, lmax): [" << -lmax << "," << lmax << "]" << endl;
  //cout << "hmin: " << hmin <<endl;
  //cout << "hmax: " << hmax <<endl;
  
  Double_t v_top[16];
  
  v_top[0] =  lmin; v_top[1] = hmin;
  v_top[2] = -lmin; v_top[3] = hmin;
  v_top[4] = -lmax; v_top[5] = hmax;
  v_top[6] =  lmax; v_top[7] = hmax;
  
  v_top[8]  =  lmin; v_top[9]  = hmin;
  v_top[10] = -lmin; v_top[11] = hmin;
  v_top[12] = -lmax; v_top[13] = hmax;
  v_top[14] =  lmax; v_top[15] = hmax;

  TGeoArb8* Layer_SideS_Shape = new TGeoArb8("Layer_SideS_Shape", rsLength/2, v_top);

  Double_t Layer_Sector_Abs_dx = lmin;
  Double_t Layer_Sector_Abs_dy = layerThickness/2;
  Double_t Layer_Sector_Abs_dz = rsLength/2;

  TGeoBBox* Layer_SideS_Abs_Base_Shape = new TGeoBBox("Layer_SideS_Abs_Base_Shape", 
                                                      Layer_Sector_Abs_dx, 
                                                      Layer_Sector_Abs_dy, 
                                                      Layer_Sector_Abs_dz);
  
                                                          
  TGeoVolume* Layer_SideS_Vol = new TGeoVolume("LayerSideSVol", Layer_SideS_Shape, pair);
  TGeoVolume* Layer_SideS_Abs_Base_Vol = new TGeoVolume("LayerSideSAbsBaseVol", Layer_SideS_Abs_Base_Shape, piron);

  Layer_SideS_Vol->AddNode(Layer_SideS_Abs_Base_Vol, 1, new TGeoCombiTrans("", 0, hmin+Layer_Sector_Abs_dy, 0, rotNoRot));
  
  Layer_SideS_Abs_Base_Vol->SetFillColor(fAbsorbColor);
  Layer_SideS_Abs_Base_Vol->SetLineColor(fAbsorbColor);
  Layer_SideS_Abs_Base_Vol->SetTransparency(30);  
  
  Layer_SideS_Vol->SetFillColor(fBaseColor);
  Layer_SideS_Vol->SetLineColor(fBaseColor);
  Layer_SideS_Vol->SetTransparency(30);  
  
  return Layer_SideS_Vol;
}

//________________________________________________________________________________
TGeoVolume* SpdRsTB2woFEB::CreateRegularLayer(Int_t layerN, Double_t rsSize, Double_t rsThickness, Double_t rsLength) 
{
  //----------------------------------------------------------------------------       
  // Creates trapezoid representing regular layer of the RS Barrel part
  // (3.0cm thick iron absorber and a layer of MDT detectors)
  // Parameters: 1. Number of layer in a sector
  //             2. Size of RS from collision point (rsSize);
  //             3. Thickness of RS rsThickness.
  // Returns: TGeoVolume object of the layer with absorber and MDTs.
  //----------------------------------------------------------------------------   
  
  SpdRsTB2GeoMapper* mapper = dynamic_cast<SpdRsTB2GeoMapper*>(fGeoMapper);
  if (!mapper) {
      cout << "-E- <SpdRsTB2woFEB::CreateMDT> Unknown mapper: " << mapper->ClassName() << endl;
      return 0;
  }
  
  Double_t layerThickness = 3.0+3.5; // cm
  Double_t Sector_dy = rsLength/2.0; // cm
  Double_t widthEnv = 8.5;           // cm xAxis
  Double_t thicknessEnv = 1.64;      // cm zAxis
  
  // Materials
  TString mat;
  
  mat = mapper->GetAbsorbMaterial();
  TGeoMedium   *piron = FindMedium(mat,"");
  
  mat = mapper->GetBaseMaterial();
  TGeoMedium   *pair  = FindMedium(mat,"");

  // Matrices
  TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
  rotNoRot->RegisterYourself();
  
  Double_t Layer_Sector_dz  = layerThickness/2; // z-coord of the layer center 
  Double_t Layer_Sector_dx1 = 1.0*(rsSize - rsThickness + 9.5 + (layerN-2)*layerThickness)*(sqrt(2.0)-1.0); // X-coord of the bottom base
  Double_t Layer_Sector_dx2 = 1.0*(rsSize - rsThickness + 9.5 + (layerN-1)*layerThickness)*(sqrt(2.0)-1.0); // X-coord of the top base
  Double_t Layer_Sector_dy  = Sector_dy;       // y-coord of the layer center
  
  //cout<<"Layer #"<< layerN << " dimensions (bottom base): ["
  //    << Layer_Sector_dx1*2 << " x " << Layer_Sector_dz*2 << " x " << Layer_Sector_dy*2 << "] cm" << endl;
  //cout<<"Layer #"<< layerN << " dimensions (top base): ["
  //    << Layer_Sector_dx2*2 << " x " << Layer_Sector_dz*2 << " x " << Layer_Sector_dy*2 << "] cm" << endl;
  
  Double_t lmin = (rsSize - rsThickness + 9.5 + (layerN-2)*layerThickness)*(sqrt(2.0)-1.0); // dimensions (bottom base)
  Double_t lmax = (rsSize - rsThickness + 9.5 + (layerN-1)*layerThickness)*(sqrt(2.0)-1.0); // dimensions (top base)
  Double_t hmin = (rsSize - rsThickness + 9.5 + (layerN-2)*layerThickness); 
  Double_t hmax = (rsSize - rsThickness + 9.5 + (layerN-1)*layerThickness);

  //cout << "Layer #" << layerN << " dimensions (bottom base, lmin): [" << -lmin << "," << lmin << "]" << endl;
  //cout << "Layer #" << layerN << " dimensions (bottom base, lmax): [" << -lmax << "," << lmax << "]" << endl;
  //cout << "hmin: " << hmin << endl;
  //cout << "hmax: " << hmax << endl;
  
  Double_t v_regular[16];
  
  v_regular[0] =  lmin; v_regular[1] = hmin;
  v_regular[2] = -lmin; v_regular[3] = hmin;
  v_regular[4] = -lmax; v_regular[5] = hmax;
  v_regular[6] =  lmax; v_regular[7] = hmax;
  
  v_regular[8]  =  lmin; v_regular[9]  = hmin;
  v_regular[10] = -lmin; v_regular[11] = hmin;
  v_regular[12] = -lmax; v_regular[13] = hmax;
  v_regular[14] =  lmax; v_regular[15] = hmax;

  TGeoArb8* Layer_SideS_Shape = new TGeoArb8("Layer_SideS_Shape", rsLength/2, v_regular);

  Double_t Layer_Sector_Abs_dx = lmin;
  Double_t Layer_Sector_Abs_dy = 3./2;
  Double_t Layer_Sector_Abs_dz = rsLength/2;

  TGeoBBox* Layer_SideS_Abs_Base_Shape = new TGeoBBox("Layer_SideS_Abs_Base_Shape", 
                                                      Layer_Sector_Abs_dx, Layer_Sector_Abs_dy, Layer_Sector_Abs_dz);
  
                                                          
  TGeoVolume* Layer_SideS_Vol          = new TGeoVolume("LayerSideSVol",        Layer_SideS_Shape,          pair);
  TGeoVolume* Layer_SideS_Abs_Base_Vol = new TGeoVolume("LayerSideSAbsBaseVol", Layer_SideS_Abs_Base_Shape, piron);

  Layer_SideS_Vol->AddNode(Layer_SideS_Abs_Base_Vol, 1, new TGeoCombiTrans("", 0, hmin+Layer_Sector_Abs_dy, 0, rotNoRot));

  if (layerN % 2 == 0) 
  {
      Double_t Layer_Envelope_startX = lmin;
      Double_t Layer_Envelope_shiftX = Layer_Envelope_startX - widthEnv/2;

      Double_t Layer_length = 2*abs(Layer_Envelope_startX);
      Int_t Nenvelopes = Int_t((Layer_length)/widthEnv);
      
      //cout << " Layer #" << layerN << " length: " << Layer_length << endl;
      //cout << " Number of MDT detector in Layer" << layerN << ": " << Nenvelopes << endl;
      
      for (UInt_t i(0); i < Nenvelopes; i++) 
      {
           Layer_SideS_Vol->AddNode(CreateMDT(493.0), i+1, new TGeoCombiTrans("", Layer_Envelope_shiftX, hmin + 3 + 3.5/2, 0, rotNoRot)); 
           Layer_Envelope_shiftX = Layer_Envelope_shiftX - widthEnv;
      }
  }
  else 
  {
      Double_t Layer_Envelope_startX = -1.0*abs(lmin);
      Double_t Layer_Envelope_shiftX = Layer_Envelope_startX + widthEnv/2;

      Double_t Layer_length = 2*abs(Layer_Envelope_startX);
      Int_t Nenvelopes = Int_t((Layer_length)/widthEnv);
      
      //cout << " Layer #" << layerN << " length: " << Layer_length << endl;
      //cout << " Number of MDT detector in Layer" << layerN << ": " << Nenvelopes << endl;
      
      for (UInt_t i(0); i < Nenvelopes; i++) 
      {
          Layer_SideS_Vol->AddNode(CreateMDT(493.0), i+1, new TGeoCombiTrans("", Layer_Envelope_shiftX, hmin + 3 + 3.5/2, 0, rotNoRot)); 
          Layer_Envelope_shiftX = Layer_Envelope_shiftX + widthEnv;
      }
  }
  
  Layer_SideS_Abs_Base_Vol->SetFillColor(fAbsorbColor);
  Layer_SideS_Abs_Base_Vol->SetLineColor(fAbsorbColor);
  Layer_SideS_Abs_Base_Vol->SetTransparency(30);  
  
  Layer_SideS_Vol->SetFillColor(fBaseColor);
  Layer_SideS_Vol->SetLineColor(fBaseColor);
  Layer_SideS_Vol->SetTransparency(30);  

  return Layer_SideS_Vol;
}

//________________________________________________________________________________
TGeoVolume* SpdRsTB2woFEB::CreateMDT(Double_t length) 
{
  //----------------------------------------------------------------------------     
  // Creates MDT of given length;
  // Envelope Dimensions(L x W x H): length x 8.5cm x 1.64cm.
  // Note: Adopted from Panda Muon System
  //----------------------------------------------------------------------------  
 
  SpdRsTB2GeoMapper* mapper = dynamic_cast<SpdRsTB2GeoMapper*>(fGeoMapper);
  if (!mapper) {
      cout << "-E- <SpdRsTB2woFEB::CreateMDT> Unknown mapper: " << mapper->ClassName() << endl;
      return 0;
  }
  
  // Materials ------------------------------------------------------------------------
  
  TString mat;
  
  mat = mapper->GetBaseMaterial();
  TGeoMedium* pair = FindMedium(mat,"");
  
  mat = mapper->GetActiveMaterialMDT();
  TGeoMedium* pMDTMixture = FindMedium(mat,"");  
  
  mat = mapper->GetPassiveMaterialMDT();
  TGeoMedium* paluminium = FindMedium(mat,"");  
  
  // Shapes and volumes ---------------------------------------------------------------
  
  // General dimensions
  Double_t widthEnv = 8.5;      //cm xAxis
  Double_t thicknessEnv = 1.64; //cm zAxis
  Double_t xSize = 1.0;

  TString transName;
  Int_t Nenvelopes;
  Int_t IncEnv = 0;

  // Matrices
  TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
  rotNoRot->RegisterYourself();
  
  Double_t empty_value = (2.0+2.0)+6.5*2;
  Double_t L_MDT_Envelope = length; //493.0;
  Double_t L_MDT_Envelope_sensitive = L_MDT_Envelope-empty_value;

  Int_t ind = Int_t(length*10);
  
  //---------------------------------------
  
  TGeoBBox* Indent20_Shape = new TGeoBBox("Indent20_Shape", widthEnv/2, thicknessEnv/2,  1.0);
  TGeoBBox* DeadMat_Shape  = new TGeoBBox("DeadMat_Shape",  4.07, 0.515,  3.25);
  
  TGeoVolume* Indent20_Vol = new TGeoVolume("Indent20Vol", Indent20_Shape, pair);
  TGeoVolume* DeadMat_Vol  = new TGeoVolume("DeadMatVol",  DeadMat_Shape,  pair);

  //---------------------------------------
  
  TGeoBBox* L_Al_A_Shape     = new TGeoBBox(Form("L%d_Al_A_Shape",ind),    4.03, 0.03, L_MDT_Envelope_sensitive/2);
  TGeoBBox* L_Al_B_Shape     = new TGeoBBox(Form("L%d_Al_B_Shape",ind),    0.03, 0.42, L_MDT_Envelope_sensitive/2);
  TGeoBBox* L_GasCell_Shape  = new TGeoBBox(Form("L%d_GasCell_Shape",ind), 0.47, 0.46, L_MDT_Envelope_sensitive/2);
  TGeoBBox* L_Cell_Shape     = new TGeoBBox(Form("L%d_Cell_Shape",ind),    0.5,  0.46, L_MDT_Envelope_sensitive/2);
  TGeoBBox* L_MDT_S_Shape    = new TGeoBBox(Form("L%d_MDT_S_Shape",ind),   4.0,  0.46, L_MDT_Envelope_sensitive/2);
  TGeoBBox* L_Envelope_Shape = new TGeoBBox(Form("L%d_Envelope_Shape",ind), widthEnv/2, thicknessEnv/2, L_MDT_Envelope/2);

  TGeoVolume* L_Al_A_Vol     = new TGeoVolume(Form("L%dAlAVol",ind),     L_Al_A_Shape,    paluminium);
  TGeoVolume* L_Al_B_Vol     = new TGeoVolume(Form("L%dAlBVol",ind),     L_Al_B_Shape,    paluminium);
  TGeoVolume* L_GasCell_Vol  = new TGeoVolume(Form("L%dGasCellVol",ind), L_GasCell_Shape, pMDTMixture);
  TGeoVolume* L_Cell_Vol     = new TGeoVolume(Form("L%dCellVol",ind),    L_Cell_Shape,    pair);
  TGeoVolume* L_MDT_S_Vol    = new TGeoVolume(Form("L%dMDTSVol",ind),    L_MDT_S_Shape,   pair);
  TGeoVolume* L_Envelope_Vol = new TGeoVolume(Form("L%dEnvelopeVol",ind), L_Envelope_Shape, pair);
  
  //---------------------------------------
  
  
  
  // Structure -----------------------------------------------------------------
  
  // GasCell and Al_B in Cell
  L_Cell_Vol->AddNode(L_GasCell_Vol, 1, new TGeoCombiTrans("", -0.03,     0,  0, rotNoRot));
  L_Cell_Vol->AddNode(L_Al_B_Vol,    1, new TGeoCombiTrans("",  0.47, -0.04,  0, rotNoRot));

  // Cells in MDT_S
  const Int_t NCells = 8;
  for (UInt_t i(0); i < NCells; i++) 
  {
      transName.Form("trans_cell_%d", i);
      TGeoCombiTrans* trans1 = new TGeoCombiTrans(transName.Data(), -4.0 + xSize/2. + (Double_t)i*xSize, 0, 0., rotNoRot);
      trans1->RegisterYourself();
      L_MDT_S_Vol->AddNode(L_Cell_Vol, i+1, trans1);
  }

  L_Envelope_Vol->AddNode(Indent20_Vol, 1, new TGeoCombiTrans("", 0, 0, L_MDT_Envelope/2 - 1.0, rotNoRot));
  L_Envelope_Vol->AddNode(Indent20_Vol, 2, new TGeoCombiTrans("", 0, 0, 1.0 - L_MDT_Envelope/2, rotNoRot));
  L_Envelope_Vol->AddNode(DeadMat_Vol,  1, new TGeoCombiTrans("", 0, 0, ((L_MDT_Envelope-(2.0+2.0))-6.5)/2, rotNoRot));
  L_Envelope_Vol->AddNode(DeadMat_Vol,  2, new TGeoCombiTrans("", 0, 0,-((L_MDT_Envelope-(2.0+2.0))-6.5)/2, rotNoRot));
  L_Envelope_Vol->AddNode(L_Al_A_Vol,   1, new TGeoCombiTrans("", 0, -0.36, 0, rotNoRot));
  L_Envelope_Vol->AddNode(L_Al_B_Vol,   1, new TGeoCombiTrans("", -4.0, 0.09, 0, rotNoRot));
  L_Envelope_Vol->AddNode(L_MDT_S_Vol,  1, new TGeoCombiTrans("", 0.03, 0.13, 0, rotNoRot));  
  
  L_Envelope_Vol->SetFillColor(kGreen+1);
  L_Envelope_Vol->SetLineColor(kGreen+1);
  L_Envelope_Vol->SetTransparency(30);   
  
  AddSensitiveVolume(L_GasCell_Vol); //ATTENTION FIXME ATTENTION
  
  return L_Envelope_Vol;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
SpdGeoMapper* SpdRsTB2woFEB::GetMapper() 
{
  if (fGeoMapper) return fGeoMapper;
  
  SpdGeoFactory* factory = SpdGeoFactory::Instance();
 
  // search for mapper
  fGeoMapper = factory->SearchForMapper("SpdRsTB2GeoMapper");
  
  if (!fGeoMapper) fGeoMapper = factory->Mapper("SpdRsTB2GeoMapper");

  return fGeoMapper; 
}

//_____________________________________________________________________________
Bool_t SpdRsTB2woFEB::LoadParsFrom(SpdParSet* params) 
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
TString SpdRsTB2woFEB::GetDataOut(Int_t n) const 
{ 
  if (n < 0 || n >= fNDataOut) return "unknown";
  return fOutDataPointObject;
}


//_____________________________________________________________________________________
TClonesArray* SpdRsTB2woFEB::GetCollection(Int_t iColl) const 
{ 
  return (iColl == 0) ? fPointCollection : 0; 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Double_t SpdRsTB2woFEB::GetCapacity() const 
{ 
  //return (fModule) ? fModule->GetShape()->Capacity() : 0;
  return 0.;
}
   
//_____________________________________________________________________________________
Double_t SpdRsTB2woFEB::GetMass() const 
{ 
  return (fModule) ? GetCapacity()*GetDensity() : 0;
}

//_____________________________________________________________________________________
Double_t SpdRsTB2woFEB::GetDensity() const 
{ 
  return (fModule) ? fModule->GetMaterial()->GetDensity() : 0;
}

//_____________________________________________________________________________________
TString SpdRsTB2woFEB::GetMaterialName() const // private
{ 
  return (fModule) ? fModule->GetMaterial()->GetName() : "";
}

//_____________________________________________________________________________
void SpdRsTB2woFEB::Print(Option_t*) const
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
   
   printf("%6s %4s %15s %14s %14s %14s \n\n",
          "Type","N","Material","Dens [g/cm^3]","Volume [m^3]","Mass [kg]"
         );
   
   if (!fModule) {
       cout << "\n";
       return;
   }
   
   printf("%6d %4d %15s %14.6f %14.6f %14.6f \n",
           1, 1, GetMaterialName().Data(),
           GetDensity(), GetCapacity()*1e-6, GetMass()*1e-3);
   
   cout << divider.Data() << "\n";
   
   printf("%6s %35s %14.3e %14.3e \n","TOTAL:","",GetCapacity()*1e-6,GetMass()*1e-3);
    
   cout << divider.Data() << "\n";
   
   cout << "\n";
}


