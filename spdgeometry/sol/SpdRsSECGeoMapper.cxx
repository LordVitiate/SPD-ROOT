// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdRsSECGeoMapper
//_____________________________________________________________________________

#include "SpdRsSECGeoMapper.h"
#include "SpdCommonGeoMapper.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdRsSECGeoMapper)

SpdRsSECGeoMapper* SpdRsSECGeoMapper::fInstance = 0;

SpdRsSECGeoMapper* SpdRsSECGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdRsSECGeoMapper(); }

SpdRsSECGeoMapper* SpdRsSECGeoMapper::GetMapper() 
{ return fInstance; } 
    
//_____________________________________________________________________________
SpdRsSECGeoMapper::SpdRsSECGeoMapper():SpdGeoMapper("RsSEC")
{
  if (fInstance) {
      Fatal("SpdRsSECGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetRsSECDefGeoType();
}

//_____________________________________________________________________________
SpdRsSECGeoMapper::SpdRsSECGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdRsSECGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetRsSECDefGeoType();
}

//_____________________________________________________________________________
SpdRsSECGeoMapper::~SpdRsSECGeoMapper() 
{
 
}

//_____________________________________________________________________________
void SpdRsSECGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdRsSECGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
   
   const Char_t* medium;
   (precise) ? medium = "vacuum2" : medium = "vacuum";
   
   SpdParameter* par = GetParameter("RsSECBaseMaterial");

   if (!par) { fParams->Add(new SpdParameter("RsSECBaseMaterial",medium)); }
   else { *par = medium; } 
}

//_____________________________________________________________________________
Int_t SpdRsSECGeoMapper::GetNSectors() 
{
   Int_t nsec;
   SpdParameter* par = GetParameter("RsSECNSectors");
   if (!par) return 0;
   par->Value(nsec);
   return nsec;
}

//_____________________________________________________________________________
Double_t SpdRsSECGeoMapper::GetSecAngle() 
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 360./nsec: 0;
}

//_____________________________________________________________________________
Double_t SpdRsSECGeoMapper::GetSecAngleOver2() 
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 180./nsec: 0;
}

//_____________________________________________________________________________
TString SpdRsSECGeoMapper::GetVolName(Int_t level, Int_t uid) 
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
Bool_t SpdRsSECGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
  return (gtype > 0 && gtype < 2); 
}

//_____________________________________________________________________________
Bool_t SpdRsSECGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdRsSECGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
  
   if (!fParams) return kFALSE;
     
   if (!CheckGeoType(gtype,"RsSECGeoType")) return kFALSE;
    
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
  
   par = GetParameter("RsSECNSectors");
   if (par) { if (reinit) *par = mapper->GetNGeoSectors(); }
   else { fParams->Add(new SpdParameter("RsSECNSectors",mapper->GetNGeoSectors())); }
   
   par = GetParameter("RsSECBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetRsSECBaseMaterial(); }
   else { fParams->Add(new SpdParameter("RsSECBaseMaterial",mapper->GetRsSECBaseMaterial())); } 
   
   //cout << "-I- <SpdRsSECGeoMapper::InitGeometry> reinit: " << reinit << endl;
     
   if (fGeoType == 1) SetParameters_1(reinit);
   
   return kTRUE;
}

//_____________________________________________________________________________
void SpdRsSECGeoMapper::SetParameters_1(Bool_t reinit) // private
{ 
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
   
   par = GetParameter("RsSECMinDist");  
   if (par) { if (reinit) *par = mapper->GetRsSECMinDist(); }
   else { fParams->Add(new SpdParameter("RsSECMinDist",mapper->GetRsSECMinDist())); } 
   
   Double_t size = mapper->GetRsSECSize();
   par = GetParameter("RsSECSize");  
   if (par) { if (reinit) *par = size; }
   else { fParams->Add(new SpdParameter("RsSECSize",size)); } 
   
   Double_t width = mapper->GetRsSECWidth();
   par = GetParameter("RsSECWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("RsSECWidth",width)); } 
   
   par = GetParameter("RsSECThickness");  
   if (par) { if (reinit) *par = mapper->GetRsSECThickness(); }
   else { fParams->Add(new SpdParameter("RsSECThickness",mapper->GetRsSECThickness())); }  
   
   /* define secondary parameters */
  
   Double_t hmin = size - width;  
   par = GetParameter("RsSECHmin");
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("RsSECHmin",hmin,1)); } 
}

//_____________________________________________________________________________
void SpdRsSECGeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdRsSECGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}




