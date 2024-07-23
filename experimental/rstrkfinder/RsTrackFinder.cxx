#include "RsTrackFinder.h"
#include "TGeoManager.h"
#include "TClonesArray.h"
#include "TDatabasePDG.h"
#include "TMath.h"

#include <cstdlib>
#include <tuple>

using namespace std;

RsTrackFinder::RsTrackFinder() {
    planeHolder.InitPlanes();
    // Consider setting material effects here
    trkProp.Init(); // Load geometry is loaded here
    trkFitter.Init();
    trkFitter.InitFitter();
    nav = gGeoManager->GetCurrentNavigator();
    pdg = 13;
    verbose_lvl = 0;
    max_point_chisq = 4.;
    min_chi2_not_exclude_point = 1.0;
    trk_quality_par = 0.2;
    // add max_first_point_chisq
    max_missing_hits_in_row = 3;
    max_missing_hits = 5;
    max_trk_candidates_size = 100;
    alpha = 0.1;
}

void RsTrackFinder::SetPdg(int pdg_) {
    pdg = pdg_;
    trkFitter.SetPdg(pdg);
}

// Repack hits
bool RsTrackFinder::ReadHits(const TClonesArray* RsHits) {
    hits_map.clear();

    for (int i=0; i<RsHits->GetEntriesFast(); i++) {
        auto hit = (SpdRsMCHit*) RsHits->At(i);
        int isubs = -1;
        if (hit->IsBarrelHit()){
            isubs = hit->GetSector() + 1;
        } else if (hit->IsEndcapHit()) {
            isubs = hit->GetSector() / 10 - 1; // asssuming 10 and 20, check me
        } else {
            return false;
        }
        int ilayer = hit->GetLayer();
        hits_map[{isubs,ilayer}].push_back(hit);
    }
    return true;
}

/*
   Starts recursive track finding from the last results in tracker,
   returns the longest track.
*/
RsRecTrack RsTrackFinder::ExtrapolateTrack(SpdTrackState* init_trk_state) {
    trk_candidates.clear();

    // Start track finding
    RsRecTrack base_trk;
    base_trk.init_state = *init_trk_state; // save init state

    _ProcessLayers(-1, 1, *init_trk_state, 0, 0, base_trk );

    // Print results
    if (verbose_lvl>0) {
        cout << "Printing results\n";
        for (auto& trk : trk_candidates) {
            cout << "New selected track candidate, npoints = " << trk.points.size() << endl;
            for (auto& point : trk.points) {
                cout << " " << point.is_hit;
            }
            cout << "\n";
            cout << "L = " << trk.length << endl;
            cout << "GetNLastLayer: "  << trk.GetNLastLayer() << endl;
            cout << "GetNEmptyLayers: "  << trk.GetNEmptyLayers()
                 << ", GetNConsecutiveEmptyLayers: " << trk.GetNConsecutiveEmptyLayers() << endl;
        }
    }

    int max_layers = 0;
    for (auto& trk : trk_candidates) {
        if (max_layers < (int)trk.points.size()) max_layers = trk.points.size();
    }
    if (verbose_lvl>0)
        cout << "max_layers=" << max_layers << endl;

    if (max_layers == 0) return {};

    double pmax = 0.;
    RsRecTrack* best_trk = nullptr;


    double best_trk_quality = - max_point_chisq * 20;  // lower boundary for track quality

    for (auto& trk : trk_candidates) {
        if (max_layers == trk.points.size()) {
            double cur_trk_quality = trk.nHits - trk_quality_par * trk.chi2;
            if (cur_trk_quality > best_trk_quality) {
                best_trk_quality = cur_trk_quality;
                best_trk = &trk;
            }
        }
    }

    return *best_trk;
}

// Function to manually reduce energy lose by a fraction "alpha".
// This is to prevent GF from stopping track whose energy losses
// fluctuated to be smaller than avarage/
SpdTrackState RsTrackFinder::_StateWithReducedEnergyLoss(SpdTrackState s0, SpdTrackState s1) {
    // Dirty magic
    static TDatabasePDG* pdgdb = TDatabasePDG::Instance();
    double m = pdgdb->GetParticle(pdg)->Mass();
    double p3_0 = s0.GetMomentum().Mag();
    double p3_1 = s1.GetMomentum().Mag();
    double   e0   = TMath::Sqrt(p3_0*p3_0 + m*m);
    double   e1   = TMath::Sqrt(p3_1*p3_1 + m*m);

    double new_e = e1 + (e0-e1) * alpha;
    double new_p = TMath::Sqrt(new_e*new_e - m*m);

    if (p3_1 == 0) {
        return s1;
    }
    SpdTrackState mod_state = s1;
    s1.SetMomentum( s1.GetMomentum() * (new_p/p3_1) );
    return s1;
}


std::tuple<bool,SpdTrackState> RsTrackFinder::_ExtrapolateToLayer(int isubs, int ilayer,
        SpdTrackState& trklaststate) {
//    cout << "<---------- In RsTrackFinder::ExtrapolateToLayer ------------>" << endl;
//    cout << "isubs=" << isubs << ", ilayer=" << ilayer << endl;
    trkProp.InitTrack( pdg );
    RsPlane plane = planeHolder.GetPlane(isubs, ilayer);

    SpdTrackState trkextstate;
    double ext_dist = trkProp.ExtrapolateToPlane( plane.pos, plane.norm, trklaststate, trkextstate);
    //cout << "ext_dist: " << ext_dist << endl;
    //cout << "    plane pos:\n";
    //plane.pos.Print();
    if (ext_dist < 0) {
        if (verbose_lvl > 0) {
            cout << "ext_dist: " << ext_dist << endl;
        }
        return std::make_tuple(false, trkextstate);
    }

    TVector3 extpos = trkextstate.GetPosition();
    double ndist = plane.norm.Dot( (plane.pos - extpos) );
    if ( std::abs( ndist ) < 1e-8 ) { // able to extrapolate to plane
        // Check node
        TGeoNode* gn = nav->FindNode( extpos.X(), extpos.Y(), extpos.Z() );
        if (!gn) return std::make_tuple(false, trkextstate);
        gn->cd();

        geoParser.ParsePath(nav->GetPath());
        if ( geoParser.GetCurrentPathLevel() < 3 ) return std::make_tuple(false, trkextstate);
        if ( TString(plane.geopath).Contains(geoParser.Path(3)) ) {
            return std::make_tuple(true, trkextstate);
        }
    }
    if (verbose_lvl > 0) {
        cout << "ndist: " << ndist << endl;
    }
    return std::make_tuple(false, trkextstate);
}


//  --------------------- The main recursive part ------------------
/* Recursively extrapolates track, stops it if
   - layer to check > 20,
   - GF extrapolation to the next layer/subs is not possible,
   - to many layer with empty hists or their total number is too large.
   After execution the "trk_candidates" object contains the results.
*/
void RsTrackFinder::_ProcessLayers(
        int isubs,   // subsystem from the previous step of extrapolation
        int ilayer,  // layer to check
        SpdTrackState curTrkState,
        int nEmptylayers,
        int nEmptylayersTot,
        RsRecTrack currrentTrk
        ) {

    if (trk_candidates.size() > max_trk_candidates_size) {
        return;
    }

    if (verbose_lvl>0) {
        cout << "<--------- In ProcessLayers for (isubs,ilayer)=" << isubs << "," << ilayer <<
            "--------->\n";
        cout << "nEmptylayersTot: " << nEmptylayersTot << ", nEmptylayers " << nEmptylayers << endl;
    }

    if (ilayer>20) {
        if (verbose_lvl>0) cout << "Stopping track in layer 20\n";
        trk_candidates.push_back(currrentTrk.CloseTrack());
        return;
    }

    // Find subsystem the track crosses by extrapolation, find neighboring ones.
    // int isubs and layer
    // considered tracks may go only from barrel to endcap
    bool isExtrapolated = false;
    SpdTrackState extState;
    pair<int,int> extrap_plane = {-1,-1};  // subs and layer
    if (isubs == -1) { // no prior infromation where to look for hits
        for (int is=9; is>=0; is--) { // usual ecap1: 0, ecap2: 1, oct+1 for barrel. Check barrel first!
            tie(isExtrapolated, extState) = _ExtrapolateToLayer(is, 1, curTrkState);
            if (isExtrapolated) {
                if (verbose_lvl>0) cout << "Extrapolated to segment " << is << "\n";
                extrap_plane = {is, 1};
                //cout << "The extrapolated stated is\n";
                //extState.Print();
                break;
            }
        }
    } else {
        // - Possibility, that track goes to the different subsystem. -
        // - only barrel -> endcap is considred to be possible -
        // Subsystem from the previous step is known (subs),
        // trying to extrapolate to is or neighboring subsystem
        vector<pair<int,int>> test_planes;
        int i = isubs;  // shortcut

        // The same octant of the RS will always goes first and will be found first!
        if ((isubs>2) && (isubs<9)) {
            test_planes = {{i,ilayer}, {i-1,ilayer}, {i+1,ilayer}};
        } else if (isubs==2) {
            test_planes = {{i,ilayer}, {i+1,ilayer}, {9,ilayer}};
        } else if (isubs==9) {
            test_planes = {{i,ilayer}, {i-1,ilayer}, {2,ilayer}};
        } else test_planes = {{isubs,ilayer}};  // 0, 1 - case of endcaps

        // migration from barrel to endcaps
        if (isubs>1) {
            // The numbering of endcaps is against Z-direction
            if (curTrkState.GetPosition().Z() < -180)  // Z-component of extrapolation might be better
                test_planes.push_back({1,1});
            if (curTrkState.GetPosition().Z() >  180)  // Z-component of extrapolation might be better
                test_planes.push_back({0,1});
        }

        // Select the plane, where track could be extrapolated
        for (auto test_plane : test_planes) {
            if (verbose_lvl>0) {
                cout << "Checking extrapolation to (" << test_plane.first
                    << "," << test_plane.second << ")\n";
            }
            tie(isExtrapolated, extState) = _ExtrapolateToLayer(
                    test_plane.first, test_plane.second, curTrkState);
            if (isExtrapolated) {
                if (verbose_lvl>0) cout << "Extrapolated to subs " << test_plane.first << "\n";
                extrap_plane = test_plane;
                break;
            } else {
                if (verbose_lvl>0) {
                    cout << "Extrapolation failed. ";
                    curTrkState.GetMomentum().Print();
                }
            }
        }
    }

    if (!isExtrapolated) { // not possible to extrapolate to any layer/subs!
        if (verbose_lvl>0) cout << "Stopping track, extrapolation is not possible\n";
        trk_candidates.push_back(currrentTrk.CloseTrack());
        return;
    }

    int is = extrap_plane.first;
    int il = extrap_plane.second;

    // Analyze hits
    double min_chi2 = 999;
    auto& vhits = hits_map[{is,il}];
    for (auto hit : vhits) {
        if (trkFitter.FitPoints({hit}, &curTrkState)) {
            double chi2 = trkFitter.GetChi2();
            min_chi2 = std::min(chi2, min_chi2);
            if (verbose_lvl>0) cout << "Point chi2: " << trkFitter.GetChi2() << "\n";
            // Save first point min chi2 to debug to matching tracks
            if (currrentTrk.points.size() == 0 ) {
                currrentTrk.first_point_chi2 = std::min(chi2, currrentTrk.first_point_chi2);
            }

            if (chi2 < max_point_chisq) {
                if (verbose_lvl>0) cout << "Point ACCEPTED! Layer = " << il << "\n";
                auto upd_trk = currrentTrk;
                upd_trk.AddPoint(
                        RsRecPoint{trkFitter.GetTrackState(), chi2, is, il, true, planeHolder.GetPlane(is, il).norm}
                        );
                //ProcessLayers(is, il+1, trkFitter.GetTrackState(), 0, nEmptylayersTot, upd_trk);
                _ProcessLayers(is, il+1,
                    _StateWithReducedEnergyLoss(curTrkState, trkFitter.GetTrackState()),
                    0, nEmptylayersTot, upd_trk);
            }
        }
    }

    // Extrapolate without hits
    // Stop track, if there are too many empty layers
    if (min_chi2 > min_chi2_not_exclude_point ) { // add variable!
        if ( (nEmptylayersTot+1 <= max_missing_hits) && (nEmptylayers+1 <= max_missing_hits_in_row) ) {
            auto upd_trk = currrentTrk;
            upd_trk.AddPoint(
                    RsRecPoint{extState, 0, is, il, false, planeHolder.GetPlane(is, il).norm}
                    );
            //ProcessLayers(is, il+1, extState, nEmptylayers+1, nEmptylayersTot+1, upd_trk);
            _ProcessLayers(is, il+1,
                    _StateWithReducedEnergyLoss(curTrkState, extState),
                    nEmptylayers+1, nEmptylayersTot+1, upd_trk);
        } else {
            if (verbose_lvl>0) {
                cout << "Stopping track, nEmptylayersTot=" << nEmptylayersTot <<
                    "nEmptylayers=" << nEmptylayers+1 << "\n";
            }
            trk_candidates.push_back(currrentTrk.CloseTrack());
        }
    }

}

ClassImp(RsTrackFinder)
