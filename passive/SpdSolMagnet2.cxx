// $Id$
// Author: artur   2018/01/25

//_____________________________________________________________________________
//
// SpdSolMagnet2
//_____________________________________________________________________________

#include "SpdSolMagnet2.h"

#include <TGeoPgon.h>
#include <TGeoManager.h>

#include "SpdCommonGeoMapper.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdSolMagnet2)

//_____________________________________________________________________________
SpdSolMagnet2::SpdSolMagnet2():
SpdPassiveModule("Magnet (sol)","SPDSOLMAGNET2",kSpdSolMagnet),
fMagnetSection(0),fInnerCryo(0),fOuterCryo(0),fEndfCryo(0)
{
   SetGeoPars(SpdCommonGeoMapper::Instance()->GetSolMagnetTwoDefGeoType());
}

//_____________________________________________________________________________
SpdSolMagnet2::SpdSolMagnet2(const char* Name, const char* Prefix): 
SpdPassiveModule(Name,Prefix,kSpdSolMagnet),
fMagnetSection(0),fInnerCryo(0),fOuterCryo(0),fEndfCryo(0)
{
   SetGeoPars(SpdCommonGeoMapper::Instance()->GetSolMagnetTwoDefGeoType());
}

//_____________________________________________________________________________
SpdSolMagnet2::~SpdSolMagnet2() 
{

}

//_____________________________________________________________________________
void SpdSolMagnet2::SetGeometryType(Int_t type)
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
   else if (type == 2) SetGeoPars(1);
   else {
       cout << "-E- <SpdSolMagnet2::SetGeometryType> "
            << " Unknown geometry type: " << type << endl;  
       SetGeoPars(0);
       return;     
   }
}

//_____________________________________________________________________________
 void SpdSolMagnet2::SetWidth(Double_t width)
{
   if (fMagnetSection) {
       cout << "-E- <SpdSolMagnet2::SetWidth> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   if (width < 1.) {
       cout << "-W- <SpdSolMagnet2::SetWidth> "
            << " The distance should be more than 1 [cm] " << endl;  
       width = 1.;
   }
   
   fWidth = width;
}
 
//_____________________________________________________________________________
void SpdSolMagnet2::SetBaseMaterial(TString material)
{
   if (fMagnetSection) {
       cout << "-E- <SpdSolMagnet2::SetBaseMaterial> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   TGeoMedium* medium = FindMedium(material,"");
   
   if (!medium) {
       cout << "-E- <SpdSolMagnet2::SetBaseMaterial> " 
            << " The material: \"" << material << "\" is not defined " << endl;
       return;     
   }
   
   fBaseMaterial = material;
}

//_____________________________________________________________________________
Double_t SpdSolMagnet2::GetCapacity() const
{
   return GetMagnetCapacity() + GetCryoCapacity();
}

//_____________________________________________________________________________
Double_t SpdSolMagnet2::GetMass() const
{
   return GetMagnetMass() + GetCryoMass();
}

//_____________________________________________________________________________
Double_t SpdSolMagnet2::GetMagnetCapacity() const
{ 
   return (fMagnetSection) ? fNSections*fMagnetSection->Capacity() : 0; 
}
   
//_____________________________________________________________________________
Double_t SpdSolMagnet2::GetMagnetMass() const
{
   if (!fMagnetSection) return 0;
   return (fNSections * fMagnetSection->Capacity() 
                      * fMagnetSection->GetMaterial()->GetDensity());
}

//_____________________________________________________________________________
Double_t SpdSolMagnet2::GetCryoCapacity() const
{
   Double_t V = 0;
   
   if (fInnerCryo) V += fInnerCryo->Capacity();
   if (fOuterCryo) V += fOuterCryo->Capacity();
   if (fEndfCryo)  V += fEndfCryo->Capacity();
     
   return V;
}
 
//_____________________________________________________________________________ 
Double_t SpdSolMagnet2::GetCryoMass() const
{
   Double_t M = 0;
 
   if (fInnerCryo) M += fInnerCryo->Capacity()*fInnerCryo->GetMaterial()->GetDensity();
   if (fOuterCryo) M += fOuterCryo->Capacity()*fOuterCryo->GetMaterial()->GetDensity();
   if (fEndfCryo)  M += fEndfCryo->Capacity()*fEndfCryo->GetMaterial()->GetDensity();
       
   return M;
}

//_____________________________________________________________________________
void SpdSolMagnet2::UnsetMaterials(TString media)
{
   if (fMagnetSection) return;
   
   fBaseMaterial = media;
   fCryoMaterial = media;
   
   fUnsetMedia = media;
}

//_____________________________________________________________________________
void SpdSolMagnet2::ResetMaterials()
{
   if (fMagnetSection) return;
   SetDefaultMaterials(fGeoType);
}

//_____________________________________________________________________________
void SpdSolMagnet2::ConstructGeometry()
{
   if (fMagnetSection) {
       cout << "-E- <SpdTorMagnet2::ConstructGeometry> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();
   
   if (!fMasterVolume) {  
       cout << "-E- <SpdSolMagnet2::ConstructGeometry> No MASTER volume " << endl;
       return;
   }
   
   if (fGeoType < 1) {
       //cout << "-I- <SpdSolMagnet2::ConstructGeometry> No geometry will be built " << endl;
       fGeoType = 0;
       return;
   }
   else if (fGeoType == 1) ConstructGeometry_1();
   else if (fGeoType == 2) ConstructGeometry_1();
   else {
       cout << "-E- <SpdSolMagnet2::ConstructGeometry> "
            << " Unknown geometry type: " << fGeoType << endl;  
       fGeoType = 0;     
   }
   
   FillParsIn(GetParameters());
}

//_____________________________________________________________________________
void SpdSolMagnet2::ConstructGeometry_1()
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
Bool_t SpdSolMagnet2::BuildSection1()
{
   TGeoMedium* material = FindMedium(fBaseMaterial,"air");
   
   if (!material) {
       cout << "-E- <SpdSolMagnet2::BuildSection1> "
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
Bool_t SpdSolMagnet2::BuildCryostat1()
{
   TGeoMedium* material = FindMedium(fCryoMaterial,"air");
   
   if (!material) {
       cout << "-E- <SpdSolMagnet2::BuildCryostat1> "
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
void SpdSolMagnet2::SetGeoPars(Int_t geotype)
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
    
   fBaseMaterial = SpdCommonGeoMapper::GetSolMagnetTwoBaseMaterial();
         
   if (fGeoType == 1) {
     
       fCryoMaterial  = SpdCommonGeoMapper::GetSolMagnetTwoCryoMaterial();
     
       fModuleLength  = SpdCommonGeoMapper::GetSolMagnetTwoLen(); 
       fHalfSize      = SpdCommonGeoMapper::GetSolMagnetTwoSize();
       fWidth         = SpdCommonGeoMapper::GetSolMagnetTwoWidth();   
       
       fNSections     = SpdCommonGeoMapper::Instance()->GetSolMagnetTwoNSections();     
      
       fTotalLength   = SpdCommonGeoMapper::GetSolMagnetTwoTotalLen();
       fTotalHalfSize = SpdCommonGeoMapper::GetSolMagnetTwoTotalSize();
       fTotalWidth    = SpdCommonGeoMapper::GetSolMagnetTwoTotalWidth();
       
       fInnerCryoWidth = SpdCommonGeoMapper::GetSolMagnetTwoInnerCryoW();   
       fOuterCryoWidth = SpdCommonGeoMapper::GetSolMagnetTwoOuterCryoW();
       fEndfCryoWidth  = SpdCommonGeoMapper::GetSolMagnetTwoEndfCryoW();
       
       fFillColor = kRed+3;
       fLineColor = kRed+3;
       fTransparency = 50;
       
       return;
   }
    
   if (fGeoType == 2) {
 
       fModuleLength  = SpdCommonGeoMapper::GetSolMagnetTwoLen(); 
       fHalfSize      = SpdCommonGeoMapper::GetSolMagnetTwoSize();
       fWidth         = SpdCommonGeoMapper::GetSolMagnetTwoWidth();   
       
       fNSections     = SpdCommonGeoMapper::Instance()->GetSolMagnetTwoNSections();
       
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
void SpdSolMagnet2::SetDefaultMaterials(Int_t geotype)
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
   
   fBaseMaterial = SpdCommonGeoMapper::GetSolMagnetTwoBaseMaterial();
         
   if (fGeoType == 1) {
     
       fCryoMaterial  = SpdCommonGeoMapper::GetSolMagnetTwoCryoMaterial();
     
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
void SpdSolMagnet2::Print(Option_t*) const
{
   cout << "<SpdSolMagnet2::Print> Name/Prefix: "  << fName << "/" << fTitle << endl;
   
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
Bool_t SpdSolMagnet2::FillParsIn(SpdPassiveGeoParSet* params)
{
   if (!SpdPassiveModule::FillParsIn(params)) return kFALSE;
   return kTRUE;
}

//_____________________________________________________________________________    
Bool_t SpdSolMagnet2::LoadParsFrom(SpdPassiveGeoParSet* params)
{
   if (!SpdPassiveModule::LoadParsFrom(params)) return kFALSE;
   return kTRUE;
}   
       
