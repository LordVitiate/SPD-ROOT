// $Id$
// Author: andre/artur   2020/07/31

#include "SpdEcalB.h"

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

#include "SpdEcalB.h"
#include "SpdEcalBPoint.h"
#include "SpdEcalBGeoMapper.h"
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

ClassImp(SpdEcalB)

//_____________________________________________________________________________________
SpdEcalB::SpdEcalB(): 

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
    fOutDataPointObject = "SpdEcalBPoint"; 
}

//_____________________________________________________________________________________
SpdEcalB::SpdEcalB(const char* name, Bool_t active):

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
   fOutDataPointObject = "SpdEcalBPoint";
}

//_____________________________________________________________________________________
SpdEcalB::~SpdEcalB()
{
  if (fPointCollection) {
      fPointCollection->Delete();
      delete fPointCollection;
      fPointCollection = 0;
  }
}

//_____________________________________________________________________________________
void SpdEcalB::Initialize()
{
  cout << "\n*******************************************************************************" << endl;
  cout << "************************* SpdEcalB::Initialize ******************************" << endl;
  cout << "*******************************************************************************\n" << endl;
  
  // data collections
  fPointCollection = new TClonesArray(fOutDataPointObject);
  
  // Initialize cell and fill parameters
  SpdDetector::Initialize(); 
 
  // SpdParSet* pars = GetParameters();
  // if (pars) pars->printParams();
}

//_____________________________________________________________________________________
void SpdEcalB::Reset()
{
  fPointCollection->Delete();
  //fPointCollection->Clear();
}

//_____________________________________________________________________________________
void SpdEcalB::Register()
{
  if (!fGeoMapper) return;
  
  if (fEnableWritingPointsScint) {
      FairRootManager::Instance()->Register(fOutDataPointObject,"SpdEcalB", 
                                            fPointCollection, kTRUE);
  }
}

//_____________________________________________________________________________________
Bool_t SpdEcalB::ProcessHits(FairVolume* vol)
{
  //cout << "<SpdEcalB::ProcessHits> " << endl;
  
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

  // Create SpdEcalBPoint at exit of active volume
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
void SpdEcalB::AddHit() 
{
  static Bool_t addhit = (fOutDataPointObject == "SpdEcalBPoint") ? kTRUE : kFALSE;
  
  if (!addhit) return;
  
  TClonesArray& clref = *fPointCollection;
  Int_t size = clref.GetEntriesFast();
  
//  cout << "SpdEcalB::AddHit: Adding hit: " << fTrackID << 
//                                   fPos.X() << " " << fPos.Y() << " " << fPos.Z() << " " << 
//                                   fMom.Px() << " " << fMom.Py() << " " << fMom.Pz() << " " << 
//                                  fTime << " " << fLength << " " << fELoss << " path = " << fNodePath <<"" << endl;
 
  new(clref[size]) SpdEcalBPoint(fTrackID, 
                                   fDetID, fBasketID, fModuleZID, fModulePhiID, fCellID, fLayerID, 
                                   fMatID,
                                   TVector3(fPos.X(),fPos.Y(),fPos.Z()),
                                   TVector3(fMom.Px(),fMom.Py(),fMom.Pz()),
                                   fTime, fLength, fELoss, fNodePath);                                 
}

//_____________________________________________________________________________________
void SpdEcalB::EndOfEvent()
{
  Reset();
}

//_____________________________________________________________________________________
void SpdEcalB::FinishRun() 
{
  //SpdDetector::FinishRun();
  FillNodesTableIn(GetParameters());
}

//_____________________________________________________________________________________
void SpdEcalB::ConstructGeometry()
{   
  fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();
      
  if (!fMasterVolume) {  
      cout << "-E- <SpdIts::ConstructGeometry> No MASTER volume " << endl;
      return;
  }
  
  if (!GetMapper()) return;
   
  if (!fGeoMapper->InitGeometry()) return;

  //fGeoMapper->Print("");
   
  SpdEcalBGeoMapper* mapper = dynamic_cast<SpdEcalBGeoMapper*>(fGeoMapper);
   
  fAbsorberVolName = mapper->GetAbsorberVolName();
  fScintVolName    = mapper->GetScintVolName();

  Int_t geo_type = fGeoMapper->GetGeoType();
  
  ConstructGeometryTDR();
  
  fGeoMapper->LockGeometry(); 
  
  cout << "\n*******************************************************************************" << endl;
  cout << "********************** SpdEcalB::ConstructGeometry **************************" << endl;
  cout << "********************** GEOMETRY TYPE: " << geo_type 
       << " ***************************************" << endl;
  cout << "*******************************************************************************\n" << endl;
}

//________________________________________________________________________________
void SpdEcalB::ConstructGeometryTDR() {
    
    SpdEcalBGeoMapper* mapper = dynamic_cast<SpdEcalBGeoMapper*>(fGeoMapper);
    if (!mapper) return; 
   
    TString baseMedium     = mapper->GetBaseMedium();
    TString absorberMedium = mapper->GetAbsorberMedium();
    TString scintMedium    = mapper->GetScintMedium();
    TString BasketZPlugMedium = mapper->GetZBasketPlugMedium();

    TGeoMedium* Air   = FindMedium(baseMedium, "");
    TGeoMedium* Lead  = FindMedium(absorberMedium, baseMedium);
    TGeoMedium* Scint = FindMedium(scintMedium, baseMedium);
    TGeoMedium* BasketZPlugMaterial = FindMedium(BasketZPlugMedium, "");

    Bool_t isZBasketsGapsPlugs = mapper->IsUsingZBasketGapsPlugs();
    Int_t nLayers = mapper->GetNLayers();
    Double_t layer1SizeZ = mapper->GetAbsorberLayerThickness();
    Double_t layer2SizeZ = mapper->GetScintLayerThickness();  
    Double_t layerZSize = layer1SizeZ + layer2SizeZ;
    Double_t cellThickness = nLayers * layerZSize;
    Double_t barrelLength = mapper->GetBarrelLength();
    Double_t barrelRadius = mapper->GetBarrelRadius();
    Double_t basketPhiClearance = mapper->GetBasketPhiClearance();
    Double_t basketZClearance = mapper->GetBasketZClearance();
    Double_t sectorClearance = mapper->GetSectorClearance();
    Int_t nSectors = mapper->GetNSectors();
    Int_t nBasketsZ = mapper->GetNZBaskets();


    // assuming clearance is not only between baskets, but also at the edges of barrel in Z direction
    Double_t basketZSize = (barrelLength - basketZClearance*nBasketsZ)/nBasketsZ;

    Double_t basketAngle = TMath::DegToRad()*360./nSectors;
    //the small (< 180°) angle at the trapezoid base
    Double_t angleBasketBase = (TMath::Pi() - basketAngle)/2;
    //distance between adjacent baskets in direction parallel to the bases
    //in other words, this is subtracted from basket φ dimensions
    Double_t sectorClearanceZProj = (sectorClearance + basketPhiClearance)/TMath::Sin(angleBasketBase);

    Double_t basketPhiInnerSize = barrelRadius*TMath::Tan(basketAngle/2)*2 - sectorClearanceZProj;
    Double_t basketPhiOuterSize = (barrelRadius + cellThickness)*TMath::Tan(basketAngle/2)*2 - sectorClearanceZProj;

    Double_t basketVertices[16]; 
    basketVertices[0] = basketPhiInnerSize/2;       basketVertices[1] = barrelRadius; 
    basketVertices[2] = -basketPhiInnerSize/2;      basketVertices[3] = barrelRadius;
    basketVertices[4] = -basketPhiOuterSize/2;      basketVertices[5] = barrelRadius + cellThickness;
    basketVertices[6] = basketPhiOuterSize/2;       basketVertices[7] = barrelRadius + cellThickness;
    basketVertices[8] = basketPhiInnerSize/2;       basketVertices[9] = barrelRadius;
    basketVertices[10] = -basketPhiInnerSize/2;     basketVertices[11] = barrelRadius;
    basketVertices[12] = -basketPhiOuterSize/2;     basketVertices[13] = barrelRadius + cellThickness;
    basketVertices[14] = basketPhiOuterSize/2;      basketVertices[15] = barrelRadius + cellThickness;
    
    fBasket = gGeoManager->MakeArb8("EcalBasket", Air, basketZSize/2, basketVertices);
    fBasket->SetFillColor(kBlue);
    fBasket->SetLineColor(kBlue);
    fBasket->SetTransparency(50);


    
    Double_t cellSizeTheta = mapper->GetCellZSize();
    Double_t cellInnerSizePhi = mapper->GetCellInnerSizePhi();
    Double_t cellClearance = mapper->GetCellClearance();
    Double_t moduleClearance = mapper->GetModuleClearance();
    Double_t moduleInnerSizePhi = 2*cellInnerSizePhi + cellClearance + moduleClearance; 

    Double_t moduleAngle = 2*TMath::ATan((moduleInnerSizePhi/2)/barrelRadius);
    Int_t nModulesPhi = int(round(basketAngle/moduleAngle));
    moduleAngle = basketAngle/nModulesPhi;
    moduleInnerSizePhi = 2*barrelRadius*TMath::Tan(moduleAngle/2); 
    cellInnerSizePhi = (moduleInnerSizePhi - cellClearance - moduleClearance)/2;
    Double_t cellOuterSizePhi = cellInnerSizePhi*(cellThickness + barrelRadius)/barrelRadius;
    Double_t moduleOuterSizePhi = 2*cellOuterSizePhi + cellClearance + moduleClearance;

    Double_t moduleSizeTheta = 2*cellSizeTheta + cellClearance + moduleClearance;
    Int_t nModulesZ = int(round(basketZSize/moduleSizeTheta));
    moduleSizeTheta = basketZSize/nModulesZ;
    cellSizeTheta = (moduleSizeTheta - cellClearance - moduleClearance)/2;

    mapper->SetBasketNModulesZ(nModulesZ);  
    mapper->SetBasketNModulesPhi(nModulesPhi);  

    cout << "-I- <SpdEcalB::ConstructGeometryTDR> : cell Z size set to " << cellSizeTheta << " cm" << endl;
    cout << "-I- <SpdEcalB::ConstructGeometryTDR> : cell phi inner size set to " << cellInnerSizePhi << " cm" << endl;
    cout << "-I- <SpdEcalB::ConstructGeometryTDR> : cell phi outer size set to " << cellOuterSizePhi << " cm" << endl;
    cout << "-I- <SpdEcalB::ConstructGeometryTDR> : cell phi inner size corresponds to angle of " << TMath::RadToDeg()*2*TMath::ATan((cellInnerSizePhi/2)/barrelRadius) << " degrees" << endl;

    vector<Double_t> rotAnglesModules;
    Double_t currAngle = -basketAngle/2 + moduleAngle/2;
    for (int i = 0; i < nModulesPhi; ++i) {
        rotAnglesModules.push_back(currAngle);
        currAngle += moduleAngle;
    }
    vector<Double_t> zTransModules;
    Double_t currZ = -basketZSize/2 + moduleSizeTheta/2;
    for (int i = 0; i < nModulesZ; ++i) {
        zTransModules.push_back(currZ);
        currZ += moduleSizeTheta;
    }

    
    Double_t cellVertices[16];
    
    cellVertices[0] = cellSizeTheta + cellClearance/2;   cellVertices[1] = cellClearance/2;
    cellVertices[2] = cellClearance/2;                   cellVertices[3] = cellClearance/2;
    cellVertices[4] = cellClearance/2;                   cellVertices[5] = cellOuterSizePhi + cellClearance/2;
    cellVertices[6] = cellSizeTheta + cellClearance/2;   cellVertices[7] = cellOuterSizePhi + cellClearance/2;
    
    cellVertices[8] =  cellSizeTheta + cellClearance/2;  cellVertices[9]  = cellClearance/2;
    cellVertices[10] = cellClearance/2;                  cellVertices[11] = cellClearance/2;
    cellVertices[12] = cellClearance/2;                  cellVertices[13] = cellInnerSizePhi + cellClearance/2;
    cellVertices[14] = cellSizeTheta + cellClearance/2;  cellVertices[15] = cellInnerSizePhi + cellClearance/2;

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
    auto CopyVtx = [](Double_t* vtx) {
        Double_t* ret = new Double_t[16];
        for (int i = 0; i < 16; ++i) {
            ret[i] = vtx[i];
        }
        return ret;
    };
    Double_t* cellVertices1 = CopyVtx(cellVertices);
    TGeoVolume* cellVolume1 = gGeoManager->MakeArb8("EcalBCell1", Lead, cellThickness/2, cellVertices1);

    for (int i = 0; i < 8; ++i) {
        cellVertices[i*2+1] = -cellVertices[i*2+1];
    }

    Reorder(cellVertices);
    Double_t* cellVertices2 = CopyVtx(cellVertices);
    TGeoVolume* cellVolume2 = gGeoManager->MakeArb8("EcalBCell3", Lead, cellThickness/2, cellVertices2); 
    for (int i = 0; i < 8; ++i) {
        cellVertices[i*2] = -cellVertices[i*2];
    }
    Reorder(cellVertices);
    Double_t* cellVertices3 = CopyVtx(cellVertices);
    TGeoVolume* cellVolume3 = gGeoManager->MakeArb8("EcalBCell4", Lead, cellThickness/2, cellVertices3); 
    for (int i = 0; i < 8; ++i) {
        cellVertices[i*2+1] = -cellVertices[i*2+1];
    }
    Reorder(cellVertices);
    Double_t* cellVertices4 = CopyVtx(cellVertices);
    TGeoVolume* cellVolume4 = gGeoManager->MakeArb8("EcalBCell2", Lead, cellThickness/2, cellVertices4); 


    //The weird ordering 1-3-4-2 of the names(first argument in MakeArb8) is intentional: this way, after placing the module into basket (with all the rotations), the cell-in-module index is following the conventions
    FillCell(cellVolume1);
    FillCell(cellVolume2);
    FillCell(cellVolume3);
    FillCell(cellVolume4);

    Double_t moduleVertices[16];
    
    moduleVertices[0] =  moduleSizeTheta/2;   moduleVertices[1] = -moduleOuterSizePhi/2;
    moduleVertices[2] = -moduleSizeTheta/2;   moduleVertices[3] = -moduleOuterSizePhi/2;
    moduleVertices[4] = -moduleSizeTheta/2;   moduleVertices[5] =  moduleOuterSizePhi/2;
    moduleVertices[6] =  moduleSizeTheta/2;   moduleVertices[7] =  moduleOuterSizePhi/2;
    
    moduleVertices[8]  =  moduleSizeTheta/2;  moduleVertices[9]  = -moduleInnerSizePhi/2;
    moduleVertices[10] = -moduleSizeTheta/2;  moduleVertices[11] = -moduleInnerSizePhi/2;
    moduleVertices[12] = -moduleSizeTheta/2;  moduleVertices[13] =  moduleInnerSizePhi/2;
    moduleVertices[14] =  moduleSizeTheta/2;  moduleVertices[15] =  moduleInnerSizePhi/2;

    fModule = gGeoManager->MakeArb8("EcalBModule", Air, cellThickness/2, moduleVertices); 

    //maintain convention left-to-right top-to-bottom
    fModule->AddNode(cellVolume1, 1, 0);
    fModule->AddNode(cellVolume2, 1, 0);
    fModule->AddNode(cellVolume3, 1, 0);
    fModule->AddNode(cellVolume4, 1, 0);
    
    fModule->SetTransparency(50);
    fModule->SetFillColor(kBlue);
    fModule->SetLineColor(kBlue);


    for (int iz = 0; iz < nModulesZ; ++iz) {
        for (int iphi = 0; iphi < nModulesPhi; ++iphi) {
            TGeoRotation * rot = new TGeoRotation();
            rot->RotateX(90.);
            rot->RotateY(90.);
            rot->RotateZ(TMath::RadToDeg()*rotAnglesModules[iphi]);
            TVector3 v3(0, barrelRadius + cellThickness/2, zTransModules[iz]);
            v3.RotateZ(rotAnglesModules[iphi]);
            TGeoTranslation * trans = new TGeoTranslation(v3.X(), v3.Y(), v3.Z());
            TGeoCombiTrans * combiTrans = new TGeoCombiTrans(*trans, *rot);
            fBasket->AddNode(fModule, (iz+1)*100 + iphi + 1, combiTrans);
       } 
    }
    
    mapper->SetCalcCellZSize(cellSizeTheta);
    mapper->SetCalcCellInnerPhiSize(cellInnerSizePhi);
    mapper->SetCalcCellOuterPhiSize(cellOuterSizePhi);

    Int_t iCurrentBasket = 1;
    for (int iphi = 0; iphi < nSectors; ++iphi) { 
        for (int iz = 0; iz < nBasketsZ; ++iz) {
            TGeoTranslation* transBasket = new TGeoTranslation(0., 0.,  -barrelLength/2 + basketZClearance/2 + basketZSize/2 + (basketZSize + basketZClearance)*iz);
            TGeoRotation* rotationZ = new TGeoRotation();
            rotationZ->RotateZ(iphi*360.0/nSectors);
            TGeoCombiTrans * combiTransBasket = new TGeoCombiTrans(*transBasket, *rotationZ);
            fMasterVolume->AddNode(fBasket, iCurrentBasket++, combiTransBasket);
        }
    }

    if (isZBasketsGapsPlugs) {
        TGeoVolume* fPlugZ = gGeoManager->MakeTube("BasketZPlug", BasketZPlugMaterial, barrelRadius, barrelRadius + cellThickness, basketZClearance/2);
        Int_t iCurrentZBasketPlug = 1;
        for (int iz = 0; iz < nBasketsZ - 1; ++iz) {
            TGeoTranslation* transPlug = new TGeoTranslation(0., 0.,  -barrelLength/2 + (basketZSize + basketZClearance)*(iz+1));
            TGeoCombiTrans * combiTransPlug= new TGeoCombiTrans(*transPlug, 0);
            fMasterVolume->AddNode(fPlugZ, iCurrentZBasketPlug++, combiTransPlug);
        }
    }


}


//_____________________________________________________________________________________
Double_t SpdEcalB::LinearInterp(Double_t x1, Double_t x2, Double_t t) 
{
    assert(t >= 0 && t <= 1);
    return (t*x2 + (1-t)*x1);
}


//_____________________________________________________________________________________
void SpdEcalB::FillCell(TGeoVolume* cell) 
{
    SpdEcalBGeoMapper* mapper = dynamic_cast<SpdEcalBGeoMapper*>(fGeoMapper);
    if (!mapper) Fatal("SpdEcalB::FillCell", "no mapper"); 

    Int_t    nLayers = mapper->GetNLayers();
    Double_t layer1SizeZ = mapper->GetAbsorberLayerThickness();
    Double_t layer2SizeZ = mapper->GetScintLayerThickness();
    Double_t layerZSize = layer1SizeZ + layer2SizeZ;
    Double_t cellThickness = nLayers * layerZSize;

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
SpdGeoMapper* SpdEcalB::GetMapper() 
{
  if (fGeoMapper) return fGeoMapper;
  
  SpdGeoFactory* factory = SpdGeoFactory::Instance();
 
  // search for mapper
  fGeoMapper = factory->SearchForMapper("SpdEcalBGeoMapper");
  
  // create default mapper
  if (!fGeoMapper) fGeoMapper = factory->Mapper("SpdEcalBGeoMapper");
  
  return fGeoMapper;
}

//_____________________________________________________________________________
Bool_t SpdEcalB::LoadParsFrom(SpdParSet* params) 
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
TString SpdEcalB::GetDataOut(Int_t n) const 
{ 
  if (n < 0 || n >= fNDataOut) return "unknown";
  return fOutDataPointObject;
}

//_____________________________________________________________________________________
TClonesArray* SpdEcalB::GetCollection(Int_t iColl) const 
{ 
   return (iColl == 0) ? fPointCollection : 0; 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   

//_____________________________________________________________________________
void SpdEcalB::Print(Option_t*) const
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
   
   cout << "\n";
}

