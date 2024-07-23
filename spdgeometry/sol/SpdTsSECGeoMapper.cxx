// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdTsSECGeoMapper
//_____________________________________________________________________________

#include "SpdTsSECGeoMapper.h"
#include "SpdCommonGeoMapper.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdTsSECGeoMapper)

SpdTsSECGeoMapper* SpdTsSECGeoMapper::fInstance = 0;

SpdTsSECGeoMapper* SpdTsSECGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdTsSECGeoMapper(); }

SpdTsSECGeoMapper* SpdTsSECGeoMapper::GetMapper() 
{ return fInstance; }
    
//_____________________________________________________________________________
SpdTsSECGeoMapper::SpdTsSECGeoMapper():SpdGeoMapper("TsSEC")
{
  if (fInstance) {
      Fatal("SpdTsSECGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetTsSECDefGeoType();
}

//_____________________________________________________________________________
SpdTsSECGeoMapper::SpdTsSECGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdTsSECGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetTsSECDefGeoType();
}

//_____________________________________________________________________________
SpdTsSECGeoMapper::~SpdTsSECGeoMapper() 
{
 
}

//_____________________________________________________________________________
void SpdTsSECGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdTsSECGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
   
   const Char_t* medium;
   (precise) ? medium = "vacuum2" : medium = "vacuum";
   
   SpdParameter* par = GetParameter("TsSECBaseMaterial");

   if (!par) { fParams->Add(new SpdParameter("TsSECBaseMaterial",medium)); }
   else { *par = medium; } 
}

//_____________________________________________________________________________
Int_t SpdTsSECGeoMapper::GetNSectors() 
{
   Int_t nsec;
   SpdParameter* par = GetParameter("TsSECNSectors");
   if (!par) return 0;
   par->Value(nsec);
   return nsec;
}

//_____________________________________________________________________________
Double_t SpdTsSECGeoMapper::GetSecAngle() 
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 360./nsec: 0;
}

//_____________________________________________________________________________
Double_t SpdTsSECGeoMapper::GetSecAngleOver2() 
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 180./nsec: 0;
}

//_____________________________________________________________________________
TString SpdTsSECGeoMapper::GetVolName(Int_t level, Int_t uid) 
{ 
  TString name("");
  
  if (fGeoType == 1) {
     
    if (level == 0) {
       
        if (uid == 0) name = "endcap";
        
        return AddPrefix(name); 
    }
  }
  
  return name;
}

//_____________________________________________________________________________
Bool_t SpdTsSECGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
  return (gtype > 0 && gtype < 2); 
}

//_____________________________________________________________________________
Bool_t SpdTsSECGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdTsSECGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
  
   if (!fParams) return kFALSE;
     
   if (!CheckGeoType(gtype,"TsSECGeoType")) return kFALSE;
    
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
  
   par = GetParameter("TsSECNSectors");
   if (par) { if (reinit) *par = mapper->GetNGeoSectors(); }
   else { fParams->Add(new SpdParameter("TsSECNSectors",mapper->GetNGeoSectors())); }
   
   par = GetParameter("TsSECBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetTsSECBaseMaterial(); }
   else { fParams->Add(new SpdParameter("TsSECBaseMaterial",mapper->GetTsSECBaseMaterial())); } 
   
   //cout << "-I- <SpdTsSECGeoMapper::InitGeometry> reinit: " << reinit << endl;
     
   if (fGeoType == 1) SetParameters_1(reinit);
   
   return kTRUE;
}

//_____________________________________________________________________________
void SpdTsSECGeoMapper::SetParameters_1(Bool_t reinit) // private
{ 
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
   
   par = GetParameter("TsSECMinDist");  
   if (par) { if (reinit) *par = mapper->GetTsSECMinDist(); }
   else { fParams->Add(new SpdParameter("TsSECMinDist",mapper->GetTsSECMinDist())); } 
   
   Double_t size = mapper->GetTsSECSize();
   par = GetParameter("TsSECSize");  
   if (par) { if (reinit) *par = size; }
   else { fParams->Add(new SpdParameter("TsSECSize",size)); } 
   
   Double_t width = mapper->GetTsSECWidth();
   par = GetParameter("TsSECWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("TsSECWidth",width)); } 
   
   par = GetParameter("TsSECThickness");  
   if (par) { if (reinit) *par = mapper->GetTsSECThickness(); }
   else { fParams->Add(new SpdParameter("TsSECThickness",mapper->GetTsSECThickness())); }  
   
   /* define secondary parameters */
  
   Double_t hmin = size - width;  
   par = GetParameter("TsSECHmin");
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("TsSECHmin",hmin,1)); } 
}

//_____________________________________________________________________________
void SpdTsSECGeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdTsSECGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}




