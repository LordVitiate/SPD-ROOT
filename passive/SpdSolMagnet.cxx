// $Id$
// Author: artur   2018/01/25

//_____________________________________________________________________________
//
// SpdSolMagnet
//_____________________________________________________________________________

#include "SpdSolMagnet.h"

#include <TGeoPgon.h>
#include <TGeoManager.h>

#include "SpdCommonGeoMapper.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdSolMagnet)

//_____________________________________________________________________________
SpdSolMagnet::SpdSolMagnet():
SpdPassiveModule("Magnet (sol)","SPDSOLMAGNET",kSpdSolMagnet),
fMagnetSection(0),fInnerCryo(0),fOuterCryo(0),fEndfCryo(0)
{
   SetGeoPars(SpdCommonGeoMapper::Instance()->GetSolMagnetDefGeoType());
}

//_____________________________________________________________________________
SpdSolMagnet::SpdSolMagnet(const char* Name, const char* Prefix): 
SpdPassiveModule(Name,Prefix,kSpdSolMagnet),
fMagnetSection(0),fInnerCryo(0),fOuterCryo(0),fEndfCryo(0)
{
   SetGeoPars(SpdCommonGeoMapper::Instance()->GetSolMagnetDefGeoType());
}

//_____________________________________________________________________________
SpdSolMagnet::~SpdSolMagnet() 
{
 
}

//_____________________________________________________________________________
void SpdSolMagnet::SetGeometryType(Int_t type)
{
   if (fMagnetSection) {
       cout << "-E- <SpdSolMagnet::SetGeometryType> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   if (type < 1)  {
       //cout <<"<SpdSolMagnet::GeometryType> " << geotype << endl;
       SetGeoPars(0);
       return;
   }
   else if (type == 1) SetGeoPars(1);
   else if (type == 2) SetGeoPars(2);
   else {
       cout << "-E- <SpdSolMagnet::SetGeometryType> "
            << " Unknown geometry type: " << type << endl;  
       SetGeoPars(0);
       return;     
   }
}

//_____________________________________________________________________________
 void SpdSolMagnet::SetWidth(Double_t width)
{
   if (fMagnetSection) {
       cout << "-E- <SpdSolMagnet::SetWidth> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   if (width < 1.) {
       cout << "-W- <SpdSolMagnet::SetWidth> "
            << " The distance should be more than 1 [cm] " << endl;  
       width = 1.;
   }
   
   fWidth = width;
}
 
//_____________________________________________________________________________
void SpdSolMagnet::SetBaseMaterial(TString material)
{
   if (fMagnetSection) {
       cout << "-E- <SpdSolMagnet::SetBaseMaterial> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   TGeoMedium* medium = FindMedium(material,"");
   
   if (!medium) {
       cout << "-E- <SpdSolMagnet::SetBaseMaterial> " 
            << " The material: \"" << material << "\" is not defined " << endl;
       return;     
   }
   
   fBaseMaterial = material;
}

//_____________________________________________________________________________
Double_t SpdSolMagnet::GetCapacity() const
{
   return GetMagnetCapacity() + GetCryoCapacity();
}

//_____________________________________________________________________________
Double_t SpdSolMagnet::GetMass() const
{
   return GetMagnetMass() + GetCryoMass();
}

//_____________________________________________________________________________
Double_t SpdSolMagnet::GetMagnetCapacity() const
{ 
   return (fMagnetSection) ? fNSections*fMagnetSection->Capacity() : 0; 
}
   
//_____________________________________________________________________________
Double_t SpdSolMagnet::GetMagnetMass() const
{
   if (!fMagnetSection) return 0;
   return (fNSections * fMagnetSection->Capacity() 
                      * fMagnetSection->GetMaterial()->GetDensity());
}

//_____________________________________________________________________________
Double_t SpdSolMagnet::GetCryoCapacity() const
{
   Double_t V = 0;
   
   if (fInnerCryo) V += fInnerCryo->Capacity();
   if (fOuterCryo) V += fOuterCryo->Capacity();
   if (fEndfCryo)  V += fEndfCryo->Capacity();
     
   return V;
}
 
//_____________________________________________________________________________ 
Double_t SpdSolMagnet::GetCryoMass() const
{
   Double_t M = 0;
 
   if (fInnerCryo) M += fInnerCryo->Capacity()*fInnerCryo->GetMaterial()->GetDensity();
   if (fOuterCryo) M += fOuterCryo->Capacity()*fOuterCryo->GetMaterial()->GetDensity();
   if (fEndfCryo)  M += fEndfCryo->Capacity()*fEndfCryo->GetMaterial()->GetDensity();
       
   return M;
}

//_____________________________________________________________________________
void SpdSolMagnet::UnsetMaterials(TString media)
{
   if (fMagnetSection) return;
   
   fBaseMaterial = media;
   fCryoMaterial = media;
   
   fUnsetMedia = media;
}

//_____________________________________________________________________________
void SpdSolMagnet::ResetMaterials()
{
   if (fMagnetSection) return;
   SetDefaultMaterials(fGeoType);
}

//_____________________________________________________________________________
void SpdSolMagnet::ConstructGeometry()
{
   if (fMagnetSection) {
       cout << "-E- <SpdTorMagnet::ConstructGeometry> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();
   
   if (!fMasterVolume) {  
       cout << "-E- <SpdSolMagnet::ConstructGeometry> No MASTER volume " << endl;
       return;
   }
   
   if (fGeoType < 1) {
       //cout << "-I- <SpdSolMagnet::ConstructGeometry> No geometry will be built " << endl;
       fGeoType = 0;
       return;
   }
   else if (fGeoType == 1) ConstructGeometry_1();
   else if (fGeoType == 2) ConstructGeometry_2();
   else {
       cout << "-E- <SpdSolMagnet::ConstructGeometry> "
            << " Unknown geometry type: " << fGeoType << endl;  
       fGeoType = 0;     
   }
   
   FillParsIn(GetParameters());
}

//_____________________________________________________________________________
void SpdSolMagnet::ConstructGeometry_1()
{
   if (!BuildSection1()) return;
  
   Double_t shift = -0.5*(fNSections-1)*fModuleLength;
   
   for (Int_t i(0); i<fNSections; i++) {
        fMasterVolume->AddNode(fMagnetSection,i+1,new TGeoTranslation(0,0,shift));
        shift += fModuleLength;
   }
   
   if (!BuildCryostat1()) return;
   
   fMasterVolume->AddNode(fInnerCryo,1,new TGeoTranslation(0,0,0));
   fMasterVolume->AddNode(fOuterCryo,1,new TGeoTranslation(0,0,0));
   
   fMasterVolume->AddNode(fEndfCryo,1,new TGeoTranslation(0,0, 0.5*(fTotalLength - fEndfCryoWidth)));
   fMasterVolume->AddNode(fEndfCryo,2,new TGeoTranslation(0,0,-0.5*(fTotalLength - fEndfCryoWidth)));
}

//_____________________________________________________________________________
void SpdSolMagnet::ConstructGeometry_2()
{
   if (!BuildSection2()) return;
   
   TGeoTranslation trans; 
   TGeoRotation rot;
   
   Int_t nsectors = SpdCommonGeoMapper::Instance()->GetNGeoSectors();
   
   rot.RotateZ(180./nsectors);
   
   Double_t shift = -0.5*(fNSections-1)*fModuleLength;
   
   for (Int_t i(0); i<fNSections; i++) {
        trans.SetTranslation(0,0,shift);
        fMasterVolume->AddNode(fMagnetSection,i+1,new TGeoCombiTrans(trans,rot));
        shift += fModuleLength;
   }
}

//_____________________________________________________________________________   
Bool_t SpdSolMagnet::BuildSection1()
{
   TGeoMedium* material = FindMedium(fBaseMaterial,"air");
   
   if (!material) {
       cout << "-E- <SpdSolMagnet::BuildSection1> "
            << "Undefined material: " << fBaseMaterial << endl;
       return kFALSE;
   }
  
   TString volname = FullName("SECTION",kFALSE);
  
   fMagnetSection = gGeoManager->MakeTube(volname, material,
                    fHalfSize-fWidth, fHalfSize, 0.5*fModuleLength);

   fMagnetSection->SetFillColor(fFillColor);
   fMagnetSection->SetLineColor(fLineColor);
   fMagnetSection->SetTransparency(fTransparency);
   
   return kTRUE;
}

//_____________________________________________________________________________   
Bool_t SpdSolMagnet::BuildSection2()
{
   TGeoMedium* material = FindMedium(fBaseMaterial,"air");
   
   if (!material) {
       cout << "-E- <SpdSolMagnet::BuildSection2> "
            << "Undefined material: " << fBaseMaterial << endl;
       return kFALSE;
   }
  
   Int_t nsectors = SpdCommonGeoMapper::Instance()->GetNGeoSectors();
  
   TString volname = FullName("SECTION",kFALSE);
  
   fMagnetSection = gGeoManager->MakePgon(volname,material,0,360,nsectors,2);
   
   TGeoPgon* pgon = (TGeoPgon*)fMagnetSection->GetShape();
   
   pgon->DefineSection(0,-0.5*fModuleLength, fHalfSize-fWidth, fHalfSize);
   pgon->DefineSection(1, 0.5*fModuleLength, fHalfSize-fWidth, fHalfSize);
   
   fMagnetSection->SetFillColor(fFillColor);
   fMagnetSection->SetLineColor(fLineColor);
   fMagnetSection->SetTransparency(fTransparency);
   
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdSolMagnet::BuildCryostat1()
{
   TGeoMedium* material = FindMedium(fCryoMaterial,"air");
   
   if (!material) {
       cout << "-E- <SpdSolMagnet::BuildCryostat1> "
            << "Undefined material: " << fCryoMaterial << endl;
       return kFALSE;
   }
   
   TString volname;
   
   double rmax, rmin;
  
   /* build inner cryostat */
   
   volname = "INNERCRYO";
   
   rmax = fTotalHalfSize - fTotalWidth + fInnerCryoWidth;
   rmin = fTotalHalfSize - fTotalWidth;
   
   fInnerCryo = gGeoManager->MakeTube(volname, material,
                rmin, rmax, 0.5*fTotalLength - fEndfCryoWidth);

   fInnerCryo->SetFillColor(fFillColor+2);
   fInnerCryo->SetLineColor(fLineColor+2);
   fInnerCryo->SetTransparency(fTransparency);
   
   /* build outer cryostat */
   
   volname = "OUTERCRYO";
   
   rmax = fTotalHalfSize;
   rmin = fTotalHalfSize - fOuterCryoWidth;
   
   fOuterCryo = gGeoManager->MakeTube(volname, material,
                rmin, rmax, 0.5*fTotalLength - fEndfCryoWidth);
   
   fOuterCryo->SetFillColor(fFillColor+2);
   fOuterCryo->SetLineColor(fLineColor+2);
   fOuterCryo->SetTransparency(fTransparency);
   
   /* build endface cryostat */
   
   volname = "ENDFCRYO";
   
   rmax = fTotalHalfSize;
   rmin = fTotalHalfSize - fTotalWidth;
   
   fEndfCryo = gGeoManager->MakeTube(volname, material,
               rmin, rmax, fEndfCryoWidth);
   
   fEndfCryo->SetFillColor(fFillColor+2);
   fEndfCryo->SetLineColor(fLineColor+2);
   fEndfCryo->SetTransparency(fTransparency);
   
   return kTRUE;
}

//_____________________________________________________________________________
void SpdSolMagnet::SetGeoPars(Int_t geotype)
{
   //cout <<"<SpdSolMagnet::SetGeoPars> " << geotype << endl;
    
   if (geotype < 1 || geotype > 2) {
       //cout << "-I- <SpdSolMagnet::SetGeoPars> No geometry will be built" << endl;
       
       fGeoType = 0;
    
       fTotalLength = 0; 
       fHalfSize    = 0; 
       fWidth       = 0;  
    
       fBaseMaterial = "vacuum2"; 
   
       fNSections = 0; 
       fModuleLength = 0;
       
       fFillColor = 0;
       fLineColor = 0;
       fTransparency = 0;
 
       return;
   }
   
   fGeoType = geotype;
    
   fBaseMaterial = SpdCommonGeoMapper::GetSolMagnetBaseMaterial();
         
   if (fGeoType == 1) {
     
       fCryoMaterial  = SpdCommonGeoMapper::GetSolMagnetCryoMaterial();
     
       fModuleLength  = SpdCommonGeoMapper::GetSolMagnetLen1(); 
       fHalfSize      = SpdCommonGeoMapper::GetSolMagnetSize1();
       fWidth         = SpdCommonGeoMapper::GetSolMagnetWidth1();   
       
       fNSections     = SpdCommonGeoMapper::Instance()->GetSolMagnetNSections1();     
      
       fTotalLength   = SpdCommonGeoMapper::GetSolMagnetTotalLen1();
       fTotalHalfSize = SpdCommonGeoMapper::GetSolMagnetTotalSize1();
       fTotalWidth    = SpdCommonGeoMapper::GetSolMagnetTotalWidth1();
       
       fInnerCryoWidth = SpdCommonGeoMapper::GetSolMagnetInnerCryoW1();   
       fOuterCryoWidth = SpdCommonGeoMapper::GetSolMagnetOuterCryoW1();
       fEndfCryoWidth  = SpdCommonGeoMapper::GetSolMagnetEndfCryoW1();
       
       fFillColor = kRed+3;
       fLineColor = kRed+3;
       fTransparency = 50;
       
       return;
   }
    
   if (fGeoType == 2) {
 
       fModuleLength  = SpdCommonGeoMapper::GetSolMagnetLen2(); 
       fHalfSize      = SpdCommonGeoMapper::GetSolMagnetSize2();
       fWidth         = SpdCommonGeoMapper::GetSolMagnetWidth2();   
       
       fNSections     = SpdCommonGeoMapper::Instance()->GetSolMagnetNSections2();
       
       fTotalLength   = fModuleLength*fNSections;
       fTotalHalfSize = fHalfSize;
       fTotalWidth    = fWidth;       
          
       fFillColor = kRed+3;
       fLineColor = kRed+3;
       fTransparency = 50;

       return;
   }   
}

//_____________________________________________________________________________
void SpdSolMagnet::SetDefaultMaterials(Int_t geotype)
{
   //cout <<"<SpdSolMagnet::SetDefaultMaterials> " << geotype << endl;
    
   if (geotype < 1 || geotype > 2) {
       //cout << "-I- <SpdSolMagnet::SetDefaultMaterials> No geometry will be built" << endl;
       
       fBaseMaterial = "vacuum2"; 
   
       fFillColor = 0;
       fLineColor = 0;
       fTransparency = 0;
 
       return;
   }
   
   fBaseMaterial = SpdCommonGeoMapper::GetSolMagnetBaseMaterial();
         
   if (fGeoType == 1) {
     
       fCryoMaterial  = SpdCommonGeoMapper::GetSolMagnetCryoMaterial();
     
       fFillColor = kRed+3;
       fLineColor = kRed+3;
       fTransparency = 50;
       
       return;
   }
    
   if (fGeoType == 2) {
 
       fFillColor = kRed+3;
       fLineColor = kRed+3;
       fTransparency = 50;

       return;
   }   
}

//_____________________________________________________________________________    
void SpdSolMagnet::Print(Option_t*) const
{
   cout << "<SpdSolMagnet::Print> Name/Prefix: "  << fName << "/" << fTitle << endl;
   
   TString divider('-',150);
   
   cout << "\n";
   
   cout << "\tGeometry type:      " << fGeoType   << endl;
   cout << "\tUnique module Id:   " << fModuleId << endl; 
   cout << "\tUnset media:        " << fUnsetMedia << endl;
   
   if (fGeoType < 1) { cout << endl; return; }
   
   cout << "\tBase material:      " << fBaseMaterial  << endl;
   cout << "\tNumber of sections: " << fNSections << endl;
   cout << "\tTotal length:       " << fTotalLength << " [cm] " << endl;
   cout << "\tModule length:      " << fModuleLength << " [cm] " << endl;
   cout << "\tModule size(X,Y):   " << 2*fHalfSize << " [cm] " << endl;
   cout << "\tModule width:       " << fWidth << " [cm] " << endl;
    
   cout <<"\n";
   
   if (!fMagnetSection) return;
   
   cout << "\tCapacity:           " << GetCapacity()*1e-6 << " [m^3] " << endl;
   cout << "\tMass:               " << GetMass()*1e-3 << " [kg] " << endl;
   cout << "\tDensity:            " << GetDensity() << " [g/(cm^3)] " << endl;
   
   cout <<"\n";
}
    
 //_____________________________________________________________________________
Bool_t SpdSolMagnet::FillParsIn(SpdPassiveGeoParSet* params)
{
   if (!SpdPassiveModule::FillParsIn(params)) return kFALSE;
   return kTRUE;
}

//_____________________________________________________________________________    
Bool_t SpdSolMagnet::LoadParsFrom(SpdPassiveGeoParSet* params)
{
   if (!SpdPassiveModule::LoadParsFrom(params)) return kFALSE;
   return kTRUE;
}   
    
    
    
    
