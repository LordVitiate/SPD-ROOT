/* Copyright 2013, Ludwig-Maximilians Universität München, Technische Universität München
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

/* This implements the Kalman fitter with reference track.  */

#include "Tools.h"
#include "Track.h"
#include "TrackPoint.h"
#include "Exception.h"
#include "IO.h"

#include "SpdKalmanFitterRefTrack.h"
#include "KalmanFitterInfo.h"
#include "KalmanFitStatus.h"

#include <TDecompChol.h>
#include <Math/ProbFunc.h>

#include <iostream>
using std::cout;
using std::endl;

using namespace genfit;

//______________________________________________________________________________________________________________________
TrackPoint* SpdKalmanFitterRefTrack::fitTrack(Track* tr, const AbsTrackRep* rep, double& chi2, double& ndf, int direction)
{
  //cout << "\n<SpdKalmanFitterRefTrack::fitTrack> *************** START FIT TRACK *************** " << "\n\n";
     
  unsigned int dim = rep->getDim();

  chi2 = 0;
  ndf = -1. * dim;
  KalmanFitterInfo* prevFi(nullptr);

  TrackPoint* retVal(nullptr);
  
  bool alreadyFitted(!refitAll_);

  p_.ResizeTo(dim);
  C_.ResizeTo(dim, dim);

  for (size_t i = 0; i < tr->getNumPointsWithMeasurement(); ++i) {
       
       TrackPoint *tp = 0;
       
       // check fit direction
       assert(direction == +1 || direction == -1);
       if (direction == +1) tp = tr->getPointWithMeasurement(i);
       else if (direction == -1) tp = tr->getPointWithMeasurement(-i-1);

       // search for FitterInfo
       if (!tp->hasFitterInfo(rep)) continue;
       KalmanFitterInfo* fi = static_cast<KalmanFitterInfo*>(tp->getFitterInfo(rep));

       //cout << "\t <SpdKalmanFitterRefTrack::fitTrack> BEFORE fit, point : " << i 
       //     << "; is already fitted: " << alreadyFitted << "\n\n";
       //tp->PrintInfoShort(); // FIXME!!!// FIXME!!!// FIXME!!!

       if (alreadyFitted && fi->hasUpdate(direction)) {
          
           prevFi = fi;
          
           chi2 += fi->getUpdate(direction)->getChiSquareIncrement();
           ndf += fi->getUpdate(direction)->getNdf();
          
           continue;
       }

       alreadyFitted = false;
  
       // fit point 
       processTrackPoint(fi, prevFi, tp, chi2, ndf, direction);
       
       //cout << "\t <SpdKalmanFitterRefTrack::fitTrack> AFTER fit, point : " << i << "\n\n";
       //tp->PrintInfoShort(); // FIXME!!!// FIXME!!!// FIXME!!!
       
       retVal = tp;

       prevFi = fi;
  }
 
  //cout << "\n<SpdKalmanFitterRefTrack::fitTrack> *************** FINISH FIT TRACK *************** " << "\n\n";
 
  return retVal;
}

//______________________________________________________________________________________________________________________
void SpdKalmanFitterRefTrack::processTrackWithRep(Track* tr, const AbsTrackRep* rep, bool resortHits)
{
  if (tr->hasFitStatus(rep) && tr->getFitStatus(rep)->isTrackPruned()) {
      Exception exc("SpdKalmanFitterRefTrack::processTrack: Cannot process pruned track!", __LINE__,__FILE__);
      throw exc;
  }
  
  cout << "\nSpdKalmanFitterRefTrack::processTrackWithRep (1)  ========================= START  ITERATIONS =========================== " << endl;
  
  double oldChi2FW = 1e6;
  double oldPvalFW = 0.;
  double oldChi2BW = 1e6;
  double oldPvalBW = 0.;
  double chi2FW(0), ndfFW(0);
  double chi2BW(0), ndfBW(0);
  int    nFailedHits(0);

  KalmanFitStatus* status = new KalmanFitStatus();
  tr->setFitStatus(status, rep);
  
  cout << "<SpdKalmanFitterRefTrack::processTrackWithRep> START ITERATIONS. TOTAL =  " << maxIterations_ << endl;
  
  status->setIsFittedWithReferenceTrack(true);

  unsigned int nIt = 0;
  
  // main loop (fit iterations)
  for (;;) {
      
    cout << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl; 
    cout << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl; 
    cout << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl; 
    
    cout << " <SpdKalmanFitterRefTrack::processTrackWithRep> (2.0) START ITERATION = " << nIt+1 
         << " FOR TRACK " << tr->getIdForRep(rep) << "\n " << endl;

    try {
        
      cout << "<SpdKalmanFitterRefTrack::processTrackWithRep> (2.1) PREPARE TRACK. "
           << "resort hits = " << resortHits << "; track rep. = " << rep->ClassName() << endl;
    
      // prepare track
      if (!prepareTrack(tr, rep, resortHits, nFailedHits) && !refitAll_) 
      {
          cout << "<SpdKalmanFitterRefTrack::processTrackWithRep> (2.1.1) REFRENCE HAS NOT BEEN CHANGED, FILL STATUS AND RETURN " << endl;
        
          status->setIsFitted();

          status->setIsFitConvergedPartially();
          
          if (nFailedHits == 0) status->setIsFitConvergedFully();
          else status->setIsFitConvergedFully(false);

          status->setNFailedPoints(nFailedHits);

          status->setHasTrackChanged(false);
          status->setCharge(rep->getCharge(*static_cast<KalmanFitterInfo*>(
                            tr->getPointWithMeasurement(0)->getFitterInfo(rep))->getBackwardUpdate())
                           );
          status->setNumIterations(nIt);
          status->setForwardChi2(chi2FW);
          status->setBackwardChi2(chi2BW);
          status->setForwardNdf(std::max(0., ndfFW));
          status->setBackwardNdf(std::max(0., ndfBW));
          
          return;
      }

      cout << "<SpdKalmanFitterRefTrack::processTrackWithRep> (2.2) TRACK HAS BEEN PREPARED, "
           << "failed hits = " << nFailedHits << " resort hits = " << resortHits << endl;

      // resort points
      if (resortHits) 
      {
          if (tr->sort()) { // re-prepare if order of hits has changed!
              prepareTrack(tr, rep, resortHits, nFailedHits);
              status->setNFailedPoints(nFailedHits);
          }
      }

      cout << "\n<SpdKalmanFitterRefTrack::processTrackWithRep> (2.3) >>>>>>>>>>>>>>>>>> FIT TRACK FORWARD >>>>>>>>>>>>>>>>>>> " << endl;

      TrackPoint* lastProcessedPoint = fitTrack(tr, rep, chi2FW, ndfFW, +1);

      // fit backward (must not necessarily start at last hit, set prediction = forward update and blow up cov)

      cout << "<SpdKalmanFitterRefTrack::processTrackWithRep> FIT FORWARD; LAST POINT: " << lastProcessedPoint << endl;
      
      if (lastProcessedPoint != nullptr) {
          
          KalmanFitterInfo* lastInfo = static_cast<KalmanFitterInfo*>(lastProcessedPoint->getFitterInfo(rep));
          
          if (!lastInfo->hasBackwardPrediction()) {
          
              lastInfo->setBackwardPrediction(new MeasuredStateOnPlane(*(lastInfo->getForwardUpdate())));
              lastInfo->getBackwardPrediction()->blowUpCov(blowUpFactor_, resetOffDiagonals_, blowUpMaxVal_);  // blow up cov
              
              cout << "<SpdKalmanFitterRefTrack::processTrackWithRep> SET BACKWARD PREDICTION (FROM FW.UPDATE) FOR THE LAST POINT " << endl;
          }
      }
    
      cout << "<SpdKalmanFitterRefTrack::processTrackWithRep> (2.4) >>>>>>>>>>>>>>>>>> FIT TRACK FORWARD (FINISHED) >>>>>>>>> " << endl;
      
      cout << "\n<SpdKalmanFitterRefTrack::processTrackWithRep> (2.5) <<<<<<<<<<<<<<<<<< FIT TRACK BACKWARD <<<<<<<<<<<<<<<<<<< " << endl;

      lastProcessedPoint = fitTrack(tr, rep, chi2BW, ndfBW, -1);
      
      cout << "<SpdKalmanFitterRefTrack::processTrackWithRep> FIT BACKWARD; LAST POINT:  " << lastProcessedPoint << endl;
      
      cout << "<SpdKalmanFitterRefTrack::processTrackWithRep> (2.6) <<<<<<<<<<<<<<<<<< FIT TRACK BACKWARD (FINISHED) <<<<<<<< " << endl;
      
      ++nIt;
    
      double PvalBW = std::max(0.,ROOT::Math::chisquared_cdf_c(chi2BW, ndfBW));
      
      // Don't calculate if not debugging as this function potentially takes a lot of time.
      // double PvalFW = (debugLvl_ > 0) ? std::max(0.,ROOT::Math::chisquared_cdf_c(chi2FW, ndfFW)) : 0; 
      //
      // if (debugLvl_ > 0) {
      //     debugOut << "SpdKalmanFitterRefTrack::Track after fit:"; tr->Print("C");
      //
      //     debugOut << "old chi2s: " << oldChi2BW << ", " << oldChi2FW
      //             << " new chi2s: " << chi2BW << ", " << chi2FW << std::endl;
      //     debugOut << "old pVals: " << oldPvalBW << ", " << oldPvalFW
      //             << " new pVals: " << PvalBW << ", " << PvalFW << std::endl;
      // }

      // See if p-value only changed little. If the initial
      // parameters are very far off, initial chi^2 and the chi^2
      // after the first iteration will be both very close to zero, so
      // we need to have at least two iterations here. Convergence
      // doesn't make much sense before running twice anyway.
      
      bool converged(false);
      bool finished(false);
      
      if (nIt >= minIterations_ && fabs(oldPvalBW - PvalBW) < deltaPval_)  
      {
          // if pVal ~ 0, check if chi2 has changed significantly
          if (fabs(1 - fabs(oldChi2BW/chi2BW)) > relChi2Change_) finished = false;
          else {
              finished  = true;
              converged = true;
          }
          if (PvalBW == 0.) converged = false;
      }

      cout << "\n<SpdKalmanFitterRefTrack::processTrackWithRep> (2.7) end iteration " << nIt   
           << "; fit: converged = " << converged << " " << "finished = " << finished 
           << " failed hits: " << nFailedHits << " points: " << tr->getNumPoints() << endl;

      if (finished) 
      {
          if (nFailedHits == 0) status->setIsFitConvergedFully(converged);
          else status->setIsFitConvergedFully(false);

          status->setIsFitConvergedPartially(converged);
          status->setNFailedPoints(nFailedHits);
          
          cout << "<SpdKalmanFitterRefTrack::processTrackWithRep> (2.8) finished iteration [1] (fihished): " 
               << nIt << " / " << maxIterations_ << endl;

          break;
      }
      else 
      {
          oldPvalBW = PvalBW;
          oldChi2BW = chi2BW;
      }

      cout << "<SpdKalmanFitterRefTrack::processTrackWithRep> (2.8) finished iteration [2]: " 
           << nIt << " / " << maxIterations_ << endl;

      if (nIt >= maxIterations_) break;
    }
    catch (Exception& e) {

      errorOut << e.what();
         
      status->setIsFitted(false);
      status->setIsFitConvergedFully(false);
      status->setIsFitConvergedPartially(false);
      status->setNFailedPoints(nFailedHits);

      return;
    }
  }
  
  cout << "\n<SpdKalmanFitterRefTrack::processTrackWithRep> (3) ========================= FINISH ITERATIONS =========================== " << endl;

  // fill FitStatus
  
  TrackPoint* tp = tr->getPointWithMeasurementAndFitterInfo(0, rep);
 
  double charge(0);
  if (tp != nullptr) {
      if (static_cast<KalmanFitterInfo*>(tp->getFitterInfo(rep))->hasBackwardUpdate()) {
          charge = static_cast<KalmanFitterInfo*>(tp->getFitterInfo(rep))->getBackwardUpdate()->getCharge();
      }
  }
  status->setCharge(charge);

  if (tp != nullptr) status->setIsFitted();
  else { 
      // none of the trackPoints has a fitterInfo
      status->setIsFitted(false);
      status->setIsFitConvergedFully(false);
      status->setIsFitConvergedPartially(false);
      status->setNFailedPoints(nFailedHits);
  }

  status->setHasTrackChanged(false);
  status->setNumIterations(nIt);
  status->setForwardChi2(chi2FW);
  status->setBackwardChi2(chi2BW);
  status->setForwardNdf(ndfFW);
  status->setBackwardNdf(ndfBW);
}

//______________________________________________________________________________________________________________________
bool SpdKalmanFitterRefTrack::prepareTrack(Track* tr, const AbsTrackRep* rep, bool setSortingParams, int& nFailedHits) 
{
  cout << "\n////////////// SpdKalmanFitterRefTrack::prepareTrack (1)           " << " //////////////" << endl;
  
  int notChangedUntil, notChangedFrom;

  // remove outdated reference states
  bool changedSmthg = removeOutdated(tr, rep,  notChangedUntil, notChangedFrom);   // ATTENTION

  // declare matrices
  FTransportMatrix_.ResizeTo(rep->getDim(), rep->getDim());
  FTransportMatrix_.UnitMatrix();
  BTransportMatrix_.ResizeTo(rep->getDim(), rep->getDim());

  FNoiseMatrix_.ResizeTo(rep->getDim(), rep->getDim());
  BNoiseMatrix_.ResizeTo(rep->getDim(), rep->getDim());

  forwardDeltaState_.ResizeTo(rep->getDim());
  backwardDeltaState_.ResizeTo(rep->getDim());

  // declare stuff
  KalmanFitterInfo* prevFitterInfo(nullptr);
  
  std::unique_ptr<MeasuredStateOnPlane> firstBackwardUpdate;

  ReferenceStateOnPlane* referenceState(nullptr);
  ReferenceStateOnPlane* prevReferenceState(nullptr);

  const MeasuredStateOnPlane* smoothedState(nullptr);
  const MeasuredStateOnPlane* prevSmoothedState(nullptr);

  double trackLen(0);

  bool newRefState(false);     // has the current Point a new reference state?
  bool prevNewRefState(false); // has the last successfull point a new reference state?
 
  nFailedHits = 0;

  unsigned int nPoints = tr->getNumPoints();
  unsigned int i = 0;

  cout << "////////////// SpdKalmanFitterRefTrack::prepareTrack (2) points = " << nPoints  << " //////////////\n" << endl;

  // loop over TrackPoints
  for (; i < nPoints; ++i) {

    try {

      TrackPoint* trackPoint = tr->getPoint(i);
      
      cout << "\n";
      cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
      cout << "<SpdKalmanFitterRefTrack::prepareTrack> ----- (3.0) PREPARE POINT " << i << " / total = " << tr->getNumPoints() << endl;
      cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n" << endl;
     
      //cout << "==================================================\n" << endl;
      //trackPoint->PrintInfoShort();                 //FIXME!!!
      //cout << "\n==================================================" << endl;
      
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // check if we have a measurement
      
      if (!trackPoint->hasRawMeasurements()) continue;

      newRefState = false;
      
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // check fitterInfo

      cout << "\n<SpdKalmanFitterRefTrack::prepareTrack> ----- (3.1) CHECK FITTER INFO; point = " << i << endl;

      KalmanFitterInfo* fitterInfo(nullptr);
      
      if (trackPoint->hasFitterInfo(rep)) fitterInfo = dynamic_cast<KalmanFitterInfo*>(trackPoint->getFitterInfo(rep));   

      if (fitterInfo == nullptr) 
      {
          changedSmthg = true;
          fitterInfo = new KalmanFitterInfo(trackPoint, rep);
          trackPoint->setFitterInfo(fitterInfo);
      }
      else 
      {
          if (prevFitterInfo == nullptr) {
              
              if (fitterInfo->hasBackwardUpdate()) {
              
                  cout << " !!!!!!! AAAAAA !!!!!!! AAAAAA !!!!!!! AAAAAA !!!!!!!   " << endl;
                  cout << " !!!!!!! AAAAAA !!!!!!! AAAAAA !!!!!!! AAAAAA !!!!!!!   " << endl;
                  
                  cout << "\n EXISTED INFO: DEFINE FIRST POINT FORWARD PREDICTION FROM BACKWARD UPDATE \n " << endl;
      
                  firstBackwardUpdate.reset(new MeasuredStateOnPlane(*(fitterInfo->getBackwardUpdate())));
                  
                  cout << " !!!!!!! AAAAAA !!!!!!! AAAAAA !!!!!!! AAAAAA !!!!!!!   " << endl; 
                  cout << " !!!!!!! AAAAAA !!!!!!! AAAAAA !!!!!!! AAAAAA !!!!!!! \n" << endl;
                  
              }
          }
      }
      
      cout << "previous FitterInfo:         " << prevFitterInfo << endl;      
      cout << "current  FitterInfo:         " << fitterInfo << endl;    
      cout << "=========================" << endl;
      
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // get smoothedState if available

      cout << "<SpdKalmanFitterRefTrack::prepareTrack> ----- (3.2) CHECK SMOOTHED STATE; point = " << i 
           << " has smoothed state (yes,no): " << fitterInfo->hasPredictionsAndUpdates() << endl;
           
      if (fitterInfo->hasPredictionsAndUpdates()) smoothedState = &(fitterInfo->getFittedState(true));
      else smoothedState = nullptr;
      
      cout << "previous Smoothed State (fitter info):     " << prevSmoothedState << endl;      
      cout << "current  Smoothed State (fitter info):     " << smoothedState << endl;    
      cout << "=========================" << endl;
      
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // check refrence state
      
      cout << "<SpdKalmanFitterRefTrack::prepareTrack> ----- (3.3) CHECK REFERENCE STATE; point = " << i 
           << " has reference state (yes,no): " << fitterInfo->hasReferenceState() << endl;
           
      cout << "previous Referenced State (fitter info):     " << prevReferenceState << endl;      
      cout << "current  Referenced State (fitter info):     " << fitterInfo->getReferenceState() << endl;    
      cout << "=========================" << endl;
      
      if (fitterInfo->hasReferenceState()) {

          referenceState = fitterInfo->getReferenceState();

          if (!prevNewRefState) {
            
              trackLen += referenceState->getForwardSegmentLength();
              if (setSortingParams) trackPoint->setSortingParameter(trackLen);

              prevNewRefState = newRefState;
              prevReferenceState = referenceState;
              prevFitterInfo = fitterInfo;
              prevSmoothedState = smoothedState;

              continue;
          }

          if (prevReferenceState == nullptr) {
          
              referenceState->resetForward();

              if (setSortingParams) trackPoint->setSortingParameter(trackLen);

              prevNewRefState = newRefState;
              prevReferenceState = referenceState;
              prevFitterInfo = fitterInfo;
              prevSmoothedState = smoothedState;

              continue;
          }

          // previous refState has been altered ->need to update transport matrices
          StateOnPlane stateToExtrapolate(*prevReferenceState);
         
          // make sure track is consistent if extrapolation fails
          prevReferenceState->resetBackward();
          referenceState->resetForward();
         
          double segmentLen = rep->extrapolateToPlane(stateToExtrapolate, fitterInfo->getReferenceState()->getPlane(), false, true);
          trackLen += segmentLen;
         
          if (segmentLen == 0) {
              FTransportMatrix_.UnitMatrix();
              FNoiseMatrix_.Zero();
              forwardDeltaState_.Zero();
              BTransportMatrix_.UnitMatrix();
              BNoiseMatrix_.Zero();
              backwardDeltaState_.Zero();
          }
          else {
              rep->getForwardJacobianAndNoise(FTransportMatrix_, FNoiseMatrix_, forwardDeltaState_);
              rep->getBackwardJacobianAndNoise(BTransportMatrix_, BNoiseMatrix_, backwardDeltaState_);
          }
         
          prevReferenceState->setBackwardSegmentLength(-segmentLen);
          prevReferenceState->setBackwardTransportMatrix(BTransportMatrix_);
          prevReferenceState->setBackwardNoiseMatrix(BNoiseMatrix_);
          prevReferenceState->setBackwardDeltaState(backwardDeltaState_);
         
          referenceState->setForwardSegmentLength(segmentLen);
          referenceState->setForwardTransportMatrix(FTransportMatrix_);
          referenceState->setForwardNoiseMatrix(FNoiseMatrix_);
          referenceState->setForwardDeltaState(forwardDeltaState_);
         
          newRefState = true;
         
          if (setSortingParams) trackPoint->setSortingParameter(trackLen);
         
          prevNewRefState = newRefState;
          prevReferenceState = referenceState;
          prevFitterInfo = fitterInfo;
          prevSmoothedState = smoothedState;
         
          continue;
      }

      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++     
      // Construct plane
      
      cout << "<SpdKalmanFitterRefTrack::prepareTrack> ----- (3.4) CONSTRUCT NEXT PLANE; point = " << i << endl;
      
      SharedPlanePtr plane;
      
      cout << "<SpdKalmanFitterRefTrack::prepareTrack> Construced plane " << plane << endl;
      
           if (smoothedState != nullptr)      plane = trackPoint->getRawMeasurement(0)->constructPlane(*smoothedState); 
      else if (prevSmoothedState != nullptr)  plane = trackPoint->getRawMeasurement(0)->constructPlane(*prevSmoothedState); 
      else if (prevReferenceState != nullptr) plane = trackPoint->getRawMeasurement(0)->constructPlane(*prevReferenceState); 
      else if (rep != tr->getCardinalRep() &&
               trackPoint->hasFitterInfo(tr->getCardinalRep()) &&
               dynamic_cast<KalmanFitterInfo*>(trackPoint->getFitterInfo(tr->getCardinalRep())) != nullptr &&
                static_cast<KalmanFitterInfo*>(trackPoint->getFitterInfo(tr->getCardinalRep()))->hasPredictionsAndUpdates() 
              ) 
      {
           cout << "<SpdKalmanFitterRefTrack::prepareTrack> (3.4.1) CONSTRUCT PLANE; point = " << i 
                << "; construct plane with smoothed state of cardinal rep fit " << endl;
      
           TVector3 pos, mom;
           
           const MeasuredStateOnPlane& fittedState = 
                 static_cast<KalmanFitterInfo*>(trackPoint->getFitterInfo(tr->getCardinalRep()))->getFittedState(true);
           
           tr->getCardinalRep()->getPosMom(fittedState, pos, mom);
           
           StateOnPlane cardinalState(rep);
           
           rep->setPosMom(cardinalState, pos, mom);
           rep->setQop(cardinalState, tr->getCardinalRep()->getQop(fittedState));
           
           plane = trackPoint->getRawMeasurement(0)->constructPlane(cardinalState);
      }
      else {
           
           cout << "<SpdKalmanFitterRefTrack::prepareTrack> (3.4.2) CONSTRUCT PLANE; point = " << i 
                << "; construct plane with state from track to plane (SEED)" << endl;
                
           StateOnPlane seedFromTrack(rep);
           rep->setPosMom(seedFromTrack, tr->getStateSeed()); // also fills auxInfo
           
           plane = trackPoint->getRawMeasurement(0)->constructPlane(seedFromTrack);
      }
 
      if (plane.get() == nullptr) {
          Exception exc("SpdKalmanFitterRefTrack::prepareTrack ==> construced plane is nullptr!",__LINE__,__FILE__);
          exc.setFatal();
          throw exc;
      }
      
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // prepare reference state 
      
      cout << "<SpdKalmanFitterRefTrack::prepareTrack> ----- (3.5) PREPARE STATE TO EXTRAPOLATE; point = " << i << endl;
      
      std::unique_ptr<StateOnPlane> stateToExtrapolate(nullptr);
      
      if (prevFitterInfo == nullptr) { // first measurement
      
             if (smoothedState != nullptr)  stateToExtrapolate.reset(new StateOnPlane(*smoothedState));
        else if (referenceState != nullptr) stateToExtrapolate.reset(new StateOnPlane(*referenceState));
        else if (rep != tr->getCardinalRep() &&
                trackPoint->hasFitterInfo(tr->getCardinalRep()) &&
                dynamic_cast<KalmanFitterInfo*>(trackPoint->getFitterInfo(tr->getCardinalRep())) != nullptr &&
                 static_cast<KalmanFitterInfo*>(trackPoint->getFitterInfo(tr->getCardinalRep()))->hasPredictionsAndUpdates() ) 
        {
             cout << "<SpdKalmanFitterRefTrack::prepareTrack> (3.5.1) PREPARE REFERENSED STATE;  point = " << i 
                  << "; set extrapolatated state with smoothed state of cardinal rep " << endl;
             
             const MeasuredStateOnPlane& fittedState = 
                   static_cast<KalmanFitterInfo*>(trackPoint->getFitterInfo(tr->getCardinalRep()))->getFittedState(true);
                   
             stateToExtrapolate.reset(new StateOnPlane(fittedState));
             stateToExtrapolate->setRep(rep);
          
        }
        else {
           
             cout << "<SpdKalmanFitterRefTrack::prepareTrack> (3.5.2) PREPARE REFERENSED STATE; point = " << i 
                  << "; set extrapolatated state with state from track to plane (SEED)" << endl;
                
             stateToExtrapolate.reset(new StateOnPlane(rep));
            
             rep->setPosMom(*stateToExtrapolate, tr->getStateSeed());
             rep->setTime(*stateToExtrapolate, tr->getTimeSeed());
        }
      } 
      else {
          
        if (prevSmoothedState != nullptr) stateToExtrapolate.reset(new StateOnPlane(*prevSmoothedState));
        else {
            assert(prevReferenceState != nullptr);
            stateToExtrapolate.reset(new StateOnPlane(*prevReferenceState));
        }
      }
      
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // do extapolation 
      
      cout << "<SpdKalmanFitterRefTrack::prepareTrack> ----- (3.6) DO EXTAPOLATION; point = " << i << endl;
  
      // make sure track is consistent if extrapolation fails
      if (prevReferenceState != nullptr) prevReferenceState->resetBackward(); // ATTENTION
     
      fitterInfo->deleteReferenceInfo(); // ATTENTION

      if (prevFitterInfo != nullptr) {
          cout << "<SpdKalmanFitterRefTrack::prepareTrack> ----- (3.6.1) Extrapolate to previous state " << endl;
          rep->extrapolateToPlane(*stateToExtrapolate, prevFitterInfo->getPlane());
      }
      
      cout << "<SpdKalmanFitterRefTrack::prepareTrack> ----- (3.6.2) Extrapolate to next state " << endl;
      double segmentLen = rep->extrapolateToPlane(*stateToExtrapolate, plane, false, true);  
       
      trackLen += segmentLen;
    
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // get jacobians and noise matrices
      cout << "<SpdKalmanFitterRefTrack::prepareTrack> ----- (3.7) GET/SET MATRICES: TRANSPORT, NOISE, DELTA; point = " << i << endl;
    
      if (segmentLen == 0) {
          FTransportMatrix_.UnitMatrix();
          FNoiseMatrix_.Zero();
          forwardDeltaState_.Zero();
          BTransportMatrix_.UnitMatrix();
          BNoiseMatrix_.Zero();
          backwardDeltaState_.Zero();
      }
      else {
          if (i > 0) rep->getForwardJacobianAndNoise(FTransportMatrix_, FNoiseMatrix_, forwardDeltaState_);
          rep->getBackwardJacobianAndNoise(BTransportMatrix_, BNoiseMatrix_, backwardDeltaState_);
      }

      // if we are at first measurement and seed state is defined somewhere else
      if (i == 0) { 
          segmentLen = 0;
          trackLen = 0;
      }
      
      if (setSortingParams) trackPoint->setSortingParameter(trackLen);

      // set backward matrices for previous reference state
      if (prevReferenceState != nullptr) {
          prevReferenceState->setBackwardSegmentLength(-segmentLen);
          prevReferenceState->setBackwardTransportMatrix(BTransportMatrix_);
          prevReferenceState->setBackwardNoiseMatrix(BNoiseMatrix_);
          prevReferenceState->setBackwardDeltaState(backwardDeltaState_);
      }
     
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // create new reference state (from extrapolated state)
      
      cout << "<SpdKalmanFitterRefTrack::prepareTrack> ----- (3.8) MAKE (AND SET FORWARD INFO) NEW REFERENCE STATE "
          <<  "(FROM EXTRAPOLATED STATE); point = " << i << endl;

      newRefState = true;
      changedSmthg = true;
      
      referenceState = new ReferenceStateOnPlane(stateToExtrapolate->getState(),
                                                 stateToExtrapolate->getPlane(),
                                                 stateToExtrapolate->getRep(),
                                                 stateToExtrapolate->getAuxInfo());
      
      // set forward matrices for current reference state
      referenceState->setForwardSegmentLength(segmentLen);
      referenceState->setForwardTransportMatrix(FTransportMatrix_);
      referenceState->setForwardNoiseMatrix(FNoiseMatrix_);
      referenceState->setForwardDeltaState(forwardDeltaState_);

      referenceState->resetBackward(); // ATTENTION

      fitterInfo->setReferenceState(referenceState);
      

      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // construct MeasurementsOnPlane

      cout << "<SpdKalmanFitterRefTrack::prepareTrack> ----- (3.9) CONSTRUCT MEASUREMENTS ON PLANE (WITH WEIGHTS); point = " << i << endl;

      fitterInfo->deleteMeasurementInfo(); // ATTENTION
        
      const std::vector<AbsMeasurement*>& rawMeasurements = trackPoint->getRawMeasurements();

      for ( std::vector< genfit::AbsMeasurement* >::const_iterator measurement = rawMeasurements.begin(), 
            lastMeasurement = rawMeasurements.end(); 
            measurement != lastMeasurement; ++measurement) 
      {
            assert((*measurement) != nullptr);
            fitterInfo->addMeasurementsOnPlane((*measurement)->constructMeasurementsOnPlane(*referenceState));
      }
    
      // copy old weights in new measurements
      std::vector<double> oldWeights = fitterInfo->getWeights();
      bool oldWeightsFixed = fitterInfo->areWeightsFixed();
      
      if (oldWeights.size() == fitterInfo->getNumMeasurements()) {
          fitterInfo->setWeights(oldWeights);
          fitterInfo->fixWeights(oldWeightsFixed);
      }
      
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // if we made it here, no Exceptions were thrown and the TrackPoint could successfully be processed
      
      prevNewRefState    = newRefState;
      prevReferenceState = referenceState;
      prevFitterInfo     = fitterInfo;
      prevSmoothedState  = smoothedState;
      
      //cout << "==================================================\n" << endl;
      //trackPoint->PrintInfoShort();   // FIXME
      //cout << "\n==================================================" << endl;
 
    }
    catch (Exception& e) {
      errorOut << e.what() << endl;
      //cout << e.what() << endl;
     // exit(1);
        
      ++nFailedHits;

      if (maxFailedHits_< 0 || nFailedHits <= maxFailedHits_) {
          
          prevNewRefState = true;
          referenceState  = nullptr;
          smoothedState   = nullptr;
        
          tr->getPoint(i)->deleteFitterInfo(rep);

          if (setSortingParams) tr->getPoint(i)->setSortingParameter(trackLen);
          
          continue;
      }

      // clean up
      removeForwardBackwardInfo(tr, rep, notChangedUntil, notChangedFrom); // ATTENTION

      // set sorting parameters of rest of TrackPoints and remove FitterInfos
      for (; i < nPoints; ++i) {
          
           TrackPoint* trackPoint = tr->getPoint(i);

           if (setSortingParams) trackPoint->setSortingParameter(trackLen);

           trackPoint->deleteFitterInfo(rep);
      }
       
      return true;
       
    } // end point preparation

  } 
  // end loop over TrackPoints
  
  cout << "\n";
  cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
  cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
  cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
  
  cout << "\n<SpdKalmanFitterRefTrack::prepareTrack> (4) END LOOP OVER TRACK POINTS, REMOVE ALL FITTER INFOS " << endl;
  
  removeForwardBackwardInfo(tr, rep, notChangedUntil, notChangedFrom); // ATTENTION
  
  if (firstBackwardUpdate && tr->getPointWithMeasurementAndFitterInfo(0, rep)) {
      
      KalmanFitterInfo* fi = static_cast<KalmanFitterInfo*>(tr->getPointWithMeasurementAndFitterInfo(0, rep)->getFitterInfo(rep));
      
      if (fi && ! fi->hasForwardPrediction()) {
        
          cout << "<SpdKalmanFitterRefTrack::prepareTrack> (4.1) SET FIRST POINT FORWARD PREDICTION FROM FIRST POINT BACKWARD UPDATE\n"; 
          
          if (fi->getPlane() != firstBackwardUpdate->getPlane()) {
              rep->extrapolateToPlane(*firstBackwardUpdate, fi->getPlane());
          }
          
          firstBackwardUpdate->blowUpCov(blowUpFactor_, resetOffDiagonals_, blowUpMaxVal_);
          fi->setForwardPrediction(new MeasuredStateOnPlane(*firstBackwardUpdate));
      }
  }

 cout << "<SpdKalmanFitterRefTrack::prepareTrack> (5) END; processed points = " << nPoints << "\n" << endl;
 
 cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
 cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
 cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n" << endl;
   
  KalmanFitStatus* fitStatus = dynamic_cast<KalmanFitStatus*>(tr->getFitStatus(rep));
  if (fitStatus != nullptr) fitStatus->setTrackLen(trackLen);

  // self check
  assert(isTrackPrepared(tr, rep));

  return changedSmthg;
}

//______________________________________________________________________________________________________________________
bool SpdKalmanFitterRefTrack::removeOutdated(Track* tr, const AbsTrackRep* rep, int& notChangedUntil, int& notChangedFrom) 
{
  bool changedSmthg(false);

  unsigned int nPoints = tr->getNumPoints();
  notChangedUntil = nPoints-1;
  notChangedFrom = 0;

  // loop over TrackPoints
  for (unsigned int i = 0; i < nPoints; ++i) {

       TrackPoint* trackPoint = tr->getPoint(i);

       // check if we have a measurement
       if (!trackPoint->hasRawMeasurements()) continue;

       // get fitterInfo
       KalmanFitterInfo* fitterInfo(nullptr);
       if (trackPoint->hasFitterInfo(rep)) fitterInfo = dynamic_cast<KalmanFitterInfo*>(trackPoint->getFitterInfo(rep));

       if (fitterInfo == nullptr) continue;

       // check if we need to calculate or update reference state
       if (fitterInfo->hasReferenceState()) {

           // reference state but not all predictions & updates -> do not touch reference state  
           if (!fitterInfo->hasPredictionsAndUpdates()) continue;

           const MeasuredStateOnPlane& smoothedState = fitterInfo->getFittedState(true);
           resM_.ResizeTo(smoothedState.getState());
           resM_ = smoothedState.getState();
           resM_ -= fitterInfo->getReferenceState()->getState();
           double chi2(0);

           // calculate chi2, ignore off diagonals
           double* resArray = resM_.GetMatrixArray();
           for (int j=0; j<smoothedState.getCov().GetNcols(); ++j) chi2 += resArray[j]*resArray[j]/smoothedState.getCov()(j,j);

            // reference state is near smoothed state -> do not update reference state
           if (chi2 < deltaChi2Ref_) continue;
       }

       fitterInfo->deleteReferenceInfo();

       changedSmthg = true;

       // decided to update reference state -> set notChangedUntil (only once)
       if (notChangedUntil == (int)nPoints-1) notChangedUntil = i-1;
       notChangedFrom = i+1;
  } 

  return changedSmthg;
}

//______________________________________________________________________________________________________________________
void SpdKalmanFitterRefTrack::removeForwardBackwardInfo(Track* tr, const AbsTrackRep* rep, 
                                                     int notChangedUntil, int notChangedFrom) const 
{
  unsigned int nPoints = tr->getNumPoints();

  if (refitAll_) {
      tr->deleteForwardInfo(0, -1, rep);
      tr->deleteBackwardInfo(0, -1, rep);
      return;
  }

  // delete forward/backward info from/to points where reference states have changed
  
  if (notChangedUntil != (int)nPoints-1) tr->deleteForwardInfo(notChangedUntil+1, -1, rep);
  
  if (notChangedFrom != 0) tr->deleteBackwardInfo(0, notChangedFrom-1, rep);
}

//______________________________________________________________________________________________________________________
void SpdKalmanFitterRefTrack::processTrackPoint(KalmanFitterInfo* fi, const KalmanFitterInfo* prevFi, 
                                             const TrackPoint* tp, double& chi2, double& ndf, int direction)
{
  if (squareRootFormalism_) {
      processTrackPointSqrt(fi, prevFi, tp, chi2, ndf, direction);
      return;
  }

  unsigned int dim = fi->getRep()->getDim();

  p_.Zero(); // p_{k|k-1}
  C_.Zero(); // C_{k|k-1}

  cout << "\n<SpdKalmanFitterRefTrack::processTrackPoint> (1) MAKE PREDICTION. " << endl;
  
  // predict
  if (prevFi != nullptr) {
      
      cout << "<SpdKalmanFitterRefTrack::processTrackPoint> (1.1) MAKE PREDICTION FROM PREVIOUS UPDATE" << endl;
      
      const TMatrixD& F = fi->getReferenceState()->getTransportMatrix(direction); // Transport matrix
    
      assert(F.GetNcols() == (int)dim);
    
      const TMatrixDSym& N = fi->getReferenceState()->getNoiseMatrix(direction); // Noise matrix
    
      //p_ = ( F * prevFi->getUpdate(direction)->getState() ) + fi->getReferenceState()->getDeltaState(direction);
      p_  = prevFi->getUpdate(direction)->getState();
      p_ *= F;
      p_ += fi->getReferenceState()->getDeltaState(direction);

      C_ = prevFi->getUpdate(direction)->getCov();
      C_.Similarity(F);
      C_ += N;
      
      // Set prediction 
      fi->setPrediction(new MeasuredStateOnPlane(p_, C_, 
                                                 fi->getReferenceState()->getPlane(), 
                                                 fi->getReferenceState()->getRep(), 
                                                 fi->getReferenceState()->getAuxInfo()), 
                        direction);
  }
  else {
      
      if (fi->hasPrediction(direction)) {
          
          cout << "<SpdKalmanFitterRefTrack::processTrackPoint> (1.2) PREDICTION IS ALREADY DEFINED" << endl;
          
          p_ = fi->getPrediction(direction)->getState();
          C_ = fi->getPrediction(direction)->getCov();
      }
      else {
          
          cout << "<SpdKalmanFitterRefTrack::processTrackPoint> (1.3) MAKE PREDICTION FROM START TRACK (SEED)" << endl;
          
          const AbsTrackRep *rep = fi->getReferenceState()->getRep();
          
          // predicted state 
          p_ = fi->getReferenceState()->getState();

          // Convert from 6D covariance of the seed to whatever the trackRep wants.
          TMatrixDSym dummy(p_.GetNrows());
          MeasuredStateOnPlane mop(p_, dummy, fi->getReferenceState()->getPlane(), rep, fi->getReferenceState()->getAuxInfo());
          
          TVector3 pos, mom;
          rep->getPosMom(mop, pos, mom);
          rep->setPosMomCov(mop, pos, mom, fi->getTrackPoint()->getTrack()->getCovSeed());
          
          // Blow up, set.
          mop.blowUpCov(blowUpFactor_, resetOffDiagonals_, blowUpMaxVal_);
          
          // Set prediction 
          fi->setPrediction(new MeasuredStateOnPlane(mop), direction);
          
          // predicted covariation matrix
          C_ = mop.getCov();
      }
  }

  cout << "<SpdKalmanFitterRefTrack::processTrackPoint> (2) MAKE UPDATE. " << endl;

  // update(s)
  double chi2inc = 0;
  double ndfInc = 0;
  
  const std::vector<MeasurementOnPlane*> measurements = getMeasurements(fi, tp, direction);
  
  for (std::vector<MeasurementOnPlane*>::const_iterator it = measurements.begin(); it != measurements.end(); ++it) {
      
       const MeasurementOnPlane& m = **it;

       //Weight of measurement is almost 0, continue 
       if (!canIgnoreWeights() && m.getWeight() <= 1.01E-10) continue;

       const AbsHMatrix* H(m.getHMatrix());
   
       // (weighted) cov
       const TMatrixDSym& V((!canIgnoreWeights() && (m.getWeight() < 0.99999)) ? 1./m.getWeight() * m.getCov() : m.getCov());

       covSumInv_.ResizeTo(C_);
       covSumInv_ = C_; // (V_k + H_k C_{k|k-1} H_k^T)^(-1)
   
       H->HMHt(covSumInv_);
       covSumInv_ += V;

       tools::invertMatrix(covSumInv_);

       const TMatrixD& CHt(H->MHt(C_));

       res_.ResizeTo(m.getState());
       res_ = m.getState();
       res_ -= H->Hv(p_); // residual
    
       p_ += TMatrixD(CHt, TMatrixD::kMult, covSumInv_) * res_; // updated state
    
       covSumInv_.Similarity(CHt); // with (C H^T)^T = H C^T = H C  (C is symmetric)
    
       C_ -= covSumInv_; // updated Cov

       // Calculate chi² increment.  At the first point chi2inc == 0 and the matrix will not be invertible.
       
       res_  = m.getState();
       res_ -= H->Hv(p_); // new residual
    
       // only calculate chi2inc if res != nullptr. If matrix inversion fails, chi2inc = 0
    
       if (res_ != 0) {
        
           Rinv_.ResizeTo(C_);
           Rinv_ = C_;
           H->HMHt(Rinv_);
           Rinv_ -= V;
           Rinv_ *= -1;

           bool couldInvert(true);
           
           try { tools::invertMatrix(Rinv_); }
           catch (Exception& e) { couldInvert = false; }

           if (couldInvert) chi2inc += Rinv_.Similarity(res_);
       }

       if (!canIgnoreWeights()) ndfInc += m.getWeight() * m.getState().GetNrows();
       else ndfInc += m.getState().GetNrows();

  } // end loop over measurements

  chi2 += chi2inc;
  ndf  += ndfInc;

  KalmanFittedStateOnPlane* upState = new KalmanFittedStateOnPlane(p_, C_, 
                                                                   fi->getReferenceState()->getPlane(), 
                                                                   fi->getReferenceState()->getRep(), 
                                                                   fi->getReferenceState()->getAuxInfo(), 
                                                                   chi2inc, ndfInc);
  upState->setAuxInfo(fi->getReferenceState()->getAuxInfo());
  
  fi->setUpdate(upState, direction);

  // check
  if (not fi->checkConsistency()) throw genfit::Exception("Consistency check failed ", __LINE__, __FILE__);
}

//______________________________________________________________________________________________________________________
void SpdKalmanFitterRefTrack::processTrackPointSqrt(KalmanFitterInfo* fi, const KalmanFitterInfo* prevFi,
                                                 const TrackPoint* tp, double& chi2, double& ndf, int direction)
{
  unsigned int dim = fi->getRep()->getDim();

  p_.Zero(); // p_{k|k-1}
  C_.Zero(); // C_{k|k-1}

  TMatrixD S(dim, dim); // sqrt(C_);

  cout << "\n<SpdKalmanFitterRefTrack::processTrackPointSqrt> (1) MAKE PREDICTION. " << endl;
  
  // predict
  if (prevFi != nullptr) {
    
      cout << "<SpdKalmanFitterRefTrack::processTrackPointSqrt> (1.1) MAKE PREDICTION FROM PREVIOUS UPDATE" << endl;
    
      const TMatrixD& F = fi->getReferenceState()->getTransportMatrix(direction); // Transport matrix
    
      assert(F.GetNcols() == (int)dim);
    
      const TMatrixDSym& N = fi->getReferenceState()->getNoiseMatrix(direction); // Noise matrix
    
      //p_ = ( F * prevFi->getUpdate(direction)->getState() ) + fi->getReferenceState()->getDeltaState(direction);
      p_ = prevFi->getUpdate(direction)->getState();
      p_ *= F;
      p_ += fi->getReferenceState()->getDeltaState(direction);

      TDecompChol decompS(prevFi->getUpdate(direction)->getCov());
      decompS.Decompose();
      
      TMatrixD Q;
      tools::noiseMatrixSqrt(N, Q);
      
      tools::kalmanPredictionCovSqrt(decompS.GetU(), F, Q, S);

      fi->setPrediction(new MeasuredStateOnPlane(p_, TMatrixDSym(TMatrixDSym::kAtA, S), 
                                                 fi->getReferenceState()->getPlane(), 
                                                 fi->getReferenceState()->getRep(), 
                                                 fi->getReferenceState()->getAuxInfo()), 
                        direction);
  }
  else {
      
    if (fi->hasPrediction(direction)) {
      
        cout << "<SpdKalmanFitterRefTrack::processTrackPointSqrt> (1.2) PREDICTION IS ALREADY DEFINED" << endl;
        
        p_ = fi->getPrediction(direction)->getState();
      
        TDecompChol decompS(fi->getPrediction(direction)->getCov());
        decompS.Decompose();
        S = decompS.GetU();
    }
    else {
   
        cout << "<SpdKalmanFitterRefTrack::processTrackPointSqrt> (1.3) MAKE PREDICTION FROM START TRACK (SEED)" << endl;
   
        const AbsTrackRep *rep = fi->getReferenceState()->getRep();
       
        // predicted state  
        p_ = fi->getReferenceState()->getState();

        // Convert from 6D covariance of the seed to whatever the trackRep wants.
        TMatrixDSym dummy(p_.GetNrows());
        MeasuredStateOnPlane mop(p_, dummy, fi->getReferenceState()->getPlane(), rep, fi->getReferenceState()->getAuxInfo());
      
        TVector3 pos, mom;
        rep->getPosMom(mop, pos, mom);
        rep->setPosMomCov(mop, pos, mom, fi->getTrackPoint()->getTrack()->getCovSeed());
     
        // Blow up, set.
        mop.blowUpCov(blowUpFactor_, resetOffDiagonals_, blowUpMaxVal_);
       
        // Set prediction
        fi->setPrediction(new MeasuredStateOnPlane(mop), direction);
      
        // predicted covariation matrix // S = sqrt(C_);
        
        TDecompChol decompS(mop.getCov());
        decompS.Decompose();
        S = decompS.GetU();
    }
  }
  
  cout << "<SpdKalmanFitterRefTrack::processTrackPointSqrt> (2) MAKE UPDATE. " << endl;
  
  // update(s)
  double chi2inc = 0;
  double ndfInc = 0;

  const std::vector<MeasurementOnPlane *> measurements = getMeasurements(fi, tp, direction);
  
  for (std::vector<MeasurementOnPlane *>::const_iterator it = measurements.begin(); it != measurements.end(); ++it) {
      
       const MeasurementOnPlane& m = **it;
       
       // Weight of measurement is almost 0, continue 
       if (!canIgnoreWeights() && m.getWeight() <= 1.01E-10) continue;

       const AbsHMatrix* H(m.getHMatrix());
   
       // (weighted) cov
       const TMatrixDSym& V((!canIgnoreWeights() && m.getWeight() < 0.99999) ? 1./m.getWeight() * m.getCov() : m.getCov());
       
       TDecompChol decompR(V);
       decompR.Decompose();
       
       const TMatrixD& R(decompR.GetU());

       res_.ResizeTo(m.getState());
       res_ = m.getState();
       res_ -= H->Hv(p_); // residual

       TVectorD update;
    
       tools::kalmanUpdateSqrt(S, res_, R, H, update, S);
       
       p_ += update;
   
       // Calculate chi² increment.  At the first point chi2inc == 0 and the matrix will not be invertible.

       res_ -= H->Hv(update); // new residual

       // only calculate chi2inc if res != 0. If matrix inversion fails, chi2inc = 0
       
       if (res_ != 0) {
           
          Rinv_.ResizeTo(V);
          Rinv_ = V - TMatrixDSym(TMatrixDSym::kAtA, H->MHt(S));

          bool couldInvert(true);
          
          try { tools::invertMatrix(Rinv_); }
          catch (Exception& e) { couldInvert = false; }

          if (couldInvert) chi2inc += Rinv_.Similarity(res_);
       }

       if (!canIgnoreWeights()) ndfInc += m.getWeight() * m.getState().GetNrows();
       else ndfInc += m.getState().GetNrows();

  } // end loop over measurements

  C_ = TMatrixDSym(TMatrixDSym::kAtA, S);

  chi2 += chi2inc;
  ndf  += ndfInc;

  KalmanFittedStateOnPlane* upState = new KalmanFittedStateOnPlane(p_, C_, fi->getReferenceState()->getPlane(), 
                                                                   fi->getReferenceState()->getRep(), 
                                                                   fi->getReferenceState()->getAuxInfo(), 
                                                                   chi2inc, ndfInc);
  upState->setAuxInfo(fi->getReferenceState()->getAuxInfo());
  
  fi->setUpdate(upState, direction);

  // check
  if (not fi->checkConsistency()) throw genfit::Exception("Consistency check failed ", __LINE__, __FILE__);
}


