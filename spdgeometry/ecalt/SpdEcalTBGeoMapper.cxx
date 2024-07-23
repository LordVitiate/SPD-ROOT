// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdEcalTBGeoMapper
//_____________________________________________________________________________

#include "SpdEcalTBGeoMapper.h"
#include "SpdCommonGeoMapper.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

SpdEcalTBGeoMapper* SpdEcalTBGeoMapper::fInstance = 0;

SpdEcalTBGeoMapper* SpdEcalTBGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdEcalTBGeoMapper(); }

SpdEcalTBGeoMapper* SpdEcalTBGeoMapper::GetMapper() 
{ return fInstance; }

ClassImp(SpdEcalTBGeoMapper)

//_____________________________________________________________________________
SpdEcalTBGeoMapper::SpdEcalTBGeoMapper():SpdGeoMapper("EcalTB")
{
  if (fInstance) {
      Fatal("SpdEcalTBGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetEcalTBDefGeoType();
}

//_____________________________________________________________________________
SpdEcalTBGeoMapper::SpdEcalTBGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdEcalTBGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetEcalTBDefGeoType();
}

//_____________________________________________________________________________
SpdEcalTBGeoMapper::~SpdEcalTBGeoMapper() 
{
 
}

//_____________________________________________________________________________
void SpdEcalTBGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdEcalTBGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
  
   const Char_t* medium;
   (precise) ? medium = "vacuum2" : medium = "vacuum";
   
   SpdParameter* par = GetParameter("EcalTBBaseMaterial");

   if (!par) { fParams->Add(new SpdParameter("EcalTBBaseMaterial",medium)); }
   else { *par = medium; } 
}

//_____________________________________________________________________________
Int_t SpdEcalTBGeoMapper::GetNSectors() 
{
   Int_t nsec;
   SpdParameter* par = GetParameter("EcalTBNSectors");
   if (!par) return 0;
   par->Value(nsec);
   return nsec;
}

//_____________________________________________________________________________
Double_t SpdEcalTBGeoMapper::GetSecAngle() 
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 360./nsec: 0;
}

//_____________________________________________________________________________
Double_t SpdEcalTBGeoMapper::GetSecAngleOver2() 
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 180./nsec: 0;
}

//_____________________________________________________________________________
Double_t SpdEcalTBGeoMapper::GetClearance2()
{
   Double_t angle = GetSecAngleOver2()*DegToRad();
   SpdParameter* par = GetParameter("EcalTBClearance");
   Double_t clearance2;
   par->Value(clearance2);
   return clearance2/Cos(angle);
}

//_____________________________________________________________________________
TString SpdEcalTBGeoMapper::GetVolName(Int_t level, Int_t uid) 
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
Bool_t SpdEcalTBGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
  return (gtype > 0 && gtype < 3); 
}

//_____________________________________________________________________________
Bool_t SpdEcalTBGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdEcalTBGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
 
   if (!fParams) return kFALSE;
   
   if (!CheckGeoType(gtype,"EcalTBGeoType")) return kFALSE;
  
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;

   par = GetParameter("EcalTBNSectors");
   if (par) { if (reinit) *par = mapper->GetNGeoSectors(); }
   else { fParams->Add(new SpdParameter("EcalTBNSectors",mapper->GetNGeoSectors())); } 
  
   par = GetParameter("EcalTBBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetEcalTBBaseMaterial(); }
   else { fParams->Add(new SpdParameter("EcalTBBaseMaterial",mapper->GetEcalTBBaseMaterial())); } 
    
   par = GetParameter("EcalTBClearance");  
   if (par) { if (reinit) *par = mapper->GetSectorClearance(); }
   else { fParams->Add(new SpdParameter("EcalTBClearance",mapper->GetSectorClearance())); } 
   
   //cout << "-I- <SpdEcalTBGeoMapper::InitGeometry> reinit: " << reinit << endl;
      
        if (fGeoType == 1) SetParameters_1(reinit);
   else if (fGeoType == 2) SetParameters_2(reinit);
    
   return kTRUE;
}

//_____________________________________________________________________________
void SpdEcalTBGeoMapper::SetParameters_1(Bool_t reinit) // private
{ 
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
   
   Double_t angle = GetSecAngleOver2()*DegToRad();
   Double_t clearance2 = GetClearance2();
   
   par = GetParameter("EcalTBClearance2");  
   if (par) { if (reinit) *par = clearance2; }
   else { fParams->Add(new SpdParameter("EcalTBClearance2",clearance2,1)); } 
  
   par = GetParameter("EcalTBLen11");  
   if (par) { if (reinit) *par = mapper->GetEcalTBLen11(); }
   else { fParams->Add(new SpdParameter("EcalTBLen11",mapper->GetEcalTBLen11())); }
    
   Double_t size11 = mapper->GetEcalTBSize11();
   par = GetParameter("EcalTBSize11");    
   if (par) { if (reinit) *par = size11; }
   else { fParams->Add(new SpdParameter("EcalTBSize11",size11)); }
    
   Double_t width11 = mapper->GetEcalTBWidth11(); 
   par = GetParameter("EcalTBWidth11");  
   if (par) { if (reinit) *par = width11; }
   else { fParams->Add(new SpdParameter("EcalTBWidth11",width11)); }
    
   par = GetParameter("EcalTBLen12");  
   if (par) { if (reinit) *par = mapper->GetEcalTBLen12(); }
   else { fParams->Add(new SpdParameter("EcalTBLen12",mapper->GetEcalTBLen12())); }
    
   Double_t size12 = mapper->GetEcalTBSize12(); 
   par = GetParameter("EcalTBSize12");  
   if (par) { if (reinit) *par = size12; }
   else { fParams->Add(new SpdParameter("EcalTBSize12",size12)); }
    
   Double_t width12 = mapper->GetEcalTBWidth12();  
   par = GetParameter("EcalTBWidth12");  
   if (par) { if (reinit) *par = mapper->GetEcalTBWidth12(); }
   else { fParams->Add(new SpdParameter("EcalTBWidth12",width12)); }
   
   /* define secondary parameters */
  
   Double_t hmin11 = size11 - width11;
   par = GetParameter("EcalTBHmin11");  
   if (par) { *par = hmin11; }
   else { fParams->Add(new SpdParameter("EcalTBHmin11",hmin11,1)); }
     
   Double_t lmin11 = 2*(hmin11*Tan(angle) - clearance2);  // < 0 ? 
   par = GetParameter("EcalTBLmin11");  
   if (par) { *par = lmin11; }
   else { fParams->Add(new SpdParameter("EcalTBLmin11",lmin11,2)); }

   Double_t lmax11 = 2*(size11*Tan(angle) - clearance2);  // < 0 ?
   par = GetParameter("EcalTBLmax11");  
   if (par) { *par = lmax11; }
   else { fParams->Add(new SpdParameter("EcalTBLmax11",lmax11,2)); }
   
   Double_t hmin12 = size12 - width12;
   par = GetParameter("EcalTBHmin12");  
   if (par) { *par = hmin11; }
   else { fParams->Add(new SpdParameter("EcalTBHmin12",hmin11,1)); }
   
   Double_t lmin12 = 2*(hmin12*Tan(angle) - clearance2);  // < 0 ? 
   par = GetParameter("EcalTBLmax12");  
   if (par) { *par = lmin12; }
   else { fParams->Add(new SpdParameter("EcalTBLmin12",lmin12,2)); }

   Double_t lmax12 = 2*(size12*Tan(angle) - clearance2);  // < 0 ?
   par = GetParameter("EcalTBLmax12");  
   if (par) { *par = lmax12; }
   else { fParams->Add(new SpdParameter("EcalTBLmax12",lmax12,2)); }
}

//_____________________________________________________________________________
void SpdEcalTBGeoMapper::SetParameters_2(Bool_t reinit) // private
{
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
   
   Double_t angle = GetSecAngleOver2()*DegToRad();
   Double_t clearance2 = GetClearance2();
   
   par = GetParameter("EcalTBClearance2");  
   if (par) { if (reinit) *par = clearance2; }
   else { fParams->Add(new SpdParameter("EcalTBClearance2",clearance2,1)); } 
   
   par = GetParameter("EcalTBLen2");  
   if (par) { if (reinit) *par = mapper->GetEcalTBLen2(); }
   else { fParams->Add(new SpdParameter("EcalTBLen2",mapper->GetEcalTBLen2())); }
    
   Double_t size2 = mapper->GetEcalTBSize2();
   par = GetParameter("EcalTBSize2");  
   if (par) { if (reinit) *par = size2; }
   else { fParams->Add(new SpdParameter("EcalTBSize2",size2)); }
    
   Double_t width2 = mapper->GetEcalTBWidth2();
   par = GetParameter("EcalTBWidth2");  
   if (par) { if (reinit) *par = width2; }
   else { fParams->Add(new SpdParameter("EcalTBWidth2",width2)); } 
   
   /* define secondary parameters */
  
   Double_t hmin2 = size2 - width2;
   par = GetParameter("EcalTBHmin2");  
   if (par) { *par = hmin2; }
   else { fParams->Add(new SpdParameter("EcalTBHmin2",hmin2,1)); }
   
   Double_t lmin2 = 2*(hmin2*Tan(angle) - clearance2);  // < 0 ? 
   par = GetParameter("EcalTBHLmin2");  
   if (par) { *par = lmin2; }
   else { fParams->Add(new SpdParameter("EcalTBLmin2",lmin2,2)); }

   Double_t lmax2 = 2*(size2*Tan(angle) - clearance2);  // < 0 ?
   par = GetParameter("EcalTBLmax2");  
   if (par) { *par = lmax2; }
   else { fParams->Add(new SpdParameter("EcalTBLmax2",lmax2,2)); }
}

//_____________________________________________________________________________
void SpdEcalTBGeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdEcalTBGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}



