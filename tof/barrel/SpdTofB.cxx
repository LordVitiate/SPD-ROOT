// $Id$
// Author: artur   2021/08/18

//_____________________________________________________________________________
//
// SpdTofB
//_____________________________________________________________________________

#include "SpdTofB.h"

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
#include "FairGeoMedia.h"
#include "FairGeoBuilder.h"

#include "TMath.h"

#include "SpdTofBPoint.h"
#include "SpdTofBGeoMapper.h"

#include "SpdDetectorList.h"
#include "SpdStack.h"
#include "SpdMCEventHeader.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include <iostream>

using namespace std;
using namespace TMath;

ClassImp(SpdTofB)

    //_____________________________________________________________________________________
    SpdTofB::SpdTofB() :

                         SpdDetector("TOF barrel (qsl)", kSpdTofB, kTRUE),
                         fTrackID(-1),
                         fVolumeID(-1),
                         fPos(), fMom(),
                         fPosOut(), fMomOut(),
                         fTime(-1.), fTimeOut(-1.),
                         fLength(-1.),
                         fSegmentLength(-1.),
                         fELoss(-1.),
                         fPointCollection(0),
                         fModule(0)
{
   SetParametersType("TofBParSet");

   fNDataOut = 1;
   fOutDataPointObject = "SpdTofBPoint";
}

//_____________________________________________________________________________________
SpdTofB::SpdTofB(const char *name, Bool_t active) :

                                                    SpdDetector(name, kSpdTofB, active),
                                                    fTrackID(-1),
                                                    fVolumeID(-1),
                                                    fPos(), fMom(),
                                                    fPosOut(), fMomOut(),
                                                    fTime(-1.), fTimeOut(-1.),
                                                    fLength(-1.),
                                                    fSegmentLength(-1.),
                                                    fELoss(-1.),
                                                    fPointCollection(0),
                                                    fModule(0)
{
   SetParametersType("TofBParSet");

   fNDataOut = 1;
   fOutDataPointObject = "SpdTofBPoint";
}

//_____________________________________________________________________________________
SpdTofB::~SpdTofB()
{
   if (fPointCollection)
   {
      fPointCollection->Delete();
      delete fPointCollection;
      fPointCollection = 0;
   }
}

//_____________________________________________________________________________________
void SpdTofB::Initialize()
{
   cout << "\n*******************************************************************************" << endl;
   cout << "************************* SpdTofB::Initialize *********************************" << endl;
   cout << "*******************************************************************************\n"
        << endl;

   // data collections
   fPointCollection = new TClonesArray(fOutDataPointObject);

   // Initialize module and fill parameters
   SpdDetector::Initialize();

   // SpdParSet* pars = GetParameters();
   // if (pars) pars->printParams();
}

//_____________________________________________________________________________________
void SpdTofB::Reset()
{
   fPointCollection->Delete();
   // fPointCollection->Clear();
}

//_____________________________________________________________________________________
void SpdTofB::Register()
{
   if (fGeoMapper && fGeoMapper->GetGeoType() < 1)
      return;

   FairRootManager::Instance()->Register(fOutDataPointObject, "SpdTofB", fPointCollection, kTRUE);
}

//_____________________________________________________________________________________
Bool_t SpdTofB::ProcessHits(FairVolume *vol)
{

   // cout << "<SpdTofB::ProcessHits> " << gMC->CurrentVolPath() << endl;

   // Set parameters at entrance of volume. Reset ELoss.
   if (gMC->IsTrackEntering())
   {
      fELoss = 0.;
      fTime = gMC->TrackTime() * 1.e9; // ns -> s
      fLength = gMC->TrackLength();
      gMC->TrackPosition(fPos);
      gMC->TrackMomentum(fMom);
   }

   // Sum energy loss for all steps in the active volume
   fELoss += gMC->Edep();

   // Create SpdTofBPoint at exit of active volume
   if (gMC->IsTrackExiting() ||
       gMC->IsTrackStop() ||
       gMC->IsTrackDisappeared())
   {
      if (!fSaveEmptyHits && fELoss == 0.)
      {
         return kFALSE;
      }

      fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
      fSegmentLength = gMC->TrackLength() - fLength;
      fTimeOut = gMC->TrackTime() * 1.e9; // ns -> s

      fVolumeID = kSpdTofB;
      fVolumePath = gMC->CurrentVolPath();
      gMC->TrackPosition(fPosOut);
      gMC->TrackMomentum(fMomOut);

      // cout << "path = " << fVolumePath << endl;

      AddHit();

      // cout << "<SpdTofB::AddHit> " << fPointCollection->GetEntriesFast() << endl;

      SpdStack *stack = (SpdStack *)gMC->GetStack();
      stack->AddPoint(kSpdTofB);
   }

   return kTRUE;
}

//_____________________________________________________________________________________
void SpdTofB::AddHit()
{
   static Bool_t addhit = (fOutDataPointObject == "SpdTofBPoint") ? kTRUE : kFALSE;

   if (!addhit)
      return;

   TClonesArray &clref = *fPointCollection;
   Int_t size = clref.GetEntriesFast();

   SpdTofBPoint *point = new (clref[size]) SpdTofBPoint(fTrackID, fVolumeID,
                                                        TVector3(fPos.X(), fPos.Y(), fPos.Z()),
                                                        TVector3(fMom.Px(), fMom.Py(), fMom.Pz()),
                                                        TVector3(fPosOut.X(), fPosOut.Y(), fPosOut.Z()),
                                                        TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
                                                        fTime, fTimeOut,
                                                        fLength, fSegmentLength, fELoss);
   point->SetNodePath(fVolumePath);
}

//_____________________________________________________________________________________
void SpdTofB::EndOfEvent()
{
   Reset();
}

//_____________________________________________________________________________________
void SpdTofB::FinishRun()
{
   // SpdDetector::FinishRun();
   FillNodesTableIn(GetParameters());
}

//_____________________________________________________________________________________
void SpdTofB::ConstructGeometry()
{
   // Create detector geometry

   fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();

   if (!fMasterVolume)
   {
      cout << "-E- <SpdTofB::ConstructGeometry> No MASTER volume " << endl;
      return;
   }

   if (!GetMapper())
      return;

   if (!fGeoMapper->InitGeometry())
      return;

   // fGeoMapper->Print("");

   Int_t geo_type = fGeoMapper->GetGeoType();

   if (geo_type == 1)
      ConstructDetector();
   else if (geo_type == 2)
      ConstructDetector_MRPC_Hybrid();

   fGeoMapper->LockGeometry();

   cout << "\n*******************************************************************************" << endl;
   cout << "********************** SpdTofB::ConstructGeometry *****************************" << endl;
   cout << "********************** GEOMETRY TYPE: " << geo_type
        << " ***************************************" << endl;
   cout << "*******************************************************************************\n"
        << endl;
}

//_____________________________________________________________________________________
void SpdTofB::ConstructDetector()
{
   SpdParameter *par;

   SpdTofBGeoMapper *mapper = dynamic_cast<SpdTofBGeoMapper *>(fGeoMapper);
   if (!mapper)
      return;

   TString mat = mapper->GetBaseMaterial();
   TGeoMedium *material = FindMedium(mat, "air");

   if (!material)
   {
      cout << "-E- <SpdTofB::BuildModule> Unknown material: " << mat << endl;
      return;
   }

   Double_t mlength = mapper->GetModuleLength();
   Double_t msize = mapper->GetModuleSize();
   Double_t mwidth = mapper->GetModuleWidth();

   fModule = gGeoManager->MakeTube(mapper->AddPrefix("module"), material, msize - mwidth, msize, 0.5 * mlength);

   if (!fModule)
   {
      cout << "-E- <SpdTofB::ConstructGeometry> Module cannot be created " << endl;
      return;
   }

   fModule->SetFillColor(kYellow);
   fModule->SetLineColor(kYellow);
   fModule->SetTransparency(30);

   AddSensitiveVolume(fModule); // ATTENTION FIXME ATTENTION

   fMasterVolume->AddNode(fModule, 1);
}

//_____________________________________________________________________________________
void SpdTofB::ConstructDetector_MRPC_Hybrid()
{
   SpdTofBGeoMapper *mapper = dynamic_cast<SpdTofBGeoMapper *>(fGeoMapper);
   if (!mapper)
      return;

   Double_t mlength = mapper->GetTofBLengthModuleMRPCProtvino();
   Double_t msize = mapper->GetTofBSizeModuleMRPCProtvino();
   Double_t mwidth = mapper->GetTofBWidthModuleMRPCProtvino();

   fModule = BuildMRPCModule(mwidth, mlength, msize);

   if (!fModule)
   {
      cout << "-E- <SpdTofB::ConstructDetector_MRPC_Hybrid> Module cannot be created " << endl;
      return;
   }

   Double_t RmaxTof = mapper->GetModuleSize();
   // Double_t RwidthTof = mapper->GetModuleWidth();
   // Double_t RminTof = RmaxTof - RwidthTof ;
   Double_t LengthTof = mapper->GetModuleLength();

   Int_t Nrow =4;
   Double_t Rmean = RmaxTof - 6;

   Double_t step_phi = 45.;
   Double_t phi_second_layer = 45. / 2.;

   Double_t shift_between_row_z = 3.625;
   Double_t shift_between_row_y = 1.0 + msize;
   Double_t shift_between_layers_y = 1.0 + 1.5 * msize + 2.5;

   Int_t max_phi = 360;

   Double_t phi, r, x_dec, y_dec, z_dec;


   Int_t inode = 0;
   Int_t item = 1;   
   for (auto irow = -Nrow; irow <= Nrow; irow++)
   {
     inode = item * 100;
     for (auto iphi = 0; iphi < max_phi; iphi += step_phi)
     {
       TGeoRotation rot1, rot2;
       TGeoTranslation trans1, trans2;
       // --- first layer ----
       r = Rmean - (shift_between_row_y * abs(irow % 2));
       phi = (90 + iphi) * (TMath::Pi() / 180.);
       x_dec = r * cos(phi);
       y_dec = r * sin(phi);

       rot1.RotateX(90);
       rot1.RotateZ(iphi);

        inode++;
       trans1.SetTranslation(x_dec, y_dec, (mwidth - shift_between_row_z) * irow);
       fMasterVolume->AddNode(fModule, inode, new TGeoCombiTrans(trans1, rot1));

       // --- second layer ----
       r = Rmean - shift_between_layers_y - (shift_between_row_y * abs(irow % 2));
       phi = (90 + iphi  + step_phi / 2.) * (TMath::Pi() / 180.);
       x_dec = r * cos(phi);
       y_dec = r * sin(phi);

       rot2.RotateX(90);
       rot2.RotateZ(iphi + phi_second_layer);
       trans2.SetTranslation(x_dec, y_dec, (mwidth - shift_between_row_z) * irow);
       fMasterVolume->AddNode(fModule, inode, new TGeoCombiTrans(trans2, rot2));
       
     }
      item++;

   }
}

//________________________________________________________________________________
TGeoVolume *SpdTofB::BuildMRPCModule(Double_t mwidth, Double_t mlength, Double_t msize)
{
   SpdTofBGeoMapper *mapper = dynamic_cast<SpdTofBGeoMapper *>(fGeoMapper);
   if (!mapper)
      return 0;

   TString mat = mapper->GetBaseMaterial();
   TGeoMedium *material = FindMedium(mat, "air");

   FairGeoLoader *geoLoad = FairGeoLoader::Instance();
   FairGeoInterface *geoFace = geoLoad->getGeoInterface();
   FairGeoBuilder *geobuild = geoLoad->getGeoBuilder();
   FairGeoMedia *geoMedia = geoFace->getMedia();

   FairGeoMedium *mRPCglass = geoMedia->getMedium("PMTglass");
   if (!mRPCglass)
      Fatal("Main", "FairMedium RPCglass not found");
   geobuild->createMedium(mRPCglass);
   TGeoMedium *pMedRPCglass = gGeoManager->GetMedium("PMTglass");
   if (!pMedRPCglass)
      Fatal("Main", "Medium RPCglass not found");

   if (!material)
   {
      cout << "-E- <SpdTofB::BuildMRPCModule> Unknown material: " << mat << endl;
      return 0;
   }

   double thickness = 11 * 0.033; // 11 glass plates with 0.33 mm thick
   TGeoVolume *vol = new TGeoVolumeAssembly(mapper->AddPrefix("module"));

   TString name_vol_air = "air" + mapper->AddPrefix("module");
   TGeoVolume *vol_air = gGeoManager->MakeBox(name_vol_air, material,
                                              mlength * 0.5, mwidth * 0.5, (msize - thickness) * 0.5);

   TString name_vol_glass = "glass" + mapper->AddPrefix("module");
   TGeoVolume *vol_glass = gGeoManager->MakeBox(name_vol_glass, pMedRPCglass,
                                                mlength * 0.5, mwidth * 0.5, thickness * 0.5);

   vol->AddNode(vol_air, 1, new TGeoTranslation(0, 0, (msize - thickness) / 2.));
   vol->AddNode(vol_glass, 2, new TGeoTranslation(0, 0, thickness / 2.));

   vol_air->SetFillColor(kGreen);
   vol_air->SetLineColor(kGreen);
   vol_air->SetTransparency(30);

   vol_glass->SetFillColor(kYellow);
   vol_glass->SetLineColor(kYellow);
   vol_glass->SetTransparency(30);

   AddSensitiveVolume(vol_air);
   AddSensitiveVolume(vol_glass);

   return vol;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
SpdGeoMapper *SpdTofB::GetMapper()
{
   if (fGeoMapper)
      return fGeoMapper;

   SpdGeoFactory *factory = SpdGeoFactory::Instance();

   // search for mapper
   fGeoMapper = factory->SearchForMapper("SpdTofBGeoMapper");

   if (!fGeoMapper)
      fGeoMapper = factory->Mapper("SpdTofBGeoMapper");

   return fGeoMapper;
}

//_____________________________________________________________________________
Bool_t SpdTofB::LoadParsFrom(SpdParSet *params)
{
   if (!params)
      return kFALSE;

   if (!SpdDetector::LoadParsFrom(params))
      return kFALSE;

   TString mapper;
   params->GetParameter("Mapper", mapper);
   fGeoMapper = SpdGeoFactory::Instance()->Mapper(mapper);
   if (fGeoMapper)
      fGeoMapper->LoadParametersFrom(params);

   fOutDataPointObject = "unknown";

   if (fNDataOut < 1)
      return kTRUE;

   params->GetParameter("Detector/NOutData_1", fOutDataPointObject);

   return kTRUE;
}

//_____________________________________________________________________________________
TString SpdTofB::GetDataOut(Int_t n) const
{
   if (n < 0 || n >= fNDataOut)
      return "unknown";
   return fOutDataPointObject;
}

//_____________________________________________________________________________________
TClonesArray *SpdTofB::GetCollection(Int_t iColl) const
{
   return (iColl == 0) ? fPointCollection : 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Double_t SpdTofB::GetCapacity() const
{
   return (fModule) ? fModule->GetShape()->Capacity() : 0;
}

//_____________________________________________________________________________________
Double_t SpdTofB::GetMass() const
{
   return (fModule) ? GetCapacity() * GetDensity() : 0;
}

//_____________________________________________________________________________________
Double_t SpdTofB::GetDensity() const
{
   return (fModule) ? fModule->GetMaterial()->GetDensity() : 0;
}

//_____________________________________________________________________________________
TString SpdTofB::GetMaterialName() const // private
{
   return (fModule) ? fModule->GetMaterial()->GetName() : "";
}

//_____________________________________________________________________________
void SpdTofB::Print(Option_t *) const
{
   SpdDetector::Print("");

   if (!fGeoMapper)
      return;

   TString divider('-', 150);

   cout << "\n";

   fGeoMapper->Print("");

   cout << "\n";
   cout << "\tCapacity (total):    " << GetCapacity() * 1e-6 << " [m^3] " << endl;
   cout << "\tMass (total):        " << GetMass() * 1e-3 << " [kg] " << endl;
   cout << "\tDensity (averaged):  " << GetDensity() << " [g/cm^3] " << endl;
   cout << "\n";

   cout << "\n"
        << divider.Data() << "\n";

   printf("%6s %4s %15s %14s %14s %14s \n\n",
          "Type", "N", "Material", "Dens [g/cm^3]", "Volume [m^3]", "Mass [kg]");

   if (!fModule)
   {
      cout << "\n";
      return;
   }

   printf("%6d %4d %15s %14.6f %14.6f %14.6f \n",
          1, 1, GetMaterialName().Data(),
          GetDensity(), GetCapacity() * 1e-6, GetMass() * 1e-3);

   cout << divider.Data() << "\n";

   printf("%6s %35s %14.3e %14.3e \n", "TOTAL:", "", GetCapacity() * 1e-6, GetMass() * 1e-3);

   cout << divider.Data() << "\n";

   cout << "\n";
}
