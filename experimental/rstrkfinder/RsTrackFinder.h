#ifndef RsTrackFinder_h
#define RsTrackFinder_h

#include "SpdTrackPropagatorGF.h"
#include "SpdGeopathParser.h"
#include "TGeoNavigator.h"
#include "TMath.h"
#include "Math/PdfFuncMathCore.h"

#include "SpdRsMCHit.h"
#include "SpdTrackState.h"

#include "RsTrackFitterGF.h"
#include "RsPlaneHolder.h"
#include "RsRecTrack.h"


#include <vector>
#include <map>
#include <tuple>
#include <algorithm>


struct RsTrackFinder {

    // Algo parameters
    int verbose_lvl;
    double max_point_chisq;            // threshold value to accept point
    double min_chi2_not_exclude_point; // a branch with extrapolation point instead
                                       // of hit is not spawned if there is a point
                                       // with chi2 below this value.
    int    max_missing_hits;
    int    max_missing_hits_in_row;
    double trk_quality_par;            // quality = nhits - trk_quality_par*chi2
    int    max_trk_candidates_size;
    double alpha;                      // reduces energy losses by fraction alpha

    // Internal variables
    std::vector<RsRecTrack> trk_candidates;  // tracks
    std::map<std::pair<int,int>, std::vector<SpdRsMCHit*>> hits_map;
    int pdg;

    SpdTrackPropagatorGF trkProp;
    RsTrackFitterGF      trkFitter;
    RsPlaneHolder        planeHolder;
    SpdGeopathParser     geoParser;
    TGeoNavigator*       nav;

    RsTrackFinder();

    // Lowers energy losses for particle to account for energy loss
    // fluctuations.
    SpdTrackState _StateWithReducedEnergyLoss(SpdTrackState s0, SpdTrackState s1);

    // The core recursive function, "private"
    void _ProcessLayers(
            int isubs,
            int ilayer,  // start layer
            SpdTrackState curTrkState,
            int nEmptylayers,
            int nEmptylayersTot,
            RsRecTrack currrentTrk
            );

    // Extrapolation to layer, "private"
    std::tuple<bool,SpdTrackState> _ExtrapolateToLayer(int isubs, int ilayer,
            SpdTrackState& trklaststate);


    // The functions below define user interface
    bool ReadHits(const TClonesArray* RsHits);

    void SetPdg(int pdg_);

    RsRecTrack ExtrapolateTrack(SpdTrackState* init_trk_state);


    ClassDef(RsTrackFinder,1)
};

#endif
