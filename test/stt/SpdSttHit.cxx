
#include "SpdSttHit.h"

//#include "SpdSttPoint.h"
//#include "SpdDetectorList.h"
//#include <iostream>

using std::cout;
using std::endl;

//_____________________________________________________________________________________
SpdSttHit::SpdSttHit() :
 FairHit(), 
 fSectionID(0), 
 fRadial(0), fRadialErr(0), 
 fDeposit(0)
{
  SetTimeStamp(0.);   
  SetTimeStampError(0.);
}

//_____________________________________________________________________________________
SpdSttHit::SpdSttHit(Int_t detID, Int_t secID, Int_t mcindex, 
                     TVector3& pos, TVector3& dpos, 
                     Double_t radial, Double_t radialErr, 
                     Double_t htime, Double_t htimeErr, 
                     Double_t deposit) : 
                     
 FairHit(detID, pos, dpos, mcindex),
 
 fSectionID(secID), 
 fRadial(radial), fRadialErr(radialErr), 
 fDeposit(deposit)
 
{
  SetTimeStamp(htime);
  SetTimeStampError(htimeErr);
  
  SetLink(FairLink("SpdSttPoint",mcindex));
}

//_____________________________________________________________________________________
void SpdSttHit::Clear() 
{
  // from FairHit
  SetDetectorID(-1);  // Tube ID
  SetRefIndex(-1);    // mc-point ID
  SetXYZ(0,0,0);      // tube position
  SetPositionError(TVector3(0,0,0)); // error tube position
  
  fSectionID = 0;
  fRadial    = 0.;
  fRadialErr = 0.;
  fDeposit   = 0.;
  
  // from FairTimeStamp
  SetTimeStamp(0.);   // hit time
  SetTimeStampError(0.); // hit time error
}

//_____________________________________________________________________________________
Bool_t SpdSttHit::Equal(FairHit* data) const
{  
  SpdSttHit* hit = dynamic_cast<SpdSttHit*>(data);
  if (!hit) return false; 
  if (DetectorID() == hit->DetectorID() && 
      SectionID() == hit->SectionID()) return true;
}

//_____________________________________________________________________________________
void SpdSttHit::Print(const Option_t* opt) const 
{
   cout << "<SpdSttHit::Print>" 
        << " Tube: " << DetectorID() << " Section: " << SectionID() 
        << " R: " << Radial() << " Deposit: " << Deposit()
        << " Time: " << HitTime()  
        
        << endl;
}

ClassImp(SpdSttHit)


