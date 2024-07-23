// $Id$
// Author: artur   2019/09/09


//_____________________________________________________________________________
//
// SpdGFMagneticField
//_____________________________________________________________________________

#include "SpdGFMagneticField.h"
#include "SpdRegion.h"
#include "SpdFieldMap.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdGFMagneticField)

//_____________________________________________________________________________
SpdGFMagneticField::SpdGFMagneticField():fField_(0),fIsOwnField_(kFALSE)
{
 
}

//_____________________________________________________________________________
SpdGFMagneticField::SpdGFMagneticField(SpdField* f, Bool_t IsOwn)
{
   fField_ = f; 
   fIsOwnField_ = IsOwn;
}

//_____________________________________________________________________________
SpdGFMagneticField::SpdGFMagneticField(TString filename, TString fieldname):
fField_(0),fIsOwnField_(kFALSE)
{
   SpdFieldMap* f = new SpdFieldMap(fieldname);
   f->InitData(filename);
    
   fField_ = f; 
   fIsOwnField_ = kTRUE;
}

//_____________________________________________________________________________
SpdGFMagneticField::~SpdGFMagneticField() 
{
   Clear();
}

//_____________________________________________________________________________
void SpdGFMagneticField::Clear()
{
   if (fField_) {
       if (fIsOwnField_) delete fField_;
       fField_ = 0 ;
   }
   fIsOwnField_ = kFALSE;
}

//_____________________________________________________________________________
SpdField* SpdGFMagneticField::SetField(SpdField* f, Bool_t IsOwn) 
{ 
   if (fField_ && fIsOwnField_) delete fField_;
  
   fField_ = f; 
   fIsOwnField_ = IsOwn;
    
   return f;
} 

//_____________________________________________________________________________
SpdField* SpdGFMagneticField::SetFieldMap(TString filename, TString fieldname)
{
   if (fField_ && fIsOwnField_) delete fField_;
    
   SpdFieldMap* f = new SpdFieldMap(fieldname);
   f->InitData(filename);
    
   fField_ = f; 
   fIsOwnField_ = kTRUE;
    
   return f;
}

//_____________________________________________________________________________
TVector3 SpdGFMagneticField::get(const TVector3& position) const 
{ 
   if (!fField_) {
       cout << "-E- <SpdGFMagneticField::get> No field " << endl; 
       return TVector3();
   }
   
   if (TMath::IsNaN(position.X()) || TMath::IsNaN(position.Y()) || TMath::IsNaN(position.Z())) 
   {
       cout << "-E- <SpdGFMagneticField::get> NAN position: " 
            << position.X() << " " << position.Y() << " " << position.Z() << endl;
        
       return TVector3();
   }

   Double_t f[3], p[3];
   
   position.GetXYZ(p);
   
   fField_->GetFieldValue(p,f);
   
   return TVector3(f);
}

//_____________________________________________________________________________
void SpdGFMagneticField::get(const double& posX, const double& posY, const double& posZ, 
                             double& Bx, double& By, double& Bz) const 
{ 
   if (!fField_) {
       Bx = 0; By = 0; Bz = 0;
       return; 
   }
    
   //static int n = 0; 
   if (TMath::IsNaN(posX) || TMath::IsNaN(posY) || TMath::IsNaN(posZ)) 
   {
       //if (n == 0) {
           cout << "-E- <SpdGFMagneticField::get> NAN position: " 
                << posX << " " << posY << " " << posZ << endl;
           //n = 1;
       //}
       Bx = 0; By = 0; Bz = 0;
       return;
   }
    
//   TVector3 pos(posX, posY, posZ);     
//   const TVector3 B = get(pos); 
//   Bx = B.X(); By = B.Y(); Bz = B.Z(); 

   Double_t p[3] = {posX, posY, posZ};
   Double_t f[3] = {0,0,0};
    
   fField_->GetFieldValue(p,f);
  
   Bx = f[0];
   By = f[1];
   Bz = f[2];  
}
 
//_____________________________________________________________________________
SpdRegion* SpdGFMagneticField::GetFieldRegion(TString region) 
{
   if (!fField_) return 0;
   
   SpdRegion* fr = fField_->GetFieldRegion();
   
   // return existed region
   if (region.IsWhitespace()) return fr;
   
   // if there is no any region let's try to create a new one
   if (!fr) return fField_->CreateFieldRegion(region);
   
   // check region type
   if (region != fr->GetRegionType()) {
       cout << "-W- <SpdGFMagneticField::GetFieldRegion> Field region already is defined as"
            << " \" "<< fr->GetRegionType() << "\" (not  \" "<< region << "\" )" << endl;
   }
   
   return fr;
}


