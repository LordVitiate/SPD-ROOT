// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdRsTEC2
//_____________________________________________________________________________

#include "SpdRsTEC2.h"

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
#include "TMath.h"

#include "SpdRsTEC2Point.h"
#include "SpdRsTEC2GeoMapper.h"

#include "SpdDetectorList.h"
#include "SpdStack.h"
#include "SpdMCEventHeader.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include <iostream>

using namespace std;
using namespace TMath;

ClassImp(SpdRsTEC2)

//_____________________________________________________________________________________
SpdRsTEC2::SpdRsTEC2(): 

  SpdDetector("RS endcaps (qsl)", kSpdRsTEC, kTRUE),
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
  SetParametersType("RsTECParSet");
   
  fNDataOut = 1;
  fOutDataPointObject = "SpdRsTEC2Point"; 

  fBaseColor = kGreen+1;   // (air)   
  fAbsorbColor = kGreen+3; // (iron)
}

//_____________________________________________________________________________________
SpdRsTEC2::SpdRsTEC2(const char* name, Bool_t active):

  SpdDetector(name, kSpdRsTEC, active),
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
  SetParametersType("RsTECParSet");
   
  fNDataOut = 1;
  fOutDataPointObject = "SpdRsTEC2Point";  

  fBaseColor = kGreen+1;   // (air)   
  fAbsorbColor = kGreen+3; // (iron)

}

//_____________________________________________________________________________________
SpdRsTEC2::~SpdRsTEC2()
{
  if (fPointCollection) {
      fPointCollection->Delete();
      delete fPointCollection;
      fPointCollection = 0;
  }
}

//_____________________________________________________________________________________
void SpdRsTEC2::Initialize()
{
  cout << "\n*******************************************************************************" << endl;
  cout << "************************* SpdRsTEC2::Initialize *******************************" << endl;
  cout << "*******************************************************************************\n" << endl;
   
  // data collections
  fPointCollection = new TClonesArray(fOutDataPointObject);
  
  // Initialize module and fill parameters
  SpdDetector::Initialize(); 
 
  // SpdParSet* pars = GetParameters();
  // if (pars) pars->printParams();
}

//_____________________________________________________________________________________
void SpdRsTEC2::Reset()
{
  fPointCollection->Delete();
  //fPointCollection->Clear();
}

//_____________________________________________________________________________________
void SpdRsTEC2::Register()
{
  if (fGeoMapper && fGeoMapper->GetGeoType() < 1) return;
    
  FairRootManager::Instance()->Register(fOutDataPointObject,"SpdRsTEC2", 
                                        fPointCollection, kTRUE);
}

//_____________________________________________________________________________________
Bool_t SpdRsTEC2::ProcessHits(FairVolume* vol)
{
  //cout << "<SpdRsTEC2::ProcessHits> " << gMC->CurrentVolPath() << endl;
  
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

  // Create SpdRsTEC2Point at exit of active volume
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) 
  {
    if (!fSaveEmptyHits && fELoss == 0.) { return kFALSE; }
    
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    fSegmentLength = gMC->TrackLength() - fLength;
    fTimeOut   = gMC->TrackTime() * 1.e9;  // ns -> s

    fVolumeID =  kSpdRsTEC;
    fVolumePath = gMC->CurrentVolPath();
    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);

    //cout << "path = " << gMC->CurrentVolPath() << endl;
   
    AddHit();
    
    //cout << "<SpdRsTEC2::AddHit> " << fPointCollection->GetEntriesFast() << endl;
   
    SpdStack* stack = (SpdStack*)gMC->GetStack();
    stack->AddPoint(kSpdRsTEC);
  } 
   
  return kTRUE;
}

//_____________________________________________________________________________________
void SpdRsTEC2::AddHit() 
{
  static Bool_t addhit = (fOutDataPointObject == "SpdRsTEC2Point") ? kTRUE : kFALSE;
  
  if (!addhit) return;
  
  TClonesArray& clref = *fPointCollection;
  Int_t size = clref.GetEntriesFast();
  
  new(clref[size]) SpdRsTEC2Point(fTrackID,fVolumeID,fVolumePath,
                                 TVector3(fPos.X(),fPos.Y(),fPos.Z()),
                                 TVector3(fMom.Px(),fMom.Py(),fMom.Pz()),
                                 TVector3(fPosOut.X(),fPosOut.Y(),fPosOut.Z()),
                                 TVector3(fMomOut.Px(),fMomOut.Py(),fMomOut.Pz()),
                                 fTime, fTimeOut, 
				 fLength, fSegmentLength, fELoss);                                 
}

//_____________________________________________________________________________________
void SpdRsTEC2::EndOfEvent()
{
  Reset();
}

//_____________________________________________________________________________________
void SpdRsTEC2::FinishRun() 
{
  //SpdDetector::FinishRun();
  FillNodesTableIn(GetParameters());
}

//_____________________________________________________________________________________
void SpdRsTEC2::ConstructGeometry()
{   
  // Create detector geometry
  
  fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();
  
  if (!fMasterVolume) {  
      cout << "-E- <SpdTsTEC2::ConstructGeometry> No MASTER volume " << endl;
      return;
  }

  if (!GetMapper()) return;
  
  if (!fGeoMapper->InitGeometry()) return;
      
  //fGeoMapper->Print("");
  
  Int_t geo_type = fGeoMapper->GetGeoType();

  ConstructDetector();

  fGeoMapper->LockGeometry();
  
  cout << "\n*******************************************************************************" << endl;
  cout << "********************** SpdRsTEC2::ConstructGeometry ***************************" << endl;
  cout << "********************** GEOMETRY TYPE: " << geo_type 
       << " ***************************************" << endl;
  cout << "*******************************************************************************\n" << endl;
}

//_____________________________________________________________________________________
void SpdRsTEC2::ConstructDetector()
{

  SpdParameter* par;
  SpdRsTEC2GeoMapper* mapper = dynamic_cast<SpdRsTEC2GeoMapper*>(fGeoMapper);
  if (!mapper) return;

  Int_t nsectors 		= mapper->GetNSectors();		//8;
  Double_t rsInnerRadius 	= mapper->GetModuleInnerRadius();	//20.0/2;
  Double_t rsOuterRadius 	= mapper->GetModuleOuterRadius(); 	//628.8/2.;
  Double_t rsThickness 		= mapper->GetModuleThickness();		//139.0;

  fModule = BuildModule(mapper->AddPrefix("Vol"), mapper->GetBaseMaterial(),                         
		  	nsectors, rsInnerRadius, rsOuterRadius, rsThickness);

  if (!fModule) 
  {       
      cout << "-E- <SpdRsTEC2::ConstructGeometry> "            
	   << "Volume " << fModule->GetName() << " cannot be created " << endl;       
      return;   
  }

  fModule->SetFillColor(kGreen);   
  fModule->SetLineColor(kGreen);   
  fModule->SetTransparency(40); 

  /*-----------------------------------------------------------*/
  TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
  rotNoRot->RegisterYourself();

  Int_t nLayers = mapper->GetNLayers();

  // Add bottom layer (absrober(6.0cm) + mdt detectors(3.5cm))
  fModule->AddNode(CreateFirstLayer(rsInnerRadius, rsOuterRadius, 0.5*rsThickness), 1, 
                   new TGeoCombiTrans("",  0, 0, 0, rotNoRot));

  // Add regular layers (3.0cm absorber + 3.5cm mdt detectors)
  for (Int_t i=2; i < nLayers+1; ++i) {   
      fModule->AddNode(CreateRegularLayer(i, rsInnerRadius, rsOuterRadius, 0.5*rsThickness), i, 
                       new TGeoCombiTrans("", 0, 0, 0, rotNoRot));
  }

  // Add top absorber (6.0cm)
  fModule->AddNode(CreateLastLayer(nLayers+1, rsInnerRadius, rsOuterRadius, 0.5*rsThickness), nLayers+1, 
                   new TGeoCombiTrans("", 0, 0, 0, rotNoRot));

  /*-----------------------------------------------------------*/

  Double_t mindist       = mapper->GetDist();       
  Double_t angle	 = mapper->GetSecAngleOver2(); //pi/8    
 
  Double_t shift = mindist+0.5*rsThickness;  // z-shift    
  
  TGeoTranslation trans;   
  TGeoRotation rot;     
  
  rot.RotateZ(45+angle);     
  trans.SetTranslation(0,0,shift);
  fMasterVolume->AddNode(fModule,1,new TGeoCombiTrans(trans, rot));   
  
  trans.SetTranslation(0,0,-shift);   
  rot.RotateX(180);
  fMasterVolume->AddNode(fModule,2,new TGeoCombiTrans(trans, rot));   
}

//_____________________________________________________________________________________
TGeoVolume* SpdRsTEC2::BuildModule(const char* name, TString mat, 
				   Int_t nsectors, Double_t rsInnerRadius, Double_t rsOuterRadius, Double_t rsThickness) 
{   

  TGeoMedium* material = FindMedium(mat,"air");

  if (!material) {       
      cout << "-E- <SpdRsTEC2::BuildModule> "             
	   << "Unknown material: " << mat << endl;       
      return 0;   
  }

  TGeoVolume* vol = gGeoManager->MakePgon(name, material, 0, 360, nsectors, 2);
  TGeoPgon* pgon = (TGeoPgon*)vol->GetShape();
  pgon->DefineSection(0, -0.5*rsThickness, rsInnerRadius, rsOuterRadius);
  pgon->DefineSection(1,  0.5*rsThickness, rsInnerRadius, rsOuterRadius);
  
  return vol;
}

//_____________________________________________________________________________________
TGeoVolume* SpdRsTEC2::CreateFirstLayer(Double_t rsInnerRadius, Double_t rsOuterRadius, Double_t rsThickness) 
{
  //
  // Creates polygon representing bottom layer of the RS EC part
  // (6.0cm thick iron absorber and a layer of MDT detectors)
  // Parameters: 1. Inner radius of RS from collision point / tube radius (rsInnerRadius)
  //             2. Outer radius of RS from collision point (rsOuterRadius)
  //             3. Thickness of RS EC rsThickness
  // Returns: TGeoVolume object of the layer with absorber and MDTs.
  //
  SpdRsTEC2GeoMapper* mapper = dynamic_cast<SpdRsTEC2GeoMapper*>(fGeoMapper);
  if (!mapper) {
      cout << "-E- <SpdRsTEC2::CreateFirstLayer> Unknown mapper: " << mapper->ClassName() << endl;
      return 0;
  }

  Double_t layerThickness = 6.0+3.5; //cm

  Double_t widthEnv = 8.5; //cm xAxis
  Double_t thicknessEnv = 1.64; //cm zAxis

  TString mat;

  mat = mapper->GetBaseMaterial();
  TGeoMedium* pair = FindMedium(mat,"");

  mat = mapper->GetAbsorbMaterial();
  TGeoMedium   *piron = FindMedium(mat,"");

   //Matrices
  TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
  rotNoRot->RegisterYourself();

  TGeoRotation* rotMDTRot = new TGeoRotation("rotMDTRot", 45./2, 90, 0); 
  rotMDTRot->RegisterYourself();

  
  TGeoPgon* Layer_EC_Shape = new TGeoPgon("Layer_EC_Shape", 0, 360, 8, 2);
  Layer_EC_Shape->DefineSection(0, -rsThickness, rsInnerRadius, rsOuterRadius);
  Layer_EC_Shape->DefineSection(1, -rsThickness+layerThickness, rsInnerRadius, rsOuterRadius);

  //cout << "1 layer: " << -rsThickness << "    " << -rsThickness + layerThickness << endl;

  TGeoVolume* Layer_EC_Vol = new TGeoVolume("LayerECVol", Layer_EC_Shape, pair);

  TGeoPgon* Layer_EC_Abs_Base_Shape = new TGeoPgon("Layer_EC_Abs_Base_Shape", 0, 360, 8, 2);
  Layer_EC_Abs_Base_Shape->DefineSection(0, -rsThickness, rsInnerRadius, rsOuterRadius);
  Layer_EC_Abs_Base_Shape->DefineSection(1, -rsThickness+6.0, rsInnerRadius, rsOuterRadius);

  TGeoVolume* Layer_EC_Abs_Base_Vol = new TGeoVolume("LayerECAbsBaseVol", Layer_EC_Abs_Base_Shape, piron);
  Layer_EC_Vol->AddNode(Layer_EC_Abs_Base_Vol, 1, new TGeoCombiTrans("", 0, 0, 0, rotNoRot)); //


  Int_t Nenvelopes = Int_t((rsOuterRadius-rsInnerRadius)/widthEnv);
  Double_t shift = 0.5*(rsOuterRadius - rsInnerRadius - Nenvelopes*widthEnv);

  Double_t minMDTsize = (rsOuterRadius)*(sqrt(2.0)-1.0); // min size of MDT
  Int_t Nenvelopes_MDTvar = Int_t((rsOuterRadius-minMDTsize)/widthEnv); // number of MDTs with varying sizes

  Double_t Layer_Envelope_startX = -rsOuterRadius+shift;
  Double_t Layer_Envelope_shiftX = Layer_Envelope_startX + widthEnv/2;


  Double_t rotX = TMath::Cos(((Double_t)1)*TMath::ACos(-1.0)/8.0);
  Double_t rotY = TMath::Sin(((Double_t)1)*TMath::ACos(-1.0)/8.0);

  Double_t smMDT = minMDTsize;
  Int_t NumberOfMDT = 1;
  for (UInt_t i(0); i<Nenvelopes_MDTvar; i++) 
  {
      Layer_EC_Vol->AddNode(CreateMDT(int(2*smMDT)), NumberOfMDT, 
                            new TGeoCombiTrans("", Layer_Envelope_shiftX*rotX, Layer_Envelope_shiftX*rotY, -rsThickness+6.0+3.5/2, rotMDTRot));

      smMDT = Layer_Envelope_shiftX + (rsOuterRadius + minMDTsize); // y = x + b
      Layer_Envelope_shiftX = Layer_Envelope_shiftX + widthEnv;
      NumberOfMDT++;
  }

  for (UInt_t i=Nenvelopes_MDTvar; i<Nenvelopes; i++) 
  {
      Layer_EC_Vol->AddNode(CreateMDT(2*(rsOuterRadius-10)), NumberOfMDT, new TGeoCombiTrans("", Layer_Envelope_shiftX*rotX, Layer_Envelope_shiftX*rotY, -rsThickness+6.0+3.5/2, rotMDTRot));
      Layer_Envelope_shiftX = Layer_Envelope_shiftX + widthEnv;
      NumberOfMDT++;
  }


  //transition is picked up
  Layer_EC_Vol->AddNode(CreateMDT((rsOuterRadius-25)), NumberOfMDT, 
      new TGeoCombiTrans("", 44+widthEnv, -rsOuterRadius/2+7, -rsThickness+6.0+3.5/2, rotMDTRot));
  NumberOfMDT++;
  Layer_EC_Vol->AddNode(CreateMDT((rsOuterRadius-25)), NumberOfMDT, 
      new TGeoCombiTrans("", 44+widthEnv*2, -rsOuterRadius/2+10, -rsThickness+6.0+3.5/2, rotMDTRot));
  NumberOfMDT++;
  Layer_EC_Vol->AddNode(CreateMDT((rsOuterRadius-25)), NumberOfMDT, 
      new TGeoCombiTrans("", 44+widthEnv*3, -rsOuterRadius/2+13, -rsThickness+6.0+3.5/2, rotMDTRot));
  NumberOfMDT++;

  Layer_EC_Vol->AddNode(CreateMDT((rsOuterRadius-25)), NumberOfMDT, 
      new TGeoCombiTrans("", -44-widthEnv, rsOuterRadius/2-7, -rsThickness+6.0+3.5/2, new TGeoRotation("rotMDTRot", 45./2, -90, 0)));
  NumberOfMDT++;
  Layer_EC_Vol->AddNode(CreateMDT((rsOuterRadius-25)), NumberOfMDT, 
      new TGeoCombiTrans("", -44-widthEnv*2, rsOuterRadius/2-10, -rsThickness+6.0+3.5/2, new TGeoRotation("rotMDTRot", 45./2, -90, 0)));
  NumberOfMDT++;
  Layer_EC_Vol->AddNode(CreateMDT((rsOuterRadius-25)), NumberOfMDT, 
      new TGeoCombiTrans("", -44-widthEnv*3, rsOuterRadius/2-13, -rsThickness+6.0+3.5/2, new TGeoRotation("rotMDTRot", 45./2, -90, 0)));
  NumberOfMDT++;

  Layer_Envelope_shiftX = -(Layer_Envelope_shiftX - widthEnv);
  for (UInt_t i=Nenvelopes_MDTvar; i<Nenvelopes; i++) 
  {
      Layer_EC_Vol->AddNode(CreateMDT(2*(rsOuterRadius-10)), NumberOfMDT, new TGeoCombiTrans("", Layer_Envelope_shiftX*rotX, Layer_Envelope_shiftX*rotY, -rsThickness+6.0+3.5/2, rotMDTRot));
      Layer_Envelope_shiftX = Layer_Envelope_shiftX + widthEnv;
      NumberOfMDT++;
  }

  for (UInt_t i=0; i<Nenvelopes_MDTvar; i++) 
  {
      smMDT = -Layer_Envelope_shiftX-widthEnv + (rsOuterRadius + minMDTsize); // y = -x + b
      Layer_EC_Vol->AddNode(CreateMDT(int(2*smMDT)), NumberOfMDT, new TGeoCombiTrans("", Layer_Envelope_shiftX*rotX, Layer_Envelope_shiftX*rotY, -rsThickness+6.0+3.5/2, rotMDTRot));
      Layer_Envelope_shiftX = Layer_Envelope_shiftX + widthEnv;
      NumberOfMDT++;
  }

  Layer_EC_Abs_Base_Vol->SetFillColor(fAbsorbColor);   
  Layer_EC_Abs_Base_Vol->SetLineColor(fAbsorbColor);   
  Layer_EC_Abs_Base_Vol->SetTransparency(40);      
  
  Layer_EC_Vol->SetFillColor(fBaseColor);   
  Layer_EC_Vol->SetLineColor(fBaseColor);   
  Layer_EC_Vol->SetTransparency(40);  

  return Layer_EC_Vol;
}

//_____________________________________________________________________________________
TGeoVolume* SpdRsTEC2::CreateRegularLayer(Int_t layerN, Double_t rsInnerRadius, Double_t rsOuterRadius, Double_t rsThickness) 
{
  //
  // Creates trapezoid representing regular layer of the RS EC part
  // (3.0cm thick iron absorber and a layer of MDT detectors)
  // Parameters: 1. Number of layer in a sector
  //             2. Inner radius of RS from collision point / tube radius (rsInnerRadius)
  //             3. Outer radius of RS from collision point (rsOuterRadius)
  //             4. Thickness of RS EC rsThickness
  // Returns: TGeoVolume object of the layer with absorber and MDTs.
  //
  SpdRsTEC2GeoMapper* mapper = dynamic_cast<SpdRsTEC2GeoMapper*>(fGeoMapper);
  if (!mapper) {
      cout << "-E- <SpdRsTEC2::CreateRegularLayer> Unknown mapper: " << mapper->ClassName() << endl;
      return 0;
  }

  TString mat;

  mat = mapper->GetBaseMaterial();
  TGeoMedium* pair = FindMedium(mat,"");

  mat = mapper->GetAbsorbMaterial();
  TGeoMedium   *piron = FindMedium(mat,"");

  Double_t layerThickness = 3.0+3.5; //cm
  Double_t widthEnv = 8.5; //cm xAxis
  Double_t thicknessEnv = 1.64; //cm zAxis

   //Matrices
  TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
  rotNoRot->RegisterYourself();

  TGeoRotation* rotMDTRot = new TGeoRotation("rotMDTRot", 45./2, 90, 0); // 
  rotMDTRot->RegisterYourself();

  Double_t shiftZ = -rsThickness + 9.5 + (layerN-2)*layerThickness;

  TGeoPgon* Layer_EC_Shape = new TGeoPgon("Layer_EC_Shape", 0, 360, 8, 2);
  Layer_EC_Shape->DefineSection(0, shiftZ, rsInnerRadius, rsOuterRadius);
  Layer_EC_Shape->DefineSection(1, shiftZ + layerThickness, rsInnerRadius, rsOuterRadius);

  //cout << layerN << " layer: " << shiftZ << "    " <<  shiftZ + layerThickness << endl;

  TGeoVolume* Layer_EC_Vol = new TGeoVolume("LayerECVol", Layer_EC_Shape, pair);


  TGeoPgon* Layer_EC_Abs_Base_Shape = new TGeoPgon("Layer_EC_Abs_Base_Shape", 0, 360, 8, 2);
  Layer_EC_Abs_Base_Shape->DefineSection(0, shiftZ, rsInnerRadius, rsOuterRadius);
  Layer_EC_Abs_Base_Shape->DefineSection(1, shiftZ + 3, rsInnerRadius, rsOuterRadius);

  TGeoVolume* Layer_EC_Abs_Base_Vol = new TGeoVolume("LayerECAbsBaseVol", Layer_EC_Abs_Base_Shape, piron);
  Layer_EC_Vol->AddNode(Layer_EC_Abs_Base_Vol, 1, new TGeoCombiTrans("", 0, 0, 0, rotNoRot)); //


  Int_t Nenvelopes = Int_t((rsOuterRadius-rsInnerRadius)/widthEnv);
  Double_t shift = 0.5*(rsOuterRadius - rsInnerRadius - Nenvelopes*widthEnv);

  Double_t minMDTsize = (rsOuterRadius)*(sqrt(2.0)-1.0); // min size of MDT
  Int_t Nenvelopes_MDTvar = Int_t((rsOuterRadius-minMDTsize)/widthEnv); // number of MDTs with varying sizes

  Double_t Layer_Envelope_startX;

  if (layerN % 2 == 0) { Layer_Envelope_startX = -rsOuterRadius+shift; }
  else 
  {
      shift = shift + 0.5; //shift for half of the size MPD cell
      Layer_Envelope_startX = -rsOuterRadius+shift;
  }


  Double_t rotX = TMath::Cos(((Double_t)1)*TMath::ACos(-1.0)/8.0);
  Double_t rotY = TMath::Sin(((Double_t)1)*TMath::ACos(-1.0)/8.0);

  Double_t Layer_Envelope_shiftX = Layer_Envelope_startX + widthEnv/2;

  Double_t smMDT = minMDTsize;
  Int_t NumberOfMDT = 1;
  for (UInt_t i=0; i<Nenvelopes_MDTvar; i++) 
  {
      Layer_EC_Vol->AddNode(CreateMDT(int(2*smMDT)), NumberOfMDT, new TGeoCombiTrans("", Layer_Envelope_shiftX*rotX, Layer_Envelope_shiftX*rotY, shiftZ+3.0+3.5/2, rotMDTRot));

      smMDT = Layer_Envelope_shiftX + (rsOuterRadius + minMDTsize); // y = x + b
      Layer_Envelope_shiftX = Layer_Envelope_shiftX + widthEnv;
      NumberOfMDT++;
  }

  for (UInt_t i=Nenvelopes_MDTvar; i<Nenvelopes; i++) 
  {

      Layer_EC_Vol->AddNode(CreateMDT(2*(rsOuterRadius-10)), NumberOfMDT, new TGeoCombiTrans("", Layer_Envelope_shiftX*rotX, Layer_Envelope_shiftX*rotY, shiftZ+3.0+3.5/2, rotMDTRot));
      Layer_Envelope_shiftX = Layer_Envelope_shiftX + widthEnv;
      NumberOfMDT++;
  }

  //transition is picked up
  Layer_EC_Vol->AddNode(CreateMDT((rsOuterRadius-25)), NumberOfMDT, 
      new TGeoCombiTrans("", 44+widthEnv, -rsOuterRadius/2+7, shiftZ+3.0+3.5/2, rotMDTRot));
  NumberOfMDT++;
  Layer_EC_Vol->AddNode(CreateMDT((rsOuterRadius-25)), NumberOfMDT, 
      new TGeoCombiTrans("", 44+widthEnv*2, -rsOuterRadius/2+10, shiftZ+3.0+3.5/2, rotMDTRot));
  NumberOfMDT++;
  Layer_EC_Vol->AddNode(CreateMDT((rsOuterRadius-25)), NumberOfMDT, 
      new TGeoCombiTrans("", 44+widthEnv*3, -rsOuterRadius/2+13, shiftZ+3.0+3.5/2, rotMDTRot));
  NumberOfMDT++;

  Layer_EC_Vol->AddNode(CreateMDT((rsOuterRadius-25)), NumberOfMDT, 
      new TGeoCombiTrans("", -44-widthEnv, rsOuterRadius/2-7, shiftZ+3.0+3.5/2, new TGeoRotation("rotMDTRot", 45./2, -90, 0)));
  NumberOfMDT++;
  Layer_EC_Vol->AddNode(CreateMDT((rsOuterRadius-25)), NumberOfMDT, 
      new TGeoCombiTrans("", -44-widthEnv*2, rsOuterRadius/2-10, shiftZ+3.0+3.5/2, new TGeoRotation("rotMDTRot", 45./2, -90, 0)));
  NumberOfMDT++;
  Layer_EC_Vol->AddNode(CreateMDT((rsOuterRadius-25)), NumberOfMDT, 
      new TGeoCombiTrans("", -44-widthEnv*3, rsOuterRadius/2-13, shiftZ+3.0+3.5/22, new TGeoRotation("rotMDTRot", 45./2, -90, 0)));
  NumberOfMDT++;

  
  Layer_Envelope_shiftX = -(Layer_Envelope_shiftX - widthEnv);
  for (UInt_t i=Nenvelopes_MDTvar; i<Nenvelopes; i++) 
  {

      Layer_EC_Vol->AddNode(CreateMDT(2*(rsOuterRadius-10)), NumberOfMDT, new TGeoCombiTrans("", Layer_Envelope_shiftX*rotX, Layer_Envelope_shiftX*rotY, shiftZ+3.0+3.5/2, rotMDTRot));
      Layer_Envelope_shiftX = Layer_Envelope_shiftX + widthEnv;
      NumberOfMDT++;
  }

  for (UInt_t i=0; i<Nenvelopes_MDTvar; i++) 
  {

      smMDT = -Layer_Envelope_shiftX-widthEnv + (rsOuterRadius + minMDTsize); // y = -x + b
      Layer_EC_Vol->AddNode(CreateMDT(int(2*smMDT)), NumberOfMDT, new TGeoCombiTrans("", Layer_Envelope_shiftX*rotX, Layer_Envelope_shiftX*rotY, shiftZ+3.0+3.5/2, rotMDTRot));
      Layer_Envelope_shiftX = Layer_Envelope_shiftX + widthEnv;
      NumberOfMDT++;
  }

  Layer_EC_Abs_Base_Vol->SetFillColor(fAbsorbColor);
  Layer_EC_Abs_Base_Vol->SetLineColor(fAbsorbColor);
  Layer_EC_Abs_Base_Vol->SetTransparency(40);

  Layer_EC_Vol->SetFillColor(fBaseColor);
  Layer_EC_Vol->SetLineColor(fBaseColor);
  Layer_EC_Vol->SetTransparency(40);

  return Layer_EC_Vol;
}

//_____________________________________________________________________________________
TGeoVolume* SpdRsTEC2::CreateLastLayer(Int_t layerN, Double_t rsInnerRadius, Double_t rsOuterRadius, Double_t rsThickness) 
{
  //
  // Creates trapezoid representing top layer of the RS EC part
  // (6.0cm thick iron absorber and no layer of MDT detectors)
  // Parameters: 1. Number of layer in a sector
  //             2. Inner radius of RS from collision point / tube radius (rsInnerRadius)
  //             3. Outer radius of RS from collision point (rsOuterRadius)
  //             4. Thickness of RS EC rsThickness
  // Returns: TGeoVolume object of the layer with absorber and MDTs.
  //
  SpdRsTEC2GeoMapper* mapper = dynamic_cast<SpdRsTEC2GeoMapper*>(fGeoMapper);
  if (!mapper) {
      cout << "-E- <SpdRsTEC2::CreateLastLayer> Unknown mapper: " << mapper->ClassName() << endl;
      return 0;
  }

  TString mat;

  mat = mapper->GetBaseMaterial();
  TGeoMedium* pair = FindMedium(mat,"");

  mat = mapper->GetAbsorbMaterial();
  TGeoMedium   *piron = FindMedium(mat,"");

  Double_t layerThickness = 6.0; //cm


   //Matrices
  TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
  rotNoRot->RegisterYourself();

  Double_t shiftZ = -rsThickness + 9.5 + (layerN-2)*6.5;

  TGeoPgon* Layer_EC_Shape = new TGeoPgon("Layer_EC_Shape", 0, 360, 8, 2);
  Layer_EC_Shape->DefineSection(0, shiftZ, rsInnerRadius, rsOuterRadius);
  Layer_EC_Shape->DefineSection(1, shiftZ + layerThickness, rsInnerRadius, rsOuterRadius);

  //cout<<"Last layer: " << shiftZ << "    " <<  shiftZ + layerThickness << endl;

  TGeoVolume* Layer_EC_Vol = new TGeoVolume("LayerECVol", Layer_EC_Shape, pair);


  TGeoPgon* Layer_EC_Abs_Base_Shape = new TGeoPgon("Layer_EC_Abs_Base_Shape", 0, 360, 8, 2);
  Layer_EC_Abs_Base_Shape->DefineSection(0, shiftZ, rsInnerRadius, rsOuterRadius);
  Layer_EC_Abs_Base_Shape->DefineSection(1, shiftZ + layerThickness, rsInnerRadius, rsOuterRadius);

  TGeoVolume* Layer_EC_Abs_Base_Vol = new TGeoVolume("LayerECAbsBaseVol", Layer_EC_Abs_Base_Shape, piron);
  Layer_EC_Vol->AddNode(Layer_EC_Abs_Base_Vol, 1, new TGeoCombiTrans("", 0, 0, 0, rotNoRot)); //

  Layer_EC_Abs_Base_Vol->SetFillColor(fAbsorbColor);
  Layer_EC_Abs_Base_Vol->SetLineColor(fAbsorbColor);
  Layer_EC_Abs_Base_Vol->SetTransparency(40);

  Layer_EC_Vol->SetFillColor(fBaseColor);
  Layer_EC_Vol->SetLineColor(fBaseColor);
  Layer_EC_Vol->SetTransparency(40);

  return Layer_EC_Vol;
}

//_____________________________________________________________________________________
TGeoVolume* SpdRsTEC2::CreateMDT(Double_t length) 
{
  //
  // Creates MDT of given length;
  // Envelope Dimensions(L x W x H): length x 8.5cm x 1.64cm.
  // Note: Adopted from Panda Muon System
  //
  SpdRsTEC2GeoMapper* mapper = dynamic_cast<SpdRsTEC2GeoMapper*>(fGeoMapper);   
  if (!mapper) {       
      cout << "-E- <SpdRsTEC2::CreateMDT> Unknown mapper: " << mapper->ClassName() << endl;       
      return 0;   
  } 

   TString mat;     
   
   mat = mapper->GetBaseMaterial();   
   TGeoMedium* pair = FindMedium(mat,"");     
   
   mat = mapper->GetActiveMaterialMDT();   
   TGeoMedium* pMDTMixture = FindMedium(mat,"");      
   
   mat = mapper->GetPassiveMaterialMDT();   
   TGeoMedium* paluminium = FindMedium(mat,"");

  // Shapes and volumes ---------------------------------------------------------------
  // General dimensions
  Double_t widthEnv = 8.5; //cm xAxis
  Double_t thicknessEnv = 1.64; //cm zAxis
  Double_t xSize = 1.0;

  TString transName;
  Int_t Nenvelopes;
  Int_t IncEnv = 0;

  //Matrices
  TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
  rotNoRot->RegisterYourself();

  Double_t empty_value = (2.0+2.0+8.5)+6.5*2;
  Double_t L_MDT_Envelope = length; //493.0;
  Double_t L_MDT_Envelope_sensitive = L_MDT_Envelope-empty_value;


  Int_t ind = int(length*10);

  //Shapes
  TGeoBBox* Indent20_Shape   	= new TGeoBBox("Indent20_Shape", widthEnv/2, thicknessEnv/2,  1.0);   
  TGeoBBox* FEB_box_Shape    	= new TGeoBBox("FEBbox_Shape",  widthEnv/2, thicknessEnv/2,  8.5/2);   
  TGeoBBox* DeadMat_Shape    	= new TGeoBBox("DeadMat_Shape",  4.07, 0.515,  3.25); 

  TGeoVolume* Indent20_Vol   	= new TGeoVolume("Indent20Vol", Indent20_Shape, pair);
  TGeoVolume* FEB_box_Vol    	= new TGeoVolume("FEBboxVol", FEB_box_Shape, pair);
  TGeoVolume* DeadMat_Vol    	= new TGeoVolume("DeadMatVol", DeadMat_Shape, pair);

  //---------------------------------------     
  
  TGeoBBox* L_Al_A_Shape     	= new TGeoBBox(Form("L%d_Al_A_Shape",ind),    4.03, 0.03, L_MDT_Envelope_sensitive/2);   
  TGeoBBox* L_Al_B_Shape     	= new TGeoBBox(Form("L%d_Al_B_Shape",ind),    0.03, 0.42, L_MDT_Envelope_sensitive/2);   
  TGeoBBox* L_GasCell_Shape  	= new TGeoBBox(Form("L%d_GasCell_Shape",ind), 0.47, 0.46, L_MDT_Envelope_sensitive/2);   
  TGeoBBox* L_Cell_Shape     	= new TGeoBBox(Form("L%d_Cell_Shape",ind),    0.5,  0.46, L_MDT_Envelope_sensitive/2);   
  TGeoBBox* L_MDT_S_Shape    	= new TGeoBBox(Form("L%d_MDT_S_Shape",ind),   4.0,  0.46, L_MDT_Envelope_sensitive/2); 
  TGeoBBox* L_Envelope_Shape 	= new TGeoBBox(Form("L%d_Envelope_Shape",ind), widthEnv/2, thicknessEnv/2, L_MDT_Envelope/2);

  //Volumes
  TGeoVolume* L_Al_A_Vol      = new TGeoVolume(Form("L%dAlAVol",ind), L_Al_A_Shape, paluminium);
  TGeoVolume* L_Al_B_Vol 	    = new TGeoVolume(Form("L%dAlBVol",ind), L_Al_B_Shape, paluminium);
  TGeoVolume* L_GasCell_Vol 	= new TGeoVolume(Form("L%dGasCellVol",ind), L_GasCell_Shape, pMDTMixture);
  TGeoVolume* L_Cell_Vol 	    = new TGeoVolume(Form("L%dCellVol",ind), L_Cell_Shape, pair);
  TGeoVolume* L_MDT_S_Vol 	  = new TGeoVolume(Form("L%dMDTSVol",ind), L_MDT_S_Shape, pair);
  TGeoVolume* L_Envelope_Vol 	= new TGeoVolume(Form("L%dEnvelopeVol",ind), L_Envelope_Shape, pair);

  // Volume hierarchy -----------------------------------------------------------------
  //Structure
  //GasCell and Al_B in Cell
  L_Cell_Vol->AddNode(L_GasCell_Vol, 1,
                          new TGeoCombiTrans("", -0.03, 0, 0, rotNoRot));
  L_Cell_Vol->AddNode(L_Al_B_Vol, 1,
                          new TGeoCombiTrans("", 0.47, -0.04, 0, rotNoRot));

  //8 Cells in MDT_S
  const Int_t NCells = 8;
  for (UInt_t i(0); i<NCells; i++) {
      transName.Form("trans_cell_%d", i);
      TGeoCombiTrans* trans1 = new TGeoCombiTrans(transName.Data(), -4.0 + xSize/2. + (Double_t)i*xSize, 0, 0., rotNoRot);
      trans1->RegisterYourself();
      L_MDT_S_Vol->AddNode(L_Cell_Vol, i+1, trans1);
  }


  L_Envelope_Vol->AddNode(Indent20_Vol, 1, new TGeoCombiTrans("", 0, 0, L_MDT_Envelope/2 - 1.0, rotNoRot));
  L_Envelope_Vol->AddNode(Indent20_Vol, 2, new TGeoCombiTrans("", 0, 0, 1.0 - L_MDT_Envelope/2, rotNoRot));
  L_Envelope_Vol->AddNode(FEB_box_Vol,  1, new TGeoCombiTrans("", 0, 0, L_MDT_Envelope/2-2.0-8.5/2, rotNoRot));
  L_Envelope_Vol->AddNode(DeadMat_Vol,  1, new TGeoCombiTrans("", 0, 0, ((L_MDT_Envelope-(2.0+2.0+8.5))-6.5)/2-8.5/2, rotNoRot));
  L_Envelope_Vol->AddNode(DeadMat_Vol,  2, new TGeoCombiTrans("", 0, 0, -((L_MDT_Envelope-(2.0+2.0+8.5))-6.5)/2-8.5/2, rotNoRot));
  L_Envelope_Vol->AddNode(L_Al_A_Vol,   1, new TGeoCombiTrans("", 0, -0.36, -8.5/2, rotNoRot));
  L_Envelope_Vol->AddNode(L_Al_B_Vol,   1, new TGeoCombiTrans("", -4.0, 0.09, -8.5/2, rotNoRot));
  L_Envelope_Vol->AddNode(L_MDT_S_Vol,  1, new TGeoCombiTrans("", 0.03, 0.13, -8.5/2, rotNoRot));
  
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
SpdGeoMapper* SpdRsTEC2::GetMapper() 
{
  if (fGeoMapper) return fGeoMapper;
  
  SpdGeoFactory* factory = SpdGeoFactory::Instance();
 
  // search for mapper
  fGeoMapper = factory->SearchForMapper("SpdRsTEC2GeoMapper");
  
  // create default mapper
  if (!fGeoMapper) fGeoMapper = factory->Mapper("SpdRsTEC2GeoMapper");
     
  return fGeoMapper;
}

//_____________________________________________________________________________
Bool_t SpdRsTEC2::LoadParsFrom(SpdParSet* params) 
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
TString SpdRsTEC2::GetDataOut(Int_t n) const 
{ 
  if (n < 0 || n >= fNDataOut) return "unknown";
  return fOutDataPointObject;
}


//_____________________________________________________________________________________
TClonesArray* SpdRsTEC2::GetCollection(Int_t iColl) const 
{ 
   return (iColl == 0) ? fPointCollection : 0; 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Double_t SpdRsTEC2::GetCapacity() const 
{ 
   // return (fModule) ? fModule->GetShape()->Capacity() : 0;
   return 0;
}
   
//_____________________________________________________________________________________
Double_t SpdRsTEC2::GetMass() const 
{ 
   return (fModule) ? GetCapacity()*GetDensity() : 0;
}

//_____________________________________________________________________________________
Double_t SpdRsTEC2::GetDensity() const 
{   
   return (fModule) ? fModule->GetMaterial()->GetDensity() : 0; 
} 

//_____________________________________________________________________________________ 
TString SpdRsTEC2::GetMaterialName() const // private
{ 
   return (fModule) ? fModule->GetMaterial()->GetName() : "";
}

//_____________________________________________________________________________
void SpdRsTEC2::Print(Option_t*) const
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


