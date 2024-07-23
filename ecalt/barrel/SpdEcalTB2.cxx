// $Id$
// Author: andre/artur   2020/07/31

#include "SpdEcalTB2.h"

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
#include "TGeoArb8.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TMatrixD.h"

#include "SpdEcalTB2.h"
#include "SpdEcalTB2Point.h"
#include "SpdEcalTB2GeoMapper.h"
#include "SpdCommonGeoMapper.h"
#include "SpdDetectorList.h"
#include "SpdStack.h"
#include "SpdParSet.h"
#include "SpdGeoFactory.h"

#include <map>
#include <iostream>
#include <vector>
#include <tuple>

using namespace std;

using std::cout;
using std::endl;

ClassImp(SpdEcalTB2)

//_____________________________________________________________________________________
SpdEcalTB2::SpdEcalTB2(): 

  SpdDetector("Ecal barrel (tor)", kSpdEcalTB, kTRUE),
  fTrackID(-1),
  fDetID(-1),fBasketID(-1),fModuleZID(-1), fModulePhiID(-1), fCellID(-1), fLayerID(-1), fMatID(""),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1.),
  fPointCollection(0),
  fEnableWritingPointsScint(kTRUE),
  fEnableWritingPointsAll(kFALSE),
  fBasket(0),
  fModule(0)
{
    SetParametersType("EcalTBParSet");
   
    fNDataOut = 1;
    fOutDataPointObject = "SpdEcalTB2Point"; 
    fUniqueModules.clear();
    
}

//_____________________________________________________________________________________
SpdEcalTB2::SpdEcalTB2(const char* name, Bool_t active):

  SpdDetector(name, kSpdEcalTB, active),
  fTrackID(-1),
  fDetID(-1),fBasketID(-1),fModuleZID(-1), fModulePhiID(-1), fCellID(-1), fLayerID(-1), fMatID(""),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1.),
  fPointCollection(0),
  fEnableWritingPointsScint(kTRUE),
  fEnableWritingPointsAll(kFALSE),
  fBasket(0),
  fModule(0)
{
   SetParametersType("EcalTBParSet");
   
   fNDataOut = 1;
   fOutDataPointObject = "SpdEcalTB2Point";
   fUniqueModules.clear(); 
}

//_____________________________________________________________________________________
SpdEcalTB2::~SpdEcalTB2()
{
  if (fPointCollection) {
      fPointCollection->Delete();
      delete fPointCollection;
      fPointCollection = 0;
  }
}

//_____________________________________________________________________________________
void SpdEcalTB2::Initialize()
{
  cout << "\n*******************************************************************************" << endl;
  cout << "************************* SpdEcalTB2::Initialize ******************************" << endl;
  cout << "*******************************************************************************\n" << endl;
  
  // data collections
  fPointCollection = new TClonesArray(fOutDataPointObject);
  
  // Initialize cell and fill parameters
  SpdDetector::Initialize(); 
 
  // SpdParSet* pars = GetParameters();
  // if (pars) pars->printParams();
}

//_____________________________________________________________________________________
void SpdEcalTB2::Reset()
{
  fPointCollection->Delete();
  //fPointCollection->Clear();
}

//_____________________________________________________________________________________
void SpdEcalTB2::Register()
{
  if (!fGeoMapper) return;
  
  if (fEnableWritingPointsScint) {
      FairRootManager::Instance()->Register(fOutDataPointObject,"SpdEcalTB2", 
                                            fPointCollection, kTRUE);
  }
}

//_____________________________________________________________________________________
Bool_t SpdEcalTB2::ProcessHits(FairVolume* vol)
{
  //cout << "<SpdEcalTB2::ProcessHits> " << endl;
  
  TString nodepath = gMC->CurrentVolPath();

  //Set parameters at entrance of volume. Reset ELoss.
  if (gMC->IsTrackEntering()) {
      //cout << "track entering" << endl;
      fELoss  = 0.;
      fTime   = gMC->TrackTime() * 1.e9;  // ns -> s
      fLength = gMC->TrackLength();
      gMC->TrackPosition(fPos);
      gMC->TrackMomentum(fMom);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Create SpdEcalTB2Point at exit of active volume
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) 
  {
    
    if (!fSaveEmptyHits && fELoss == 0.) { return kFALSE; }
    
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    
    fNodePath = gMC->CurrentVolPath();
    
    SpdGeopathParser parser;
    parser.ParsePath(fNodePath);
    
    fDetID = kSpdEcalTB;
    fBasketID = parser.Num(2, true);
    
    int rawModuleID = parser.Num(3, true);
    
    fModuleZID = int(double(rawModuleID)/100);
    fModulePhiID = rawModuleID - fModuleZID*100;
    
    fCellID  = parser.VNum(4, true);
    fLayerID = parser.Num(5, true);
    fMatID   = parser.Name(5, true);
    
    //cout << " <<< BARREL >>> " << fMatID << " " << gMC->CurrentVolPath() << endl;
    
    assert(fMatID == fAbsorberVolName|| fMatID == fScintVolName);
    
    if (fEnableWritingPointsAll) {
        AddHit();
    }
    else if (fEnableWritingPointsScint && fMatID == fScintVolName) {
        AddHit();
    }
        
    SpdStack* stack = (SpdStack*)gMC->GetStack();
    stack->AddPoint(kSpdEcalTB);
  } 
   
  return kTRUE;
}

//_____________________________________________________________________________________
void SpdEcalTB2::AddHit() 
{
  static Bool_t addhit = (fOutDataPointObject == "SpdEcalTB2Point") ? kTRUE : kFALSE;
  
  if (!addhit) return;
  
  TClonesArray& clref = *fPointCollection;
  Int_t size = clref.GetEntriesFast();
  
  //cout << "SpdEcalTB2::AddHit: Adding hit: " << fTrackID << 
  //                                 fPos.X() << " " << fPos.Y() << " " << fPos.Z() << " " << 
  //                                 fMom.Px() << " " << fMom.Py() << " " << fMom.Pz() << " " << 
  //                                fTime << " " << fLength << " " << fELoss << " path = " << fNodePath <<"" << endl;
 
  new(clref[size]) SpdEcalTB2Point(fTrackID, 
                                   fDetID, fBasketID, fModuleZID, fModulePhiID, fCellID, fLayerID, 
                                   fMatID,
                                   TVector3(fPos.X(),fPos.Y(),fPos.Z()),
                                   TVector3(fMom.Px(),fMom.Py(),fMom.Pz()),
                                   fTime, fLength, fELoss, fNodePath);                                 
}

//_____________________________________________________________________________________
void SpdEcalTB2::EndOfEvent()
{
  Reset();
}

//_____________________________________________________________________________________
void SpdEcalTB2::FinishRun() 
{
  //SpdDetector::FinishRun();
  FillNodesTableIn(GetParameters());
}

//_____________________________________________________________________________________
void SpdEcalTB2::ConstructGeometry()
{   
  fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();
      
  if (!fMasterVolume) {  
      cout << "-E- <SpdIts::ConstructGeometry> No MASTER volume " << endl;
      return;
  }
  
  if (!GetMapper()) return;
   
  if (!fGeoMapper->InitGeometry()) return;

  //fGeoMapper->Print("");
   
  SpdEcalTB2GeoMapper* mapper = dynamic_cast<SpdEcalTB2GeoMapper*>(fGeoMapper);
   
  fAbsorberVolName = mapper->GetAbsorberVolName();
  fScintVolName    = mapper->GetScintVolName();

  Int_t geo_type = fGeoMapper->GetGeoType();
  
  ConstructGeometry_1();
  
  fGeoMapper->LockGeometry(); 
  
  cout << "\n*******************************************************************************" << endl;
  cout << "********************** SpdEcalTB2::ConstructGeometry **************************" << endl;
  cout << "********************** GEOMETRY TYPE: " << geo_type 
       << " ***************************************" << endl;
  cout << "*******************************************************************************\n" << endl;
}

//________________________________________________________________________________
void SpdEcalTB2::ConstructGeometry_1()
{ 
    SpdEcalTB2GeoMapper* mapper = dynamic_cast<SpdEcalTB2GeoMapper*>(fGeoMapper);
    if (!mapper) return; 
  
    Bool_t forceCellSize = mapper->IsForceCellSize();
    
    BuildBasket();
    
    if (forceCellSize) {
        fModule = BuildModule();
        fUniqueModules.push_back(fModule);
        FillBasketFixedCells(fBasket,fModule);
    }
    else {
        FillBasketTightly(fBasket);
    }
    
    BuildBarrel();
}

//________________________________________________________________________________
void SpdEcalTB2::FillBasketTightly(TGeoVolume* basketVolume) 
{
    SpdEcalTB2GeoMapper* mapper = dynamic_cast<SpdEcalTB2GeoMapper*>(fGeoMapper);
    if (!mapper) return; 
    
    Int_t     nSectors = mapper->GetNSectors();
    Double_t  basketZSize = ((TGeoArb8*)basketVolume->GetShape())->GetDz()*2;
    Double_t* basketVertices = ((TGeoArb8*)basketVolume->GetShape())->GetVertices();
    
    assert(basketVertices[0] > basketVertices[2]);
    assert(basketVertices[0] == -basketVertices[2]);
    
    Double_t basketPhiInnerSize = basketVertices[0] - basketVertices[2];
    Double_t doubleAngleBasket = 360./nSectors;
    Double_t layerInnerSizeTheta = mapper->GetLayerInnerSizeTheta();
    Double_t layerInnerSizePhi = mapper->GetLayerInnerSizePhi();
    Double_t cellClearance = mapper->GetCellClearance();
    Double_t moduleClearance = mapper->GetModuleClearance();
    
    Int_t    nCellsTheta = int(basketZSize/(2*layerInnerSizeTheta + moduleClearance));
    Double_t moduleInnerSizeTheta = (basketZSize - nCellsTheta*moduleClearance)/nCellsTheta;
    Int_t    nCellsPhi = 2*int((basketPhiInnerSize/2)/(2*layerInnerSizePhi + moduleClearance));        
    
    cout << "-I- <SpdEcalTB2::FillBasketTightly> : the Z size of ECAL barrel modules is set to " 
         << (moduleInnerSizeTheta - cellClearance)/2 << " cm" << endl;
       
    vector<TGeoCombiTrans*> vectorTrans;
   
    Int_t iphi_min = -nCellsPhi/2;
    Int_t iphi_max = nCellsPhi/2;
     
    for (int iphi = iphi_min; iphi < iphi_max; ++iphi) { //stack to x+
        Double_t angle1 = iphi*(doubleAngleBasket/2)/(nCellsPhi/2);
        Double_t angle2 = (iphi+1)*(doubleAngleBasket/2)/(nCellsPhi/2);
        Double_t angleModuleCenter = (angle1 < 0) ? angle2 : angle1;
        Double_t angleModulePeriph = (angle1 < 0) ? angle1 : angle2;
        pair<TGeoCombiTrans*, TGeoVolume*> res = StackModuleInSector(basketVolume, angleModuleCenter, angleModulePeriph, moduleInnerSizeTheta, iphi - iphi_min);
        vectorTrans.push_back(res.first);
        fUniqueModules.push_back(res.second);
    }
    
    for (int iv = 0; iv < fUniqueModules.size(); ++iv) {
        BuildModule(fUniqueModules[iv]);
    }
    
    fModule = fUniqueModules[0];
    
    for (int iv = 0; iv < vectorTrans.size(); ++iv) {
        Double_t currZ = -basketZSize/2 + moduleClearance/2 + moduleInnerSizeTheta/2;
        for (int iz = 0; iz < nCellsTheta; ++iz) {
            TGeoCombiTrans* currTrans = new TGeoCombiTrans(*(vectorTrans[iv]));
            currTrans->SetDz(currZ);
            TGeoCombiTrans* currTransMirror = (TGeoCombiTrans*)currTrans->MakeClone();
            currTransMirror->ReflectX(true);
            basketVolume->AddNode(fUniqueModules[iv], (iz+1)*100 + iv + 1, currTransMirror);
            currZ += moduleClearance + moduleInnerSizeTheta;
        }
    }    

    mapper->SetBasketNModulesZ(nCellsTheta);  
    mapper->SetBasketNModulesPhi(vectorTrans.size());  
}

//________________________________________________________________________________
pair<TGeoCombiTrans*, TGeoVolume*> SpdEcalTB2::StackModuleInSector(TGeoVolume* basketVolume, 
    Double_t angleModuleCenter, Double_t angleModulePeriph, 
    Double_t moduleInnerSizeTheta, Int_t imod) 
{        
    SpdEcalTB2GeoMapper* mapper = dynamic_cast<SpdEcalTB2GeoMapper*>(fGeoMapper);
    if (!mapper) Fatal("SpdEcalTB2::StackModuleInSector", "no mapper"); 
    
    Int_t nLayers = mapper->GetNLayers();
    Double_t layer1SizeZ = mapper->GetLayer1SizeZ();
    Double_t layer2SizeZ = mapper->GetLayer2SizeZ();
    Double_t totalZSize = (layer1SizeZ + layer2SizeZ)*nLayers; 
    TString baseMedium = mapper->GetBaseMedium();
    TGeoMedium* Air = FindMedium(baseMedium, "");
    Double_t cellClearance = mapper->GetCellClearance();
    Double_t moduleClearance = mapper->GetModuleClearance();
    Bool_t trimModuleLength = mapper->IsTrimModuleLength();
    
    Double_t basketZSize = ((TGeoArb8*)basketVolume->GetShape())->GetDz()*2;
    Double_t* basketVertices = ((TGeoArb8*)basketVolume->GetShape())->GetVertices();
    assert(basketVertices[0] > basketVertices[2]);
    assert(basketVertices[0] == -basketVertices[2]);
    Double_t basketPhiInnerSize = basketVertices[0] - basketVertices[2];
    assert(basketVertices[5] > basketVertices[3]);
    Double_t basketHeight = basketVertices[5] - basketVertices[3];
    assert(basketVertices[6] > basketVertices[4]);
    Double_t basketPhiOuterSize = basketVertices[6] - basketVertices[4];
    Double_t yCoordBasketBottom = basketVertices[1];
    
    Double_t yRayStart = yCoordBasketBottom - ((basketPhiInnerSize/2)/((basketPhiOuterSize - basketPhiInnerSize)/2))*basketHeight;
    
    Int_t stackingXdirection;
    if (angleModulePeriph > 0) stackingXdirection = 1;
    else stackingXdirection = -1;
        
    TVector3 vectorAtZeroAngle = TVector3(0, 1, 0);
    TVector3 vectorUnitModuleCenter = vectorAtZeroAngle; vectorUnitModuleCenter.RotateZ(-TMath::DegToRad()*angleModuleCenter);
    TVector3 vectorUnitModuleMiddle = vectorAtZeroAngle; vectorUnitModuleMiddle.RotateZ(-TMath::DegToRad()*(angleModuleCenter + angleModulePeriph)/2);
    TVector3 vectorUnitModulePeriph = vectorAtZeroAngle; vectorUnitModulePeriph.RotateZ(-TMath::DegToRad()*angleModulePeriph);
    
    struct planeEq { //ax*x + ay*y = c
        Double_t ax, ay, c;
    };
    auto GetDistancePointLine2D = [](planeEq line, Double_t x, Double_t y) {
        return fabs(line.ax*x + line.ay*y - line.c)/sqrt(line.ax*line.ax + line.ay*line.ay);
    };
    
    //get plane/line equations for left/right edge of module: crosses (0, yRayStart)
    planeEq planeEqCenter = {vectorUnitModuleCenter.Y(), -vectorUnitModuleCenter.X(), -vectorUnitModuleCenter.X()*yRayStart};
    planeEq planeEqPeriph = {vectorUnitModulePeriph.Y(), -vectorUnitModulePeriph.X(), -vectorUnitModulePeriph.X()*yRayStart};
    planeEq planeEqMiddle = {vectorUnitModuleMiddle.Y(), -vectorUnitModuleMiddle.X(), -vectorUnitModuleMiddle.X()*yRayStart};
    
    //find intersection with y = yCoordBasketBottom and y = yCoordBasketBottom+basketHeight of lines passing through yRayStart
    Double_t xBottomPeriph = (planeEqPeriph.c - planeEqPeriph.ay*yCoordBasketBottom)/planeEqPeriph.ax;
    if (stackingXdirection >= 0) xBottomPeriph -= (moduleClearance/2)/TMath::Cos(TMath::DegToRad()*angleModulePeriph);
    else xBottomPeriph += (moduleClearance/2)/TMath::Cos(TMath::DegToRad()*angleModulePeriph);
    
    Double_t xTopCenter = (planeEqCenter.c - planeEqCenter.ay*(yCoordBasketBottom+basketHeight))/planeEqCenter.ax;
    if (stackingXdirection >= 0) xTopCenter += (moduleClearance/2)/TMath::Cos(TMath::DegToRad()*angleModuleCenter);
    else xTopCenter -= (moduleClearance/2)/TMath::Cos(TMath::DegToRad()*angleModuleCenter);
    Double_t yTopCenter = yCoordBasketBottom + basketHeight;
    
    TVector3 vectorUnitBottomPeriphToHeight = TVector3(vectorUnitModuleMiddle.Y(), -vectorUnitModuleMiddle.X(), 0);
    if (vectorUnitBottomPeriphToHeight.X()*stackingXdirection >= 0) {
        vectorUnitBottomPeriphToHeight.SetX(-vectorUnitBottomPeriphToHeight.X());
        vectorUnitBottomPeriphToHeight.SetY(-vectorUnitBottomPeriphToHeight.Y());
        vectorUnitBottomPeriphToHeight.SetZ(-vectorUnitBottomPeriphToHeight.Z());
    }
    
    //get distance from point (xBottomPeriph, yCoordBasketBottom) to planeEqMiddle
    Double_t distBottomPeriphToHeight = GetDistancePointLine2D(planeEqMiddle, xBottomPeriph, yCoordBasketBottom);
    TVector3 vectorScaledInnerPhiToCenter = vectorUnitBottomPeriphToHeight; vectorScaledInnerPhiToCenter.SetMag(distBottomPeriphToHeight*2);
    Double_t xBottomCenter = xBottomPeriph + vectorScaledInnerPhiToCenter.X();
    Double_t yBottomCenter = yCoordBasketBottom + vectorScaledInnerPhiToCenter.Y();
        
    if (fabs(angleModuleCenter) < 1e-10 && fabs(xBottomCenter - stackingXdirection*moduleClearance/2) > 1e-10) {
        Fatal("SpdEcalTB2::StackModuleInSector", "oops! for the first filled module, the bottom left x should zero! something is wrong!");
    }
    
    TVector3 vectorUnitTopCenterToHeight = TVector3(vectorUnitModuleMiddle.Y(), -vectorUnitModuleMiddle.X(), 0); //!!!
    if (vectorUnitTopCenterToHeight.X()*stackingXdirection < 0) {
        vectorUnitTopCenterToHeight.SetX(-vectorUnitTopCenterToHeight.X());
        vectorUnitTopCenterToHeight.SetY(-vectorUnitTopCenterToHeight.Y());
        vectorUnitTopCenterToHeight.SetZ(-vectorUnitTopCenterToHeight.Z());
    }
    
    Double_t distTopCenterToHeight = GetDistancePointLine2D(planeEqMiddle, xTopCenter, yTopCenter);
    TVector3 vectorScaledOuterPhiToPeriph = vectorUnitTopCenterToHeight; vectorScaledOuterPhiToPeriph.SetMag(distTopCenterToHeight*2);
    
    Double_t xMiddleInnerPhi = xBottomPeriph + vectorScaledInnerPhiToCenter.X()/2;
    Double_t yMiddleInnerPhi = yCoordBasketBottom + vectorScaledInnerPhiToCenter.Y()/2;
    Double_t xMiddleOuterPhi = xTopCenter + vectorScaledOuterPhiToPeriph.X()/2;
    Double_t yMiddleOuterPhi = yTopCenter + vectorScaledOuterPhiToPeriph.Y()/2;
    
    TVector3 vectorTrapezoidHeightToOuter = TVector3(xMiddleOuterPhi - xMiddleInnerPhi, yMiddleOuterPhi - yMiddleInnerPhi, 0);
    Double_t trapezoidHeight = vectorTrapezoidHeightToOuter.Mag();

    Double_t moduleInnerSizePhi = 2*distBottomPeriphToHeight;
    Double_t moduleOuterSizePhi = 2*distTopCenterToHeight;
    if (trimModuleLength == false) {
        double halfAngleTrapezoid = TMath::ATan((0.5*moduleOuterSizePhi - 0.5*moduleInnerSizePhi)/trapezoidHeight);
        double distToIntersect = (moduleInnerSizePhi/2)/TMath::Tan(halfAngleTrapezoid);
        moduleOuterSizePhi *= (totalZSize + distToIntersect)/(trapezoidHeight + distToIntersect);
    }
    
    //make a tgeovolume, remembering about all the conventions 
    //z ->-> from large to small side, X ~ theta, Y ~ phi, +-/--/-+/++
    
    Double_t moduleVertices[16];
    moduleVertices[0] = moduleInnerSizeTheta/2;       moduleVertices[1] = -moduleOuterSizePhi/2;
    moduleVertices[2] = -moduleInnerSizeTheta/2;      moduleVertices[3] = -moduleOuterSizePhi/2;
    moduleVertices[4] = -moduleInnerSizeTheta/2;      moduleVertices[5] = moduleOuterSizePhi/2;
    moduleVertices[6] = moduleInnerSizeTheta/2;       moduleVertices[7] = moduleOuterSizePhi/2;
    
    moduleVertices[8] = moduleInnerSizeTheta/2;       moduleVertices[9] = -moduleInnerSizePhi/2;
    moduleVertices[10] = -moduleInnerSizeTheta/2;     moduleVertices[11] = -moduleInnerSizePhi/2;
    moduleVertices[12] = -moduleInnerSizeTheta/2;     moduleVertices[13] = moduleInnerSizePhi/2;
    moduleVertices[14] = moduleInnerSizeTheta/2;      moduleVertices[15] = moduleInnerSizePhi/2;
    
    Double_t moduleDz = (trimModuleLength) ? trapezoidHeight/2 : totalZSize/2;
    if (!trimModuleLength) {
        Double_t outerPhi = moduleOuterSizePhi/2;
        Double_t innerPhi = moduleInnerSizePhi/2;
        Double_t innerPhi2 = outerPhi - (outerPhi - innerPhi)*totalZSize/trapezoidHeight;
        moduleVertices[9] = -innerPhi;
        moduleVertices[11] = -innerPhi;
        moduleVertices[13] = innerPhi;
        moduleVertices[15] = innerPhi;
    }
    TGeoVolume* moduleVolume = gGeoManager->MakeArb8(
            ("EcalBModule"+to_string(imod+1)).c_str(), 
            Air, totalZSize/2, moduleVertices);

    
    TMatrixD matrixModule = GetMatrixToEmplaceModule(planeEqCenter.ax, planeEqCenter.ay, false, moduleVolume);
    if (stackingXdirection < 0) matrixModule = GetMatrixToEmplaceModule(planeEqCenter.ax, planeEqCenter.ay, true, moduleVolume);
    
    if (angleModulePeriph > 0 && trimModuleLength) {
        cout << "-I- <SpdEcalTB2::StackModuleInSector> : module φ size is set to " << (moduleInnerSizePhi - cellClearance)/2 
        << " cm, outer φ size = " << (moduleOuterSizePhi - cellClearance)/2 
        << " cm, module length (in radial direction): " << trapezoidHeight 
        << " cm" << endl;
    }
    if (angleModulePeriph > 0 && !trimModuleLength) {
        cout << "-I- <SpdEcalTB2::StackModuleInSector> : module φ size is set to " << (moduleInnerSizePhi - cellClearance)/2 
        << " cm, outer φ size = " << (moduleOuterSizePhi - cellClearance)/2 
        << " cm, module length (in radial direction): " << totalZSize 
        << " cm" << endl;
    }
    
    moduleVolume->SetLineColor(kBlue);
    moduleVolume->SetFillColor(kBlue);
    moduleVolume->SetTransparency(50);
    
    if (!trimModuleLength) {
        vectorTrapezoidHeightToOuter.SetMag(totalZSize);
    }
    
    Double_t xHeightMiddle = xMiddleInnerPhi + vectorTrapezoidHeightToOuter.X()/2;
    Double_t yHeightMiddle = yMiddleInnerPhi + vectorTrapezoidHeightToOuter.Y()/2;
    
    TGeoRotation * rot = new TGeoRotation();
    rot->SetMatrix(matrixModule.GetMatrixArray());
    TGeoTranslation * trans = new TGeoTranslation(xHeightMiddle, yHeightMiddle, 0);
    TGeoCombiTrans * combiTrans = new TGeoCombiTrans(*trans, *rot);
    return make_pair(combiTrans, moduleVolume);
}

//________________________________________________________________________________
void SpdEcalTB2::BuildBasket() 
{    
    SpdEcalTB2GeoMapper* mapper = dynamic_cast<SpdEcalTB2GeoMapper*>(fGeoMapper);
    if (!mapper) return; 
   
    Int_t nLayers = mapper->GetNLayers();
    Double_t layer1SizeZ = mapper->GetLayer1SizeZ();
    Double_t layer2SizeZ = mapper->GetLayer2SizeZ();  
    TString baseMedium = mapper->GetBaseMedium();
    TGeoMedium* Air = FindMedium(baseMedium, "");
    Double_t lzsize = layer1SizeZ + layer2SizeZ;
    Double_t tzsize = nLayers * lzsize;
    Double_t barrelLength = mapper->GetBarrelLength();
    Double_t barrelRadius = mapper->GetBarrelOuterSize() - mapper->GetBarrelWidth();
    Double_t basketClearance = mapper->GetBasketClearance();
    Double_t sectorClearance = mapper->GetSectorClearance();
    Int_t nSectors = mapper->GetNSectors();
    Bool_t forceCellSize = mapper->IsForceCellSize();
    Double_t barrelModuleRadialSize = mapper->GetBarrelModuleRadialSize();
    
    //get basket dimensions
    Double_t basketHeight = (forceCellSize) ? tzsize : barrelModuleRadialSize;
    Double_t basketZSize = barrelLength/2 - 2*basketClearance;
    Double_t doubleAngleBasket = 360./nSectors;
    Double_t angleSmallBasketFoundation = (180.0 - doubleAngleBasket)/2;
    Double_t basketPhiInnerSize = 2*barrelRadius*TMath::Tan(TMath::DegToRad()*doubleAngleBasket/2) - sectorClearance/TMath::Sin(TMath::DegToRad()*angleSmallBasketFoundation);
    Double_t basketPhiOuterSize = (basketPhiInnerSize + sectorClearance/TMath::Sin(TMath::DegToRad()*angleSmallBasketFoundation))*(barrelRadius + basketHeight)/barrelRadius - sectorClearance/TMath::Sin(TMath::DegToRad()*angleSmallBasketFoundation);
    
    Double_t basketVertices[16];
    
    basketVertices[0] = basketPhiInnerSize/2;       basketVertices[1] = barrelRadius;
    basketVertices[2] = -basketPhiInnerSize/2;      basketVertices[3] = barrelRadius;
    basketVertices[4] = -basketPhiOuterSize/2;      basketVertices[5] = barrelRadius + basketHeight;
    basketVertices[6] = basketPhiOuterSize/2;       basketVertices[7] = barrelRadius + basketHeight;
    
    basketVertices[8] = basketPhiInnerSize/2;       basketVertices[9] = barrelRadius;
    basketVertices[10] = -basketPhiInnerSize/2;     basketVertices[11] = barrelRadius;
    basketVertices[12] = -basketPhiOuterSize/2;     basketVertices[13] = barrelRadius + basketHeight;
    basketVertices[14] = basketPhiOuterSize/2;      basketVertices[15] = barrelRadius + basketHeight;
    
    fBasket = gGeoManager->MakeArb8(mapper->AddPrefix("Basket"), Air, basketZSize/2, basketVertices);
   
    fBasket->SetFillColor(kBlue);
    fBasket->SetLineColor(kBlue);
    fBasket->SetTransparency(50);
}

//________________________________________________________________________________
void SpdEcalTB2::BuildBarrel() 
{    
    SpdEcalTB2GeoMapper* mapper = dynamic_cast<SpdEcalTB2GeoMapper*>(fGeoMapper);
    if (!mapper) return; 
     
    TString baseMedium = mapper->GetBaseMedium();
    TGeoMedium* Air = FindMedium(baseMedium, "");
    Int_t nSectors = mapper->GetNSectors();
    Double_t barrelLength = mapper->GetBarrelLength();
    
    Int_t iCurrentBasket = 1;
    for (int iphi = 0; iphi < nSectors; ++iphi) { 
        
        TGeoTranslation* transBasketPos = new TGeoTranslation(0, 0,  barrelLength/4);
        TGeoTranslation* transBasketNeg = new TGeoTranslation(0, 0, -barrelLength/4);
        TGeoRotation* rotationZY180 = new TGeoRotation();
        TGeoRotation* rotationZ = new TGeoRotation();
        rotationZY180->RotateY(180);
        rotationZY180->RotateZ(iphi*360.0/nSectors);
        rotationZ->RotateZ(iphi*360.0/nSectors);
        TGeoCombiTrans * combiTransBasketY180 = new TGeoCombiTrans(*transBasketNeg, *rotationZY180);
        TGeoCombiTrans * combiTransBasket = new TGeoCombiTrans(*transBasketPos, *rotationZ);
        fMasterVolume->AddNode(fBasket, iCurrentBasket++, combiTransBasket);
        fMasterVolume->AddNode(fBasket, iCurrentBasket++, combiTransBasketY180);
        
    }
}

//________________________________________________________________________________
TMatrixD SpdEcalTB2::GetMatrixToEmplaceModule(Double_t ax, Double_t ay, 
                                              Bool_t rotateClockwise, TGeoVolume* moduleVolume) 
{
   // 
   // return rotation matrix for given TGeoVolume (of shape TGeoArb8, trapezoid), such that: 
   // for module, rotated using this matrix, the projection of the module face, which goes along Z axis, 
   // on Z = 0 plane is parallel to the line, given by equation ax*x + ay*y = 0, where ax, ay 
   // are the arguments for this function;
   // the argument rotateClockwise indicates whether the module should be placed clockwise 
   // of the line (true) or anti-clockwise (false) it is assumed that the face which will go along Z axis 
   // in the rotated module, goes along X axis for the unrotated module.
   // It is assumed the two sides are properly centered around 0.
   //
   assert(TString(moduleVolume->GetShape()->ClassName()).CompareTo("TGeoArb8") == 0);
   
   TGeoArb8 * moduleShape = (TGeoArb8*)moduleVolume->GetShape();
   Double_t* moduleVertices = moduleShape->GetVertices();
   
   //ASSUMING THE first 4 vertices are larger side
   assert(fabs(moduleVertices[0]) >= fabs(moduleVertices[8]));
   assert(fabs(moduleVertices[1]) >= fabs(moduleVertices[9]));
   
   //ASSUMING vertex 0 in +-, 1 in --, 2 in -+, 3 in ++ quadrant
   assert(moduleVertices[0] > moduleVertices[2]);
   assert(moduleVertices[5] > moduleVertices[3]);
   assert(moduleVertices[6] > moduleVertices[4]);
   assert(moduleVertices[7] > moduleVertices[1]);
   assert(moduleVertices[0] >= moduleVertices[8]);
   
   Double_t thetaLargeEdge = moduleVertices[0]  - moduleVertices[2];
   Double_t thetaSmallEdge = moduleVertices[8]  - moduleVertices[10];
   Double_t phiLargeEdge   = moduleVertices[7]  - moduleVertices[1];
   Double_t phiSmallEdge   = moduleVertices[15] - moduleVertices[9];
   
   Double_t moduleLength = 2*moduleShape->GetDz();

   //pick some triplet of vectors that go along edges of the unrotated module
   TVector3 initialVector1 = TVector3(moduleVertices[0] - moduleVertices[8], 
                                      moduleVertices[1] - moduleVertices[9], 
                                     -moduleLength);
   
   TVector3 initialVector2 = TVector3(moduleVertices[2] - moduleVertices[0], moduleVertices[3] - moduleVertices[1], 0);
   TVector3 initialVector3 = TVector3(moduleVertices[6] - moduleVertices[0], moduleVertices[7] - moduleVertices[1], 0);
   TVector3 initialVectorMidfacePhi = TVector3(0, moduleVertices[1] - moduleVertices[9], -moduleLength);
   TVector3 initialVectorMidfacePhi2 = TVector3(initialVectorMidfacePhi);
   initialVectorMidfacePhi2.SetY(-initialVectorMidfacePhi2.Y());
   
   TVector3 initialVectorMidfaceTheta = TVector3(moduleVertices[0] - moduleVertices[8], 0, -moduleLength);
   TVector3 initialVectorMidfaceTheta2 = TVector3(initialVectorMidfaceTheta);
   initialVectorMidfaceTheta2.SetX(-initialVectorMidfaceTheta2.X());
   
   Double_t doubleAngleModulePhi = 2*TMath::ATan((0.5*phiLargeEdge - 0.5*phiSmallEdge)/moduleLength);
   Double_t doubleAngleModuleTheta = 2*TMath::ATan((0.5*thetaLargeEdge - 0.5*thetaSmallEdge)/moduleLength);
   
   //get same triplet for rotated module using the plane equation
   //first vector: bottom-to-top middle-edge at-leanwall vector of the trapezoid + y(x) component
   TVector3 finalVector1 = TVector3(-ay, ax, 0);
   finalVector1.SetMag(initialVectorMidfacePhi.Mag());
   if (rotateClockwise) {
       finalVector1.SetZ(-moduleVertices[0] + moduleVertices[8]);
   }
   else {
       finalVector1.SetZ(moduleVertices[0] - moduleVertices[8]);
   }
   
   //second vector: pure Z/theta vector
   TVector3 finalVector2;
   if (rotateClockwise) finalVector2 = TVector3(0, 0, 1);
   else finalVector2 = TVector3(0, 0, -1);
   finalVector2.SetMag(initialVector2.Mag());
   
   //second vector: phi+ vector, according to 2d plane equation
   TVector3 finalVector3 = TVector3(ax, ay, 0);
   if (rotateClockwise) finalVector3.RotateZ(doubleAngleModulePhi/2);
   else finalVector3.RotateZ(-doubleAngleModulePhi/2);
   
   //save the orientation
   if (finalVector3.X() > 0 && rotateClockwise) finalVector3 *= -1;
   finalVector3.SetMag(initialVector3.Mag());
       
   TMatrixD rotationMatrix = GetMatrixToRotateVectorTriplet(initialVector1, initialVector2, initialVector3, 
                                                            finalVector1, finalVector2, finalVector3);
   
   return rotationMatrix;    
}

//_______________________________________________________________________________
TMatrixD SpdEcalTB2::GetMatrixToRotateVectorTriplet(TVector3 initialVector1, TVector3 initialVector2, TVector3 initialVector3,
                                                    TVector3 finalVector1, TVector3 finalVector2, TVector3 finalVector3) 
{
   Double_t tolerance = 1e-10;  
   
   assert(fabs(finalVector1.Mag() - initialVector1.Mag()) < tolerance);
   assert(fabs(finalVector2.Mag() - initialVector2.Mag()) < tolerance);
   assert(fabs(finalVector3.Mag() - initialVector3.Mag()) < tolerance);
   assert(fabs(initialVector1.Angle(initialVector2) - finalVector1.Angle(finalVector2)) < tolerance);
   assert(fabs(initialVector1.Angle(initialVector3) - finalVector1.Angle(finalVector3)) < tolerance);
   assert(fabs(initialVector2.Angle(initialVector3) - finalVector2.Angle(finalVector3)) < tolerance);
   
   const Double_t finalStateMatrixElements[9] = {
       finalVector1.X(), finalVector2.X(), finalVector3.X(), 
       finalVector1.Y(), finalVector2.Y(), finalVector3.Y(), 
       finalVector1.Z(), finalVector2.Z(), finalVector3.Z()
   };
   
   TMatrixD finalStateMatrix = TMatrixD(3, 3, finalStateMatrixElements); // a[irow,icol] = elements[irow*no_cols+icol]
   
   const Double_t initialStateMatrixElements[9] = {
       initialVector1.X(), initialVector2.X(), initialVector3.X(), 
       initialVector1.Y(), initialVector2.Y(), initialVector3.Y(), 
       initialVector1.Z(), initialVector2.Z(), initialVector3.Z()
   };
   
   TMatrixD initialStateMatrix = TMatrixD(3, 3, initialStateMatrixElements); // a[irow,icol] = elements[irow*no_cols+icol]
   
   TMatrixD initialStateMatrixInverted = initialStateMatrix.Invert();
   TMatrixD rotationMatrix = finalStateMatrix*initialStateMatrixInverted;
   
   TVector3 vectorColumn1 = TVector3(rotationMatrix[0][0], rotationMatrix[1][0], rotationMatrix[2][0]);
   TVector3 vectorColumn2 = TVector3(rotationMatrix[0][1], rotationMatrix[1][1], rotationMatrix[2][1]);
   TVector3 vectorColumn3 = TVector3(rotationMatrix[0][2], rotationMatrix[1][2], rotationMatrix[2][2]);
   
   assert(fabs(vectorColumn1.Angle(vectorColumn2) - TMath::Pi()/2) < tolerance);
   assert(fabs(vectorColumn1.Angle(vectorColumn3) - TMath::Pi()/2) < tolerance);
   assert(fabs(vectorColumn2.Angle(vectorColumn3) - TMath::Pi()/2) < tolerance);
   
   assert(fabs(vectorColumn1.Mag() - 1.) < tolerance);
   assert(fabs(vectorColumn2.Mag() - 1.) < tolerance);
   assert(fabs(vectorColumn3.Mag() - 1.) < tolerance);
   
   return rotationMatrix;
}

//________________________________________________________________________________
void SpdEcalTB2::GetEmplacedModulePosition(TGeoVolume* moduleVolume, 
     Double_t ax, Double_t ay, Double_t arhs, 
     Double_t ax2, Double_t ay2, Double_t arhs2, 
     Double_t & xMiddle, Double_t & yMiddle, 
     Double_t & ax_new, Double_t & ay_new, Double_t & arhs_new, 
     Double_t & xPlane2Bottom, Double_t & yPlane2Bottom, 
     Double_t & xPlane2Top, Double_t & yPlane2Top, 
     Int_t rotateZdirection, Int_t deltaXdirection, Int_t deltaYdirection) 
{
   // 
   // Place a module in its place.
   // moduleVolume:  module to place
   // ax/ay/arhs:    the equation that describes the plane that the face of the module should lean on (ax*x + ay*y = arhs)
   // ax2/ay2/arhs2: this equation describes the plane that an edge (or a face) of the module should stand on, 
   //                while leaning on the ax/ay/arhs plane.
   // 
    
//   if (ax < 0 || ay < 0) {
//       Fatal("SpdEcalTB2::GetEmplacedModulePosition", 
//             "can't stack module: function only valid for plane with ax >=0, ay >= 0");
//   }
   
   // deltaXdirection/deltaYdirection: (+1/-1): sign of the X(Y) component of vector 
   // parallel to the module edge along the Z axis and starting at plane ax/ay/arhs
   // rotateZdirection: (+1/-1): if +1, the module is placed clockwise of plane ax/ay/arhs. 
   // Else it is placed anticlockwise.
   
   //Int_t deltaXdirection = 1;
   //Int_t deltaYdirection = -1;
   //Int_t rotateZdirection = 1;
   
   SpdEcalTB2GeoMapper* mapper = dynamic_cast<SpdEcalTB2GeoMapper*>(fGeoMapper);
   if (!mapper) Fatal("SpdEcalTB2::GetEmplacedModulePosition", "no mapper");
   
   Int_t nLayers = mapper->GetNLayers();
   Double_t layer1SizeZ = mapper->GetLayer1SizeZ();
   Double_t layer2SizeZ = mapper->GetLayer2SizeZ();
   Double_t lzsize = layer1SizeZ + layer2SizeZ;
   Double_t tzsize = nLayers * lzsize;
   
   TGeoArb8 * moduleShape = (TGeoArb8*)moduleVolume->GetShape();
   Double_t* moduleVertices = moduleShape->GetVertices();
   
   //ASSUMING THE first 4 vertices are larger side
   assert(fabs(moduleVertices[0]) >= fabs(moduleVertices[8]));
   assert(fabs(moduleVertices[1]) >= fabs(moduleVertices[9]));
   
   //ASSUMING vertex 0 in +-, 1 in --, 2 in -+, 3 in ++ quadrant
   assert(moduleVertices[0] > moduleVertices[2]);
   assert(moduleVertices[5] > moduleVertices[3]);
   assert(moduleVertices[6] > moduleVertices[4]);
   assert(moduleVertices[7] > moduleVertices[1]);
   assert(moduleVertices[0] >= moduleVertices[8]);
       
   Double_t thetaLargeEdge = moduleVertices[0]  - moduleVertices[2];
   Double_t thetaSmallEdge = moduleVertices[8]  - moduleVertices[10];
   Double_t phiLargeEdge   = moduleVertices[7]  - moduleVertices[1];
   Double_t phiSmallEdge   = moduleVertices[15] - moduleVertices[9];
   Double_t moduleLength   = 2*moduleShape->GetDz();
   
   Double_t angleTrapezoidPhi = TMath::ATan((0.5*phiLargeEdge - 0.5*phiSmallEdge)/moduleLength);
   Double_t angleTrapezoidTheta = TMath::ATan((0.5*thetaLargeEdge - 0.5*thetaSmallEdge)/moduleLength);
   
   //given the plane equation ax/ay/arhs and the bottom plane, get the module center and the new plane
   
   TVector3 vectorMidfaceCenter = TVector3(ay, -ax, 0); //from inner(small) to outer(large) side of the trapezoid
   if (vectorMidfaceCenter.Y() < 0) vectorMidfaceCenter *= -1;
   
   TVector3 vectorTrapezoidHeight = vectorMidfaceCenter; //from inner(small) to outer(large) side of the trapezoid
   if (rotateZdirection > 0) {
       vectorTrapezoidHeight.RotateZ(angleTrapezoidPhi); 
   }
   else {
       vectorTrapezoidHeight.RotateZ(-angleTrapezoidPhi);
   }
   TVector3 vectorMidfacePeriph(vectorMidfaceCenter); //from inner(small) to outer(large) side of the trapezoid
   if (rotateZdirection > 0) {
       vectorMidfacePeriph.RotateZ(2*angleTrapezoidPhi);
   }
   else {
       vectorMidfacePeriph.RotateZ(-2*angleTrapezoidPhi);
   }
   
   // !!! here +/- means in which quadrant we want to stick the module. All 4 +- combinations are solutions.
   
   Double_t deltaY = deltaYdirection*phiSmallEdge/sqrt(1 + vectorTrapezoidHeight.Y()*vectorTrapezoidHeight.Y()/(vectorTrapezoidHeight.X()*vectorTrapezoidHeight.X()));
   Double_t deltaX;
   if (deltaXdirection < 0) {
       deltaX = -phiSmallEdge/sqrt(1 + vectorTrapezoidHeight.X()*vectorTrapezoidHeight.X()/(vectorTrapezoidHeight.Y()*vectorTrapezoidHeight.Y()));
   }
   else {
       deltaX = phiSmallEdge/sqrt(1 + vectorTrapezoidHeight.X()*vectorTrapezoidHeight.X()/(vectorTrapezoidHeight.Y()*vectorTrapezoidHeight.Y()));
   }
   
   //lower point at plane1. Weird expression but oh well
   
   Double_t yPlane1 = (arhs*(-ax2) - (-ax)*(arhs2 - deltaY*ay2 - (-deltaX)*(-ax2)))/(ay*(-ax2) - (-ax)*ay2);
   Double_t xPlane1 = -(arhs*ay2 - ay*(arhs2 - deltaY*ay2 - (-deltaX)*(-ax2)))/((-ax)*ay2 - ay*(-ax2));
   
   Double_t yInnerSideMiddle = yPlane1 + deltaY/2;
   Double_t xInnerSideMiddle = xPlane1 + deltaX/2;
   
   // (0,0) -> (zInnerSideMiddle, xInnerSideMiddle) && plane2, 
   // x*zInnerSideMiddle - z*xInnerSideMiddle = 0 && plane2.ax*x + plane2.az*z = plane2.arhs
       
   yMiddle = yInnerSideMiddle + vectorTrapezoidHeight.Unit().Y()*tzsize/2;
   xMiddle = xInnerSideMiddle + vectorTrapezoidHeight.Unit().X()*tzsize/2;
   
   yPlane2Bottom = yPlane1 + deltaY;
   xPlane2Bottom = xPlane1 + deltaX;
       
   //get plane equation for peripheral side of the module -> will be used to place the next module
   
   ax_new = vectorMidfacePeriph.Y();
   ay_new = -vectorMidfacePeriph.X();
   arhs_new = ax_new*xPlane2Bottom + ay_new*yPlane2Bottom;
       
   Double_t sizePhiSide = tzsize/TMath::Cos(angleTrapezoidPhi);
   vectorMidfacePeriph.SetMag(sizePhiSide);
   
   yPlane2Top = yPlane2Bottom + vectorMidfacePeriph.Y();
   xPlane2Top = xPlane2Bottom + vectorMidfacePeriph.X();
}

//________________________________________________________________________________   
void SpdEcalTB2::FillBasketFixedCells(TGeoVolume* basketVolume, TGeoVolume* moduleVolume) 
{ 
   SpdEcalTB2GeoMapper* mapper = dynamic_cast<SpdEcalTB2GeoMapper*>(fGeoMapper);
   if (!mapper) return; 
   
   Double_t barrelInnerSize = mapper->GetBarrelOuterSize() - mapper->GetBarrelWidth();
   Double_t moduleClearance = mapper->GetModuleClearance();
   Int_t nSectors = mapper->GetNSectors();
   
   TGeoArb8* moduleShape = (TGeoArb8*)moduleVolume->GetShape();
   Double_t* moduleVertices = moduleShape->GetVertices();
   Double_t moduleInnerSizeTheta = fabs(moduleVertices[0] - moduleVertices[2]);
   Double_t moduleOuterSizeTheta = fabs(moduleVertices[8] - moduleVertices[10]);
   assert(moduleInnerSizeTheta == moduleOuterSizeTheta); //non-projective geometry
   
   Double_t basketZSize = ((TGeoArb8*)basketVolume->GetShape())->GetDz()*2;
   
   Double_t* basketVertices = ((TGeoArb8*)basketVolume->GetShape())->GetVertices();
   
   assert(basketVertices[0] > basketVertices[2]);
   
   Double_t basketPhiInnerSize = basketVertices[0] - basketVertices[2];
   
   assert(basketVertices[5] > basketVertices[3]);
   
   Double_t basketHeight = basketVertices[5] - basketVertices[3];
   
   assert(basketVertices[6] > basketVertices[4]);
   
   Double_t basketPhiOuterSize = basketVertices[6] - basketVertices[4];
   Double_t doubleAngleBasket = 360./nSectors;
   
   Double_t nmodulesZ = 1; //number of modules in a basket in Z direction
   Double_t basketFilledZSize = moduleOuterSizeTheta;
   
   while (basketFilledZSize < basketZSize) {
       nmodulesZ += 1;
       basketFilledZSize += moduleOuterSizeTheta + moduleClearance;
   }
   
   basketFilledZSize -= moduleOuterSizeTheta + moduleClearance;
   nmodulesZ -= 1;
   
   Int_t iPhiModule = 1;
   
   struct planeEqY { //plane(line) equation of the form y = ax*x + c
       Double_t ax;
       Double_t c;
   };
   
   planeEqY basketRightEdge; 
   basketRightEdge.ax = basketHeight/(basketPhiOuterSize/2 - basketPhiInnerSize/2);
   basketRightEdge.c = barrelInnerSize - (basketPhiInnerSize/2)*basketHeight/(basketPhiOuterSize/2 - basketPhiInnerSize/2);
   planeEqY basketLeftEdge;
   basketLeftEdge.ax = -basketRightEdge.ax;
   basketLeftEdge.c = basketRightEdge.c;
   
   Double_t ax = 1;
   Double_t ay = 0;
   Double_t arhs = -moduleClearance/2;
   
   Double_t ax2 = 0;
   Double_t ay2 = 1;
   Double_t arhs2 = barrelInnerSize;
   
   Bool_t moduleInBasket = true;

   struct transInfo {
        TGeoRotation* rot;
        Double_t xMiddle;
        Double_t yMiddle;
   };
   vector<transInfo> transPart1;

   while (moduleInBasket) {
   
       Double_t xMiddle, yMiddle, ax_new, ay_new, arhs_new, xPlane2Bottom, yPlane2Bottom, xPlane2Top, yPlane2Top;
       
       TMatrixD rotationMatrix = GetMatrixToEmplaceModule(ax, ay, true, moduleVolume);
       
       GetEmplacedModulePosition(moduleVolume, ax, ay, arhs, ax2, ay2, arhs2, 
                                 xMiddle, yMiddle, ax_new, ay_new, arhs_new, 
                                 xPlane2Bottom, yPlane2Bottom, xPlane2Top, yPlane2Top, 1, -1, -1);
                   
       ax = ax_new;
       ay = ay_new;
       arhs = arhs_new;
       
       //add clearance between modules and change arhs accordingly
       Double_t angleBorderPlane = TMath::ATan(-ay/ax);
       arhs += -ax*moduleClearance/TMath::Cos(angleBorderPlane);
       
       Bool_t bottomCornerInBasket = yPlane2Bottom > (basketLeftEdge.ax*xPlane2Bottom + basketLeftEdge.c);
       Bool_t topCornerInBasket = yPlane2Top > (basketLeftEdge.ax*xPlane2Top + basketLeftEdge.c);
       moduleInBasket = bottomCornerInBasket && topCornerInBasket;
       
       if (moduleInBasket) {
           TGeoRotation * rot = new TGeoRotation();
           rot->SetMatrix(rotationMatrix.GetMatrixArray());
           transPart1.push_back(transInfo({rot, xMiddle, yMiddle}));
       }
   }

   for (int i = transPart1.size() - 1; i >= 0; --i) {
       Double_t currZcenter = -basketZSize/2 + moduleClearance/2 + moduleOuterSizeTheta/2;
       for (Int_t iz = 0; iz < nmodulesZ; ++iz) {
            TGeoTranslation * trans = new TGeoTranslation(transPart1[i].xMiddle, transPart1[i].yMiddle, currZcenter);
            TGeoCombiTrans * combiTrans = new TGeoCombiTrans(*trans, *(transPart1[i].rot));
            TGeoCombiTrans* combiTransMirror = (TGeoCombiTrans*)combiTrans->MakeClone(); 
            combiTransMirror->ReflectX(true);
            basketVolume->AddNode(moduleVolume, 100*(iz+1) + iPhiModule, combiTransMirror);
            currZcenter += moduleClearance + moduleOuterSizeTheta;
       }
       iPhiModule += 1; 
   }

   ax = 1;
   ay = 0;
   arhs = moduleClearance/2;
   
   ax2 = 0;
   ay2 = 1;
   arhs2 = barrelInnerSize;
   
   moduleInBasket = true;
   while (moduleInBasket) {
   
       Double_t xMiddle, yMiddle, ax_new, ay_new, arhs_new, xPlane2Bottom, yPlane2Bottom, xPlane2Top, yPlane2Top;
       
       TMatrixD rotationMatrix = GetMatrixToEmplaceModule(ax, ay, false, moduleVolume);
       
       GetEmplacedModulePosition(moduleVolume, ax, ay, arhs, ax2, ay2, arhs2, 
                                 xMiddle, yMiddle, ax_new, ay_new, arhs_new, 
                                 xPlane2Bottom, yPlane2Bottom, xPlane2Top, yPlane2Top, -1, 1, -1);
                   
       ax = ax_new;
       ay = ay_new;
       arhs = arhs_new;
       
       //add clearance between modules and change arhs accordingly
       Double_t angleBorderPlane = TMath::ATan(-ay/ax);
       arhs += ax*moduleClearance/TMath::Cos(angleBorderPlane);
       
       Bool_t bottomCornerInBasket = yPlane2Bottom > (basketRightEdge.ax*xPlane2Bottom + basketRightEdge.c);
       Bool_t topCornerInBasket = yPlane2Top > (basketRightEdge.ax*xPlane2Top + basketRightEdge.c);
       moduleInBasket = bottomCornerInBasket && topCornerInBasket;
       
       Double_t currZcenter = -basketZSize/2 + moduleClearance/2 + moduleOuterSizeTheta/2;
       
       if (moduleInBasket) {
       
           TGeoRotation * rot = new TGeoRotation();
           rot->SetMatrix(rotationMatrix.GetMatrixArray());
               
           for (int iz = 0; iz < nmodulesZ; ++iz) {
               TGeoTranslation * trans = new TGeoTranslation(xMiddle, yMiddle, currZcenter);
               TGeoCombiTrans * combiTrans = new TGeoCombiTrans(*trans, *rot);
//               basketVolume->AddNode(moduleVolume, 100*(iz+1) + iPhiModule, combiTrans);
               TGeoCombiTrans* combiTransMirror = (TGeoCombiTrans*)combiTrans->MakeClone(); 
               combiTransMirror->ReflectX(true);
               basketVolume->AddNode(moduleVolume, 100*(iz+1) + iPhiModule, combiTransMirror);
               currZcenter += moduleClearance + moduleOuterSizeTheta;
           }
       }
       iPhiModule += 1;
   }

   Int_t nmodulesPhi = iPhiModule - 1;
   
   mapper->SetBasketNModulesZ(nmodulesZ);    
   mapper->SetBasketNModulesPhi(nmodulesPhi);    
}

//_____________________________________________________________________________________
Double_t SpdEcalTB2::LinearInterp(Double_t x1, Double_t x2, Double_t t) 
{
    assert(t >= 0 && t <= 1);
    return (t*x2 + (1-t)*x1);
}

//_____________________________________________________________________________________
void SpdEcalTB2::BuildModule(TGeoVolume* module) 
{        
    SpdEcalTB2GeoMapper* mapper = dynamic_cast<SpdEcalTB2GeoMapper*>(fGeoMapper);
    if (!mapper) return; 
    
    TString baseMedium = mapper->GetBaseMedium();
    TString absorberMedium = mapper->GetAbsorberMedium();
    TString scintMedium = mapper->GetScintMedium();
    TGeoMedium* Air = FindMedium(baseMedium, "");
    TGeoMedium* Lead = FindMedium(absorberMedium, baseMedium);
    TGeoMedium* Scint = FindMedium(scintMedium, baseMedium);
    Double_t layer1SizeZ = mapper->GetLayer1SizeZ();
    Double_t layer2SizeZ = mapper->GetLayer2SizeZ();
    Double_t cellClearance = mapper->GetCellClearance();
    
    Double_t moduleZsize = ((TGeoArb8*)module->GetShape())->GetDz()*2;
    Double_t* moduleVertices = ((TGeoArb8*)module->GetShape())->GetVertices();
   
    assert(moduleVertices[0] == -moduleVertices[2]);
    assert(moduleVertices[4] == -moduleVertices[6]);
    assert(moduleVertices[1] == -moduleVertices[7]);
    assert(moduleVertices[3] == -moduleVertices[5]);
    
    Double_t layerZSize = layer1SizeZ + layer2SizeZ;
    
    //build quarter of module into (+,-) quadrant
    Double_t cellVertices[16];
    
    cellVertices[0] = moduleVertices[0];        cellVertices[1] = moduleVertices[1];
    cellVertices[2] = cellClearance/2;          cellVertices[3] = moduleVertices[3];
    cellVertices[4] = cellClearance/2;          cellVertices[5] = -cellClearance/2;
    cellVertices[6] = moduleVertices[6];        cellVertices[7] = -cellClearance/2;
    
    cellVertices[8] = moduleVertices[8];        cellVertices[9] = moduleVertices[9];
    cellVertices[10] = cellClearance/2;         cellVertices[11] = moduleVertices[11];
    cellVertices[12] = cellClearance/2;         cellVertices[13] = -cellClearance/2;
    cellVertices[14] = moduleVertices[14];      cellVertices[15] = -cellClearance/2;

    auto Reorder = [](Double_t* vtx) {
        std::swap(vtx[0], vtx[6]);
        std::swap(vtx[2], vtx[4]);
        std::swap(vtx[1], vtx[7]);
        std::swap(vtx[3], vtx[5]);
        std::swap(vtx[8], vtx[14]);
        std::swap(vtx[10], vtx[12]);
        std::swap(vtx[9], vtx[15]);
        std::swap(vtx[11], vtx[13]);
    };

    TGeoVolume* cellVolume1 = gGeoManager->MakeArb8("EcalBCell1", Air, moduleZsize/2, cellVertices); 
    for (int i = 0; i < 8; ++i) {
        cellVertices[i*2+1] = -cellVertices[i*2+1];
    }
    
    Reorder(cellVertices);
    TGeoVolume* cellVolume2 = gGeoManager->MakeArb8("EcalBCell2", Lead, moduleZsize/2, cellVertices); 
    for (int i = 0; i < 8; ++i) {
        cellVertices[i*2] = -cellVertices[i*2];
    }
    
    Reorder(cellVertices);
    TGeoVolume* cellVolume3 = gGeoManager->MakeArb8("EcalBCell3", Lead, moduleZsize/2, cellVertices); 
    for (int i = 0; i < 8; ++i) {
        cellVertices[i*2+1] = -cellVertices[i*2+1];
    }
    
    Reorder(cellVertices);
    TGeoVolume* cellVolume4 = gGeoManager->MakeArb8("EcalBCell4", Lead, moduleZsize/2, cellVertices); 
    
    FillCell(cellVolume1);
    FillCell(cellVolume2);
    FillCell(cellVolume3);
    FillCell(cellVolume4);
    
    module->AddNode(cellVolume1, 1, 0);
    module->AddNode(cellVolume2, 1, 0);
    module->AddNode(cellVolume3, 1, 0);
    module->AddNode(cellVolume4, 1, 0);
}

//_____________________________________________________________________________________
void SpdEcalTB2::FillCell(TGeoVolume* cell) 
{
    SpdEcalTB2GeoMapper* mapper = dynamic_cast<SpdEcalTB2GeoMapper*>(fGeoMapper);
    if (!mapper) Fatal("SpdEcalTB2::BuildModule", "no mapper"); 

    Int_t    nLayers = mapper->GetNLayers();
    Double_t layer1SizeZ = mapper->GetLayer1SizeZ();
    Double_t layer2SizeZ = mapper->GetLayer2SizeZ();
    Double_t layerZSize = layer1SizeZ + layer2SizeZ;
    Double_t totalZSize = nLayers * layerZSize;

    TString baseMedium      = mapper->GetBaseMedium();
    TString absorberMedium  = mapper->GetAbsorberMedium();
    TString scintMedium     = mapper->GetScintMedium();

    TGeoMedium* Air   = FindMedium(baseMedium, "");
    TGeoMedium* Lead  = FindMedium(absorberMedium, baseMedium);
    TGeoMedium* Scint = FindMedium(scintMedium, baseMedium);

    // z ->-> from large to small side, X ~ theta, Y ~ phi
    // dz (3rd argument) - half length in Z;
    // xy[8][2] (4th argument) - vector of (x,y) coordinates of vertices:
    // first four points (xy[i][j], i<4,  j<2) are the (x,y) coordinates of the vertices sitting on the -dz plane;
    // last four points  (xy[i][j], i>=4, j<2) are the (x,y) coordinates of the vertices sitting on the +dz plane;

    Double_t moduleZend = ((TGeoArb8*)cell->GetShape())->GetDz();
    Double_t moduleZbegin = -moduleZend;
    Double_t* cellVertices = ((TGeoArb8*)cell->GetShape())->GetVertices();
    
    for (Int_t ilayer = 0; ilayer < nLayers; ++ilayer) {
        
        Double_t layerZbegin = moduleZbegin + ilayer*(layerZSize);
        Double_t layerZend = layerZbegin + layerZSize;
        Double_t currZcenter = (layer1SizeZ + layer2SizeZ)*(-float(nLayers)/2 + 0.5 + ilayer);        
        Double_t layer1Zbegin = layerZbegin;
        Double_t layer1Zend = layerZbegin + layer1SizeZ;
        Double_t layer2Zbegin = layerZbegin + layer1SizeZ;
        Double_t layer2Zend = layerZend;
        
        Double_t layer1Vertices[16];
        for (int iv = 0; iv < 8; ++iv) {
            Double_t interpT = (layer1Zbegin - moduleZbegin)/(moduleZend - moduleZbegin);
            if (ilayer == 0) interpT = 0.;
            layer1Vertices[iv] = LinearInterp(cellVertices[iv], cellVertices[iv+8], interpT);
        }
        for (int iv = 8; iv < 16; ++iv) {
            Double_t interpT = (layer1Zend - moduleZbegin)/(moduleZend - moduleZbegin);
            layer1Vertices[iv] = LinearInterp(cellVertices[iv-8], cellVertices[iv], interpT);
        }
        
        TGeoVolume* mat1layerVolume = gGeoManager->MakeArb8(fAbsorberVolName, Lead, layer1SizeZ/2, layer1Vertices); 
            
        Double_t layer2Vertices[16];
        for (int iv = 0; iv < 8; ++iv) {
            Double_t interpT = (layer2Zbegin - moduleZbegin)/(moduleZend - moduleZbegin);
            layer2Vertices[iv] = LinearInterp(cellVertices[iv], cellVertices[iv+8], interpT);
        }
        for (int iv = 8; iv < 16; ++iv) {
            Double_t interpT = (layer2Zend - moduleZbegin)/(moduleZend - moduleZbegin);
            if (ilayer == nLayers - 1) interpT = 1.;
            layer2Vertices[iv] = LinearInterp(cellVertices[iv-8], cellVertices[iv], interpT);
        }
        TGeoVolume* mat2layerVolume = gGeoManager->MakeArb8(fScintVolName, Scint, layer2SizeZ/2, layer2Vertices); 
        
        mat1layerVolume->SetFillColor(kBlue+4);
        mat1layerVolume->SetLineColor(kBlue+4);
        mat1layerVolume->SetTransparency(50);
        
        mat2layerVolume->SetFillColor(kCyan);
        mat2layerVolume->SetLineColor(kCyan);
        mat2layerVolume->SetTransparency(0); 
        
        cell->AddNode(mat1layerVolume, ilayer+1, 
                            new TGeoTranslation(0, 0, currZcenter-(layer1SizeZ+layer2SizeZ)/2 + layer1SizeZ/2));
        cell->AddNode(mat2layerVolume, ilayer+1, 
                            new TGeoTranslation(0, 0, currZcenter+(layer1SizeZ+layer2SizeZ)/2 - layer2SizeZ/2));
        
        if (fEnableWritingPointsAll)   AddSensitiveVolume(mat1layerVolume); // ATTENTION
        if (fEnableWritingPointsScint) AddSensitiveVolume(mat2layerVolume); // ATTENTION
    }
    
    cell->SetFillColor(kAzure+1);
    cell->SetLineColor(kAzure+1);
    cell->SetTransparency(50);
}

//_____________________________________________________________________________________
TGeoVolume* SpdEcalTB2::BuildModule() 
{    
    SpdEcalTB2GeoMapper* mapper = dynamic_cast<SpdEcalTB2GeoMapper*>(fGeoMapper);
    if (!mapper) Fatal("SpdEcalTB2::BuildModule", "no mapper"); 
    
    TString baseMedium     = mapper->GetBaseMedium();
    TString absorberMedium = mapper->GetAbsorberMedium();
    TString scintMedium    = mapper->GetScintMedium();
    
    TGeoMedium* Air   = FindMedium(baseMedium, "");
    TGeoMedium* Lead  = FindMedium(absorberMedium, baseMedium);
    TGeoMedium* Scint = FindMedium(scintMedium, baseMedium);
    
    Double_t layerInnerSizeTheta = mapper->GetLayerInnerSizeTheta();
    Double_t layerInnerSizePhi = mapper->GetLayerInnerSizePhi();
    Double_t layerOuterSizeTheta = mapper->GetLayerOuterSizeTheta();
    Double_t layerOuterSizePhi = mapper->GetLayerOuterSizePhi();
    Double_t cellClearance = mapper->GetCellClearance();
    
    Int_t    nLayers = mapper->GetNLayers();
    Double_t layer1SizeZ = mapper->GetLayer1SizeZ();
    Double_t layer2SizeZ = mapper->GetLayer2SizeZ();
    Double_t layerZSize = layer1SizeZ + layer2SizeZ;
    Double_t totalZSize = nLayers * layerZSize;
    
    //build quarter of module into (+,+) quadrant
    Double_t cellVertices[16];
    
    cellVertices[0] = layerOuterSizeTheta + cellClearance/2;   cellVertices[1] = cellClearance/2;
    cellVertices[2] = cellClearance/2;                         cellVertices[3] = cellClearance/2;
    cellVertices[4] = cellClearance/2;                         cellVertices[5] = layerOuterSizePhi + cellClearance/2;
    cellVertices[6] = layerOuterSizeTheta + cellClearance/2;   cellVertices[7] = layerOuterSizePhi + cellClearance/2;
    
    cellVertices[8] =  layerInnerSizeTheta + cellClearance/2;  cellVertices[9]  = cellClearance/2;
    cellVertices[10] = cellClearance/2;                        cellVertices[11] = cellClearance/2;
    cellVertices[12] = cellClearance/2;                        cellVertices[13] = layerInnerSizePhi + cellClearance/2;
    cellVertices[14] = layerInnerSizeTheta + cellClearance/2;  cellVertices[15] = layerInnerSizePhi + cellClearance/2;

    auto Reorder = [](Double_t* vtx) {
        for (int i = 0; i < 4; ++i) {
            std::swap(vtx[i*2], vtx[6-i*2]);
            std::swap(vtx[i*2+1], vtx[8-i*2-1]);
        }
        for (int i = 4; i < 8; ++i) {
            std::swap(vtx[i*2], vtx[22-i*2]);
            std::swap(vtx[i*2+1], vtx[24-i*2-1]);
        }
    };

    TGeoVolume* cellVolume1 = gGeoManager->MakeArb8("EcalBCell1", Lead, totalZSize/2, cellVertices); 
    for (int i = 0; i < 8; ++i) {
        cellVertices[i*2+1] = -cellVertices[i*2+1];
    }
    
    Reorder(cellVertices);
    TGeoVolume* cellVolume2 = gGeoManager->MakeArb8("EcalBCell2", Lead, totalZSize/2, cellVertices); 
    for (int i = 0; i < 8; ++i) {
        cellVertices[i*2] = -cellVertices[i*2];
    }
    
    Reorder(cellVertices);
    TGeoVolume* cellVolume3 = gGeoManager->MakeArb8("EcalBCell3", Lead, totalZSize/2, cellVertices); 
    for (int i = 0; i < 8; ++i) {
        cellVertices[i*2+1] = -cellVertices[i*2+1];
    }
    
    Reorder(cellVertices);
    TGeoVolume* cellVolume4 = gGeoManager->MakeArb8("EcalBCell4", Lead, totalZSize/2, cellVertices); 
    
    FillCell(cellVolume1);
    FillCell(cellVolume2);
    FillCell(cellVolume3);
    FillCell(cellVolume4);

    Double_t moduleVertices[16];
    
    moduleVertices[0] =  layerOuterSizeTheta + cellClearance/2;   moduleVertices[1] = -layerOuterSizePhi - cellClearance/2;
    moduleVertices[2] = -layerOuterSizeTheta - cellClearance/2;   moduleVertices[3] = -layerOuterSizePhi - cellClearance/2;
    moduleVertices[4] = -layerOuterSizeTheta - cellClearance/2;   moduleVertices[5] =  layerOuterSizePhi + cellClearance/2;
    moduleVertices[6] =  layerOuterSizeTheta + cellClearance/2;   moduleVertices[7] =  layerOuterSizePhi + cellClearance/2;
    
    moduleVertices[8]  =  layerInnerSizeTheta + cellClearance/2;  moduleVertices[9]  = -layerInnerSizePhi - cellClearance/2;
    moduleVertices[10] = -layerInnerSizeTheta - cellClearance/2;  moduleVertices[11] = -layerInnerSizePhi - cellClearance/2;
    moduleVertices[12] = -layerInnerSizeTheta - cellClearance/2;  moduleVertices[13] =  layerInnerSizePhi + cellClearance/2;
    moduleVertices[14] =  layerInnerSizeTheta + cellClearance/2;  moduleVertices[15] =  layerInnerSizePhi + cellClearance/2;

    TGeoVolume* moduleVolume = gGeoManager->MakeArb8("EcalBModule", Air, totalZSize/2, moduleVertices); 

    //maintain convention left-to-right top-to-bottom
    
    moduleVolume->AddNode(cellVolume1, 1, 0);
    moduleVolume->AddNode(cellVolume2, 1, 0);
    moduleVolume->AddNode(cellVolume3, 1, 0);
    moduleVolume->AddNode(cellVolume4, 1, 0);
    
    moduleVolume->SetTransparency(50);
    moduleVolume->SetFillColor(kBlue);
    moduleVolume->SetLineColor(kBlue);
   
    return moduleVolume;
}

//_____________________________________________________________________________________
SpdGeoMapper* SpdEcalTB2::GetMapper() 
{
  if (fGeoMapper) return fGeoMapper;
  
  SpdGeoFactory* factory = SpdGeoFactory::Instance();
 
  // search for mapper
  fGeoMapper = factory->SearchForMapper("SpdEcalTB2GeoMapper");
  
  // create default mapper
  if (!fGeoMapper) fGeoMapper = factory->Mapper("SpdEcalTB2GeoMapper");
  
  return fGeoMapper;
}

//_____________________________________________________________________________
Bool_t SpdEcalTB2::LoadParsFrom(SpdParSet* params) 
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
TString SpdEcalTB2::GetDataOut(Int_t n) const 
{ 
  if (n < 0 || n >= fNDataOut) return "unknown";
  return fOutDataPointObject;
}

//_____________________________________________________________________________________
TClonesArray* SpdEcalTB2::GetCollection(Int_t iColl) const 
{ 
   return (iColl == 0) ? fPointCollection : 0; 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   
//_____________________________________________________________________________
TGeoVolume* SpdEcalTB2::GetModule(Int_t i = 0) const 
{    
  if (i >= fUniqueModules.size()) {
      cout << "-W- <SpdEcalTB2::GetModule> : module with id " << i 
           << " not found, returning zero pointer" << endl;
      return 0;
  }
  
  return fUniqueModules[i]; 
}

//_____________________________________________________________________________
void SpdEcalTB2::Print(Option_t*) const
{
   SpdDetector::Print("");
   
   if (!fGeoMapper) return;
   
   TString divider('-',150);
   
   cout << "\n";

   fGeoMapper->Print("");
   
   cout << "\n";
   cout <<"\tNumber of unique modules:  " << fUniqueModules.size() << endl;
   cout << "\n";
   cout <<"\tCapacity (total):    " << GetCapacity()*1e-6 << " [m^3] " << endl;
   cout <<"\tMass (total):        " << GetMass()*1e-3 << " [kg] " << endl;
   cout <<"\tDensity (averaged):  " << GetDensity() << " [g/cm^3] " << endl;
   cout << "\n";
   
   cout << "\n";
}

