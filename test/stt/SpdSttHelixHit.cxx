
#include "SpdSttHelixHit.h"
#include "SpdSttHit.h"

#include "TArrayI.h"

//#include "SpdDetectorList.h"
//#include <iostream>

using std::cout;
using std::endl;

using namespace std;

//_____________________________________________________________________________________
SpdSttHelixHit::SpdSttHelixHit() :

 fDetectorID(-1),
 fSectionID(0), 
 fRadial(0), fRadialErr(0), 
 fDeposit(0),
 fIsFake(kFALSE)
{
  fHitsID.Set(1);
  fHitsID[0] = -1;
}

//_____________________________________________________________________________________
SpdSttHelixHit::SpdSttHelixHit(Int_t hitID, SpdSttHit* hit, Bool_t addRadials) :

 fDetectorID(-1),
 fSectionID(0), 
 fRadial(0), fRadialErr(0), 
 fDeposit(0),
 fIsFake(kFALSE)
{
  fHitsID.Set(1);
  fHitsID[0] = hitID;
  
  if (!hit) return;
  
  fDetectorID = hit->DetectorID();
  fSectionID  = hit->SectionID();
  fDeposit    = hit->Deposit();
  
  fDetPos.SetXYZ(hit->GetX(),hit->GetY(),hit->GetZ());
  hit->PositionError(fDetPosErr);
  
  if (!addRadials) return;
  
  fRadial = hit->Radial();
  fRadialErr = hit->RadialError();
}

//_____________________________________________________________________________________
SpdSttHelixHit::SpdSttHelixHit(Int_t detID, Int_t secID, Int_t hitID, 
                               TVector3& pos, TVector3& dpos, 
                               Double_t radial, Double_t radialErr,
                               Double_t deposit) : 
                     
 fDetectorID(detID),
 fSectionID(secID), 
 fRadial(radial), fRadialErr(radialErr), 
 fDeposit(deposit),
 fIsFake(kFALSE)
{
  fHitsID.Set(1);
  fHitsID[0] = hitID;
  
  fDetPos = pos;
  fDetPosErr = dpos;
  
}

//_____________________________________________________________________________________
void SpdSttHelixHit::Clear() 
{
   fHitsID.Set(1); 
   fHitsID[0] = -1;
  
   fDetectorID = -1;
   fSectionID = 0;    
  
   fDetPos.SetXYZ(0,0,0);    
   fDetPosErr.SetXYZ(0,0,0); 
  
   fRadial    = 0.;
   fRadialErr = 0.;
  
   fDeposit   = 0.;
   
   fIsFake = kFALSE;
}

//_____________________________________________________________________________________
Bool_t SpdSttHelixHit::AddDeposit(Int_t hitID, SpdSttHit* hit) 
{
   if (!Equal(hit)) return kFALSE; 
   fDeposit += hit->Deposit();
   if (FindHit(hitID)) return kFALSE; 
   Int_t n = fHitsID.GetSize();
   fHitsID.Set(n+1);
   fHitsID.fArray[n] = hitID;
   return kTRUE;
}

//_____________________________________________________________________________________
Bool_t SpdSttHelixHit::AddDeposit(Int_t hitID, Double_t dep) 
{
   fDeposit += dep;
   if (FindHit(hitID)) return kFALSE; 
   Int_t n = fHitsID.GetSize();
   fHitsID.Set(n+1);
   fHitsID.fArray[n] = hitID;
   return kTRUE;
}

//_____________________________________________________________________________________
Bool_t SpdSttHelixHit::Equal(SpdSttHit* hit) const 
{
   return (fDetectorID != hit->DetectorID()) ? kFALSE 
           :(fSectionID != hit->SectionID()) ? kFALSE 
           : kTRUE; 
}

//_____________________________________________________________________________________
Bool_t SpdSttHelixHit::FindHit(Int_t hitID) 
{
   Int_t n = fHitsID.GetSize();
   Int_t* val = fHitsID.GetArray();
   
   if (n == 1) { return (hitID == val[0]) ? kTRUE : kFALSE; }
   while (n > 0) { if (hitID == val[--n]) return kTRUE; }
   
   return kFALSE;
}

//_____________________________________________________________________________________
void SpdSttHelixHit::Print(const Option_t* opt) const 
{
   cout << "<SpdSttHelixHit::Print>" 
        << " Tube: " << DetectorID() << " Section: " << SectionID() 
        << " Hits: " << NHits()  << " R: " << Radial() << " Deposit: " << Deposit()
        << endl;
}

ClassImp(SpdSttHelixHit)



