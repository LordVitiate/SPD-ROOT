// $Id$
// Author: artur   2018/01/30


//_____________________________________________________________________________
//
// SpdGeoFrame
//_____________________________________________________________________________

#include "SpdGeoFrame.h"
#include "SpdCommonGeoMapper.h"

#include <TGeoManager.h>

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdGeoFrame)

//_____________________________________________________________________________
SpdGeoFrame::SpdGeoFrame():
SpdPassiveModule("Frame (tor)","SPDTORFRAME",kSpdTorFrame),
fLockGeom(kFALSE)
{
   fGeoType = 1;
}

//_____________________________________________________________________________
SpdGeoFrame::SpdGeoFrame(const char* Name, const char* Prefix):
SpdPassiveModule(Name,Prefix,kSpdTorFrame),
fLockGeom(kFALSE)
{
   fGeoType = 1;
}

//_____________________________________________________________________________
SpdGeoFrame::~SpdGeoFrame() 
{
 
}

//_____________________________________________________________________________________
void SpdGeoFrame::ConstructGeometry()
{
   if (fLockGeom) {
       cout << "-E- <SpdGeoFrame::ConstructGeometry> "
            << " Geometry is already built " << endl;  
       return;
   }
   
   fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();
   
   if (!fMasterVolume) {  
       cout << "-E- <SpdGeoFrame::ConstructGeometry> No MASTER volume " << endl;
       return;
   }
   
   fLockGeom = kTRUE;
}

//_____________________________________________________________________________
Double_t SpdGeoFrame::GetCapacity() const
{
  
}
   
//_____________________________________________________________________________
Double_t SpdGeoFrame::GetMass() const
{
  
}

//_____________________________________________________________________________________
void SpdGeoFrame::UnsetMaterials(TString media)
{
   fUnsetMedia = media;
}

//_____________________________________________________________________________________
void SpdGeoFrame::Print(Option_t*) const
{
   cout << "<SpdGeoFrame::Print>" << endl;
   
   cout <<"\n";
}

//_____________________________________________________________________________
Bool_t SpdGeoFrame::FillParsIn(SpdPassiveGeoParSet* params)
{
   if (!SpdPassiveModule::FillParsIn(params)) return kFALSE;
   return kTRUE;
}

//_____________________________________________________________________________    
Bool_t SpdGeoFrame::LoadParsFrom(SpdPassiveGeoParSet* params)
{
   if (!SpdPassiveModule::LoadParsFrom(params)) return kFALSE;
   return kTRUE;
}


