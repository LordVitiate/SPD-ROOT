#ifndef RsRecTrack_h
#define RsRecTrack_h

#include "SpdTrackState.h"

#include <vector>

struct RsRecPoint {
    SpdTrackState trk_state;
    double   chi2;
    int      isubs;
    int      ilayer;
    bool     is_hit;  // whether it is RsHit or just an intermediate extrapolation point
    TVector3 plane_norm;

    RsRecPoint() = default;

    // For stupid ROOT
    RsRecPoint(SpdTrackState _state, double _chi2, int _isubs, int _ilayer, bool _is_hit, TVector3 _norm);

    ClassDef(RsRecPoint,1)
};

struct RsRecTrack {
    std::vector<RsRecPoint> points;
    SpdTrackState init_state;

    double length;
    double chi2;
    int    nHits;
    double first_point_chi2;


    RsRecTrack();
    void   AddPoint(const RsRecPoint& point);

    // Remove auxiliary points from extrapolation at the end of the tracks
    // and calculate length, chi2, nHits/
    RsRecTrack& CloseTrack();
    // Function called in CloseTrack(), "private"
    int    _GetNRealHitPoints();
    double _GetChi2();
    double _GetLength();

   // Functions to provide additional track characteristics
    int GetNLastLayer();
    int GetNEmptyLayers();
    int GetNConsecutiveEmptyLayers();
    ClassDef(RsRecTrack,1)
};

#endif
