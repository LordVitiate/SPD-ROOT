// $Id$
// Author: artur   2018/08/27

#ifndef __SPDMCTRACKS_H__
#define __SPDMCTRACKS_H__

#include "FairMCTracks.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMCTracks                                                                //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TGeoTrack;

class SpdMCTracks: public FairMCTracks {

public:

    SpdMCTracks(const char* name = "GeoTracks", Int_t iVerbose = 1);
    
    virtual ~SpdMCTracks();
    
    virtual InitStatus Init();
    virtual void Exec(Option_t* option);
    
    virtual Bool_t AcceptTrack(TGeoTrack* track);
    virtual Bool_t DeactivateTrack(TParticle* particle);
   
private:
    
    void           ResetTracks();
    TEveTrackList* GetTracksGroup(TParticle* P); 
    
    ClassDef(SpdMCTracks,1)
};

#endif  /* __SPDMCTRACKS_H__ */

