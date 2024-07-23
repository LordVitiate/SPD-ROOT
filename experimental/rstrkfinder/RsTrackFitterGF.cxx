// TODO:
//  - time (done) and dist (not done)!
//  - see KalmanFitterRefTrack::processTrackPointSqrt for details

#include "RsTrackFitterGF.h"

#include "SpdRunAna.h"

#include "SpdFieldCreator.h"
#include "SpdField.h"
#include "SpdFieldPar.h"
#include "SpdGFMagneticField.h"
#include "SpdTrackFitPar.h"
#include "SpdHit.h"


#include "SpdRKTrackRep.h"

#include "KalmanFitterRefTrack.h"
#include "KalmanFitStatus.h"
#include "KalmanFitterInfo.h"

#include "GFCore.hh"
#include "GFTrackReps.hh"

#include <TRandom3.h>
#include <TGeoManager.h>

#include <iostream>

using std::cout;
using std::cin;
using std::endl;

using namespace genfit;

ClassImp(RsTrackFitterGF)

//_____________________________________________________________________________
RsTrackFitterGF::RsTrackFitterGF():
    fIsInit(false),
    fMeasurementMaker(nullptr),
    fFitter(nullptr),
    fExtField(nullptr),
    fTrack(nullptr),
    // start values
    fPdg(0),
    // fitter
    fMaterialsOpt(1),
    fFitterMaxIterations(1),
    //
    fVerboseLevel(0)
{
}

/* Called before fitting set of points */
Bool_t RsTrackFitterGF::CheckInits() {
   if (!fIsInit) {
       cout << "-E- <RsTrackFitterGF::FitTrack>"
            << " Geometry and/or Magnetic Field are undefined" << endl;
       return false;
   }

   if (!fFitter) {
       cout << "-E- <RsTrackFitterGF::FitTrack> Track fitter is not initialized" << endl;
       return false;
   }

   if (fPdg == 0) {
       cout << "-E- <RsTrackFitterGF::FitTrack>"
            << " The type of particle (pdg code) should be defined previously" << endl;
       return false;
   }

   return true;
}

//_____________________________________________________________________________
Bool_t RsTrackFitterGF::FitPoints(const std::vector<SpdRsMCHit*>& points, SpdTrackState* start_state) {
   //cout  << "<---- In sTrackFitterGF::FitPoints ----> " << endl;

   if (!CheckInits()) return false;
   if (points.empty()) return false;
   if (!start_state) return false;

   if (fVerboseLevel > 0)  {
        cout << "\n-I- <RsTrackFitterGF::FitTrack> Particle (pdg): " << fPdg
             << ", Points: " << points.size() << endl;
   }

   // Material effects
   Bool_t material_effects;
   if (fMaterialsOpt < 1) {
       material_effects = false;
       MaterialEffects::getInstance()->setNoEffects(true);
   }
   else {
       material_effects = true;
       MaterialEffects::getInstance()->setNoEffects(false);
   }

   // Reset exception flags
   Exception::setEmessgflag(true);


   // Create BaseStateOn the plane based on init pos, vect and cov
   // Build track
   if (!BuildTrack(points, start_state)) {
       return false;
   }

   // --------------- The main part --------------
  
   try
   {
       fFitter->processTrack(fTrack);

       // check status
       auto status = (KalmanFitStatus*) fTrack->getFitStatus();
       if ( (!status) || (!(status->isFitConverged() || status->isFitConvergedPartially())) ) {
           return false;
       }
       fChi2 = status->getForwardChi2();
       fNdf = status->getForwardNdf();

       TrackPoint* point = fTrack->getPointWithFitterInfo(-1);
       const MeasuredStateOnPlane* mop = &(fTrack->getFittedState(-1));
       fTrackState.SetMomentum(mop->getMom());
       fTrackState.SetPosition(mop->getPos());
       fTrackState.SetCovariance(mop->get6DCov());
       fTrackState.SetTime(start_state->GetTime() + mop->getTime());

   }
   catch(Exception& e) // check throw outside?
   {
       cout << "Exception raised when processing points!\n";
       return false;
   }

   
   /*
   try
   {
       int nFailedHits;

       //cout << "Initial state" << endl;
       //start_state->GetPosition().Print();
       //start_state->GetMomentum().Print();
       //start_state->GetCovariance().Print();
       
       fFitter->prepareTrack(fTrack, fTrack->getCardinalRep(), true, nFailedHits);
       TrackPoint* tp = fFitter->fitTrack(fTrack, fTrack->getCardinalRep(), fChi2, fNdf, 1);
       if (!tp) {
           return false;
       }
       cout << "chi2=" << fChi2 << ", ndf=" << fNdf << endl;
       KalmanFitterInfo* kf_info = tp->getKalmanFitterInfo();
       if (!kf_info) {
           return false;
       }
       // Get info from the last fitted point
       KalmanFittedStateOnPlane* mop2 = kf_info->getForwardUpdate();
      
       fTrackState.SetMomentum(mop2->getMom());
       fTrackState.SetPosition(mop2->getPos());
       fTrackState.SetCovariance(mop2->get6DCov());
       fTrackState.SetTime(start_state->GetTime() + mop2->getTime());
       //mop2->getPos().Print();
       //mop2->getMom().Print();
       //mop2->get6DCov().Print();

       //KalmanFitterInfo* kf_info0 = fTrack->getPoint(0)->getKalmanFitterInfo();
       //KalmanFittedStateOnPlane* mop0 = kf_info0->getForwardUpdate();
       //mop0->getPos().Print();
       //mop0->getMom().Print();
       //mop0->get6DCov().Print();
      

       //fFitter->fitTrack(fTrack, fTrack->getCardinalRep(), fChi2, fNdf, 0, 0, nFailedHits);
       //char c;
       //in >> c;

       //auto status = (KalmanFitStatus*)fTrack->getFitStatus();
       //cout << "status->getNumIterations(): " << status->getNumIterations() << endl;
       //cout << "status->getForwardChi2(): " << status->getForwardChi2() << endl;

   }
   catch(Exception& e) // check throw outside?
   {
       cout << "Exception raised when processing points!\n";
       return false;
   }
   */

   Exception::clearEmessg();  // to avoid memory leaks?

   return true;
}

//_____________________________________________________________________________
bool RsTrackFitterGF::BuildTrack(const std::vector<SpdRsMCHit*>& points, SpdTrackState* start_state) {
    //cout  << "<---- In sTrackFitterGF::BuildTrack ----> " << endl;
    // Try to avoid new/delete here
    if (fTrack) {
        delete fTrack;
        fTrack = 0;
    }

    TVector3 pos = start_state->GetPosition();
    TVector3 mom = start_state->GetMomentum();
    TMatrixDSym cov;
    start_state->GetCovariance(cov);

    AbsTrackRep* rep = new genfit::RKTrackRep(fPdg);

    MeasuredStateOnPlane state(rep); // attaches state to rep, sets internal demensions according to rep

    rep->setPosMomCov(state, pos, mom, cov);

    TVectorD    trkState(6);
    TMatrixDSym trkCov(6);

    rep->get6DStateCov(state, trkState, trkCov); // get the same back in a bit different format

    fTrack = new Track(rep, trkState, trkCov);
    fTrack->setCovSeed(cov);
    
    // Add initial state of teh track as a point with measurement
    
    {
        // make StateOnThePlane
        MeasuredStateOnPlane last_track_point(rep); // attaches state to rep, sets internal demensions according to rep
        last_track_point.setPosMomCov(pos, mom, cov);
        // use it to make measurements
        std::vector<genfit::AbsMeasurement*> measurements = {new genfit::FullMeasurement(last_track_point)};
        // make point from measurments
        TrackPoint* point = new TrackPoint(measurements, fTrack);
        //insert point to track
        fTrack->insertPoint(point);
    }
   

    // Add the second poin to the track
    fMeasurementMaker->ResetCounter();

    for (auto hit : points)
    {
        std::vector<AbsMeasurement*> measurements = fMeasurementMaker->ProcessHit(hit);
        if (measurements.size() > 0) {
            TrackPoint* point = new TrackPoint(measurements, fTrack);
            fTrack->insertPoint(point);
        } else {
            return false;
        }
    }
    return true;
}

//_____________________________________________________________________________
RsTrackFitterGF::~RsTrackFitterGF()
{
   if (fFitter) delete fFitter;
   if (fMeasurementMaker) delete fMeasurementMaker;
   if (fTrack) delete fTrack;
}

//_____________________________________________________________________________
Bool_t RsTrackFitterGF::LoadGeometry() // private
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance();
   if (!gGeoManager && run) run->GetParameters("FairGeoParSet");
   return (gGeoManager) ? true : false;
}

//_____________________________________________________________________________
Bool_t RsTrackFitterGF::LoadField() // private
{
   SpdField* field(0);
   SpdGFMagneticField* Field(0);

   // Check if magnetis SPD field already initialized

   if (FieldManager::getInstance()->isInitialized())
   {
       Field = dynamic_cast<SpdGFMagneticField*>(FieldManager::getInstance()->getField());
       if (Field && fVerboseLevel > -1) {
           cout << "-I- <RsTrackFitterGF::LoadField> "
                << "Field has been already initialized ... ok" << endl;
       }
       return (Field) ? true : false;
   }

   // Search for external field

   if (fExtField)
   {
       field = fExtField;

       // Initialize GenFit field
       Field = new SpdGFMagneticField();
       Field->SetField(field,false); // ATTENTION  false = not field owner!

       // Initialize GenFit field manager
       FieldManager::getInstance()->init(Field);
       return true;
   }

   // Try to create field using it's parameters

   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance();
   if (!run) return false;

   SpdFieldPar* field_pars = dynamic_cast<SpdFieldPar*>(run->GetParameters("SpdFieldPar"));

   if (!field_pars) {
       SpdSetParSet* pars = (SpdSetParSet*)run->GetParameters("SpdSetParSet");
       if (pars) {
           field_pars = pars->GetFieldPars();
           //((FairParSet*)field_pars)->init();
       }
   }

   if (!field_pars) {
       cout << "-E- <RsTrackFitterGF::LoadField> No field parameters (\"SpdFieldPar\")" << endl;
       return false;
   }

   // Create SPD field using parameters
   SpdFieldCreator* fieldCreator = new SpdFieldCreator();
   field = (SpdField*)fieldCreator->createFairField(field_pars);
   if (field && fVerboseLevel > -1) field->Print(""); // FIXME
   delete fieldCreator;

   // Initialize GenFit field
   Field = new SpdGFMagneticField();
   Field->SetField(field,true);  // ATTENTION  field owner!

   // Initialize GenFit field manager
   FieldManager::getInstance()->init(Field);

   //FieldManager::getInstance()->useCache(true);

   return true;
}

//_____________________________________________________________________________
void RsTrackFitterGF::InitFitter()
{
   if (fFitter) delete fFitter;
   fFitter = new KalmanFitterRefTrack(fFitterMaxIterations,1e-3,1e4,true);
   fFitter->setMinIterations(1);
   fFitter->setMaxIterations(4); // Affectes fitter
//   fFitter->setResetOffDiagonals(false);
   fFitter->setBlowUpMaxVal(1e10); //see MeasuredStateOnPlane::blowUpCov
   fFitter->setDebugLvl(0); // Set 2 for the fitter debug output
}

//_____________________________________________________________________________
void RsTrackFitterGF::DeleteFitter()
{
   if (fFitter) {
       delete fFitter;
       fFitter = 0;
   }
}

//_____________________________________________________________________________
SpdField* RsTrackFitterGF::GetField()
{
   FieldManager* fm = FieldManager::getInstance();
   if (!fm->isInitialized()) return 0;
   SpdGFMagneticField* field = dynamic_cast<SpdGFMagneticField*>(fm->getField());
   return (field) ? field->GetField() : 0;
}

//_____________________________________________________________________________
Bool_t RsTrackFitterGF::Init()
{
   if (fVerboseLevel > -1) cout << "-I- <RsTrackFitterGF::Init> ... " << endl;

   if (fIsInit) return fIsInit;

   // Load Geometry

   if (!LoadGeometry()) {
       cout << "-E- <RsTrackFitterGF::Init> Geometry not defined " << endl;
       return false;
   }

   // Load Field

   if (!LoadField()) {
       cout << "-E- <RsTrackFitterGF::Init> Bad field initialization " << endl;
       return false;
   }

   // Init GF materials
   if (!MaterialEffects::getInstance()->isInitialized()) {
       MaterialEffects::getInstance()->init(new TGeoMaterialInterface());
       if (fVerboseLevel > -1) {
           cout << "-I- <RsTrackFitterGF::Init> "
                << "Geometry initialization ... ok " << endl;
       }
   }
   else {
       if (fVerboseLevel > -1) {
           cout << "-I- <RsTrackFitterGF::Init> "
                << "Geometry has been already initialized ... ok" << endl;
       }
   }
   // Create GF-measurements Creator

   fMeasurementMaker = new RsGFMeasurementMaker();

   fIsInit = true;

   if (fVerboseLevel > -1) {
       cout << "-I- <RsTrackFitterGF::Init> Intialization successfull " << endl;
   }

   return fIsInit;
}

