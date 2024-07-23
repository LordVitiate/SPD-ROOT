#include "RsRecTrack.h"

#include "TMath.h"

#include "SpdTrackState.h"

#include <vector>

// ******** RsRecPoint ********

// For stupid ROOT
RsRecPoint:: RsRecPoint(SpdTrackState _state, double _chi2, int _isubs, int _ilayer, bool _is_hit, TVector3 _norm) {
       trk_state  = _state;
       chi2       = _chi2;
       isubs      = _isubs;
       ilayer     = _ilayer;
       is_hit     = _is_hit;
       plane_norm = _norm;
}

ClassImp(RsRecPoint)

// ******** RsRecPoint ********

RsRecTrack::RsRecTrack() {
        length = 0;
        nHits = 0;
        chi2 = 999;  // not used?
        first_point_chi2 = 999999;
}

void RsRecTrack::AddPoint(const RsRecPoint& point) {
    points.push_back(point);
}

int RsRecTrack::_GetNRealHitPoints() {
    int n = 0;
    for (auto& point : points) {
        if (point.is_hit) {
            n++;
        }
    }
    return n;
}

double RsRecTrack::_GetChi2() {
    double chi2_ = 0;
    for (auto& point : points) {
        if (point.is_hit) {
            chi2_ += point.chi2;
        }
    }
    return chi2_;
}

double RsRecTrack::_GetLength() {
    TVector3 prev_pos;
    int      prev_subs;
    double   L = 0;
    bool isfirst = true;
    for(const auto& point : points) {
        if (isfirst) {
            isfirst = false;
            prev_pos = point.trk_state.GetPosition();
            prev_subs = point.isubs;
        } else {
            auto pos = point.trk_state.GetPosition();
            int subs = point.isubs;
            TVector3 norm = point.plane_norm;

            if (subs == prev_subs) {
                TVector3 d = pos - prev_pos;
                double alpha = d.Angle(norm);
                L += 3 / TMath::Cos(alpha);  // 3 is the iron width
            } else if ( (subs < 2) && (prev_subs>1) ) {  // barrel -> endcap track
                TVector3 d = point.trk_state.GetMomentum();
                double alpha = d.Angle(norm);
                L += 6 / TMath::Cos(alpha);  // 6 outer iron layer
            } // other cases are ignored!
        }
    }
    return L;
}


// Remove auxiliary points from extrapolation at the end of the tracl
RsRecTrack& RsRecTrack::CloseTrack() { // -> FinalizeTrack
    while ( (!points.empty()) && (!points.back().is_hit) ) {
        points.pop_back();
    }
    // May be calculate all the quantitites here and put results to class fields??

    length = _GetLength();
    chi2   = _GetChi2();
    nHits  = _GetNRealHitPoints();

    return *this;
}

int RsRecTrack::GetNLastLayer() {
    int res = -1;
    for(const auto& point : points) {
        if (point.ilayer > res)
        res = point.ilayer;
    }
    return res;
}

int RsRecTrack::GetNEmptyLayers() {
    int n_empty = 0;
    for (const auto& point : points) {
        if (!point.is_hit)
        n_empty++;
    }
    return n_empty;
}

int RsRecTrack::GetNConsecutiveEmptyLayers() {
    int max_chunk = 0;
    auto istart = points.begin();
    auto iend   = points.end(); // init value irrerelevant
    while (true) {
        istart = std::find_if(istart, points.end(),
            [](const RsRecPoint& point) { return !point.is_hit;});
        if (istart == points.end()) break;

        iend = std::find_if(istart, points.end(),
            [](const RsRecPoint& point) { return point.is_hit;});
        int cur_chunk = iend - istart;
        max_chunk = std::max(max_chunk, cur_chunk);
        istart = iend;
    }
    return max_chunk;
}

ClassImp(RsRecTrack)
