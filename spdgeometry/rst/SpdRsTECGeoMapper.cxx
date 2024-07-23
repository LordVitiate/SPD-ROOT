// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdRsTECGeoMapper
//_____________________________________________________________________________

#include "SpdRsTECGeoMapper.h"
#include "SpdCommonGeoMapper.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdRsTECGeoMapper)

SpdRsTECGeoMapper* SpdRsTECGeoMapper::fInstance = 0;

SpdRsTECGeoMapper* SpdRsTECGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdRsTECGeoMapper(); }

SpdRsTECGeoMapper* SpdRsTECGeoMapper::GetMapper() 
{ return fInstance; } 
    
//_____________________________________________________________________________
SpdRsTECGeoMapper::SpdRsTECGeoMapper():SpdGeoMapper("RsTEC")
{
  if (fInstance) {
      Fatal("SpdRsTECGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetRsTECDefGeoType();
}

//_____________________________________________________________________________
SpdRsTECGeoMapper::SpdRsTECGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdRsTECGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetRsTECDefGeoType();
}

//_____________________________________________________________________________
SpdRsTECGeoMapper::~SpdRsTECGeoMapper() 
{
 
}

//_____________________________________________________________________________
void SpdRsTECGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdRsTECGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
   
   const Char_t* medium;
   (precise) ? medium = "vacuum2" : medium = "vacuum";
   
   SpdParameter* par = GetParameter("RsTECBaseMaterial");

   if (!par) { fParams->Add(new SpdParameter("RsTECBaseMaterial",medium)); }
   else { *par = medium; } 
}

//_____________________________________________________________________________
Int_t SpdRsTECGeoMapper::GetNSectors() 
{
   Int_t nsec;
   SpdParameter* par = GetParameter("RsTECNSectors");
   if (!par) return 0;
   par->Value(nsec);
   return nsec;
}

//_____________________________________________________________________________
Double_t SpdRsTECGeoMapper::GetSecAngle() 
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 360./nsec: 0;
}

//_____________________________________________________________________________
Double_t SpdRsTECGeoMapper::GetSecAngleOver2() 
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 180./nsec: 0;
}

//_____________________________________________________________________________
TString SpdRsTECGeoMapper::GetVolName(Int_t level, Int_t uid) 
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
Bool_t SpdRsTECGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
  return (gtype > 0 && gtype < 3); 
}

//_____________________________________________________________________________
Bool_t SpdRsTECGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdRsTECGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
  
   if (!fParams) return kFALSE;
   
   if (!CheckGeoType(gtype,"RsTECGeoType")) return kFALSE;
     
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
  
   par = GetParameter("RsTECNSectors");
   if (par) { if (reinit) *par = mapper->GetNGeoSectors(); }
   else { fParams->Add(new SpdParameter("RsTECNSectors",mapper->GetNGeoSectors())); }
   
   par = GetParameter("RsTECBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetRsTECBaseMaterial(); }
   else { fParams->Add(new SpdParameter("RsTECBaseMaterial",mapper->GetRsTECBaseMaterial())); } 
   
   //cout << "-I- <SpdRsTECGeoMapper::InitGeometry> reinit: " << reinit << endl;
     
        if (fGeoType == 1) SetParameters_1(reinit);
   else if (fGeoType == 2) SetParameters_2(reinit);
    
   return kTRUE;
}

//_____________________________________________________________________________
void SpdRsTECGeoMapper::SetParameters_1(Bool_t reinit) // private
{ 
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
   
   par = GetParameter("RsTECMinDist1");  
   if (par) { if (reinit) *par = mapper->GetRsTECMinDist1(); }
   else { fParams->Add(new SpdParameter("RsTECMinDist1",mapper->GetRsTECMinDist1())); } 
   
   Double_t size = mapper->GetRsTECSize();
   par = GetParameter("RsTECSize");  
   if (par) { if (reinit) *par = size; }
   else { fParams->Add(new SpdParameter("RsTECSize",size)); } 
   
   Double_t width = mapper->GetRsTECWidth();
   par = GetParameter("RsTECWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("RsTECWidth",width)); } 
   
   par = GetParameter("RsTECThickness");  
   if (par) { if (reinit) *par = mapper->GetRsTECThickness(); }
   else { fParams->Add(new SpdParameter("RsTECThickness",mapper->GetRsTECThickness())); }  
   
   /* define secondary parameters */
  
   Double_t hmin = size - width;  
   par = GetParameter("RsTECHmin");
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("RsTECHmin",hmin,1)); } 
}

//_____________________________________________________________________________
void SpdRsTECGeoMapper::SetParameters_2(Bool_t reinit) // private
{ 
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
   
   par = GetParameter("RsTECMinDist2");  
   if (par) { if (reinit) *par = mapper->GetRsTECMinDist2(); }
   else { fParams->Add(new SpdParameter("RsTECMinDist2",mapper->GetRsTECMinDist2())); } 
   
   Double_t size = mapper->GetRsTECSize();
   par = GetParameter("RsTECSize");  
   if (par) { if (reinit) *par = size; }
   else { fParams->Add(new SpdParameter("RsTECSize",size)); } 
   
   Double_t width = mapper->GetRsTECWidth();
   par = GetParameter("RsTECWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("RsTECWidth",width)); } 
   
   par = GetParameter("RsTECThickness");  
   if (par) { if (reinit) *par = mapper->GetRsTECThickness(); }
   else { fParams->Add(new SpdParameter("RsTECThickness",mapper->GetRsTECThickness())); }  
   
   /* define secondary parameters */
  
   Double_t hmin = size - width;  
   par = GetParameter("RsTECHmin");
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("RsTECHmin",hmin,1)); }  
}

//_____________________________________________________________________________
void SpdRsTECGeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdRsTECGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}




