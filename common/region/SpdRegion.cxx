// $Id$
// Author: artur   2017/11/28

#include <TMath.h>
#include <TGeoManager.h>
#include "SpdRegion.h"

#include <iostream>

using std::cout;
using std::endl;

#define SR_EPS  1e-9

ClassImp(SpdBoxRegion)
ClassImp(SpdTubeRegion)
ClassImp(SpdPhysicalRegion)
ClassImp(SpdRegion)

//_____________________________________________________________________________
//
// SpdBoxRegion
//_____________________________________________________________________________

//_____________________________________________________________________________
SpdBoxRegion::SpdBoxRegion():SpdRegion("SPD BOX REGION","box"),
fXmin(0.), fXmax(0.), fYmin(0.), fYmax(0.), fZmin(0.), fZmax(0.)
{
 
}

//_____________________________________________________________________________
Bool_t SpdBoxRegion::SetBoxRegion(Double_t xmin, Double_t xmax, Double_t ymin, 
                                  Double_t ymax, Double_t zmin, Double_t zmax)
{
  if ((xmax - xmin) < SR_EPS) { fIsRegionInit = kFALSE; return kFALSE; }
  if ((ymax - ymin) < SR_EPS) { fIsRegionInit = kFALSE; return kFALSE; }
  if ((zmax - zmin) < SR_EPS) { fIsRegionInit = kFALSE; return kFALSE; }
   
  fXmin = xmin; fXmax = xmax;
  fYmin = ymin; fYmax = ymax;
  fZmin = zmin; fZmax = zmax;
  
  fIsRegionInit = kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdBoxRegion::IsInside(Double_t x, Double_t y, Double_t z) const
{
  if (!fIsRegionInit) return kFALSE;
  
  return !(x < fXmin  ||  x > fXmax  ||
           y < fYmin  ||  y > fYmax  ||
           z < fZmin  ||  z > fZmax);
}

//_____________________________________________________________________________
Bool_t SpdBoxRegion::IsInside(Double_t r, Double_t z) const
{
  if (!fIsRegionInit) return kFALSE;
  
  return !(z < fZmin || z > fZmax ||
           r < fXmin || r > fXmax ||
           r < fYmin || r > fYmax);
}

//_____________________________________________________________________________
void SpdBoxRegion::Print(Option_t* option) 
{
  cout << "\t<SpdBoxRegion::Print> Name/Type: " 
       << GetName() << " / " << GetRegionType() << endl;
       
  cout << "\t X [min,max]: " << fXmin << ", " << fXmax << endl;
  cout << "\t Y [min,max]: " << fYmin << ", " << fYmax << endl;
  cout << "\t Z [min,max]: " << fZmin << ", " << fZmax << endl;
}

//_____________________________________________________________________________
//
// SpdTubeRegion
//_____________________________________________________________________________


//_____________________________________________________________________________
SpdTubeRegion::SpdTubeRegion():SpdRegion("SPD TUBE REGION","tube"),
fRmin(0), fRmax(0), fZmin(0), fZmax(0)
{
 
}

//_____________________________________________________________________________
Bool_t SpdTubeRegion::SetTubeRegion(Double_t rmin, Double_t rmax, 
                                    Double_t zmin, Double_t zmax)
{
   if ((rmax - rmin) < SR_EPS) { fIsRegionInit = kFALSE; return kFALSE; }
   if ((zmax - zmin) < SR_EPS) { fIsRegionInit = kFALSE; return kFALSE; }
  
   fRmin = rmin; fRmax = rmax;
   fZmin = zmin; fZmax = zmax;
   
   fIsRegionInit = kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdTubeRegion::IsInside(Double_t x, Double_t y, Double_t z) const
{
   if (!fIsRegionInit) return kFALSE;
   
   if (z < fZmin || z > fZmax) return kFALSE;
   
   Double_t r = TMath::Sqrt(x*x+y*y);
   return !(r < fRmin || r > fRmax);
}

//_____________________________________________________________________________
Bool_t SpdTubeRegion::IsInside(Double_t r, Double_t z) const
{
   if (!fIsRegionInit) return kFALSE;
   
   return !(r < fRmin || r > fRmax || z < fZmin || z > fZmax);
}

//_____________________________________________________________________________
void SpdTubeRegion::Print(Option_t* option) 
{
   cout << "\t<SpdTubeRegion::Print> Name/Type: " 
       << GetName() << " / " << GetRegionType() << endl;
       
   cout << "\t R [min,max]: " << fRmin << ", " << fRmax << endl;
   cout << "\t Z [min,max]: " << fZmin << ", " << fZmax << endl;
}

//_____________________________________________________________________________
//
// SpdPhysicalRegion
//_____________________________________________________________________________


//_____________________________________________________________________________
SpdPhysicalRegion::SpdPhysicalRegion():SpdRegion("SPD PHYSICAL REGION","physical"),
fRegion("undefined"),fType(-1)
{
 
}

//_____________________________________________________________________________
Bool_t SpdPhysicalRegion::SetPhysicalRegion(TString region, TString type)
{
   fRegion = region;
   
   fType = -1;
   
   if (type == "volume") fType = 0;
   else if (type == "medium") fType = 1;
   
   fIsRegionInit = kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdPhysicalRegion::IsInside(Double_t x, Double_t y, Double_t z) const
{
   if (!fIsRegionInit) return kFALSE;
   if (fType == -1) return kTRUE;
   
   if (!fNavigator) return kFALSE;
   
   if (fType == 0) {
       static TString path;
       fNavigator->SetCurrentPoint(x,y,z); 
       path = fNavigator->GetPath();
       return path.Contains(fRegion);
   }
   
   if (fType == 1) {   
       static TGeoNode* cnode;
       static TString mname;
       fNavigator->SetCurrentPoint(x,y,z); 
       cnode = fNavigator->SearchNode();
       if (!cnode) return kFALSE;
       mname = cnode->GetMedium()->GetName();
       return mname.Contains(fRegion);
   }
   
   return kFALSE;
}

//_____________________________________________________________________________
TString SpdPhysicalRegion::GetType() const 
{ 
  switch (fType) {
      case -1: return "undefined";
      case  0: return "volume";
      case  1: return "medium"; 
      default: return "unknown";
  }
  
  return "unknown";
}

//_____________________________________________________________________________
void SpdPhysicalRegion::Print(Option_t* option) 
{
   cout << "\t<SpdPhysicalRegion::Print> Name/Type: " 
        << GetName() << " / " << GetRegionType() << endl;
     
   cout << "\t Region: " << fRegion << endl;
   cout << "\t Type:   " << GetType() << endl;
}

//_____________________________________________________________________________
//
// SpdRegion
//_____________________________________________________________________________


//_____________________________________________________________________________
SpdRegion::SpdRegion():TNamed("Undefined","Undefined"),
fIsRegionInit(kFALSE),fNavigator(0)
{
   if (gGeoManager) fNavigator = gGeoManager->GetCurrentNavigator();
}

//_____________________________________________________________________________
SpdRegion::SpdRegion(const Char_t* name, const Char_t* type):TNamed(name,type),
fIsRegionInit(kFALSE),fNavigator(0)
{
   if (gGeoManager) fNavigator = gGeoManager->GetCurrentNavigator();
}

//_____________________________________________________________________________
TString SpdRegion::GetMedium(Double_t x, Double_t y, Double_t z) const
{
   if (!fNavigator) return "";
   if (!IsInside(x,y,z)) return "";
   
   static TGeoNode* cnode;
   
   fNavigator->SetCurrentPoint(x,y,z); 
   cnode = fNavigator->SearchNode();
   
   return (cnode) ? cnode->GetMedium()->GetName() : "";
}

//_____________________________________________________________________________
TString SpdRegion::GetGeoPath(Double_t x, Double_t y, Double_t z) const
{
   if (!fNavigator) return "";
   if (!IsInside(x,y,z)) return "";
   
   fNavigator->SetCurrentPoint(x,y,z); 
   return fNavigator->GetPath();
}


    

    


