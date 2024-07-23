// $Id$
// Author: artur   2020/02/01

//_____________________________________________________________________________
//
// SpdGFTrack
//_____________________________________________________________________________

#include "SpdGFTrackW.h"
#include "TrackPoint.h"
#include <iostream>

using std::cout;
using std::endl;

using namespace genfit;

ClassImp(SpdGFTrackW)

//_____________________________________________________________________________
SpdGFTrackW::SpdGFTrackW():fTrackGF(0),fDataID(-1)
{

}

//_____________________________________________________________________________
SpdGFTrackW::SpdGFTrackW(Track* track):fTrackGF(track),fDataID(-1)
{

}

//_____________________________________________________________________________
SpdGFTrackW::SpdGFTrackW(Track* track, Int_t id):fTrackGF(track),fDataID(id)
{

}

//_____________________________________________________________________________
SpdGFTrackW::~SpdGFTrackW() 
{
   DeleteTrack();
}

//_____________________________________________________________________________
void SpdGFTrackW::Reset()
{
    DeleteTrack();
    fDataID = -1;
}

//_____________________________________________________________________________
void SpdGFTrackW::DeleteTrack()
{
   if (fTrackGF) {
       delete fTrackGF; 
       fTrackGF = 0;
   }
}

//_____________________________________________________________________________
void SpdGFTrackW::SetTrack(Track* track, Int_t dataid)
{
   DeleteTrack();
   fTrackGF = track;
   fDataID  = -1;
}

//_____________________________________________________________________________
void SpdGFTrackW::SetTrack(Track* track)
{
   DeleteTrack();
   fTrackGF = track;
}

//_____________________________________________________________________________
Track* SpdGFTrackW::ReplaceTrack(Track* track)
{
   Track* t = fTrackGF;
   fTrackGF = track;
   return t;
}


