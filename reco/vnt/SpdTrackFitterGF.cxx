// $Id$
// Author: artur   2021/02/08


//_____________________________________________________________________________
//
// SpdTrackFitterGF
//_____________________________________________________________________________

#include "SpdTrackFitterGF.h"

#include "SpdRunAna.h"

#include "SpdFieldCreator.h"
#include "SpdField.h"
#include "SpdFieldPar.h"
#include "SpdGFMagneticField.h"
#include "SpdTrackFitPar.h"
#include "SpdHit.h"

#include "SpdGFHitCreator.h"

#include "SpdRKTrackRep.h"

#include "KalmanFitterRefTrack.h"
#include "KalmanFitStatus.h"

#include "GFCore.hh"
#include "GFTrackReps.hh"

#include <TRandom3.h>
#include <TGeoManager.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace genfit;

ClassImp(SpdTrackFitterGF)

//_____________________________________________________________________________
SpdTrackFitterGF::SpdTrackFitterGF():fIsInit(false),
fHitCreator(0),fFitter(0),fExtField(0),fTrack(0),
// start values
fPdg(0),
fStartMomentum(0,0,1),
fStartVertex(0,0,0),
fStartTime(0),
// fitter
fFitterOption(1),
fMaterialsOpt(1),
fFitterMaxIterations(4),
fFitterMaxTrials(2),
fStartSeedMethod(1),
// storing additional fitter info 
fStoreImPoints(false), 
fStoreImMomentum(true),       
fStoreImPosition(false),        
fStoreImCovariance(false),     
// aux
fSmearStartVertex(true),
fSmearStartMom(true),
fSmearingDMom(0.1),
fSmearingDMomTheta(3.0),
fSmearingDMomPhi(1.0),
fSmearingDVxyz(0.5),
//
fVerboseLevel(1)
{
    ResetCounter();
    
    // Set default covariance matrix
    SetCovarianceMatrix(); 
}

//_____________________________________________________________________________
SpdTrackFitterGF::~SpdTrackFitterGF() 
{
   if (fFitter) delete fFitter;
   if (fHitCreator) delete fHitCreator;
   if (fTrack) delete fTrack;
}

//_____________________________________________________________________________
Bool_t SpdTrackFitterGF::LoadGeometry() // private
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance(); 
   if (!gGeoManager && run) run->GetParameters("FairGeoParSet");
   return (gGeoManager) ? true : false; 
}

//_____________________________________________________________________________
Bool_t SpdTrackFitterGF::LoadField() // private
{
   SpdField* field(0);
   SpdGFMagneticField* Field(0);

   // Check if magnetis SPD field already initialized
   
   if (FieldManager::getInstance()->isInitialized()) 
   {
       Field = dynamic_cast<SpdGFMagneticField*>(FieldManager::getInstance()->getField());
       if (Field && fVerboseLevel > -1) {
           cout << "-I- <SpdTrackFitterGF::LoadField> "
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
       cout << "-E- <SpdTrackFitterGF::LoadField> No field parameters (\"SpdFieldPar\")" << endl;
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
void SpdTrackFitterGF::InitFitter()
{
   if (fFitter) delete fFitter;
   if (fFitterOption == 1) fFitter = new KalmanFitterRefTrack(fFitterMaxIterations,1e-3,1e3,true);
   else fFitter = new KalmanFitterRefTrack(fFitterMaxIterations,1e-3,1e3,false);
}

//_____________________________________________________________________________
void SpdTrackFitterGF::DeleteFitter()
{
   if (fFitter) {
       delete fFitter;
       fFitter = 0;
   }
}

//_____________________________________________________________________________
SpdField* SpdTrackFitterGF::GetField()
{
   FieldManager* fm = FieldManager::getInstance();
   if (!fm->isInitialized()) return 0;
   SpdGFMagneticField* field = dynamic_cast<SpdGFMagneticField*>(fm->getField());
   return (field) ? field->GetField() : 0;
}

//_____________________________________________________________________________
Bool_t SpdTrackFitterGF::Init()
{
   if (fVerboseLevel > -1) cout << "-I- <SpdTrackFitterGF::Init> ... " << endl; 
   
   if (fIsInit) return fIsInit;
   
   // Load Geometry
       
   if (!LoadGeometry()) {
       cout << "-E- <SpdTrackFitterGF::Init> Geometry not defined " << endl;
       return false;  
   }
   
   // Load Field 
   
   if (!LoadField()) {
       cout << "-E- <SpdTrackFitterGF::Init> Bad field initialization " << endl;
       return false; 
   }
   
   // Init GF materials 
   if (!MaterialEffects::getInstance()->isInitialized()) {
       MaterialEffects::getInstance()->init(new TGeoMaterialInterface());
       if (fVerboseLevel > -1) {
           cout << "-I- <SpdTrackFitterGF::Init> "
                << "Geometry initialization ... ok " << endl;
       }
   }
   else {
       if (fVerboseLevel > -1) {
           cout << "-I- <SpdTrackFitterGF::Init> "
                << "Geometry has been already initialized ... ok" << endl;
       }
   }   
   // Create GF-measurements Creator 
       
   fHitCreator = new SpdGFHitCreator();
   
   fIsInit = true;
    
   if (fVerboseLevel > -1) {
       cout << "-I- <SpdTrackFitterGF::Init> Intialization successfull " << endl;
   }
     
   return fIsInit;   
}

//_____________________________________________________________________________
Bool_t SpdTrackFitterGF::FitTrack(const std::vector<SpdHit*>& points, 
                                  SpdTrackFitPar* pars) 
{   
   DeleteTrack();
    
   if (!fIsInit) {
       cout << "-E- <SpdTrackFitterGF::FitTrack>"
            << " Geometry and/or Magnetic Field are undefined" << endl;
       return false;
   }
 
   if (!fFitter) {
       cout << "-E- <SpdTrackFitterGF::FitTrack> Track fitter is not initialized" << endl;
       return false; 
   }
   
   if (fPdg == 0) {
       cout << "-E- <SpdTrackFitterGF::FitTrack>"
            << " The type of particle (pdg code) should be defined previously" << endl;
       return false;  
   }
       
   if (points.empty()) return false; 
   if (!pars) return false;
   
   if (fVerboseLevel > 0)  {
        cout << "\n-I- <SpdTrackFitterGF::FitTrack> Particle (pdg): " << fPdg
             << ", Points: " << points.size();// << endl;
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
   
   // Build track
   
   fN_proc++;
    
   Track* track = 0;
   
   track = BuildTrack(pars,points,track);
   
   if (!track) {
       fN_empty++;
       pars->SetErrorFlag(kSpdTrEmpty);
       delete track;
       return false;
   }
   
   if (track->getNumPoints() == 0) {
       fN_empty++;
       pars->SetErrorFlag(kSpdTrEmpty);
       delete track;
       return false;
   }
   
   // Set start track fit parameters
   
   pars->SetTrackPdgCode(fPdg);
   pars->SetNPoints(track->getNumPoints());
   
   // Fit data
   
   KalmanFitStatus* status = 0;
   const MeasuredStateOnPlane* first_state = 0;
   const MeasuredStateOnPlane* last_state = 0;
   Int_t first_hit_id(-1), last_hit_id(-1); 
   
   /*-------------------------------*/
   /* ATTENTION  Fitting  ATTENTION */
   /*-------------------------------*/
   
   Bool_t is_fitted = false;
   Bool_t refit_tracks = false;     
   Int_t  n_fit = 0;
   
   while (true) {
           
      try
      { 
         fFitter->processTrack(track);
         is_fitted = true;
      }
      catch(Exception& e)
      {
         is_fitted = false;
      }
   
      n_fit++;

      if (n_fit < fFitterMaxTrials) 
      {   
          status = (KalmanFitStatus*)track->getFitStatus();
          
          if (status && !status->isFitConverged()) 
          {
 
              if (fVerboseLevel > 3) { 
                  cout << "\n\t   Rebuild and refit track: " 
                       << n_fit << "/" << fFitterMaxTrials;
              }
              
              track = BuildTrack(pars,points,track); 

              Exception::clearEmessg();
              continue;
          }
      }
      
      track->checkConsistency();
      
      break;
   }

   //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   // Check track for FATAL errors & checking fit data 
   //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    
   if (!is_fitted) 
   {
       fN_notfitted++;
       pars->SetErrorFlag(kSpdTrNotFitted);
       delete track;
       return false;
   }
   
   status = (KalmanFitStatus*)track->getFitStatus();
   
   if (!status) {
       fN_notfitted++;
       pars->SetErrorFlag(kSpdTrNoStatus);
       delete track;
       return false;
   }
   
   if (!status->isFitted()) {
       fN_notfitted++;
       pars->SetErrorFlag(kSpdTrBadFitStatus);
       delete track;
       return false;
   }
   
   TrackPoint* point = 0;
  
   // get first point fit result
   point = track->getPointWithFitterInfo(0);
   if (point) {
       try { first_state = &(track->getFittedState(0)); }
       catch (Exception& e) { first_state = 0; }
   }
   
   if (!first_state) {
       fN_notfitted++;
       pars->SetErrorFlag(kSpdTrNoFirstState);
       delete track;
       return false;      
   }
   else {
       genfit::AbsMeasurement* meas = point->getRawMeasurement();
       if (meas) first_hit_id = meas->getHitId();
   }
   
   // get second point fit result
   point = track->getPointWithFitterInfo(-1);
   if (point) {
       try { last_state = &(track->getFittedState(-1)); }
       catch (Exception& e) { last_state = 0; }
   }
   
   if (!last_state) {
       fN_notfitted++;
       pars->SetErrorFlag(kSpdTrNoLastState);
       delete track;
       return false;      
   }
   else {
       genfit::AbsMeasurement* meas = point->getRawMeasurement();
       if (meas) last_hit_id = meas->getHitId();
   }
   
   /*ATTENTION Check if track is turned out ATTENTION*/
   
   if (fStartMomentum.Dot(first_state->getMom()) < 0) {
       fN_notfitted++;
       pars->SetErrorFlag(kSpdTrTurnedOut);
       delete track;
       return false;       
   }
   
   pars->SetErrorFlag(kSpdTrNoFatalErrors);
   
   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   
   // fill error/warning messages
   
   Int_t nem = Exception::getNEmessg();
   if (nem > 0) {
       fN_ewmsgs++;
       TString emsg;
       for (Int_t j(0); j<nem; j++) {
            emsg += Exception::getEmessg(j); 
            if (j != nem-1) emsg += "/";
       }
       Exception::clearEmessg();
       pars->SetErrorMesg(emsg);
   }

   //----------------------------------------------------------------
   // fill First & Last states

   SpdTrackState* state;
   Double_t sigma;
   
   // first state 
   state = pars->FirstState();
   
   state->SetMomentum(first_state->getMom());
   state->SetPosition(first_state->getPos());
   state->SetCovariance(first_state->get6DCov());
   state->SetTime(fStartTime+first_state->getTime());
   
   sigma = first_state->getMomVar();
   sigma = (sigma < 0) ? 0 : TMath::Sqrt(sigma);
   
   state->SetMomSigma(sigma);
   
   //points[0]->Print(); //FIXME
   
   // last state 
   state = pars->LastState();
   
   state->SetMomentum(last_state->getMom());
   state->SetPosition(last_state->getPos());
   state->SetCovariance(last_state->get6DCov());
   state->SetTime(fStartTime+last_state->getTime());
   
   sigma = last_state->getMomVar();
   sigma = (sigma < 0) ? 0 : TMath::Sqrt(sigma);
   
   state->SetMomSigma(sigma);
    
   //points[points.size()-1]->Print(); //FIXME
   
   //----------------------------------------------------------------
   // fill Fit parameters 
   
   pars->SetTrackLength(status->getTrackLen());
   pars->SetMaterialEffects(fMaterialsOpt);
   
   // set first/last point id
   if (!fHtoP.empty()) {
       std::map<Int_t,Int_t>::const_iterator it;
       it = fHtoP.find(first_hit_id);
       if (it != fHtoP.end()) pars->SetFirstPointId(it->second);
       it = fHtoP.find(last_hit_id);
       if (it != fHtoP.end()) pars->SetLastPointId(it->second);
   }
   else {
       pars->SetFirstPointId(-1);
       pars->SetLastPointId(-1);
   }
   
   // set chi2, ndf, dchi2
   Double_t chi2 = status->getForwardChi2();
   Int_t ndf = status->getForwardNdf();
   
   if (chi2 > 0 && ndf > 0) {
       pars->SetChi2(chi2);
       pars->SetNDF(ndf);
       if (TMath::Abs(chi2) > 1e-8) {
           Double_t dchi2 = (chi2-status->getBackwardChi2())/chi2;
           pars->SetDChi2(TMath::Abs(dchi2));
       }
       else pars->SetDChi2(0);
   }
   else {
       pars->SetChi2(-1);
       pars->SetNDF(1);
       pars->SetDChi2(-1);
   }
   
   // number of failed hits      
   pars->SetNFailedHits(status->getNFailedPoints()); 
             
   // convergency      
   pars->SetConvergency(0);
   if (status->isFitConvergedPartially()) pars->SetConvergency(-1);
   if (status->isFitConvergedFully()) pars->SetConvergency(1);   
    
   //----------------------------------------------------------------
   
   // fill counters 
   if (pars->GetIsFittedOk()) fN_fitted++;
   if (pars->GetConvergency() != 0) fN_conv++;
   if (pars->GetIsGood()) fN_good++;
   if (pars->GetIsAcceptable()) fN_acceptable++;
   
   // check if track charge was changed during the fit 
   RKTrackRep* rep = (RKTrackRep*)track->getCardinalRep();
   
   Double_t charge1 = rep->getPDGCharge();
   Double_t charge2 = rep->getCharge(*last_state);
   Bool_t is_changed = (charge1*charge2 > 0) ? false : true;
   
   if (is_changed) pars->ChangeTrackCharge();
   
   //cout << endl;
   //cout << "q(pdg): " << charge1 << " pdg: " << rep->getPDG();
   //cout << ";  q(fit): " << charge2; 
   //cout << ";  is changed: " << is_changed; 
   //cout << endl; 
   
   fTrack = track; 
      
   if (fStoreImPoints) AddIntermediateHits(pars,points);
   
   //pars->Print("imstates");
      
   return true;
}

//_____________________________________________________________________________                                                                      
Track* SpdTrackFitterGF::BuildTrack(SpdTrackFitPar* pars, const std::vector<SpdHit*>& points, 
                                    Track* track)
{
    if (track) {
        delete track;
        track = 0;
    }
   
    // init start position & momentum
    TVector3 vertex = fStartVertex;
    TVector3 momentum = fStartMomentum;
    
    if (fStartSeedMethod == 0) 
    {
        if (fSmearStartVertex) SmearStartVertex(vertex); 
        if (fSmearStartMom) SmearStartMom(momentum);
    }
    else 
    {
        if (!DefineStartParameters(points,vertex,momentum))
        {
            vertex   = fStartVertex;
            momentum = fStartMomentum;

            if (fSmearStartVertex) SmearStartVertex(vertex); 
            if (fSmearStartMom) SmearStartMom(momentum);
        }
    }
    
    // --------- create & init track representation --------- 
    
    AbsTrackRep* rep = new SpdRKTrackRep(fPdg);
         
    // set start values: state, position, momentum, covariance
   
    MeasuredStateOnPlane state(rep);
    rep->setPosMomCov(state, vertex, momentum, fStartCovariance);
    
    // --------- create & init track --------- 
    
    TVectorD    seedState(6);
    TMatrixDSym seedCov(6);
    
    rep->get6DStateCov(state, seedState, seedCov);
    
    track = new Track(rep, seedState, seedCov);
     
    // add hits to the track  

    TrackPoint* point;
    
    fHitCreator->ResetHitCounter();
    fHtoP.clear();
    
    Int_t np(0);
    for (auto ihit : points) 
    {
        std::vector<AbsMeasurement*> measurements = fHitCreator->CreateHit(ihit);
        if (measurements.size() > 0) {
            fHtoP.insert(std::pair<Int_t,Int_t>(fHitCreator->GetLastHitId(),fTrackPointId[np]));
            point = new TrackPoint(measurements,track);
            track->insertPoint(point);
        }
        np++;
    }
    
    //cout << "-I- <SpdTrackFitterGF::BuildTrack> Points: " <<  np << endl;
    
    // fill initaial state parameters
    
    SpdTrackState* istate = pars->GetInitialState();
    
    istate->SetMomentum(momentum);
    istate->SetPosition(vertex);
    istate->SetCovariance(seedCov);
    istate->SetTime(fStartTime);
    
    istate->SetPosDelta((fStartVertex-vertex).Mag());
    
    Double_t psigma = fStartCovariance(3,3) + fStartCovariance(4,4) + fStartCovariance(5,5);
                    
    istate->SetMomSigma(TMath::Sqrt(psigma));
    
    //istate->Print();

    return track; 
}

//________________________________________________________________________________________
void SpdTrackFitterGF::SmearStartMom(TVector3& mom)
{
    Double_t theta0 = fStartMomentum.Theta()*TMath::RadToDeg();
    Double_t phi0   = fStartMomentum.Phi()*TMath::RadToDeg();
    Double_t p0     = fStartMomentum.Mag();
    
    const Int_t nmax(100);
    Double_t theta, phi, p;
    Int_t n;

    n = 0;
    while (true) 
    {
       theta = gRandom->Gaus(theta0,fSmearingDMomTheta);
       if (theta < 0) theta = -theta; 
       if (theta > 180) theta = 360 - theta;
       if (!(theta < 0) && !(theta > 180)) break;
       if (++n == nmax) { theta = theta0; break; }
    }
    
    n = 0;
    while (true) 
    {
       phi = gRandom->Gaus(phi0,fSmearingDMomPhi);
       if (phi < -180) phi = 360 + phi; 
       if (phi > 180) phi = phi - 360;
       if (!(phi < -180) && !(phi > 180)) break;
       if (++n == nmax) { phi = phi0; break; }
    }
    
    n = 0;
    while (true) {
      p = p0*gRandom->Uniform(1-fSmearingDMom,1+fSmearingDMom);
      if (p > 0) break;
      if (++n == nmax) { p = p0; break; }
    }
        
    //cout << "p,theta,phi:  " << p0 << "/" << p << "; " 
    //                         << theta0 << "/" << theta*TMath::DegToRad() << "; "
    //                         << phi0 << "/" << phi*TMath::DegToRad() << endl;

    mom.SetMagThetaPhi(p,theta*TMath::DegToRad(),phi*TMath::DegToRad()); 
}

//________________________________________________________________________________________
void SpdTrackFitterGF::SmearStartVertex(TVector3& v)
{
    Double_t x = gRandom->Uniform(v.X() - fSmearingDVxyz, v.X() + fSmearingDVxyz);
    Double_t y = gRandom->Uniform(v.Y() - fSmearingDVxyz, v.Y() + fSmearingDVxyz);
    Double_t z = gRandom->Uniform(v.Z() - fSmearingDVxyz, v.Z() + fSmearingDVxyz);
    
    //cout << "x,y,z:  " << fStartVertex.X() << "/" << x << "; " 
    //                   << fStartVertex.Y() << "/" << y << "; "
    //                   << fStartVertex.Z() << "/" << z << endl;
    
    v.SetXYZ(x,y,z);
}

//________________________________________________________________________________________
void SpdTrackFitterGF::SetCovarianceMatrix() 
{    
    fStartCovariance.Clear();
    fStartCovariance.ResizeTo(6,6);    
    
    for (Int_t i(0); i < 3; i++) 
    {
         fStartCovariance(i,i) = 1.e6;     // position
         fStartCovariance(i+3,i+3) = 1.e4; // momentum
    }
}

//________________________________________________________________________________________
Bool_t SpdTrackFitterGF::DefineStartParameters(const std::vector<SpdHit*>& hits, TVector3& v, TVector3& mom) 
{
    Int_t np = hits.size();

    std::vector<TVector3> points;
    points.reserve(np);
    
    SpdMCSiliconHit* hit;
    for (auto ih : hits) {
         hit = dynamic_cast<SpdMCSiliconHit*>(ih);
         if (hit) points.push_back(hit->GetHit());
    }
    
    if (points.size() < 1) {
        if (fVerboseLevel > 3) {
            cout << "\n-W- <SpdTrackFitterGF::DefineStartParameters> Not enough ITS points: " 
                 << points.size();// << endl;
        }
        return false;
    }

    // ----- Set momentum -----
    
    TVector3 dir;
    
    if (points.size() == 1) dir = points[0];
    else dir = points[1]-points[0];
    
    if (dir.Dot(mom) < 0) {
        if (fVerboseLevel > 3) {
            cout << "\n-W- <SpdTrackFitterGF::DefineStartParameters> poor track start directon " 
                 << "(its points: " <<  points.size() << "), p = " << mom.Mag()*1e3 << " [MeV] ";// << endl;
        }
        return false;
    }
    
    Double_t p0 = 1.; // FIXME
    
    if (fSmearStartMom) p0 *= gRandom->Uniform(0.95,1.05);

    // ----- Set vertex position -----
    
    TVector3 na(0,0,1), a(0,0,0);
    TVector3 nb(dir.Unit()), b(points[0]);
    
    TVector3 nc = na.Cross(nb);
    double mag2 = nc.Mag2();
    
    if (mag2 == 0) {
        if (fVerboseLevel > 3) {
            cout << "\n-W- <SpdTrackFitterGF::DefineStartParameters> poor track start directon, " 
                 << "theta: " << dir.Theta()*TMath::RadToDeg();// << endl;
        }
        return false;
    }
    
    TVector3 s = b-a;
    
    double ka = (s.Cross(nb)).Dot(nc)/mag2;
    TVector3 A = a+ka*na; // point on z-axis 
    
    double kb = (s.Cross(na)).Dot(nc)/mag2;
    TVector3 B = b+kb*nb; // nearest point to z-axis from the track=(b,dir)
    
    //cout << "\n\nV0: " << v.X() << " " << v.Y() << " " << v.Z() << endl;
    //cout << "ka = " << ka << " kb = " << kb << " mag2 = " << mag2 << endl;
    //cout << " s = " << s.X() << ", " << s.Y() << " " << s.Z() << endl;
    //cout << " b = " << b.X() << ", " << b.Y() << " " << b.Z() << endl;
    //cout << " nb = " << nb.X() << ", " << nb.Y() << " " << nb.Z() << endl;
    
    if ((B-v).Mag() > 10) {
        if (fVerboseLevel > 3) {
            cout << "\n-W- <SpdTrackFitterGF::DefineStartParameters> poor start vertex, " 
                 << "|v(seed)-v(mc)| > 10 cm : " << ((B-v).Mag()) << " [cm], "
                 << "p = " << mom.Mag()*1e3 << " [MeV] "; // << endl;
        }
        return false; 
    }
    
    if (fSmearStartVertex) {
        TVector3 n = B-A;
        Double_t l = gRandom->Uniform(0,1)*n.Mag();   
        v = A+l*n.Unit();
    }
    else v = B;
    
    //cout << "A: " << A.X() << " " << A.Y() << " " << A.Z() << endl;
    //cout << "B: " << B.X() << " " << B.Y() << " " << B.Z() << endl;
    //cout << "V: " << v.X() << " " << v.Y() << " " << v.Z() << endl;
    
    mom = dir;
    mom.SetMag(p0);
    
    return true;
}

//________________________________________________________________________________________
void SpdTrackFitterGF::ResetCounter()
{
   fN_proc = 0;
   fN_empty = 0;
   fN_notfitted = 0;
   fN_ternedout = 0;
   fN_ewmsgs = 0;   
   fN_fitted = 0;
   fN_conv = 0;
   fN_good = 0;
   fN_acceptable = 0;
}

//________________________________________________________________________________________
void SpdTrackFitterGF::DeleteTrack()
{
   if (fTrack) {
       delete fTrack;
       fTrack = 0;
   }  
}
 
//________________________________________________________________________________________
void SpdTrackFitterGF::PrintCounter()
{
   cout << "\n\t-I- <SpdTrackFitterGF::PrintCounter> " << "\n\n";
   
   cout << "\tTotal number of processed tracks:      " << fN_proc << endl;
   cout << "\t[fatal_error] \"Empty\" tracks:          " << fN_empty << endl;
   cout << "\t[fatal_error] \"Not-fitted\" tracks:     " << fN_notfitted << endl;
   cout << "\t[fatal_error] \"Turned out\" tracks:     " << fN_ternedout << endl;
   cout << "\t[error] \"Error & warning\" tracks:      " << fN_ewmsgs << endl;
   cout << "\tFitted tracks:                         " << fN_fitted << endl;
   cout << "\t\"GF-converged\" tracks:                 " << fN_conv << endl;
   cout << "\t\"Good-fitted\" tracks:                  " << fN_good << endl;
   cout << "\t\"Acceptable\" tracks (chi2/ndf < 2):    " << fN_acceptable << endl;
   //cout << endl;
}

//________________________________________________________________________________________
void SpdTrackFitterGF::AddIntermediateHits(SpdTrackFitPar* pars, const std::vector<SpdHit*>& hits) //-> private
{
   if (!fTrack || !pars) return;
   
   const std::vector<genfit::TrackPoint*>& points = fTrack->getPointsWithMeasurement();
   const AbsTrackRep* rep = fTrack->getCardinalRep();
   const TrackPoint* point;
   
   //cout << "\n-I- <SpdTrackFitterGF::AddIntermediateHits> hits/points: " << points.size() << " | " << fHtoP.size() << endl;
   //cout << "\n-I- <SpdTrackFitterGF::AddIntermediateHits> first/last point id: "
   //     << pars->GetFirstPointNum() << " | " << pars->GetLastPointNum() << endl;
   
   std::map<Int_t,Int_t>::const_iterator itx;
   const genfit::AbsMeasurement* meas;
   Int_t hit_id, point_id;  
   
   SpdTrackState2* imstate;
   std::vector<SpdTrackState2*>& imstates = pars->ImStates();
   if (!imstates.empty())  pars->RemoveImStates(); 
   imstates.reserve(points.size());
   
   const Bool_t biased = true; 
   
   //Int_t ii(-1);
   for (std::vector<TrackPoint*>::const_iterator it = points.begin(); 
        it != points.end(); 
        ++it) 
   {
      //ii++;
    
      if (!(*it)->hasFitterInfo(rep)) continue;
      
      point = *it;
      meas = point->getRawMeasurement();
      
      if (meas) {
          hit_id = meas->getHitId();
          itx = fHtoP.find(hit_id);
          point_id = (itx != fHtoP.end()) ?  itx->second : -1;
      }
      else {
          hit_id = -1;
          point_id = -1;
          continue;
      }
      
      if (  hit_id == -1 || 
          point_id == -1 || 
          point_id == pars->GetFirstPointId() ||
          point_id == pars->GetLastPointId() ) continue;
      
      const MeasuredStateOnPlane& fitted_state = point->getFitterInfo(rep)->getFittedState(biased);
      
      //cout << hit_id << " -> " << point_id << " -> " << ii << endl;
      
      imstate = new SpdTrackState2();
      
      imstate->SetTime(fStartTime + fitted_state.getTime());
      imstate->SetPointId(point_id);
      
      if (fStoreImMomentum) imstate->SetMomentum(fitted_state.getMom());
      if (fStoreImPosition) imstate->SetPosition(fitted_state.getPos());    
      if (fStoreImCovariance) imstate->SetCovariance(fitted_state.get6DCov());
     
      imstates.push_back(imstate);
     
      //fitted_state.Print();
      
      /*
      if (point_id != ii) continue;
      
      TVector3 mc_pos = ((SpdMCTrackHit*)hits[point_id])->GetMCPoint();
      TVector3 fit_pos = fitted_state.getPos();
      
      cout << " mc_pos       = " << " (" << mc_pos.X() << " " << mc_pos.Y() << " " << mc_pos.Z() 
                               << ")  mag = " << mc_pos.Mag()  << endl;
      cout << " fit_pos      = " << " (" << fit_pos.X() << " " << fit_pos.Y() << " " << fit_pos.Z() 
                               << ") mag = " << fit_pos.Mag() << endl;
      cout << " delta(pos)   = " << (fit_pos-mc_pos).Mag() << endl;
      cout << " fit_mom(mag) = " << fit_mom.Mag() << endl;
      cout << " time (ns)    = " << fStartTime + fitted_state.getTime() << endl;
      */
   }
}

