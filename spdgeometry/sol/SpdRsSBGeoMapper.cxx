// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdRsSBGeoMapper
//_____________________________________________________________________________

#include "SpdRsSBGeoMapper.h"
#include "SpdCommonGeoMapper.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdRsSBGeoMapper)

SpdRsSBGeoMapper* SpdRsSBGeoMapper::fInstance = 0;

SpdRsSBGeoMapper* SpdRsSBGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdRsSBGeoMapper(); }

SpdRsSBGeoMapper* SpdRsSBGeoMapper::GetMapper() 
{ return fInstance; }
    
//_____________________________________________________________________________
SpdRsSBGeoMapper::SpdRsSBGeoMapper():SpdGeoMapper("RsSB")
{
  if (fInstance) {
      Fatal("SpdRsSBGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetRsSBDefGeoType();
}

//_____________________________________________________________________________
SpdRsSBGeoMapper::SpdRsSBGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdRsSBGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetRsSBDefGeoType();
}

//_____________________________________________________________________________
SpdRsSBGeoMapper::~SpdRsSBGeoMapper() 
{
 
}

//_____________________________________________________________________________
void SpdRsSBGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdRsSBGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
   
   const Char_t* medium;
   (precise) ? medium = "vacuum2" : medium = "vacuum";
   
   SpdParameter* par = GetParameter("RsSBBaseMaterial");

   if (!par) { fParams->Add(new SpdParameter("RsSBBaseMaterial",medium)); }
   else { *par = medium; } 
}

//_____________________________________________________________________________
Int_t SpdRsSBGeoMapper::GetNSectors() 
{
   Int_t nsec;
   SpdParameter* par = GetParameter("RsSBNSectors");
   if (!par) return 0;
   par->Value(nsec);
   return nsec;
}

//_____________________________________________________________________________
Double_t SpdRsSBGeoMapper::GetSecAngle() 
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 360./nsec: 0;
}

//_____________________________________________________________________________
Double_t SpdRsSBGeoMapper::GetSecAngleOver2() 
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 180./nsec: 0;
}

//_____________________________________________________________________________
TString SpdRsSBGeoMapper::GetVolName(Int_t level, Int_t uid) 
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
Bool_t SpdRsSBGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
  return (gtype > 0 && gtype < 3); 
}

//_____________________________________________________________________________
Bool_t SpdRsSBGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdRsSBGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
  
   if (!fParams) return kFALSE;
       
   if (!CheckGeoType(gtype,"RsSBGeoType")) return kFALSE;
    
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
  
   par = GetParameter("RsSBNSectors");
   if (par) { if (reinit) *par = mapper->GetNGeoSectors(); }
   else { fParams->Add(new SpdParameter("RsSBNSectors",mapper->GetNGeoSectors())); }
   
   par = GetParameter("RsSBBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetRsSBBaseMaterial(); }
   else { fParams->Add(new SpdParameter("RsSBBaseMaterial",mapper->GetRsSBBaseMaterial())); } 
   
   //cout << "-I- <SpdRsSBGeoMapper::InitGeometry> reinit: " << reinit << endl;
     
        if (fGeoType == 1) SetParameters_1(reinit);
   else if (fGeoType == 2) SetParameters_2(reinit);
    
   return kTRUE;
}

//_____________________________________________________________________________
void SpdRsSBGeoMapper::SetParameters_1(Bool_t reinit) // private
{ 
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
   
   Int_t nsections = mapper->GetRsSBNSections1();
   par = GetParameter("RsSBNSections1");  
   if (par) { if (reinit) *par = nsections; }
   else { fParams->Add(new SpdParameter("RsSBNSections1",nsections)); } 
   
   Double_t length = mapper->GetRsSBLen();
   par = GetParameter("RsSBLen");  
   if (par) { if (reinit) *par = length; }
   else { fParams->Add(new SpdParameter("RsSBLen",length)); }  
  
   Double_t size = mapper->GetRsSBSize();
   par = GetParameter("RsSBSize");  
   if (par) { if (reinit) *par = size; }
   else { fParams->Add(new SpdParameter("RsSBSize",size)); } 
   
   Double_t width = mapper->GetRsSBWidth();
   par = GetParameter("RsSBWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("RsSBWidth",width)); } 
     
   /* define secondary parameters */
  
   Double_t hmin = size - width;  
   par = GetParameter("RsSBHmin");
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("RsSBHmin",hmin,1)); } 
   
   Double_t seclen = length/nsections;
   par = GetParameter("RsSBSectionLen");
   if (par) { *par = seclen; }
   else { fParams->Add(new SpdParameter("RsSBSectionLen",seclen,1)); }  
}

//_____________________________________________________________________________
void SpdRsSBGeoMapper::SetParameters_2(Bool_t reinit) // private
{ 
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
   
   Int_t nsections = mapper->GetRsSBNSections2();
   par = GetParameter("RsSBNSections2");  
   if (par) { if (reinit) *par = nsections; }
   else { fParams->Add(new SpdParameter("RsSBNSections2",nsections)); } 
   
   Double_t length = mapper->GetRsSBLen();
   par = GetParameter("RsSBLen");  
   if (par) { if (reinit) *par = length; }
   else { fParams->Add(new SpdParameter("RsSBLen",length)); }  
   
   Double_t size = mapper->GetRsSBSize();
   par = GetParameter("RsSBSize");  
   if (par) { if (reinit) *par = size; }
   else { fParams->Add(new SpdParameter("RsSBSize",size)); } 
   
   Double_t width = mapper->GetRsSBWidth();
   par = GetParameter("RsSBWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("RsSBWidth",width)); } 
   
   /* define secondary parameters */
  
   Double_t hmin = size - width;  
   par = GetParameter("RsSBHmin");
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("RsSBHmin",hmin,1)); }  
   
   Double_t seclen = length/nsections;
   par = GetParameter("RsSBSectionLen");
   if (par) { *par = seclen; }
   else { fParams->Add(new SpdParameter("RsSBSectionLen",seclen,1)); }  
}

//_____________________________________________________________________________
void SpdRsSBGeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdRsSBGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}




