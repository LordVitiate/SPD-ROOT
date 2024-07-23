// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdRsTBGeoMapper
//_____________________________________________________________________________

#include "SpdRsTBGeoMapper.h"
#include "SpdCommonGeoMapper.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdRsTBGeoMapper)

SpdRsTBGeoMapper* SpdRsTBGeoMapper::fInstance = 0;

SpdRsTBGeoMapper* SpdRsTBGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdRsTBGeoMapper(); }

SpdRsTBGeoMapper* SpdRsTBGeoMapper::GetMapper() 
{ return fInstance; }   
    
//_____________________________________________________________________________
SpdRsTBGeoMapper::SpdRsTBGeoMapper():SpdGeoMapper("RsTB")
{
  if (fInstance) {
      Fatal("SpdRsTBGeoMapper", "Singleton instance already exists.");
      return;
  }
   
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetRsTBDefGeoType();
}

//_____________________________________________________________________________
SpdRsTBGeoMapper::SpdRsTBGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdRsTBGeoMapper", "Singleton instance already exists.");
      return;
  }
   
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetRsTBDefGeoType();
}

//_____________________________________________________________________________
SpdRsTBGeoMapper::~SpdRsTBGeoMapper() 
{
 
}

//_____________________________________________________________________________
void SpdRsTBGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdRsTBGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
  
   const Char_t* medium;
   (precise) ? medium = "vacuum2" : medium = "vacuum";
   
   SpdParameter* par = GetParameter("RsTBBaseMaterial");

   if (!par) { fParams->Add(new SpdParameter("RsTBBaseMaterial",medium)); }
   else { *par = medium; } 
}

//_____________________________________________________________________________
Int_t SpdRsTBGeoMapper::GetNSectors() 
{
   Int_t nsec;
   SpdParameter* par = GetParameter("RsTBNSectors");
   if (!par) return 0;
   par->Value(nsec);
   return nsec;
}

//_____________________________________________________________________________
Double_t SpdRsTBGeoMapper::GetSecAngle() 
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 360./nsec: 0;
}

//_____________________________________________________________________________
Double_t SpdRsTBGeoMapper::GetSecAngleOver2() 
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 180./nsec: 0;
}

//_____________________________________________________________________________
Double_t SpdRsTBGeoMapper::GetClearance2()
{
   Double_t angle = GetSecAngleOver2()*DegToRad();
   SpdParameter* par = GetParameter("RsTBClearance");
   Double_t clearance2;
   par->Value(clearance2);
   return clearance2/Cos(angle);
}


//_____________________________________________________________________________
TString SpdRsTBGeoMapper::GetVolName(Int_t level, Int_t uid) 
{ 
  TString name("");
  
  if (fGeoType == 1) {
     
    if (level == 0) {
        
        if (uid == 0)  name = "module1";
        if (uid == 1)  name = "module2";
        
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
Bool_t SpdRsTBGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
  return (gtype > 0 && gtype < 3); 
}

//_____________________________________________________________________________
Bool_t SpdRsTBGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdRsTBGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
 
   if (!fParams) return kFALSE;
   
   if (!CheckGeoType(gtype,"RsTBGeoType")) return kFALSE;
    
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
     
   SpdParameter* par;
   
   par = GetParameter("RsTBNSectors");
   if (par) { if (reinit) *par = mapper->GetNGeoSectors(); }
   else { fParams->Add(new SpdParameter("RsTBNSectors",mapper->GetNGeoSectors())); } 
  
   par = GetParameter("RsTBBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetRsTBBaseMaterial(); }
   else { fParams->Add(new SpdParameter("RsTBBaseMaterial",mapper->GetRsTBBaseMaterial())); } 
    
   par = GetParameter("RsTBClearance");  
   if (par) { if (reinit) *par = mapper->GetSectorClearance(); }
   else { fParams->Add(new SpdParameter("RsTBClearance",mapper->GetSectorClearance())); } 
   
   //cout << "-I- <SpdRsTBGeoMapper::InitGeometry> reinit: " << reinit << endl;
      
        if (fGeoType == 1) SetParameters_1(reinit);
   else if (fGeoType == 2) SetParameters_2(reinit);
    
   return kTRUE;
}

//_____________________________________________________________________________
void SpdRsTBGeoMapper::SetParameters_1(Bool_t reinit) // private
{ 
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
   
   Double_t angle = GetSecAngleOver2()*DegToRad();
   Double_t clearance2 = GetClearance2();
   
   par = GetParameter("RsTBClearance2");  
   if (par) { if (reinit) *par = clearance2; }
   else { fParams->Add(new SpdParameter("RsTBClearance2",clearance2,1)); } 
  
   par = GetParameter("RsTBLen11");  
   if (par) { if (reinit) *par = mapper->GetRsTBLen11(); }
   else { fParams->Add(new SpdParameter("RsTBLen11",mapper->GetRsTBLen11())); }
    
   Double_t size11 = mapper->GetRsTBSize11();
   par = GetParameter("RsTBSize11");    
   if (par) { if (reinit) *par = size11; }
   else { fParams->Add(new SpdParameter("RsTBSize11",size11)); }
    
   Double_t width11 = mapper->GetRsTBWidth11(); 
   par = GetParameter("RsTBWidth11");  
   if (par) { if (reinit) *par = width11; }
   else { fParams->Add(new SpdParameter("RsTBWidth11",width11)); }
    
   par = GetParameter("RsTBLen12");  
   if (par) { if (reinit) *par = mapper->GetRsTBLen12(); }
   else { fParams->Add(new SpdParameter("RsTBLen12",mapper->GetRsTBLen12())); }
    
   Double_t size12 = mapper->GetRsTBSize12(); 
   par = GetParameter("RsTBSize12");  
   if (par) { if (reinit) *par = size12; }
   else { fParams->Add(new SpdParameter("RsTBSize12",size12)); }
    
   Double_t width12 = mapper->GetRsTBWidth12();  
   par = GetParameter("RsTBWidth12");  
   if (par) { if (reinit) *par = mapper->GetRsTBWidth12(); }
   else { fParams->Add(new SpdParameter("RsTBWidth12",width12)); }
   
   /* define secondary parameters */
  
   Double_t hmin11 = size11 - width11;
   par = GetParameter("RsTBHmin11");  
   if (par) { *par = hmin11; }
   else { fParams->Add(new SpdParameter("RsTBHmin11",hmin11,1)); }
     
   Double_t lmin11 = 2*(hmin11*Tan(angle) - clearance2);  // < 0 ? 
   par = GetParameter("RsTBLmin11");  
   if (par) { *par = lmin11; }
   else { fParams->Add(new SpdParameter("RsTBLmin11",lmin11,2)); }

   Double_t lmax11 = 2*(size11*Tan(angle) - clearance2);  // < 0 ?
   par = GetParameter("RsTBLmax11");  
   if (par) { *par = lmax11; }
   else { fParams->Add(new SpdParameter("RsTBLmax11",lmax11,2)); }
   
   Double_t hmin12 = size12 - width12;
   par = GetParameter("RsTBHmin12");  
   if (par) { *par = hmin11; }
   else { fParams->Add(new SpdParameter("RsTBHmin12",hmin11,1)); }
   
   Double_t lmin12 = 2*(hmin12*Tan(angle) - clearance2);  // < 0 ? 
   par = GetParameter("RsTBLmax12");  
   if (par) { *par = lmin12; }
   else { fParams->Add(new SpdParameter("RsTBLmin12",lmin12,2)); }

   Double_t lmax12 = 2*(size12*Tan(angle) - clearance2);  // < 0 ?
   par = GetParameter("RsTBLmax12");  
   if (par) { *par = lmax12; }
   else { fParams->Add(new SpdParameter("RsTBLmax12",lmax12,2)); }
}

//_____________________________________________________________________________
void SpdRsTBGeoMapper::SetParameters_2(Bool_t reinit) // private
{
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
   
   Double_t angle = GetSecAngleOver2()*DegToRad();
   Double_t clearance2 = GetClearance2();
   
   par = GetParameter("RsTBClearance2");  
   if (par) { if (reinit) *par = clearance2; }
   else { fParams->Add(new SpdParameter("RsTBClearance2",clearance2,1)); } 
   
   par = GetParameter("RsTBLen2");  
   if (par) { if (reinit) *par = mapper->GetRsTBLen2(); }
   else { fParams->Add(new SpdParameter("RsTBLen2",mapper->GetRsTBLen2())); }
    
   Double_t size2 = mapper->GetRsTBSize2();
   par = GetParameter("RsTBSize2");  
   if (par) { if (reinit) *par = size2; }
   else { fParams->Add(new SpdParameter("RsTBSize2",size2)); }
    
   Double_t width2 = mapper->GetRsTBWidth2();
   par = GetParameter("RsTBWidth2");  
   if (par) { if (reinit) *par = width2; }
   else { fParams->Add(new SpdParameter("RsTBWidth2",width2)); } 
   
   /* define secondary parameters */
  
   Double_t hmin2 = size2 - width2;
   par = GetParameter("RsTBHmin2");  
   if (par) { *par = hmin2; }
   else { fParams->Add(new SpdParameter("RsTBHmin2",hmin2,1)); }
   
   Double_t lmin2 = 2*(hmin2*Tan(angle) - clearance2);  // < 0 ? 
   par = GetParameter("RsTBHLmin2");  
   if (par) { *par = lmin2; }
   else { fParams->Add(new SpdParameter("RsTBLmin2",lmin2,2)); }

   Double_t lmax2 = 2*(size2*Tan(angle) - clearance2);  // < 0 ?
   par = GetParameter("RsTBLmax2");  
   if (par) { *par = lmax2; }
   else { fParams->Add(new SpdParameter("RsTBLmax2",lmax2,2)); }
}

//_____________________________________________________________________________
void SpdRsTBGeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdRsTBGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}




