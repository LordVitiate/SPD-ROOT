// $Id$
// Author: artur   2020/02/01

#ifndef __SPDGFTRACKW_H__
#define __SPDGFTRACKW_H__

#include <TObject.h>
#include <Track.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdGFTrackW                                                                //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdGFTrackW: public TObject {

public:

    SpdGFTrackW();
    SpdGFTrackW(genfit::Track* track);
    SpdGFTrackW(genfit::Track* track, Int_t id = -1);
    
    virtual ~SpdGFTrackW();
    
    void Reset();
    void DeleteTrack();
    
    void SetTrack(genfit::Track* track, Int_t id = -1);
    
    void SetTrack(genfit::Track* track);
    void SetDataID(Int_t id) { fDataID = id; }
    
    genfit::Track* GetTrack() { return fTrackGF; }
    Int_t GetDataID() const   { return fDataID; }
    
    genfit::Track* ReplaceTrack(genfit::Track* track);

private:

    genfit::Track* fTrackGF; // ownership
    Int_t          fDataID;

    ClassDef(SpdGFTrackW,1)
};

#endif  /* __SPDGFTRACKW_H__ */

