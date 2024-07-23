// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdEcalSBGeoMapper
//_____________________________________________________________________________

#include "SpdEcalSBGeoMapper.h"
#include "SpdCommonGeoMapper.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdEcalSBGeoMapper)

SpdEcalSBGeoMapper* SpdEcalSBGeoMapper::fInstance = 0;

SpdEcalSBGeoMapper* SpdEcalSBGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdEcalSBGeoMapper(); }

SpdEcalSBGeoMapper* SpdEcalSBGeoMapper::GetMapper() 
{ return fInstance; }
    
//_____________________________________________________________________________
SpdEcalSBGeoMapper::SpdEcalSBGeoMapper():SpdGeoMapper("EcalSB")
{
  if (fInstance) {
      Fatal("SpdEcalSBGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetEcalSBDefGeoType();
}

//_____________________________________________________________________________
SpdEcalSBGeoMapper::SpdEcalSBGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdEcalSBGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetEcalSBDefGeoType();
}

//_____________________________________________________________________________
SpdEcalSBGeoMapper::~SpdEcalSBGeoMapper() 
{
 
}

//_____________________________________________________________________________
void SpdEcalSBGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdEcalSBGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
   
   const Char_t* medium;
   (precise) ? medium = "vacuum2" : medium = "vacuum";
   
   SpdParameter* par = GetParameter("EcalSBBaseMaterial");

   if (!par) { fParams->Add(new SpdParameter("EcalSBBaseMaterial",medium)); }
   else { *par = medium; } 
}

//_____________________________________________________________________________
Int_t SpdEcalSBGeoMapper::GetNSectors() 
{
   Int_t nsec;
   SpdParameter* par = GetParameter("EcalSBNSectors");
   if (!par) return 0;
   par->Value(nsec);
   return nsec;
}

//_____________________________________________________________________________
Double_t SpdEcalSBGeoMapper::GetSecAngle() 
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 360./nsec: 0;
}

//_____________________________________________________________________________
Double_t SpdEcalSBGeoMapper::GetSecAngleOver2() 
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 180./nsec: 0;
}

//_____________________________________________________________________________
TString SpdEcalSBGeoMapper::GetVolName(Int_t level, Int_t uid) 
{ 
  TString name("");
  
  if (fGeoType == 1) {
     
    if (level == 0) {
        
        if (uid == 0)  name = "module";
        
        return AddPrefix(name); 
    }
  }
  
  if (fGeoType == 2) {
     
    if (level == 0) {
      
        if (uid == 0) name  = "module";          
      
        return AddPrefix(name); 
    }
  }
  
  return name;
}

//_____________________________________________________________________________
Bool_t SpdEcalSBGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
  return (gtype > 0 && gtype < 3); 
}

//_____________________________________________________________________________
Bool_t SpdEcalSBGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdEcalSBGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
  
   if (!fParams) return kFALSE;
      
   if (!CheckGeoType(gtype,"EcalSBGeoType")) return kFALSE;
    
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
  
   par = GetParameter("EcalSBNSectors");
   if (par) { if (reinit) *par = mapper->GetNGeoSectors(); }
   else { fParams->Add(new SpdParameter("EcalSBNSectors",mapper->GetNGeoSectors())); }
   
   par = GetParameter("EcalSBBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetEcalSBBaseMaterial(); }
   else { fParams->Add(new SpdParameter("EcalSBBaseMaterial",mapper->GetEcalSBBaseMaterial())); } 
   
   //cout << "-I- <SpdEcalSBGeoMapper::InitGeometry> reinit: " << reinit << endl;
     
        if (fGeoType == 1) SetParameters_1(reinit);
   else if (fGeoType == 2) SetParameters_2(reinit);
    
   return kTRUE;
}

//_____________________________________________________________________________
void SpdEcalSBGeoMapper::SetParameters_1(Bool_t reinit) // private
{ 
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
   
   Int_t nsections = mapper->GetEcalSBNSections1();
   par = GetParameter("EcalSBNSections1");  
   if (par) { if (reinit) *par = nsections; }
   else { fParams->Add(new SpdParameter("EcalSBNSections1",nsections)); } 
   
   Double_t length = mapper->GetEcalSBLen();
   par = GetParameter("EcalSBLen");  
   if (par) { if (reinit) *par = length; }
   else { fParams->Add(new SpdParameter("EcalSBLen",length)); }  
  
   Double_t size = mapper->GetEcalSBSize();
   par = GetParameter("EcalSBSize");  
   if (par) { if (reinit) *par = size; }
   else { fParams->Add(new SpdParameter("EcalSBSize",size)); } 
   
   Double_t width = mapper->GetEcalSBWidth();
   par = GetParameter("EcalSBWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("EcalSBWidth",width)); } 
     
   /* define secondary parameters */
  
   Double_t hmin = size - width;  
   par = GetParameter("EcalSBHmin");
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("EcalSBHmin",hmin,1)); } 
   
   Double_t seclen = length/nsections;
   par = GetParameter("EcalSBSectionLen");
   if (par) { *par = seclen; }
   else { fParams->Add(new SpdParameter("EcalSBSectionLen",seclen,1)); }  
}

//_____________________________________________________________________________
void SpdEcalSBGeoMapper::SetParameters_2(Bool_t reinit) // private
{ 
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
   
   Int_t nsections = mapper->GetEcalSBNSections2();
   par = GetParameter("EcalSBNSections2");  
   if (par) { if (reinit) *par = nsections; }
   else { fParams->Add(new SpdParameter("EcalSBNSections2",nsections)); } 
   
   Double_t length = mapper->GetEcalSBLen();
   par = GetParameter("EcalSBLen");  
   if (par) { if (reinit) *par = length; }
   else { fParams->Add(new SpdParameter("EcalSBLen",length)); }  
   
   Double_t size = mapper->GetEcalSBSize();
   par = GetParameter("EcalSBSize");  
   if (par) { if (reinit) *par = size; }
   else { fParams->Add(new SpdParameter("EcalSBSize",size)); } 
   
   Double_t width = mapper->GetEcalSBWidth();
   par = GetParameter("EcalSBWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("EcalSBWidth",width)); } 
   
   /* define secondary parameters */
  
   Double_t hmin = size - width;  
   par = GetParameter("EcalSBHmin");
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("EcalSBHmin",hmin,1)); }  
   
   Double_t seclen = length/nsections;
   par = GetParameter("EcalSBSectionLen");
   if (par) { *par = seclen; }
   else { fParams->Add(new SpdParameter("EcalSBSectionLen",seclen,1)); }  
}

//_____________________________________________________________________________
void SpdEcalSBGeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdEcalSBGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}




