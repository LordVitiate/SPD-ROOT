// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdEcalSECGeoMapper
//_____________________________________________________________________________

#include "SpdEcalSECGeoMapper.h"
#include "SpdCommonGeoMapper.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdEcalSECGeoMapper)

SpdEcalSECGeoMapper* SpdEcalSECGeoMapper::fInstance = 0;

SpdEcalSECGeoMapper* SpdEcalSECGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdEcalSECGeoMapper(); }

SpdEcalSECGeoMapper* SpdEcalSECGeoMapper::GetMapper() 
{ return fInstance; } 
    
//_____________________________________________________________________________
SpdEcalSECGeoMapper::SpdEcalSECGeoMapper():SpdGeoMapper("EcalSEC")
{
  if (fInstance) {
      Fatal("SpdEcalSECGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetEcalSECDefGeoType();
}

//_____________________________________________________________________________
SpdEcalSECGeoMapper::SpdEcalSECGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdEcalSECGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetEcalSECDefGeoType();
}

//_____________________________________________________________________________
SpdEcalSECGeoMapper::~SpdEcalSECGeoMapper() 
{
 
}

//_____________________________________________________________________________
void SpdEcalSECGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdEcalSECGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
   
   const Char_t* medium;
   (precise) ? medium = "vacuum2" : medium = "vacuum";
   
   SpdParameter* par = GetParameter("EcalSECBaseMaterial");

   if (!par) { fParams->Add(new SpdParameter("EcalSECBaseMaterial",medium)); }
   else { *par = medium; } 
}

//_____________________________________________________________________________
Int_t SpdEcalSECGeoMapper::GetNSectors() 
{
   Int_t nsec;
   SpdParameter* par = GetParameter("EcalSECNSectors");
   if (!par) return 0;
   par->Value(nsec);
   return nsec;
}

//_____________________________________________________________________________
Double_t SpdEcalSECGeoMapper::GetSecAngle() 
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 360./nsec: 0;
}

//_____________________________________________________________________________
Double_t SpdEcalSECGeoMapper::GetSecAngleOver2() 
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 180./nsec: 0;
}

//_____________________________________________________________________________
TString SpdEcalSECGeoMapper::GetVolName(Int_t level, Int_t uid) 
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
Bool_t SpdEcalSECGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
  return (gtype > 0 && gtype < 2); 
}

//_____________________________________________________________________________
Bool_t SpdEcalSECGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdEcalSECGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
  
   if (!fParams) return kFALSE;
  
   if (!CheckGeoType(gtype,"EcalSECGeoType")) return kFALSE;
    
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
  
   par = GetParameter("EcalSECNSectors");
   if (par) { if (reinit) *par = mapper->GetNGeoSectors(); }
   else { fParams->Add(new SpdParameter("EcalSECNSectors",mapper->GetNGeoSectors())); }
   
   par = GetParameter("EcalSECBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetEcalSECBaseMaterial(); }
   else { fParams->Add(new SpdParameter("EcalSECBaseMaterial",mapper->GetEcalSECBaseMaterial())); } 
   
   //cout << "-I- <SpdEcalSECGeoMapper::InitGeometry> reinit: " << reinit << endl;
     
   if (fGeoType == 1) SetParameters_1(reinit);
   
   return kTRUE;
}

//_____________________________________________________________________________
void SpdEcalSECGeoMapper::SetParameters_1(Bool_t reinit) // private
{ 
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
   
   par = GetParameter("EcalSECMinDist");  
   if (par) { if (reinit) *par = mapper->GetEcalSECMinDist(); }
   else { fParams->Add(new SpdParameter("EcalSECMinDist",mapper->GetEcalSECMinDist())); } 
   
   Double_t size = mapper->GetEcalSECSize();
   par = GetParameter("EcalSECSize");  
   if (par) { if (reinit) *par = size; }
   else { fParams->Add(new SpdParameter("EcalSECSize",size)); } 
   
   Double_t width = mapper->GetEcalSECWidth();
   par = GetParameter("EcalSECWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("EcalSECWidth",width)); } 
   
   par = GetParameter("EcalSECThickness");  
   if (par) { if (reinit) *par = mapper->GetEcalSECThickness(); }
   else { fParams->Add(new SpdParameter("EcalSECThickness",mapper->GetEcalSECThickness())); }  
   
   /* define secondary parameters */
  
   Double_t hmin = size - width;  
   par = GetParameter("EcalSECHmin");
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("EcalSECHmin",hmin,1)); } 
}

//_____________________________________________________________________________
void SpdEcalSECGeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdEcalSECGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}




