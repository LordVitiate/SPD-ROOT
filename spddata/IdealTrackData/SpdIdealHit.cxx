// $Id$
// Author: artur   2019/09/06

//_____________________________________________________________________________
//
// SpdIdealHit
//_____________________________________________________________________________

#include "SpdIdealHit.h"

#include <iostream>

using std::cout;
using std::endl;

#define IDSIZE 3

ClassImp(SpdIdealHit)

//_____________________________________________________________________________
SpdIdealHit::SpdIdealHit():FairTimeStamp(),fSpecifity('n'),
fEdep(0.),fId(-1),fInfoNum(-1),fMotherPointNum(-1),fResolution(-1,-1,-1)
{
  fDetId.assign(IDSIZE,0);
}

//_____________________________________________________________________________
SpdIdealHit::~SpdIdealHit() 
{
   
}

//_____________________________________________________________________________
void SpdIdealHit::Clear()
{
  fEdep = 0.;
  
  fTimeStamp = -1.;
  fTimeStampError = -1.;
  
  fId = -1;
  fInfoNum  = -1;
  fMotherPointNum = -1;
  
  fTrackId.clear();
  
  fDetId.assign(IDSIZE,0);
  
  fResolution.SetXYZ(-1,-1,-1);
}

//_____________________________________________________________________________
void SpdIdealHit::SetDetId(Int_t i, Long_t id)
{
   if (i < 0) return;
    
   if (i < IDSIZE) fDetId[i] = id;
   else {
       fDetId.resize(i+1,0);
       fDetId[i] = id;
   }
}

//_____________________________________________________________________________
Long_t SpdIdealHit::GetDetId(Int_t i) const
{
   if (i < 0) return Int_t(kSpdUndefined);
   if (i < IDSIZE) return fDetId[i];
   if (i < fDetId.size()) return fDetId[i];
   return Int_t(kSpdUndefined);
}

//_____________________________________________________________________________
void SpdIdealHit::PrintHit(Bool_t print_point, Bool_t print_res) const
{
   cout << "\t<" << this->ClassName() << "::PrintHit>" << "\n\n";
   cout << "\tHit unique ID:       " << fId << endl;
   cout << "\tDetector id:         "; for (Int_t i(0); i<fDetId.size(); i++) { cout << fDetId[i] << " "; } cout << endl;
   cout << "\tTrack id:            "; for (Int_t i(0); i<fTrackId.size(); i++) { cout << fTrackId[i] << " "; } cout << endl;
   cout << "\tInfo, mcpoint:       " << fInfoNum << ", " << fMotherPointNum << endl;
   cout << "\tEdep, Time:          " << fEdep << " [GeV],  " << fTimeStamp << " [ns] " << endl;
   cout << "\tSpecifity:           " << fSpecifity << endl;

   if (print_point) {
       cout << "\tPoint(x,y,z):        " 
            << fPoint.X() << ", " << fPoint.Y() << ", " << fPoint.Z() << " [cm] " << endl;
   }
   if (print_res) {
       cout << "\tResolution:        " 
            << fResolution.X() << ", " << fResolution.Y() << ", " << fResolution.Z() << " [cm] " << endl;
   }
   cout << "\n";
}


