
#include "SpdSttHitInfo.h"

#include <iostream>

using std::cout;
using std::endl;

//_____________________________________________________________________________________
SpdSttHitInfo::SpdSttHitInfo() :
fFileID(-1), fEventID(-1), 
fTrackID(-1), fPointID(-1), 
fIsFake(kFALSE)
{
}

//_____________________________________________________________________________________
SpdSttHitInfo::SpdSttHitInfo(Int_t fileID, Int_t eventID,
                             Int_t trackID, Int_t pointID,
                             Bool_t isFake) :
fFileID(fileID), fEventID(eventID), 
fTrackID(trackID), fPointID(pointID), 
fIsFake(isFake)
{
}

//_____________________________________________________________________________________
void SpdSttHitInfo::Clear() 
{
  
  fFileID  = -1;
  fEventID = -1;
  fTrackID = -1;
  fPointID = -1;
  fIsFake  = kFALSE;
}

//_____________________________________________________________________________________
void SpdSttHitInfo::Print(const Option_t* opt) const
{
  cout << "<SpdSttHitInfo::Print>" 
       << " File: " << fFileID << " Event: " << fEventID
       << " Track: " << fTrackID << " Point: " << fPointID << endl;
}

ClassImp(SpdSttHitInfo)




