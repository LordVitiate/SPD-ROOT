// $Id$
// Author: artur   2019/04/18

#include <iostream>

using std::cout;
using std::endl;

#include "SpdGeoVVolume.h"

ClassImp(SpdGeoVVolume)
ClassImp(SpdGeoVVolumeBox2D)

//_____________________________________________________________________________
//
// SpdGeoVVolume
//_____________________________________________________________________________

//_____________________________________________________________________________
SpdGeoVVolume::SpdGeoVVolume():
TNamed()
{
 
}

//_____________________________________________________________________________
SpdGeoVVolume::SpdGeoVVolume(const Char_t* name, const Char_t* mother):
TNamed(name,mother) 
{
 
}

//_____________________________________________________________________________
SpdGeoVVolume::~SpdGeoVVolume() 
{
 
}
//_____________________________________________________________________________
Int_t SpdGeoVVolume::GetCellId(Double_t x, Double_t y, Double_t z) const 
{ 
  return -1; 
}

//_____________________________________________________________________________ 
Bool_t SpdGeoVVolume::GetCellPos(Int_t id, Double_t& x, Double_t& y, Double_t& z) const 
{ 
  x = 0; y = 0; z = 0; 
  return kFALSE; 
}

//_____________________________________________________________________________    
Double_t SpdGeoVVolume::GetMaxStep() const
{ 
  return 1.; 
}
    
//_____________________________________________________________________________
Bool_t SpdGeoVVolume::MasterToLocal(Int_t id, Double_t& x, Double_t& y, Double_t& z)
{
  //printf("<SpdGeoVVolume::MasterToLocal> [cell %d] local position:  %12.8f %12.8f %12.8f\n",id,x,y,z);
  
  Double_t px(0), py(0), pz(0);
 
  GetCellPos(id,px,py,pz);
  
  x -= px;
  y -= py;
  z -= pz;
  
  //printf("<SpdGeoVVolume::MasterToLocal> [cell %d] master position: %12.8f %12.8f %12.8f\n",id,x,y,z);
}

//_____________________________________________________________________________
Bool_t SpdGeoVVolume::LocalToMaster(Int_t id, Double_t& x, Double_t& y, Double_t& z)
{
  //printf("<SpdGeoVVolume::LocalToMaster> [cell %d] local position:  %12.8f %12.8f %12.8f\n",id,x,y,z);
  
  Double_t px(0), py(0), pz(0);
 
  GetCellPos(id,px,py,pz);
 
  x += px;
  y += py;
  z += pz;
  
  //printf("<SpdGeoVVolume::LocalToMaster> [cell %d] master position: %12.8f %12.8f %12.8f\n",id,x,y,z);
}

//_____________________________________________________________________________
void SpdGeoVVolume::PrintGeoVVolume() const
{
   cout << "<SpdGeoVVolume::PrintGeoVVolume>" << endl;
   
   cout << "VVolume type:         " << ClassName() << endl;
   cout << "VVolume name:         " << fName << endl;
   cout << "Mother volume name:   " << fTitle << endl;  
   
   cout << endl;
}

//_____________________________________________________________________________
//
// SpdGeoVVolumeBox2D
//_____________________________________________________________________________

//_____________________________________________________________________________
SpdGeoVVolumeBox2D::SpdGeoVVolumeBox2D():
SpdGeoVVolume(),fN1(1),fN2(1),fL1(0),fL2(0),fD1(0),fD2(0),
fMaxStep(1),fMaxStepFactor(0.1),fAxes(0)
{
 
}

//_____________________________________________________________________________
SpdGeoVVolumeBox2D::SpdGeoVVolumeBox2D(const Char_t* name, const Char_t* title):
SpdGeoVVolume(name,title),fN1(1),fN2(1),fL1(0),fL2(0),fD1(0),fD2(0),
fMaxStep(1),fMaxStepFactor(0.1),fAxes(0)
{
 
}

//_____________________________________________________________________________
SpdGeoVVolumeBox2D::~SpdGeoVVolumeBox2D() 
{
 
}

//_____________________________________________________________________________
void SpdGeoVVolumeBox2D::SetCells(Double_t n1, Double_t n2)
{
  fN1 = (n1 < 1) ? 1 : n1; 
  fN2 = (n2 < 1) ? 1 : n2; 
  
  fD1 = fL1/fN1;
  fD2 = fL2/fN2;
  
  fMaxStep = (fD1 > fD2) ? fMaxStepFactor*fD2 : fMaxStepFactor*fD1;
}

//_____________________________________________________________________________
void SpdGeoVVolumeBox2D::SetSizes(Double_t L1, Double_t L2)
{
  fL1 = !(L1 > 0) ? 0 : L1; 
  fL2 = !(L2 > 0) ? 0 : L2; 
  
  fD1 = fL1/fN1;
  fD2 = fL2/fN2;
  
  fMaxStep = (fD1 > fD2) ? fMaxStepFactor*fD2 : fMaxStepFactor*fD1;
}

//_____________________________________________________________________________
void SpdGeoVVolumeBox2D::SetAxes(Char_t x1, Char_t x2)
{
        if (x1 == 'x' && x2 == 'y') fAxes = 0;
   else if (x1 == 'x' && x2 == 'z') fAxes = 1;
   else if (x1 == 'y' && x2 == 'z') fAxes = 2;
   else if (x1 == 'y' && x2 == 'x') fAxes = 3;
   else if (x1 == 'z' && x2 == 'x') fAxes = 4;
   else if (x1 == 'z' && x2 == 'y') fAxes = 5;
   else fAxes = 0;
}

//_____________________________________________________________________________
Int_t SpdGeoVVolumeBox2D::GetCellId(Double_t x, Double_t y, Double_t z) const
{ 
  switch (fAxes) 
  {
     case 0 : return GetCellId(x,y); 
     case 1 : return GetCellId(x,z); 
     case 2 : return GetCellId(y,z); 
     case 3 : return GetCellId(y,x); 
     case 4 : return GetCellId(z,x); 
     case 5 : return GetCellId(z,y); 
  }
  return GetCellId(x,y); 
}

//_____________________________________________________________________________
Bool_t SpdGeoVVolumeBox2D::GetCellPos(Int_t id, Double_t& x, Double_t& y, Double_t& z) const
{   
   switch (fAxes) 
   {
     case 0 : { z = 0; return GetCellPos(id,x,y); } 
     case 1 : { y = 0; return GetCellPos(id,z,x); } 
     case 2 : { x = 0; return GetCellPos(id,y,z); } 
     case 3 : { z = 0; return GetCellPos(id,y,x); } 
     case 4 : { y = 0; return GetCellPos(id,z,x); } 
     case 5 : { x = 0; return GetCellPos(id,z,y); } 
  }
  
  GetCellPos(id,x,y);
  z = 0; 
  
  return kFALSE;
}

//_____________________________________________________________________________
Bool_t SpdGeoVVolumeBox2D::GetCellPos(Int_t id, Double_t* pos) const
{
  switch (fAxes) 
  {
     case 0 : { pos[2] = 0; return GetCellPos(id,pos[0],pos[1]); } 
     case 1 : { pos[1] = 0; return GetCellPos(id,pos[2],pos[0]); } 
     case 2 : { pos[0] = 0; return GetCellPos(id,pos[1],pos[2]); } 
     case 3 : { pos[2] = 0; return GetCellPos(id,pos[1],pos[0]); } 
     case 4 : { pos[1] = 0; return GetCellPos(id,pos[2],pos[0]); } 
     case 5 : { pos[0] = 0; return GetCellPos(id,pos[2],pos[1]); } 
  }
  
  GetCellPos(id,pos[0],pos[1]);
  pos[2] = 0; 
  
  return kFALSE;   
}

//_____________________________________________________________________________
Bool_t SpdGeoVVolumeBox2D::GetPos(Double_t id1, Double_t id2, Double_t* pos) const
{
  switch (fAxes) 
  {
     case 0 : { pos[2] = 0; return GetPos(id1,id2,pos[0],pos[1]); } 
     case 1 : { pos[1] = 0; return GetPos(id1,id2,pos[2],pos[0]); } 
     case 2 : { pos[0] = 0; return GetPos(id1,id2,pos[1],pos[2]); } 
     case 3 : { pos[2] = 0; return GetPos(id1,id2,pos[1],pos[0]); } 
     case 4 : { pos[1] = 0; return GetPos(id1,id2,pos[2],pos[0]); } 
     case 5 : { pos[0] = 0; return GetPos(id1,id2,pos[2],pos[1]); } 
  }
  
  GetPos(id1,id2,pos[0],pos[1]);
  pos[2] = 0; 
  
  return kFALSE;   
}

//_____________________________________________________________________________
void SpdGeoVVolumeBox2D::PrintVolume() const
{
   printf("%20s %8d %8d %10.4f %10.4f %10.3e %10.3e %10.3e %5s\n",
          fName.Data(),fN1,fN2,fL1,fL2,fD1,fD2,fMaxStep,GetAxes());
}

//_____________________________________________________________________________
Int_t SpdGeoVVolumeBox2D::GetCellId(Double_t x1, Double_t x2) const
{
  x1 += 0.5*fL1;
  if (x1 < 0 || x1 > fL1) return -1;
  
  x2 += 0.5*fL2;
  if (x2 < 0 || x2 > fL2) return -1;
  
  Int_t id1 = x1/fD1;
  Int_t id2 = x2/fD2;
  
  if (!(id1 < fN1)) id1 = fN1-1;
  if (!(id2 < fN2)) id2 = fN2-1;
    
  //cout << "<SpdGeoVVolumeBox2D::GetCellId> " << x1 << " " << x2 
  //     << "  id: " << CellId(id1,id2)  << " = ("<< id1 << ", " << id2 << ") " << endl;
       
  return CellId(id1,id2);    
}

//_____________________________________________________________________________
Bool_t SpdGeoVVolumeBox2D::GetCellPos(Int_t id, Double_t& x1, Double_t& x2) const
{
   x1 = 0; x2 = 0;
  
   Int_t id1, id2;
   CellId(id,id1,id2);  
   
   if (id1 < 0 || !(id1 < fN1)) return kFALSE;
   if (id2 < 0 || !(id2 < fN2)) return kFALSE;
   
   x1 = -0.5*fL1 + (id1 + 0.5)*fD1;
   x2 = -0.5*fL2 + (id2 + 0.5)*fD2;
   
   //cout << "<SpdGeoVVolumeBox2D::GetCellPos> " << x1 << " " << x2 << endl;
   
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdGeoVVolumeBox2D::GetPos(Double_t id1, Double_t id2, Double_t& x1, Double_t& x2) const
{
   x1 = 0; x2 = 0;
  
   if (id1 < 0 || !(id1 < Double_t(fN1))) return kFALSE;
   if (id2 < 0 || !(id2 < Double_t(fN2))) return kFALSE;
   
   x1 = -0.5*fL1 + (id1 + 0.5)*fD1;
   x2 = -0.5*fL2 + (id2 + 0.5)*fD2;
   
   //cout << "<SpdGeoVVolumeBox2D::GetPos> " << x1 << " " << x2 << endl;
   
   return kTRUE;
}

//_____________________________________________________________________________
const Char_t* SpdGeoVVolumeBox2D::GetAxes() const
{
   switch (fAxes) 
   {
     case 0 : return "xy"; 
     case 1 : return "xz"; 
     case 2 : return "yz"; 
     case 3 : return "yx"; 
     case 4 : return "zx"; 
     case 5 : return "zy"; 
   }
   return "xy"; 
}

//_____________________________________________________________________________
Char_t SpdGeoVVolumeBox2D::GetAxis(Int_t axis) const
{
   switch (fAxes) 
   {
     case 0 : return (axis == 1) ? 'x' : 'y'; 
     case 1 : return (axis == 1) ? 'x' : 'z';  
     case 2 : return (axis == 1) ? 'y' : 'z'; 
     case 3 : return (axis == 1) ? 'y' : 'x'; 
     case 4 : return (axis == 1) ? 'z' : 'x'; 
     case 5 : return (axis == 1) ? 'z' : 'y'; 
   }
   return 'n';   
}






