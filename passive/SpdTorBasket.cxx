// $Id$
// Author: artur   2018/02/13

//_____________________________________________________________________________
//
// SpdTorBasket
//_____________________________________________________________________________

#include "SpdTorBasket.h"

#include "SpdCommonGeoMapper.h"

#include <TGeoManager.h>
#include <TGeoBBox.h>
#include <TGeoArb8.h>
#include <TMath.h>

#include <iostream>
using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdTorBasket)

//_____________________________________________________________________________
SpdTorBasket::SpdTorBasket():
SpdPassiveModule("Basket (tor)","SPDTORBASKET",kSpdTorBasket),
fLockGeom(kFALSE),fLeafBox(0),fCasingWall(0),
fLeafBoxNNodes(0),fLeafBoxCapacity(0),fLeafBoxMass(0),
fCasingWallNNodes(0),fCasingWallCapacity(0),fCasingWallMass(0)
{
   SetGeoPars(SpdCommonGeoMapper::Instance()->GetTorBasketDefGeoType());
}

//_____________________________________________________________________________
SpdTorBasket::SpdTorBasket(const char* Name, const char* Prefix):
SpdPassiveModule(Name,Prefix,kSpdTorBasket),
fLockGeom(kFALSE),fLeafBox(0),fCasingWall(0),
fLeafBoxNNodes(0),fLeafBoxCapacity(0),fLeafBoxMass(0),
fCasingWallNNodes(0),fCasingWallCapacity(0),fCasingWallMass(0)
{
   SetGeoPars(SpdCommonGeoMapper::Instance()->GetTorBasketDefGeoType());
}

//_____________________________________________________________________________
SpdTorBasket::~SpdTorBasket() 
{
 
}

//_____________________________________________________________________________________
void SpdTorBasket::ConstructGeometry()
{
   if (fLockGeom) {
       cout << "-E- <SpdTorBasket::ConstructGeometry> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();
   
   if (!fMasterVolume) {  
       cout << "-E- <SpdTorBasket::ConstructGeometry> No MASTER volume " << endl;
       return;
   }
   
   if (fGeoType < 1) {
       //cout << "-I- <SpdTorBasket::ConstructGeometry> No geometry will be built" << endl;
       fGeoType = 0;
       return;
   }
   
   if (!BuildLeaf()) return;
   if (!BuildCasingWall()) return;
  
   BuildSection(0, 0.);
   BuildSection(1,-2*fLeafBoxHalfZ);
   BuildSection(2, 2*fLeafBoxHalfZ);
 
   fLockGeom = kTRUE;
   
   FillParsIn(GetParameters());
}

//_____________________________________________________________________________
void SpdTorBasket::BuildSection(Int_t ns, Double_t zshift)
{
   TGeoTranslation trans; 
   
   TGeoRotation rot1; 
   rot1.RotateZ(-90.);
   
   TGeoRotation rot2; 
   rot2.RotateZ(-90.);
  
   Int_t nsectors = SpdCommonGeoMapper::Instance()->GetNGeoSectors();
   
   Double_t dangle = 360./nsectors;
   Double_t dr;
   
   Double_t angle = -dangle/2.;
   rot1.RotateZ(angle);
   
   ns *= nsectors;
   
   for (int i(1); i<=nsectors; i++) {
     
        // add leaf
        angle += dangle;
        
        rot1.RotateZ(dangle); 
       
        dr = fLeafBoxAxialDistance + fLeafBoxHalfY;  // radial shift 
     
        trans.SetTranslation(dr * Cos(angle*DegToRad()),
                             dr * Sin(angle*DegToRad()),
                             zshift);
         
        fMasterVolume->AddNodeOverlap(fLeafBox,ns+i,new TGeoCombiTrans(trans,rot1));
        fLeafBoxNNodes++;
        
        // add casting wall
        trans.SetTranslation(0, 0, zshift); 
        fMasterVolume->AddNode(fCasingWall,ns+i,new TGeoCombiTrans(trans,rot2));
        fCasingWallNNodes++;
       
        rot2.RotateZ(dangle); 
   }
}

//_____________________________________________________________________________
Double_t SpdTorBasket::GetCapacity() const
{
   return fCasingWallNNodes*fCasingWallCapacity + fLeafBoxNNodes*fLeafBoxCapacity;
}
   
//_____________________________________________________________________________
Double_t SpdTorBasket::GetMass() const
{
   return fCasingWallNNodes*fCasingWallMass + fLeafBoxNNodes*fLeafBoxMass;
}

//_____________________________________________________________________________
Double_t SpdTorBasket::GetLeafDensity() const
{
   return (fLeafBoxCapacity > 1e-9) ? fLeafBoxMass/fLeafBoxCapacity : 0;
}

//_____________________________________________________________________________
Double_t SpdTorBasket::GetCasingWallDensity() const
{
   return (fCasingWallCapacity > 1e-9) ? fCasingWallMass/fCasingWallCapacity : 0;
}

//_____________________________________________________________________________________
void SpdTorBasket::UnsetMaterials(TString media)
{
   if (fLockGeom) {
       cout << "-E- <SpdTorBasket::UnsetMaterials> "
            << " Geometry is already built " << endl;  
       return;
   }  
   
   fLeafMaterial   = media;
   fCasingMaterial = media;
   
   fUnsetMedia = media;
}

//_____________________________________________________________________________
void SpdTorBasket::ResetMaterials()
{
   if (fLockGeom) return;
   SetDefaultMaterials(fGeoType);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
Bool_t SpdTorBasket::BuildLeaf() // private
{ 
   TGeoMedium* box_material = FindMedium(fLeafMaterial,"air");
   
   if (!box_material) {
       cout << "-E- <SpdTorBasket::BuildLeaf> "
            << "Undefined \"box\" material: " << fLeafMaterial << endl;
       return kFALSE;
   }
  
   TString volname = FullName("LEAFBOX",kFALSE);
  
   fLeafBox = gGeoManager->MakeBox(volname, box_material, 
                                   fLeafBoxHalfX, fLeafBoxHalfY, fLeafBoxHalfZ);
   
   fLeafBox->SetFillColor(kBlue+4);
   fLeafBox->SetLineColor(kBlue+4);
   fLeafBox->SetTransparency(50); 
   
   fLeafBoxCapacity = fLeafBox->Capacity();
   fLeafBoxMass = fLeafBoxCapacity * fLeafBox->GetMaterial()->GetDensity();
   
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdTorBasket::BuildCasingWall()
{  
   TString volname = FullName("CASINGWALL",kFALSE);
   
   fCasingWall = BuildArb(volname, fCasingMaterial, fCasingWallHalfLen,
                          fCasingWallHmin, fCasingWallHmax,
                          fCasingWallHalfLmin, fCasingWallHalfLmax);
   
   fCasingWall->SetFillColor(kBlue+4);
   fCasingWall->SetLineColor(kBlue+4);
   fCasingWall->SetTransparency(50); 
   
   fCasingWallCapacity = fCasingWall->Capacity();
   fCasingWallMass = fCasingWallCapacity * fCasingWall->GetMaterial()->GetDensity();
   
   return (fCasingWall);
}

//_____________________________________________________________________________
TGeoVolume* SpdTorBasket::BuildArb(TString volname, TString matname, Double_t HalfLen,
                                   Double_t Hmin, Double_t Hmax, 
                                   Double_t Lmin, Double_t Lmax)
{
   TGeoMedium* material = FindMedium(matname,"air");
   
   if (!material) {
       cout << "-E- <SpdTorBasket::BuildCasingWall> "
            << "Undefined material: " << fCasingMaterial << endl;
       return 0;
   }
  
   Double_t v[16];
  
   v[0]  =  Lmin; v[1] = Hmin;
   v[2]  = -Lmin; v[3] = Hmin;
   v[4]  = -Lmax; v[5] = Hmax;
   v[6]  =  Lmax; v[7] = Hmax;
  
   v[8]  =  Lmin; v[9]  = Hmin;
   v[10] = -Lmin; v[11] = Hmin;
   v[12] = -Lmax; v[13] = Hmax;
   v[14] =  Lmax; v[15] = Hmax;
  
   return gGeoManager->MakeArb8(volname,material,HalfLen,v);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//_____________________________________________________________________________
void SpdTorBasket::SetGeoPars(Int_t geotype) // private
{ 
   //cout <<"<SpdTorBasket::SetGeoPars> " << geotype << endl;
    
   if (geotype != 1) {
       //cout << "-I- <SpdTorBasket::SetGeoPars> No geometry will be built " << endl;
       fGeoType = 0;
       return;
   }
   
   if (SpdCommonGeoMapper::Instance()->GetTorMagnetDefGeoType() != 1) {
       //cout << "-I- <SpdTorBasket::SetGeoPars> No basket is defined for such a magnet" << endl;
       fGeoType = 0;
       return;
   }
    
   fGeoType = geotype;
   
   fBaseMaterial = SpdCommonGeoMapper::Instance()->GetTorBasketBaseMaterial();
   
   //fLeafMaterial = fBaseMaterial;
   //fCasingMaterial = fBaseMaterial;
   
   fLeafMaterial = SpdCommonGeoMapper::Instance()->GetTorBasketLeafMaterial();
   fCasingMaterial = SpdCommonGeoMapper::Instance()->GetTorBasketCasingMaterial();
   
   Double_t angle = 180./SpdCommonGeoMapper::Instance()->GetNGeoSectors();
   angle *= DegToRad();
   
   Double_t clearance = SpdCommonGeoMapper::Instance()->GetSectorClearance();
   
   Double_t width = SpdCommonGeoMapper::Instance()->GetTorBasketTotalLen()/3.;
   
   Double_t x, y;
  
   Double_t csize_max;
    
   x = SpdCommonGeoMapper::Instance()->GetEcalTBSize11()
     - SpdCommonGeoMapper::Instance()->GetEcalTBWidth11();
   y = SpdCommonGeoMapper::Instance()->GetEcalTBSize12()
     - SpdCommonGeoMapper::Instance()->GetEcalTBWidth12();
 
   csize_max = (x < y) ? x : y;
   
   Double_t csize_min;
   
   x = SpdCommonGeoMapper::Instance()->GetTsTBSize11();
   y = SpdCommonGeoMapper::Instance()->GetTsTBSize12();
   
   csize_min = (x > y) ? x : y;
   
   if (csize_max - csize_min < 0.1) {
       cout << "-E- <SpdTorBasket::SetGeoPars> Bad geometry parameters (1)" << endl;
       fGeoType = 0;
       return;
   }
   
   fCasingWallHmin = csize_min; 
   fCasingWallHmax = csize_max;
   
   fCasingWallHalfLen = width/2.;
   
   fCasingWallHalfLmin = fCasingWallHmin*Tan(angle)-clearance/Cos(angle);
   fCasingWallHalfLmax = fCasingWallHmax*Tan(angle)-clearance/Cos(angle);
   
  
   Double_t lsize_min = SpdCommonGeoMapper::Instance()->GetTorMagnetSize12()
                      + SpdCommonGeoMapper::Instance()->GetTorMagnetDistToAxis1();
                      
   if (lsize_min > csize_min) {
       cout << "-E- <SpdTorBasket::SetGeoPars> Bad geometry parameters (2)" << endl;
       fGeoType = 0;
       return;
   }
  
   Double_t lsize_max;
  
   x = SpdCommonGeoMapper::Instance()->GetRsTBSize11() 
     - SpdCommonGeoMapper::Instance()->GetRsTBWidth11();
      
   y = SpdCommonGeoMapper::Instance()->GetRsTBSize12() 
     - SpdCommonGeoMapper::Instance()->GetRsTBWidth12(); 
   
   lsize_max = (x > y) ? x : y;
   
   if (lsize_max - lsize_min < 10.) {
       cout << "-E- <SpdTorBasket::SetGeoPars> Bad geometry parameters (3)" << endl;
       fGeoType = 0;
       return;
   }
   
   fLeafBoxAxialDistance = lsize_min; 
   
   fLeafBoxHalfX = clearance;
   fLeafBoxHalfY = (lsize_max/Cos(angle)-fLeafBoxAxialDistance)/2.;
   fLeafBoxHalfZ = width/2.;
}

//_____________________________________________________________________________
void SpdTorBasket::SetDefaultMaterials(Int_t geotype) // private
{ 
   //cout <<"<SpdTorBasket::SetDefaultMaterials> " << geotype << endl;
    
   if (geotype != 1) return;
   
   if (SpdCommonGeoMapper::Instance()->GetTorMagnetDefGeoType() != 1) return;
    
   fBaseMaterial = SpdCommonGeoMapper::Instance()->GetTorBasketBaseMaterial();
   
   //fLeafMaterial = fBaseMaterial;
   //fCasingMaterial = fBaseMaterial;
   
   fLeafMaterial = SpdCommonGeoMapper::Instance()->GetTorBasketLeafMaterial();
   fCasingMaterial = SpdCommonGeoMapper::Instance()->GetTorBasketCasingMaterial();
}

//_____________________________________________________________________________________
void SpdTorBasket::Print(Option_t*) const
{
   cout << "<SpdTorBasket::Print>" << endl;
   
   TString divider('-',150);
   
   cout << "\n";
   
   cout << "\tGeometry type:           " << fGeoType << endl;
   cout << "\tUnique module Id:        " << fModuleId << endl; 
   cout << "\tUnset media:             " << fUnsetMedia << endl;
   
   if (fGeoType < 1) { cout << endl; return; }
   
   cout << "\tBase material:           " << fBaseMaterial << endl;
   cout << "\tLeaf axial dist. (min)   " << fLeafBoxAxialDistance << " [cm] "  << endl;
   cout << "\tLeaf axial length        " << 2*fLeafBoxHalfY << " [cm] " << endl;
   cout << "\tCasing axial dist. (min) " << fCasingWallHmin << " [cm] "  << endl;
   cout << "\tCasing width             " << fCasingWallHmax - fCasingWallHmin << " [cm] " << endl;
  
   
   cout << "\n" << divider.Data() << "\n";
   
   printf("%6s %4s %15s %14s %14s %14s %14s\n\n",
          "Name","N","Material","Dens [g/cm^3]","Volume [m^3]","Mass [kg]",
          "Length [cm]"
         );
   
   printf("%6s %4d %15s %14.6f %14.6f %14.6f %14.6f\n",
           "Leaf", fLeafBoxNNodes, fLeafMaterial.Data(),
           GetLeafDensity(), fLeafBoxCapacity*1e-6, fLeafBoxMass*1e-3, 
           2*fLeafBoxHalfZ);
   
   printf("%6s %4d %15s %14.6f %14.6f %14.6f %14.6f\n",
           "C.Wall", fCasingWallNNodes, fCasingMaterial.Data(),
           GetCasingWallDensity(), fCasingWallCapacity*1e-6, fCasingWallMass*1e-3,
           2*fCasingWallHalfLen);
   
   cout << divider.Data() << "\n";
   
   printf("%6s %35s %14.3e %14.3e \n","TOTAL:","",GetCapacity()*1e-6,GetMass()*1e-3);
    
   cout << divider.Data() << "\n";
   
   cout << "\n";
}

//_____________________________________________________________________________
Bool_t SpdTorBasket::FillParsIn(SpdPassiveGeoParSet* params)
{
   if (!SpdPassiveModule::FillParsIn(params)) return kFALSE;
   return kTRUE;
}

//_____________________________________________________________________________    
Bool_t SpdTorBasket::LoadParsFrom(SpdPassiveGeoParSet* params)
{
   if (!SpdPassiveModule::LoadParsFrom(params)) return kFALSE;
   return kTRUE;
}
