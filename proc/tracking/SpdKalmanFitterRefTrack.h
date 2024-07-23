/* Copyright 2013, Ludwig-Maximilians Universität München,
   Authors: Tobias Schlüter & Johannes Rauch

   This file is part of GENFIT.

   GENFIT is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   GENFIT is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with GENFIT.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @addtogroup genfit
 * @{
 */

#ifndef __SPDKALMANFITTERREFTRACK_H__
#define __SPDKALMANFITTERREFTRACK_H__

#include "AbsKalmanFitter.h"

using genfit::AbsKalmanFitter;
using genfit::AbsTrackRep;
using genfit::Track;
using genfit::TrackPoint;
using genfit::KalmanFitterInfo;

/**
 * @brief Kalman filter implementation with linearization around a reference track
 */

class SpdKalmanFitterRefTrack : public AbsKalmanFitter {
    
public:
  
   SpdKalmanFitterRefTrack(unsigned int maxIterations = 4, double deltaPval = 1e-3, double blowUpFactor = 1e3,
                       bool squareRootFormalism = false)
    : AbsKalmanFitter(maxIterations, deltaPval, blowUpFactor), refitAll_(false), deltaChi2Ref_(1),
      squareRootFormalism_(squareRootFormalism) 
      {}

  virtual ~SpdKalmanFitterRefTrack() {}

  /** @brief Main loop. 
   */
  void processTrackWithRep(Track* tr, const AbsTrackRep* rep, bool resortHits = false) override;

  /** @brief Prepare the track.
   *
   * Calc all reference states.
   * If setSortingParams is true, the extrapolation lengths will be set as sorting parameters
   * of the TrackPoints.
   * Returns if the track has been changed.
   */
  bool prepareTrack(Track* tr, const AbsTrackRep* rep, bool setSortingParams, int& nFailedHits);

   /** @brief Fit the track.
   *
   * Needs a prepared track! Return last TrackPoint that has been processed.
   */
  TrackPoint* fitTrack(Track* tr, const AbsTrackRep* rep, double& chi2, double& ndf, int direction);
  
  /*********** SETTERS ************/
  
  //! If true always refit all points, otherwise fit points only if reference states have changed
  void setRefitAll(bool refit = true) { refitAll_ = refit; }

  /**
   * When will the reference track be updated?
   * If (smoothedState - referenceState) * smoothedCov^(-1) * (smoothedState - referenceState)^T >= deltaChi2Ref_.
   */
  void setDeltaChi2Ref(double dChi2) { deltaChi2Ref_ = dChi2; }

private:
     
  void processTrackPoint(KalmanFitterInfo* fi, const KalmanFitterInfo* prevFi, const TrackPoint* tp, 
                         double& chi2, double& ndf, int direction);
  
  void processTrackPointSqrt(KalmanFitterInfo* fi, const KalmanFitterInfo* prevFi, const TrackPoint* tp, 
                         double& chi2, double& ndf, int direction);

  /**
   * @brief Remove referenceStates if they are too far from smoothed states.
   *
   * Does NOT remove forward and backward info, but returns from/to where they have to be removed later
   * Return if anything has changed.
   */
  bool removeOutdated(Track* tr, const AbsTrackRep* rep, int& notChangedUntil, int& notChangedFrom);

  //! If refitAll_, remove all information.
  void removeForwardBackwardInfo(Track* tr, const AbsTrackRep* rep, int notChangedUntil, int notChangedFrom) const;

  bool    refitAll_;                 // always refit all points or only if reference states have changed
  double  deltaChi2Ref_;             // reference track update cut
  bool    squareRootFormalism_;      // extrapolation algorithm flag
  
  // aux variables for prepareTrack
  TMatrixD    FTransportMatrix_;     //!
  TMatrixD    BTransportMatrix_;     //!
  TMatrixDSym FNoiseMatrix_;         //!
  TMatrixDSym BNoiseMatrix_;         //!
  TVectorD    forwardDeltaState_;    //!
  TVectorD    backwardDeltaState_;   //!

  // aux variables for processTrackPoint
  TVectorD    p_;                    //! state
  TMatrixDSym C_;                    //! state cov. matrix
  TVectorD    res_;                  //! residual
  TMatrixDSym covSumInv_;            //! [V_k + H_k C_{k|k-1} H_k^T]^(-1)
  TMatrixDSym Rinv_;                 //! matrix needs to calculate chisquare increment

  // aux variables for removeOutdated
  TVectorD resM_;                    //!
     
  ClassDefOverride(SpdKalmanFitterRefTrack,1)

};


#endif //__SPDKALMANFITTERREFTRACK_H__
