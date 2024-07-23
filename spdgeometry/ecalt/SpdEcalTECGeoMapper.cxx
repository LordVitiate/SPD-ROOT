// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdEcalTECGeoMapper
//_____________________________________________________________________________

#include "SpdEcalTECGeoMapper.h"
#include "SpdCommonGeoMapper.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdEcalTECGeoMapper)

SpdEcalTECGeoMapper* SpdEcalTECGeoMapper::fInstance = 0;

SpdEcalTECGeoMapper* SpdEcalTECGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdEcalTECGeoMapper(); }

SpdEcalTECGeoMapper* SpdEcalTECGeoMapper::GetMapper() 
{ return fInstance; }
    
//_____________________________________________________________________________
SpdEcalTECGeoMapper::SpdEcalTECGeoMapper():SpdGeoMapper("EcalTEC")
{
  if (fInstance) {
      Fatal("SpdEcalTECGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetEcalTECDefGeoType();
}

//_____________________________________________________________________________
SpdEcalTECGeoMapper::SpdEcalTECGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdEcalTECGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetEcalTECDefGeoType();
}

//_____________________________________________________________________________
SpdEcalTECGeoMapper::~SpdEcalTECGeoMapper() 
{
 
}

//_____________________________________________________________________________
void SpdEcalTECGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdEcalTECGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
   
   const Char_t* medium;
   (precise) ? medium = "vacuum2" : medium = "vacuum";
   
   SpdParameter* par = GetParameter("EcalTECBaseMaterial");

   if (!par) { fParams->Add(new SpdParameter("EcalTECBaseMaterial",medium)); }
   else { *par = medium; } 
}

//_____________________________________________________________________________
Int_t SpdEcalTECGeoMapper::GetNSectors() 
{
   Int_t nsec;
   SpdParameter* par = GetParameter("EcalTECNSectors");
   if (!par) return 0;
   par->Value(nsec);
   return nsec;
}

//_____________________________________________________________________________
Double_t SpdEcalTECGeoMapper::GetSecAngle() 
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 360./nsec: 0;
}

//_____________________________________________________________________________
Double_t SpdEcalTECGeoMapper::GetSecAngleOver2() 
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 180./nsec: 0;
}

//_____________________________________________________________________________
TString SpdEcalTECGeoMapper::GetVolName(Int_t level, Int_t uid) 
{ 
  TString name("");
  
  if (fGeoType == 1) {
     
    if (level == 0) {
       
        if (uid == 0) name = "endcap";
        
        return AddPrefix(name); 
    }
  }
  
  if (fGeoType == 2) {
     
    if (level == 0) {
      
        if (uid == 0) name = "endcap";  
        
        return AddPrefix(name); 
    }
  }
  
  return name;
}

//_____________________________________________________________________________
Bool_t SpdEcalTECGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
  return (gtype > 0 && gtype < 3); 
}

//_____________________________________________________________________________
Bool_t SpdEcalTECGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdEcalTECGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
  
   if (!fParams) return kFALSE;
     
   if (!CheckGeoType(gtype,"EcalTECGeoType")) return kFALSE;
   
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
   
   par = GetParameter("EcalTECNSectors");
   if (par) { if (reinit) *par = mapper->GetNGeoSectors(); }
   else { fParams->Add(new SpdParameter("EcalTECNSectors",mapper->GetNGeoSectors())); }
   
   par = GetParameter("EcalTECBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetEcalTECBaseMaterial(); }
   else { fParams->Add(new SpdParameter("EcalTECBaseMaterial",mapper->GetEcalTECBaseMaterial())); } 
   
   //cout << "-I- <SpdEcalTECGeoMapper::InitGeometry> reinit: " << reinit << endl;
     
        if (fGeoType == 1) SetParameters_1(reinit);
   else if (fGeoType == 2) SetParameters_2(reinit);
    
   return kTRUE;
}

//_____________________________________________________________________________
void SpdEcalTECGeoMapper::SetParameters_1(Bool_t reinit) // private
{ 
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
   
   par = GetParameter("EcalTECMinDist1");  
   if (par) { if (reinit) *par = mapper->GetEcalTECMinDist1(); }
   else { fParams->Add(new SpdParameter("EcalTECMinDist1",mapper->GetEcalTECMinDist1())); } 
   
   Double_t size = mapper->GetEcalTECSize();
   par = GetParameter("EcalTECSize");  
   if (par) { if (reinit) *par = size; }
   else { fParams->Add(new SpdParameter("EcalTECSize",size)); } 
   
   Double_t width = mapper->GetEcalTECWidth();
   par = GetParameter("EcalTECWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("EcalTECWidth",width)); } 
   
   par = GetParameter("EcalTECThickness");  
   if (par) { if (reinit) *par = mapper->GetEcalTECThickness(); }
   else { fParams->Add(new SpdParameter("EcalTECThickness",mapper->GetEcalTECThickness())); }  
   
   /* define secondary parameters */
  
   Double_t hmin = size - width;  
   par = GetParameter("EcalTECHmin");
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("EcalTECHmin",hmin,1)); } 
}

//_____________________________________________________________________________
void SpdEcalTECGeoMapper::SetParameters_2(Bool_t reinit) // private
{ 
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
   
   par = GetParameter("EcalTECMinDist2");  
   if (par) { if (reinit) *par = mapper->GetEcalTECMinDist2(); }
   else { fParams->Add(new SpdParameter("EcalTECMinDist2",mapper->GetEcalTECMinDist2())); } 
   
   Double_t size = mapper->GetEcalTECSize();
   par = GetParameter("EcalTECSize");  
   if (par) { if (reinit) *par = size; }
   else { fParams->Add(new SpdParameter("EcalTECSize",size)); } 
   
   Double_t width = mapper->GetEcalTECWidth();
   par = GetParameter("EcalTECWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("EcalTECWidth",width)); } 
   
   par = GetParameter("EcalTECThickness");  
   if (par) { if (reinit) *par = mapper->GetEcalTECThickness(); }
   else { fParams->Add(new SpdParameter("EcalTECThickness",mapper->GetEcalTECThickness())); }  
   
   /* define secondary parameters */
  
   Double_t hmin = size - width;  
   par = GetParameter("EcalTECHmin");
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("EcalTECHmin",hmin,1)); }  
}

//_____________________________________________________________________________
void SpdEcalTECGeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdEcalTECGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}




