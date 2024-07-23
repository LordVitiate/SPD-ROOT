
// $Id$
// Author: vladimir   2022/04/10

//_____________________________________________________________________________
//
// SpdRCTrackFitterGF (on the base of I.Denisenko example)
//_____________________________________________________________________________

#include "SpdRCTrackFitterGF.h"

#include "SpdRunAna.h"

#include "SpdFieldCreator.h"
#include "SpdField.h"
#include "SpdFieldPar.h"
#include "SpdGFMagneticField.h"
#include "SpdTrackFitPar.h"
#include "SpdHit.h"
#include "SpdMCStrawHit1D.h"

#include "SpdGFHitCreator.h"

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

ClassImp(SpdRCTrackFitterGF)

//_____________________________________________________________________________
SpdRCTrackFitterGF::SpdRCTrackFitterGF():fIsInit(false),
fHitCreator(nullptr),
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

//_____________________________________________________________________________
SpdRCTrackFitterGF::~SpdRCTrackFitterGF() 
{
   if (fFitter)     delete fFitter;
   if (fHitCreator) delete fHitCreator;
   if (fTrack)      delete fTrack;
}

//_____________________________________________________________________________
Bool_t SpdRCTrackFitterGF::LoadGeometry() // private
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance(); 
   if (!gGeoManager && run) run->GetParameters("FairGeoParSet");
   return (gGeoManager) ? true : false; 
}

//_____________________________________________________________________________
Bool_t SpdRCTrackFitterGF::LoadField() // private
{
   SpdField* field(0);
   SpdGFMagneticField* Field(0);

   // Check if magnetis SPD field already initialized
   //-------------------------------------------------
   if (FieldManager::getInstance()->isInitialized()) 
   {
       Field = dynamic_cast<SpdGFMagneticField*>(FieldManager::getInstance()->getField());
       if (Field && fVerboseLevel > -1) {
           cout << "-I- <SpdRCTrackFitterGF::LoadField> "
                << "Field has been already initialized ... ok" << endl;
       }
       return (Field) ? true : false;
   }

   // Search for external field
   //----------------------------   
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
   //--------------------------------------------   
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
       cout << "-E- <SpdRCTrackFitterGF::LoadField> No field parameters (\"SpdFieldPar\")" << endl;
       return false; 
   }

   // Create SPD field using parameters
   //------------------------------------
   SpdFieldCreator* fieldCreator = new SpdFieldCreator();  
   field = (SpdField*)fieldCreator->createFairField(field_pars);

   if (field && fVerboseLevel > -1) field->Print(""); // FIXME

   delete fieldCreator;
    
   // Initialize GenFit field
   //-------------------------
   Field = new SpdGFMagneticField();
   Field->SetField(field,true);  // ATTENTION  field owner! 
    
   // Initialize GenFit field manager
   //--------------------------------
   FieldManager::getInstance()->init(Field);
   
   //FieldManager::getInstance()->useCache(true);
    
   return true;
}

//_____________________________________________________________________________
void SpdRCTrackFitterGF::InitFitter()
{
   if (fFitter) delete fFitter;
   fFitter = new KalmanFitterRefTrack(fFitterMaxIterations,1e-3,1e4,true);

   fFitter->setMinIterations(1);
   fFitter->setMaxIterations(4);        // Affectes fitter
   fFitter->setBlowUpMaxVal(1e10);      // see MeasuredStateOnPlane::blowUpCov
   fFitter->setDebugLvl(0);             // Set 2 for the fitter debug output
}

//_____________________________________________________________________________
void SpdRCTrackFitterGF::DeleteFitter()
{
   if (fFitter) {
       delete fFitter;
       fFitter = 0;
   }
}

//_____________________________________________________________________________
SpdField* SpdRCTrackFitterGF::GetField()
{
   FieldManager* fm = FieldManager::getInstance();
   if (!fm->isInitialized()) return 0;
   SpdGFMagneticField* field = dynamic_cast<SpdGFMagneticField*>(fm->getField());
   return (field) ? field->GetField() : 0;
}

//_____________________________________________________________________________
Bool_t SpdRCTrackFitterGF::Init()
{
   if (fVerboseLevel > -1) cout << "-I- <SpdRCTrackFitterGF::Init> ... " << endl;

   if (fIsInit) return fIsInit;

   // Load Geometry
   //---------------
   if (!LoadGeometry()) {
       cout << "-E- <SpdRCTrackFitterGF::Init> Geometry not defined " << endl;
       return false;
   }

   // Load Field
   //-------------
   if (!LoadField()) {
       cout << "-E- <SpdRCTrackFitterGF::Init> Bad field initialization " << endl;
       return false;
   }

  // Init GF materials
   //--------------------
   if (!MaterialEffects::getInstance()->isInitialized()) {
        MaterialEffects::getInstance()->init(new TGeoMaterialInterface());
        if (fVerboseLevel > -1) {
           cout << "-I- <SpdRCTrackFitterGF::Init> "
                << "Geometry initialization ... ok " << endl;
       }
   }
   else {
       if (fVerboseLevel > -1) {
           cout << "-I- <SpdRCTrackFitterGF::Init> "
                << "Geometry has been already initialized ... ok" << endl;
       }
   }

   // Create GF-measurements Creator 
       
   fHitCreator = new SpdGFHitCreator();
   
   fIsInit = true;
    
   if (fVerboseLevel > -1) {
       cout << "-I- <SpdRCTrackFitterGF::Init> Intialization successfull " << endl;
   }
     
   return fIsInit;   
}

//_____________________________________________________________________________
/* Called before fitting set of points */
Bool_t SpdRCTrackFitterGF::CheckInits() 
{
   if (!fIsInit) {
       cout << "-E- <SpdRCTrackFitterGF::FitTrack>"
            << " Geometry and/or Magnetic Field are undefined" << endl;
       return false;
   }

   if (!fFitter) {
       cout << "-E- <SpdRCTrackFitterGF::FitTrack> Track fitter is not initialized" << endl;
       return false;
   }

   if (fPdg == 0) {
       cout << "-E- <SpdRCTrackFitterGF::FitTrack>"
            << " The type of particle (pdg code) should be defined previously" << endl;
       return false;
   }

   return true;
}

//_____________________________________________________________________________
Bool_t SpdRCTrackFitterGF::FitPoints(const std::vector<SpdMCStrawHit1D*>& points, 
                                      SpdTrackState* start_state, SpdTrackFitPar* pars) 
{
   //cout  << "<---- In sTrackFitterGF::FitPoints ----> " << endl;

   if (!CheckInits())  return false;
   if (points.empty()) return false;
   if (!start_state)   return false;
   if (!pars)          return false;

   if (fVerboseLevel > 0)  {
        cout << "\n-I- <SpdRCTrackFitterGF::FitPoints> Particle (pdg): " << fPdg
             << ", Points: " << points.size() << endl;
   }

   // Material effects
   //------------------
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

   pars->SetTrackPdgCode(fPdg);
   pars->SetNPoints(fTrack->getNumPoints());

   // --------------- The main part --------------
  
   try
   {
       fFitter->processTrack(fTrack);

       // check status
       auto status = (KalmanFitStatus*) fTrack->getFitStatus();

       if (!status) {
          pars->SetErrorFlag(kSpdTrNoStatus);
          return false;
       }

       if (!status->isFitted()) {
          pars->SetErrorFlag(kSpdTrBadFitStatus);
          return false;
       }

       if ( (!status) || (!(status->isFitConverged() || status->isFitConvergedPartially())) ) {
           return false;
       }

       fChi2 = status->getForwardChi2();
       fNdf  = status->getForwardNdf();

       TrackPoint* point = fTrack->getPointWithFitterInfo(-1);

       const MeasuredStateOnPlane* last_state = 0;
       SpdTrackState* state;

       if (point) {
          try { last_state = &(fTrack->getFittedState(-1)); }
          catch (Exception& e) { last_state = 0; }
       }

       // last state 
       state = pars->LastState();
   
       state->SetMomentum(last_state->getMom());
       state->SetPosition(last_state->getPos());
       state->SetCovariance(last_state->get6DCov());
       state->SetTime(start_state->GetTime() + last_state->getTime());
   }
   catch(Exception& e) // check throw outside?
   {
       cout << "Exception raised when processing points!\n";
       return false;
   }
   
   Exception::clearEmessg();  // to avoid memory leaks?

   return true;
}

//_____________________________________________________________________________
bool SpdRCTrackFitterGF::BuildTrack(const std::vector<SpdMCStrawHit1D*>& points, 
                                                       SpdTrackState* start_state) 
{
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
    
    // Add initial state of the track as a point with measurement
    //-----------------------------------------------------------    
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
   
    // Add the second point to the track
    //---------------------------------

    fHitCreator->ResetHitCounter();
 
    for (auto ihit : points)
    {
        std::vector<AbsMeasurement*> measurements = fHitCreator->CreateHit(ihit);
        if (measurements.size() > 0) {
            TrackPoint* point = new TrackPoint(measurements, fTrack);
            fTrack->insertPoint(point);
        } else {
            return false;
        }
    }
    return true;
}


