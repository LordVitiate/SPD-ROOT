// $Id$
// Author: vladimir andreev   2020/08/29

//_____________________________________________________________________________
//
// SpdIdealV0Finder
//_____________________________________________________________________________

#include <TRandom.h>
#include <TVectorD.h>
#include <TMatrixDSym.h>

#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRunAna.h"

#include "SpdFieldCreator.h"
#include "SpdFieldPar.h"

#include "SpdIdealV0Finder.h"

#include "SpdGeoLoader.h"
#include "SpdKFSimpleRes.h"

#include "SpdIdealTrackData.hh"

#include "SpdGFMagneticField.h"

#include "SpdKalmanFitterRefTrack.h"
#include "KalmanFitterRefTrack.h"
#include "KalmanFitter.h"

#include "RKTrackRep.h"
#include "MeasuredStateOnPlane.h"
#include "Track.h"
#include "TrackPoint.h"
#include "Exception.h"
#include "KalmanFitStatus.h"

#include <vector>
#include <iostream>

using std::cout;
using std::endl;
using std::vector;

#include "GFCore.hh"
#include "GFTrackReps.hh"

#include "SpdGFTrackW.h"

using namespace genfit;

//using TMath::SignBit;

ClassImp(SpdIdealV0Finder)

//_____________________________________________________________________________
SpdIdealV0Finder::SpdIdealV0Finder():FairTask("SPD Ideal V0 finder"),
fField(0),fMeasCreator(0),fFitter(0),fOwnGeoLoader(false),fGeoLoader(0),
fEvent(0),fKFres(0),fTracks(0),fParameters(0),
fVerboseLevel(1),
// Fitter options: 
fMaterialsOpt(1),
fSeedMomentum(1.),
fVertexFitMethod(1),
fVertexFitAngleCut(3.0),
fStartSeedMethod(2),
fSmearSeedValues(true), 
fSmearingMomDelta(0.1),
fSmearingAngleDelta(3.0),
fSmearingVertexDelta(0.5),
fNPointsVertexUse(0),
fFitterMaxIterations(8),
fFitterMaxTrials(10),
fFitterOption(1),
fStoreTracks(false),
// V0 finder
fFirstDaughter(321),
fSecondDaughter(211),
fUsedTypeOfPV(0),
fConstrainToPV(1),
fMinChi2PV(2.0),
fMinDist(0.075),
fMinMass(0.00),
fMaxMass(2.50)
{
 
}

//_____________________________________________________________________________
SpdIdealV0Finder::~SpdIdealV0Finder() 
{
    if (fField) delete fField;
    if (fMeasCreator) delete fMeasCreator;
    if (fFitter) delete fFitter;

    if (fGeoLoader) delete fGeoLoader;
       
    if (fEvent)  delete fEvent;  
    if (fKFres)  delete fKFres; 
    if (fTracks) delete fTracks;  

    if (fV0file) delete fV0file;  
    if (fV0tree) delete fV0tree;  
}

//_____________________________________________________________________________
void SpdIdealV0Finder::DeleteGeoLoader()
{
    if (fOwnGeoLoader && fGeoLoader) {
        cout << "-I- <SpdIdealD0Finder::DeleteGeoLoader> Remove geometry loader" << endl;
        delete fGeoLoader;
        fGeoLoader = 0;
        fOwnGeoLoader = false;
    }

}

//_____________________________________________________________________________
void SpdIdealV0Finder::SetTrackingGeoModules(Bool_t add_passives)
{
    fGeoModules.clear(); 
    if (add_passives) fGeoModules.push_back("Passives");
    
    fGeoModules.push_back("Inner tracker system");
    fGeoModules.push_back("TS barrel (tor)");
    fGeoModules.push_back("TS endcaps (tor)");
}

//_____________________________________________________________________________
void SpdIdealV0Finder::LoadGeometry() 
{ 
    fGeoLoader = SpdGeoLoader::Instance();

    if (!fGeoLoader) { 
        fGeoLoader = new SpdGeoLoader(); 
        fOwnGeoLoader = true; 
    }
    else fOwnGeoLoader = false;

    if (fMaterialsOpt < 1) fGeoLoader->UnsetMaterials(true); // ATTENTION
    
    fGeoLoader->LoadGeometry();   // construct TOP level only (cave)
    
    //return;
    
    Int_t n = fGeoModules.size();
    
    if (n < 1) return;
    
    for (Int_t i(0); i<n; i++) {
         if (fGeoModules[i].IsDigit()) fGeoLoader->LoadModule(fGeoModules[i].Atoi());
         else fGeoLoader->LoadModule(fGeoModules[i]);
    }

    fGeoLoader->PrintActualGeometry();
    
    MaterialEffects::getInstance()->init(new TGeoMaterialInterface());
}

//_____________________________________________________________________________
void SpdIdealV0Finder::LoadField()
{
    FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();  
    SpdFieldCreator* fieldCreator = new SpdFieldCreator();    
    SpdFieldPar *fpars = (SpdFieldPar*)rtdb->getContainer("SpdFieldPar");
    
    fField = new SpdGFMagneticField();
    
    if (fpars) {
       ((FairParSet*)fpars)->init();
        SpdField* field = (SpdField*)fieldCreator->createFairField(fpars);
        fField->SetField(field,kTRUE);
        if (field) field->Print(""); 
    }
    
    FieldManager::getInstance()->init(fField);
}

//_____________________________________________________________________________
void SpdIdealV0Finder::InitFitter()
{
    if (fFitter) delete fFitter;
    if (fFitterOption == 1) fFitter = new KalmanFitterRefTrack(fFitterMaxIterations,1e-3,1e3,true);
    else fFitter = new KalmanFitterRefTrack(fFitterMaxIterations,1e-3,1e3,false);
    //fFitter = new SpdKalmanFitterRefTrack(fFitterMaxIterations,1e-3,1e3,true);
}

//_____________________________________________________________________________
InitStatus SpdIdealV0Finder::Init() 
{   
    // get RootManager
    
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- <SpdIdealV0Finder::Init> "
             << "RootManager not instantiated!" << endl;
        return kFATAL;
    }
    
    // Load Field, Geometry 
   
    LoadField();
    LoadGeometry();
    
    // Init 
    fMeasCreator = new SpdGFMeasurementCreator();
   
    // Init Fitter
    InitFitter();
    
    // get Input data objects
    
    fEvent = (SpdIdealTrackColl*)ioman->GetObject("IdealTrackColl.");
   
    if (!fEvent) {
        cout << "-E- <SpdIdealV0Finder::Init>" << "No HEADER object inside " << endl;
        return kERROR;
    }
    
    TFile* infile = ioman->GetInFile();
    if (infile) fParameters = (SpdBaseParSet*)infile->Get("IdealTrackFinderParams");
    if (!fParameters) fParameters = new SpdBaseParSet();
    
    fParameters->print(1);
    
    FillParametersIn(fParameters);
    
    TFile* f = gFile;
    TFile* ff = ioman->GetOutFile();
    ff->cd();
//    fParameters->Write("IdealKalmanFitterParams",TObject::kOverwrite);
    fParameters->Write("IdealV0FinderParams",TObject::kOverwrite);
    gFile = f;

    // Create and register output data objects
    
    fKFres = new SpdKFSimpleRes();
    ioman->Register("SpdKFSimpleRes.", "IdealKFres", fKFres, kTRUE);
    
    fTracks = new TObjArray(100);
    if (fStoreTracks) ioman->Register("SpdGFTrackW.", "GFTracks", fTracks, kTRUE);
    
    cout << "-I- <SpdIdealV0Finder::Init> Intialization successfull " << endl;

    // name of output root-file for V0 particle
    //--------------------------------------------
    FairRunAna* userFile = FairRunAna::Instance();
    if (!userFile) {
        cout << "-E- <SpdIdealV0Finder::Init> "
             << "FairRunAna not instantiated!" << endl;
        return kFATAL;
    }

    TString v0_outfile_name = userFile->GetUserOutputFileName();
    InitV0nTuple( v0_outfile_name);

    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdIdealV0Finder::Exec(Option_t* opt) 
{
    //cout << "-I- <SpdIdealV0Finder::Exec> " << endl;
    
    if (!fEvent)  return;
    if (!fTracks) return;
    if (!fKFres)  return;

    //if (fVerboseLevel > 0) 
        cout << "-I- <SpdIdealV0Finder::Exec> Event: " << fEvent->GetEventId() << endl;
    
    fKFres->ClearData();
    fTracks->Delete();
    
    // Init Fitter
    //InitFitter();
    
    fKFres->SetRunId(fEvent->GetRunId());
    fKFres->SetEventId(fEvent->GetEventId());
    fKFres->SetNPrimTracks(fEvent->GetNPrimTracks());
    
    TVector3 mcMom, recMom;
    Track* track(0);
   
    TObjArray* tracks = fEvent->GetTracks(); 
    Int_t NTracks = tracks->GetEntriesFast();
    
    fKFres->SetNSelectedTracks(NTracks);
    
    Int_t n;
    
    SpdIdealDataInfo* dinfo_its   = fEvent->FindDataInfo(kSpdIts);
    SpdIdealDataInfo* dinfo_tstb  = fEvent->FindDataInfo(kSpdTsTB);
    SpdIdealDataInfo* dinfo_tstec = fEvent->FindDataInfo(kSpdTsTEC);
        
    KalmanFitStatus* status;
   
    Bool_t material_effects;
    
    static Int_t ntotal_tracks = 0;
    static Int_t nconv_tracks = 0;
    static Int_t ncorrd = 0;
    static Int_t not_fitted = 0;
    static Int_t n_errors = 0;
    static Int_t n_good = 0;
  
    Int_t  n_refit;
    Bool_t refit_tracks = false;

    if (fMaterialsOpt < 1) {
        material_effects = false;
        MaterialEffects::getInstance()->setNoEffects(true);
    }
    else {
        material_effects = true;
        MaterialEffects::getInstance()->setNoEffects(false);
    }
    
    Exception::setEmessgflag(true);
    
    for (Int_t i(0); i<NTracks; i++) {
    
         SpdIdealTrack* itrack = (SpdIdealTrack*)tracks->At(i);
         
         if (fVerboseLevel > 3) {
             cout << " >>>>>>>  <SpdIdealV0Finder::Exec> track (start): " << i << " / event: " << fEvent->GetEventId() 
                  << " points = " << itrack->GetNPoints() << endl;
         }
         
         //itrack->PrintTrack();
         //continue;
         
         ntotal_tracks++;
         
         n = fKFres->AddTrack(itrack->GetTrackId(),itrack->GetPdgCode());
  
         if (dinfo_its) fKFres->SetNVhits(n,dinfo_its->GetTrackHits(n));
         if (dinfo_tstb) fKFres->SetNTBhits(n,dinfo_tstb->GetTrackHits(n));
         if (dinfo_tstec) fKFres->SetNTEChits(n,dinfo_tstec->GetTrackHits(n));

         // build track
         track = BuildTrack(i);
         if (!track) continue;
         
         Bool_t is_fitted = false;
         
         n_refit = 0;
         
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
    
            if (n_refit < fFitterMaxTrials) {    
                status = (KalmanFitStatus*)track->getFitStatus();
                if (status && !status->isFitConverged()) {
                    n_refit++;
                    if (fVerboseLevel > 2) cout << "\t   Rebuild and refit track: " << n_refit << "/" << fFitterMaxTrials << endl;
                    track = BuildTrack(i,track); 
                    Exception::clearEmessg();
                    continue;
                }
            }
            
            //track->checkConsistency();
            
            break;
         }
         
         Int_t nem = Exception::getNEmessg();
         if (nem > 0) {
             n_errors++;
             TString emsg;
             for (Int_t j(0); j<nem; j++) {
                  emsg += Exception::getEmessg(j); 
                  if (j != nem-1) emsg += "/";
             }
             Exception::clearEmessg();
            
             fKFres->SetErrorFlag(n,1);
             fKFres->SetErrorMessage(n,emsg);
             
             if (fVerboseLevel > 2) {
                 cout << "-W- <SpdIdealV0Finder::Exec> Fitter errors: " << emsg << endl; 
             }
         }
  
         if (!is_fitted) { 
             not_fitted++; 
             mcMom = itrack->GetStartMomentum();
             fKFres->SetPmc(n,mcMom);
             delete track; 
             continue; 
         }
        
         status = (KalmanFitStatus*)track->getFitStatus();
  
         if (status) {

             is_fitted = status->isFitted();
             fKFres->SetIsFitted(n,is_fitted);
             
             fKFres->SetMaterialEffects(n,material_effects);
             fKFres->SetNFailedHits(n,status->getNFailedPoints()); 
             fKFres->SetConvergency(n,0);
             if (status->isFitConvergedPartially()) fKFres->SetConvergency(n,-1);
             if (status->isFitConvergedFully()) fKFres->SetConvergency(n,1);   
         }
         else {
             
             is_fitted = false;
             fKFres->SetIsFitted(n,is_fitted);
             
             fKFres->SetMaterialEffects(n,material_effects);
             fKFres->SetConvergency(n,0);
             
             fKFres->AddErrorMessage(n,"Unknown_error:_no_fit_status");
             
             if (!(nem > 0)) n_errors++;
 
             if (fVerboseLevel > 2) {
                 cout << "-W- <SpdIdealV0Finder::Exec> Fitter errors: " << fKFres->GetErrorMessage(n) << endl;
             }
         }
     
         if (fVerboseLevel > 3) {
             cout << " >>>>>>>  <SpdIdealV0Finder::Exec> track (finish): " << i << " is fitted: " << is_fitted << endl;
         }
        
         if (!is_fitted) { 
             not_fitted++; 
             mcMom = itrack->GetStartMomentum();
             fKFres->SetPmc(n,mcMom);
             fKFres->AddErrorMessage(n,"not_fitted_track");
             delete track; 
             continue; 
         }
 
         if (fKFres->GetConvergency(n)) nconv_tracks++;
        
         mcMom  = itrack->GetStartMomentum();
         
         track->checkConsistency();
         
         // let's try to get averaged final state
         try {
             recMom = track->getFittedState().getMom();
         }
         catch (Exception& e) {
             is_fitted = false;
         }
        
         // ill-conditioned covariance matrix (KalmanFitterInfo::calcAverageState)
         if (!is_fitted) {
             fKFres->SetIsFitted(n,is_fitted);
             not_fitted++; 
             fKFres->SetPmc(n,mcMom);
             fKFres->AddErrorMessage(n,"ill-conditioned_cov._matrix_in_the_final_state");
             delete track; 
             continue; 
         }
         
         if (fVerboseLevel > 3) 
           {
//             cout << " >>>>>>>>>  <SpdIdealV0Finder::Exec> (finish 1): track = " 
             cout << " track = " 
                  << i << " / event: " << fEvent->GetEventId() 
                  << " mom (mc) = " << itrack->GetStartMomentumMag() 
                  << " mom (rec) = "   << recMom.Mag() 
                  << " pdg = " << itrack->GetPdgCode() << endl;
         
//             cout <<" >>>>>>>>>  <SpdIdealV0Finder::Exec> (finish 2): track = " << i  << " / event: " << fEvent->GetEventId() 
//                  << " mom (rec) = " << recMom.Mag() << endl;
         }
        
         // particle charge correction FIXME
         if (mcMom.Dot(recMom) < 0) {
             recMom *= -1; 
             fKFres->AddErrorMessage(n,"Track_is_turned_out");
             
             ncorrd++;
             if (!(nem > 0)) n_errors++;
              
             if (fVerboseLevel > 2) cout << "-W- <SpdIdealV0Finder::Exec> Fitter errors: " << fKFres->GetErrorMessage(n) << endl; 
         }
         
         fKFres->SetSeedMomentum(n,fStartMomentum);
         fKFres->SetSeedVertex(n,fStartVertex);
         fKFres->SetSeedCov(n,fStartSeedCov); 
         
         fKFres->SetPmc(n,mcMom);
         fKFres->SetPrec(n,recMom);
         
         if (fKFres->GetIsGood(n)) n_good++;
         
         Double_t chi2 = status->getForwardChi2();
         Int_t ndf = status->getForwardNdf();
      
         if (chi2 > 0 && ndf > 0) {
             fKFres->SetChi2(n,chi2);
             fKFres->SetNDF(n,ndf);
             if (TMath::Abs(chi2) > 1e-8) fKFres->SetChi2Delta(n,(chi2-status->getBackwardChi2())/chi2);
         }
         
//         if (fVerboseLevel > 1) 
         if (fVerboseLevel > 3) 
         {
             cout << " >>>>>>>  <SpdIdealV0Finder::Exec> Fitted tracks (nice/total): " 
                  << n_good << "/" << ntotal_tracks << " = " << 0.01*Int_t(10000.*(1-n_good/Double_t(ntotal_tracks))) << " % "  
                  << " (" << ncorrd << ") " 
                  << " n/fitted: " << not_fitted 
                  << "; n/converged: " << ntotal_tracks-nconv_tracks
                  << "; errors: " << n_errors 
                  << "; chi2/NDF: " << status->getForwardChi2() << "/" << status->getForwardNdf() 
                  << " => " << fKFres->GetChi2toNDF(n) << ", d(chi2) = " << fKFres->GetChi2Delta(n) << endl;
         }
         
         track->setMcTrackId(n); // ATTENTION here n is NOT mc-track id (mc-track id = itrack->GetTrackId())
         
         fTracks->Add(new SpdGFTrackW(track,n));
    }
    
    Exception::setEmessgflag(false);
    
    if (fVerboseLevel > 2) cout << "\n >>>>>>> VERTEX RECONSTRUCTION " << endl;
    
    //return;
  
    // Interaction vertex
    
    TVector3 v_mc = fEvent->GetVertex();
    TVector3 v_rec;

    if (fVertexFitMethod > 0) FindVertex(v_rec); 
    
    fKFres->SetVertex(v_mc,v_rec);

    NTracks = fTracks->GetEntriesFast();
    SpdGFTrackW* t;
     
    for (Int_t i(0); i<NTracks; i++) 
    {
         t = (SpdGFTrackW*)fTracks->At(i);
         n = t->GetDataID();
    
         MeasuredStateOnPlane s = t->GetTrack()->getFittedState(); 
         TMatrixDSym cov = s.get6DCov().GetSub(3,5,3,5);
        
         fKFres->SetMomCov(n,cov);
    }

    FindDecayVertex();            // V0 vertex finder

    //fKFres->PrintData();
    //cout << "\n\n";
    
    fEvent->DeleteAll();
}

//________________________________________________________________________________________
genfit::Track* SpdIdealV0Finder::BuildTrack(Int_t n, genfit::Track* track)
{
    if (track) {
        delete track;
        track = 0;
    }
    
    TObjArray* tracks = fEvent->GetTracks(); 
    if (!tracks) return 0;
    
    SpdIdealTrack* itrack = (SpdIdealTrack*)tracks->At(n);
    if (!itrack) return 0;
    
    SpdIdealHit*  ihit;
    TrackPoint*   point;
 
    AbsTrackRep* rep = new RKTrackRep(itrack->GetPdgCode());
 
    // (1) init start state
 
    MeasuredStateOnPlane state(rep);
    
    // (2) init start position and momentum
    
         if (fStartSeedMethod == 1) DefineTrackStartVertexAndMom(itrack);
    else if (fStartSeedMethod == 2) DefineTrackStartVertexAndMom(n,itrack);
    else DefineTrackStartVertexAndMom(itrack);
        
    // (3) init covariance

    DefineStartVertexCovariance();
    
    // set start values: state, position, momentum, covariance
    
    rep->setPosMomCov(state, fStartVertex, fStartMomentum, fStartSeedCov);
    
    // --------- create track --------- 
    
    TVectorD    seedState(6);
    TMatrixDSym seedCov(6);
    
    rep->get6DStateCov(state, seedState, seedCov);

    track = new Track(rep, seedState, seedCov);

    // add hits to the track  
     
    Int_t NTrackHits = itrack->GetNHits();
          
    for (Int_t j(0); j<NTrackHits; j++) {
    
         ihit = (SpdIdealHit*)itrack->GetHit(j);
         vector<AbsMeasurement*> measurements = fMeasCreator->CreateHit(ihit,GetHitRepresentation(ihit->GetDetId()));
         point = new TrackPoint(measurements,track);
         track->insertPoint(point);
    }
    
    return track;
}

//________________________________________________________________________________________
void SpdIdealV0Finder::SetHitRepresentation(Int_t DetId, GFMeasurementType GFhitType)
{
  fHitRepresentation[DetId] = GFhitType;
  
//   for (auto& it1 : fHitRepresentation) {
//        cout << "Size: " << fHitRepresentation.size() 
//             << "  Detector/Hit: " << it1.first << " " << it1.second << endl;
//   }
}

//________________________________________________________________________________________
GFMeasurementType SpdIdealV0Finder::GetHitRepresentation(Int_t DetId) const
{
  auto it = fHitRepresentation.find(DetId);
  return (it != fHitRepresentation.end()) ? GFMeasurementType(it->second) : pkSpacepoint;
}
 
//________________________________________________________________________________________
void SpdIdealV0Finder::DefineTrackStartVertexAndMom(const SpdIdealTrack* track)
{
    fStartVertex = track->GetPrimVertex();
    fStartMomentum = track->GetStartMomentum();
     
    fStartMomentum.SetMag(fSeedMomentum);
    
    if (!fSmearSeedValues) return;
             
    fStartVertex.SetX(gRandom->Uniform(fStartVertex.X()-fSmearingVertexDelta,fStartVertex.X()+fSmearingVertexDelta)); // cm
    fStartVertex.SetY(gRandom->Uniform(fStartVertex.Y()-fSmearingVertexDelta,fStartVertex.X()+fSmearingVertexDelta)); // cm
    fStartVertex.SetZ(gRandom->Uniform(fStartVertex.Z()-fSmearingVertexDelta,fStartVertex.X()+fSmearingVertexDelta)); // cm 

    Double_t theta = gRandom->Gaus(fStartMomentum.Theta(),fSmearingAngleDelta*TMath::DegToRad());
    Double_t phi = gRandom->Gaus(fStartMomentum.Phi(),fSmearingAngleDelta*TMath::DegToRad());
    
    fStartMomentum.SetMagThetaPhi(gRandom->Uniform(fSeedMomentum-fSmearingMomDelta,fSeedMomentum+fSmearingMomDelta),theta,phi);
}

//________________________________________________________________________________________
void SpdIdealV0Finder::DefineTrackStartVertexAndMom(const Int_t ntrack, const SpdIdealTrack* track) 
{
    SpdIdealDataInfo* info = fEvent->FindDataInfo(kSpdIts);
    
    Int_t np(0);
    if (info) np = info->GetTrackHits(ntrack);
        
    if (np < 1) cout << "-W- <SpdIdealD0Finder::TrackStartVertexAndMom> Its points (data): " << np << endl;
    
    TObjArray* hits = track->GetHits();
    Int_t nhits = hits->GetEntriesFast();
    SpdIdealHit* xhit;
    SpdIdealSpacepointHit* hit;
    
    Long_t id;
      
    std::vector<TVector3> points;
    if (np > 0) points.reserve(np);
    
    Int_t na = nhits;
    if (fNPointsVertexUse > 0 && fNPointsVertexUse < nhits) na = fNPointsVertexUse;
    
    Int_t npx(0), nerr(0);
    for (Int_t i(0); i<na; i++) {
         xhit = (SpdIdealHit*)hits->At(i);
         id = xhit->GetDetId(0);
         if (id != kSpdIts) continue;
         hit = static_cast<SpdIdealSpacepointHit*>(xhit);
         if (!hit) continue;
         if (++npx > np) nerr = 1;
         points.push_back(hit->GetHit()); 
    }
    
    TVector3 p1(0,0,0), n1(0,0,1);
    TVector3 p2 = points[0], n2;
    
    if (npx == 1) n2 = points[0];
    else {
        TVector3 nn;
        for (Int_t i(0); i<npx-1; i++) {
             p2 += points[i+1];
             for (Int_t j = i+1; j<npx; j++) {
                  nn = points[j]-points[i];
                  nn.SetMag(1);
                  n2 += nn;
             }
        }
        p2 *= 1./npx;
    }
    
    n2.SetMag(1);
    
    Double_t theta = n2.Theta();   
    Double_t phi = points[0].Phi();     
    
    if (fSmearSeedValues) {
        Double_t mmag = gRandom->Uniform(fSeedMomentum-fSmearingMomDelta,fSeedMomentum+fSmearingMomDelta);
        fStartMomentum.SetMagThetaPhi(mmag,theta,phi);
    }
    else fStartMomentum.SetMagThetaPhi(fSeedMomentum,theta,phi);
    
    if (npx < 2) {
        fStartVertex.SetXYZ(0,0,0);
    }
    else {
        
        fStartVertex = p2;
        
        Double_t c = n1.Dot(n2);
        Double_t cc = 1.-c*c;

        if (cc < 1.e-12) {
            cout << "-W- <SpdIdealV0Finder::TrackStartVertexAndMom> Vertex is very far from origin: " << endl;
            (p2.Z() < 0) ? fStartVertex.SetXYZ(0,0,1.e9) : fStartVertex.SetXYZ(0,0,-1.e9);
            return;
        }
        
        TVector3 p0 = p2-p1;
        Double_t f = (c*p0.Dot(n1)-p0.Dot(n2))/cc;
        
        fStartVertex += f*n2;
        
        fStartVertex.SetX(0);
        fStartVertex.SetY(0);
    }
    
//     cout << "\t-I- <SpdIdealKalmanFitter::TrackStartVertexAndMom>"
//          << " points: " << npx << "/" << np << ", warnings: " << nerr 
//          << "; theta: " << track->GetStartMomentum().Theta()*TMath::RadToDeg() << " -> " << theta*TMath::RadToDeg()    
//          << ", phi: "  << track->GetStartMomentum().Phi()*TMath::RadToDeg() << " -> "  << phi*TMath::RadToDeg() 
//          << ", v(z): "  << track->GetPrimVertex().Z() << " -> "  << fStartVertex.Z() << endl; 
}

//________________________________________________________________________________________
void SpdIdealV0Finder::DefineStartVertexCovariance() 
{    
    fStartSeedCov.Clear();
    fStartSeedCov.ResizeTo(6,6);
    
    for (Int_t i(0); i < 3; i++) fStartSeedCov(i,i) = 10.;  // position
    for (Int_t i(3); i < 6; i++) fStartSeedCov(i,i) = 100.; // momentum
}

//________________________________________________________________________________________
void SpdIdealV0Finder::FindVertex(TVector3& vertex)
{
    vertex.SetXYZ(0,0,0);
    
    //---------------------------------------
    // vertex fit program for CBM experiment
    //---------------------------------------
    
    if (!fTracks) return;
    
    // Iterative fit of the vertex (CDM)
    //----------------------------------
    const Double_t CutChi2 = 3.5*3.5;
    const Int_t MaxIter = 3;

    // Vertex state vector and covariance matrix
    //-------------------------------------------
    Double_t r[3] = {0,0,0}; 
    Double_t C[6];
    
    for(Int_t i = 0; i<6; i++) C[i] = 0;
    C[0] = C[2] = 5.;
    C[5] = 0.25;
  
    // Chi^2 & NDF
    //-------------------------------------------
    Int_t    refNDF;
    Double_t refChi2;

    // Iterative fit of the vertex
    //-----------------------------
    for (Int_t iteration = 0; iteration < MaxIter; ++iteration)
    {
       // Store the vertex from the previous iteration
       //---------------------------------------------
       Double_t r0[3], C0[6];
       for( Int_t i=0; i<3; i++ ) r0[i] = r[i];
       for( Int_t i=0; i<6; i++ ) C0[i] = C[i];

       // Initialize the vertex covariance, Chi^2 & NDF
       //-----------------------------------------------
       C[0] = 100.;
       C[1] =   0.; C[2] = 100.;
       C[3] =   0.; C[4] =   0.; C[5] = 100.;

       refNDF = -3;
       refChi2 = 0.0;
      
       Int_t NTracks = fTracks->GetEntriesFast();

       SpdGFTrackW* wtr;
       Track* track;
       Int_t  ntrack;
       
       TMatrixDSym vCov;
       TVector3 vPos, vMom;
        
       // start loop over tracks
       //-----------------------
   
       for (Int_t itrk(0); itrk < NTracks; ++itrk) {
           
            wtr = (SpdGFTrackW*)fTracks->At(itrk);
            
            track  = wtr->GetTrack();
            ntrack = wtr->GetDataID();
            
            if (!fKFres->GetIsFitted(ntrack)) continue;        // FIXME
            if (fKFres->GetConvergency(ntrack) != 1) continue; // FIXME
            
            MeasuredStateOnPlane stateF = track->getFittedState(); // copy
        
            Double_t mTheta = stateF.getDir().Theta();
            
            // skip tracks with theta 90+-fVertexFitAngleCut [degrees]
            if (fabs(mTheta-TMath::PiOver2()) < fVertexFitAngleCut*TMath::DegToRad()) continue; // FIXME

            TVector3 planePos(0.0, 0.0, r0[2]);
            TVector3 planeDir(0.0, 0.0, 1.0);
  
            SharedPlanePtr plane(new DetPlane(planePos, planeDir));
            
            stateF.getPosMomCov(vPos, vMom, vCov);    // in global, before extrapolation 

            // ATTENTION --- track extrapolation --- ATTENTION
            try 
            { 
              stateF.extrapolateToPlane(plane); 
            }
            catch(Exception& e)
            { 
              fKFres->SetPrec(ntrack,vMom);
              continue;
            }
                      
            stateF.getPosMomCov(vPos, vMom, vCov);    // in global, after extrapolation  

            if (iteration == MaxIter-1) { 
                
                // Update track momentum and its covariance
                fKFres->SetPrec(ntrack,vMom);
                 
                // cout << "TRACK: " << itrk << endl;
                // cout << "POS:   " << vPos.X() << " " << vPos.Y() << " " << vPos.Z() << endl;
                // cout << "MOM:   " << vMom.X() << " " << vMom.Y() << " " << vMom.Z() << endl;
                // cout << "pt =   " << vMom.Perp() << " p = " << vMom.Mag() << endl;

            }
            
            TVectorD& state   = stateF.getState();    // on plane, 5
            TMatrixDSym& covV = stateF.getCov();      // on plane, 5x5 
            
            TVector3 vMU(-state[1], -state[2], 1.0);

            Double_t vV[10] = { covV[3][3], covV[3][4], covV[4][4],
                                covV[1][3], covV[1][4], covV[1][1],
                                covV[2][3], covV[2][4], covV[2][1],
                                covV[2][2] };

            Double_t a = 0, b = 0;
            {
               Double_t zeta[2] = { r0[0]-vPos.X(), r0[1]-vPos.Y() };

               // Check the track Chi^2 deviation from the r0 vertex estimate    
               //-------------------------------------------------------------
               Double_t S[3] = { (C0[2]+vV[2]), -(C0[1]+vV[1]), (C0[0]+vV[0]) };
               Double_t s = S[2]*S[0] - S[1]*S[1];
               Double_t chi2 = zeta[0]*zeta[0]*S[0] + 2*zeta[0]*zeta[1]*S[1] 
                                                  +   zeta[1]*zeta[1]*S[2];
               // std::cout << " chi2 = " << chi2 << std::endl;

               if ( chi2 > s*CutChi2 ) continue;

               // Fit of the vertex track slopes (a,b) to the r0 vertex estimate
               //-----------------------------------------------------------------
               s = vV[0]*vV[2] - vV[1]*vV[1];
               
               if ( s < 1.E-20 ) continue;
               
               s = 1./s;
            
               a = vMU.X() + s*( ( vV[3]*vV[2] - vV[4]*vV[1])*zeta[0]
                           +     (-vV[3]*vV[1] + vV[4]*vV[0])*zeta[1] );       

               b = vMU.Y() + s*( ( vV[6]*vV[2] - vV[7]*vV[1])*zeta[0]
                           +     (-vV[6]*vV[1] + vV[7]*vV[0])*zeta[1] );       
            }        

            // std::cout << " a = " << a << " b = " << b << std::endl; 

            // Update the vertex (r,C) with the track estimate (m,V) :
            //--------------------------------------------------------

            // Linearized measurement matrix H = { { 1, 0, -a}, { 0, 1, -b} };
            //-----------------------------------------------------------------

            // Residual zeta (measured - estimated)
            //--------------------------------------
            Double_t zeta[2] = { vPos.X() - ( r[0] - a*(r[2]-r0[2]) ),
                                 vPos.Y() - ( r[1] - b*(r[2]-r0[2]) ) };

            // std::cout << " zeta[0] = " << zeta[0] << " zeta[1] = " << zeta[1]
            //           << " zeta[2] = " << zeta[2] << std::endl;

            // CHt = CH’
            //----------
            Double_t CHt[3][2] = { { C[0] - a*C[3], C[1] - b*C[3]},
                                   { C[1] - a*C[4], C[2] - b*C[4]},
                                   { C[3] - a*C[5], C[4] - b*C[5]} };

            // S = (H*C*H’ + V )^{-1}
            //-------------------------
            Double_t S[3] = { vV[0] + CHt[0][0] - a*CHt[2][0],
                              vV[1] + CHt[1][0] - b*CHt[2][0],
                              vV[2] + CHt[1][1] - b*CHt[2][1] };

            // Invert S
            //----------
            { 
               Double_t s = S[0]*S[2] - S[1]*S[1];
               if (s < 1.E-20) continue;
               s = 1./s;
               Double_t S0 = S[0];
               S[0] = s*S[2];
               S[1] = -s*S[1];
               S[2] = s*S0;
            }

            // Calculate Chi^2
            //-----------------
            refChi2 += zeta[0]*zeta[0]*S[0] + 2*zeta[0]*zeta[1]*S[1]
                                            +   zeta[1]*zeta[1]*S[2];
            refNDF += 2; 

            // std::cout << " refChi2 = " << refChi2 << " refNDF = " << refNDF << std::endl; 

            // Kalman gain K = CH’*S
            //------------------------
            Double_t K[3][2];
            for (Int_t i(0); i<3; ++i) { K[i][0] = CHt[i][0]*S[0] + CHt[i][1]*S[1] ; 
                                         K[i][1] = CHt[i][0]*S[1] + CHt[i][1]*S[2] ; }

            // New estimation of the vertex position r += K*zeta 
            //---------------------------------------------------
            for (Int_t i(0); i<3; ++i) r[i] += K[i][0]*zeta[0] + K[i][1]*zeta[1];

            // std::cout << " trk = " << itrk << " r[0] = " << r[0] << " r[1] = " << r[1]
            //           << " r[2] = " << r[2] << std::endl;

            // New covariance matrix C -= K*(CH’)’
            //--------------------------------------
            C[0] -= K[0][0]*CHt[0][0] + K[0][1]*CHt[0][1];
            C[1] -= K[1][0]*CHt[0][0] + K[1][1]*CHt[0][1];
            C[2] -= K[1][0]*CHt[1][0] + K[1][1]*CHt[1][1];
            C[3] -= K[2][0]*CHt[0][0] + K[2][1]*CHt[0][1];
            C[4] -= K[2][0]*CHt[1][0] + K[2][1]*CHt[1][1];
            C[5] -= K[2][0]*CHt[2][0] + K[2][1]*CHt[2][1];

       }  // end loop over tracks

    } // end loop over iteration  

    vertex.SetX(r[0]);
    vertex.SetY(r[1]);
    vertex.SetZ(r[2]);
    
    //cout << "\nrefChi2 = " << refChi2 << " refNDF = " << refNDF << endl; 
    
    TMatrixDSym VC(3);
    VC(0,0) = C[0]; VC(0,1) = C[1]; VC(0,2) = C[3];
    VC(1,0) = C[1]; VC(1,1) = C[2]; VC(1,2) = C[4]; 
    VC(2,0) = C[3]; VC(2,1) = C[4]; VC(2,2) = C[5];
    
    if (refNDF > 0) {
        fKFres->SetVertexChi2overNDF(refChi2/refNDF); 
        fKFres->SetVertexRECcov(VC);
    }
    else fKFres->ClearVertexData();
    
    //VC.Print();
}

//________________________________________________________________________________________
void SpdIdealV0Finder::FindDecayVertex()
{
    // set initial output values
    //---------------------------
    v0_mass    = -99.;
    v0_massErr = -99.;
    v0_pp      = -99.;
    v0_energy  = -99.;
    v0_eta     = -99.;
    v0_theta   = -99.;
    v0_phi     = -99.;
    v0_length  = -99.;

    Int_t nTracks = fTracks->GetEntriesFast();    // number of tracks     

    // set primary vertex (reconstructed or simulated)
    //-------------------------------------------------
    TVector3    recVtx = fKFres->GetVertexRec();      // reconstructed PV
    TMatrixDSym recCov = fKFres->GetVertexRECcov();

    TVector3 pvVtx( fEvent->GetVertex() );           // simulated PV
    if (fUsedTypeOfPV > 1) pvVtx = recVtx;           // set PV vertex   

    // set field
    //-----------
    float fieldBz = (float)fField->GetField()->GetBz(pvVtx.X(), pvVtx.Y(), pvVtx.Z());
    KFParticle::SetField(fieldBz);                 

    // track parameters
    //----------------
    SpdGFTrackW* wtr;
    Track* track;
    Int_t  ntrack;

    TVector3 trkPos, trkMom;
    TMatrixDSym trkCov;

    fKFparticles.clear();
    for (Int_t itrk(0); itrk < nTracks; ++itrk)   // loop over tracks
    {
       wtr = (SpdGFTrackW*)fTracks->At(itrk);
            
       track  = wtr->GetTrack();
       ntrack = wtr->GetDataID();
            
       if (!fKFres->GetIsFitted(ntrack))         continue;  // FIXME
       if ( fKFres->GetConvergency(ntrack) != 1) continue;  // FIXME

       int pdgTrk    = (int)fKFres->GetTrackPdg(ntrack);
       TParticlePDG* part = TDatabasePDG::Instance()->GetParticle(pdgTrk);

       // check only first or second daugter particle types
       //---------------------------------------------------
       if ( fabs(pdgTrk) != fFirstDaughter && fabs(pdgTrk) != fSecondDaughter) continue;

       MeasuredStateOnPlane stateF = track->getFittedState(); // copy
       stateF.getPosMomCov(trkPos, trkMom, trkCov);           // get track parameters at first  
                                                              // hit position
       // extrapolate to primary vertex (sim or rec)
       //-------------------------------------------
       try 
       { 
         stateF.extrapolateToPoint(pvVtx); 
       }
       catch(Exception& e)
       {
         cout << " bad track extrapolation " << endl; 
         continue;
       }

       TMatrixDSym pvCov;
       TVector3 pvPos, pvMom;
       stateF.getPosMomCov(pvPos, pvMom, pvCov);    // in global, after extrapolation 

       // track deviation from the primary vertex
       //-------------------------------------------

       // track Cov matrix in DCA point
       //-------------------------------
       float mS[6] = { (float)pvCov(0,0)+(float)recCov(0,0), 
                       (float)pvCov(1,0)+(float)recCov(1,0), (float)pvCov(1,1)+(float)recCov(1,1),
                       (float)pvCov(2,0)+(float)recCov(2,0), (float)pvCov(2,1)+(float)recCov(2,1), 
                                                             (float)pvCov(2,2)+(float)recCov(2,2) }; 
       // deviation track from PV
       //------------------------
       float zeta[3] = { (float)(pvPos.X()-pvVtx.X()) , 
                         (float)(pvPos.Y()-pvVtx.Y()) , 
                         (float)(pvPos.Z()-pvVtx.Z()) };

       InvertCholetsky3(mS);         // invert matrix  C => C^{-1}

       // chi2 = (R_trk - R_pv)^{T} * (C_trk + C_pv)^{-1} * (R_trk - R_pv);
       //-------------------------------------------------------------------
       float fChi2 = (mS[0]*zeta[0] + mS[1]*zeta[1] + mS[3]*zeta[2]) * zeta[0]
                   + (mS[1]*zeta[0] + mS[2]*zeta[1] + mS[4]*zeta[2]) * zeta[1]
                   + (mS[3]*zeta[0] + mS[4]*zeta[1] + mS[5]*zeta[2]) * zeta[2];

       if (fChi2 < fMinChi2PV) continue;                // check min topo Chi2

       // set KF track parameters
       //------------------------ 
       KFPTrack kfTrack;
       kfTrack.SetParameters( trkPos.X(), trkPos.Y(), trkPos.Z(), 
                              trkMom.X(), trkMom.Y(), trkMom.Z() );

       Double_t C[21] = { trkCov(0,0),
                          trkCov(1,0),trkCov(1,1),
                          trkCov(2,0),trkCov(2,1),trkCov(2,2),
                          trkCov(3,0),trkCov(3,1),trkCov(3,2),trkCov(3,3),
                          trkCov(4,0),trkCov(4,1),trkCov(4,2),trkCov(4,3),trkCov(4,4),
                          trkCov(5,0),trkCov(5,1),trkCov(5,2),trkCov(5,3),trkCov(5,4),trkCov(5,5) };

       kfTrack.SetCovarianceMatrix(C);

       kfTrack.SetNDF( track->getFitStatus()->getNdf() );
       kfTrack.SetChi2(track->getFitStatus()->getChi2());
       kfTrack.SetCharge( (part->Charge())/3);

       KFParticle p1(kfTrack, pdgTrk);        
       fKFparticles.push_back(p1);
    }

    // check vector with daughter candidates
    //--------------------------------------
    if (fKFparticles.size() >= 2)                //  number of KF particles >= 2
    {
       for (int i=0; i<fKFparticles.size()-1; ++i)
       {
          for (int j=i+1; j<fKFparticles.size(); ++j)
          { 
             if (fKFparticles[i].GetQ()*fKFparticles[j].GetQ() > 0) continue;  

             if ( ( fabs(fKFparticles[i].GetPDG())==fFirstDaughter    && 
                    fabs(fKFparticles[j].GetPDG())!=fSecondDaughter ) ||
                  ( fabs(fKFparticles[i].GetPDG())==fSecondDaughter   && 
                    fabs(fKFparticles[j].GetPDG())!=fFirstDaughter )   ) continue;

             // distance between two daughter particles
             //----------------------------------------
             float dist2part = fKFparticles[i].GetDistanceFromParticle(fKFparticles[j]);

             if (dist2part > fMinDist) continue;      // check distance

             // parameter 1 - array of the daughter particles
             // parameter 2 - number of the daughter particles
             // parameter 3 - vertex (it should be the object of the KFParticle class)

              KFPVertex vert;
              vert.SetXYZ( pvVtx.X(), pvVtx.Y(), pvVtx.Z() );
              vert.SetCovarianceMatrix( (float)recCov(0,0),
                                        (float)recCov(1,0), (float)recCov(1,1),
                                        (float)recCov(2,0), (float)recCov(2,1), (float)recCov(2,2) ); 
              vert.SetNContributors(nTracks-2);
              float vtxChi2 = (nTracks-2)*fKFres->GetVertexChi2overNDF();
              vert.SetChi2(vtxChi2);
              KFParticle p0(vert);            // set primary vertex as KFParticle

              const KFParticle pVertex = p0;
              int   NDaughters = 2;
              const KFParticle *vDaughters[2] = { &fKFparticles[i], &fKFparticles[j] };

              // construct deacay V0 particle without PV and mass constrain
              //------------------------------------------------------------
              KFParticle K0_1;
              K0_1.SetProductionVertex(pVertex);
              K0_1.Construct(vDaughters,NDaughters,0,-1);

              TVector3 v1;
              v1.SetXYZ(K0_1.GetX(), K0_1.GetY(), K0_1.GetZ());  // decay vertex

              float mass1    = K0_1.GetMass();
              float massErr1 = K0_1.GetErrMass();
              float mom1     = K0_1.GetP();
              float ene1     = K0_1.GetE();
              float eta1     = K0_1.GetRapidity();
              float theta1   = K0_1.GetTheta();
              float phi1     = K0_1.GetPhi();
              float length1  = (v1-pvVtx).Mag();

              // construct decay V0 particle with PV constrain
              //-----------------------------------------------
              KFParticle K0_2;
              K0_2.SetProductionVertex(pVertex);
              K0_2.Construct(vDaughters,NDaughters,&pVertex,-1);

              v0_mass    = K0_2.GetMass();
              v0_massErr = K0_2.GetErrMass();
              v0_pp      = K0_2.GetP();
              v0_energy  = K0_2.GetE();
              v0_eta     = K0_2.GetRapidity();
              v0_theta   = K0_2.GetTheta();
              v0_phi     = K0_2.GetPhi();
              v0_length  = K0_2.GetDecayLength();

              if ( fConstrainToPV < 1)     // without PV constrain
              {
                 v0_mass    = mass1;
                 v0_massErr = massErr1;
                 v0_pp      = mom1;
                 v0_energy  = ene1;
                 v0_eta     = eta1;
                 v0_theta   = theta1;
                 v0_phi     = phi1;
                 v0_length  = length1;
              } 
     
              // check V0 mass window
              //----------------------
              if( v0_mass < fMinMass || v0_mass > fMaxMass) continue;  // mass window

              fV0tree->Fill();
          }
       } 
    }

}

//________________________________________________________________________________________
void SpdIdealV0Finder::InvertCholetsky3(float a[6])
{
  /* Inverts symmetric 3x3 matrix a using modified Choletsky decomposition. 
     The result is stored to the same matrix a.
      param[in,out] a - 3x3 symmetric matrix
  */
  
  float d[3], uud, u[3][3];
  for(int i=0; i<3; i++) 
  {
    d[i] = 0;
    for(int j=0; j<3; j++) u[i][j] = 0;
  }

  for(int i=0; i<3 ; i++)
  {
    uud = 0;
    for(int j=0; j<i; j++) uud += u[j][i]*u[j][i]*d[j];

    uud = a[i*(i+3)/2] - uud;

    if(fabs(uud)<1.e-12f) uud = 1.e-12f;

    d[i] = uud/fabs(uud);
    u[i][i] = sqrt(fabs(uud));

    for(int j=i+1; j<3; j++) 
    {
      uud = 0;
      for(int k=0; k<i; k++) uud += u[k][i]*u[k][j]*d[k];

      uud = a[j*(j+1)/2+i] - uud;
      u[i][j] = d[i]/u[i][i]*uud;
    }
  }

  float u1[3];
  for(int i=0; i<3; i++)
  {
    u1[i]   = u[i][i];
    u[i][i] = 1/u[i][i];
  }

  for(int i=0; i<2; i++)
  {
    u[i][i+1] = - u[i][i+1]*u[i][i]*u[i+1][i+1];
  }

  for(int i=0; i<1; i++)
  {
    u[i][i+2] = u[i][i+1]*u1[i+1]*u[i+1][i+2]-u[i][i+2]*u[i][i]*u[i+2][i+2];
  }

  for(int i=0; i<3; i++) a[i+3] = u[i][2]*u[2][2]*d[2];

  for(int i=0; i<2; i++) a[i+1] = u[i][1]*u[1][1]*d[1] + u[i][2]*u[1][2]*d[2];

  a[0] = u[0][0]*u[0][0]*d[0] + u[0][1]*u[0][1]*d[1] + u[0][2]*u[0][2]*d[2];
}

//________________________________________________________________________________________
void SpdIdealV0Finder::Finish() 
{
    cout << "-I- <SpdIdealV0Finder::Finish>" << endl;

    CloseOutputV0file();
}

//________________________________________________________________________________________
void SpdIdealV0Finder::FillParametersIn(SpdBaseParSet* params)
{
    if (!params) return;
    
    params->SetParameter("IdealKalmanFitter/MaterialsOpt",fMaterialsOpt);
    
    params->SetParameter("IdealKalmanFitter/SeedMomentum",fSeedMomentum);
    params->SetParameter("IdealKalmanFitter/VertexFitMethod",fVertexFitMethod);
    params->SetParameter("IdealKalmanFitter/VertexFitAngleCut",fVertexFitAngleCut);
    
    Int_t n = fHitRepresentation.size();

    params->SetParameter("IdealKalmanFitter/NHRepresentations",n);
    
    if (n > 0) {
        Int_t i(0);
        for (auto& it : fHitRepresentation) {
             params->SetParameter(Form("IdealKalmanFitter/HitRepDetId[%d]",i),it.first);
             params->SetParameter(Form("IdealKalmanFitter/HitRepDet[%d]",i),it.second);
             i++;
        }  
    }

    params->SetParameter("IdealKalmanFitter/fStartSeedMethod",fStartSeedMethod);
    params->SetParameter("IdealKalmanFitter/fSmearSeedValues",fSmearSeedValues);
    
    if (fSmearSeedValues) {
        params->SetParameter("IdealKalmanFitter/fSmearingMomDelta",fSmearingMomDelta);
        if (fStartSeedMethod == 2) {
            params->SetParameter("IdealKalmanFitter/fNPointsVertexUse",fNPointsVertexUse);
        }
        else 
        {
            params->SetParameter("IdealKalmanFitter/fSmearingAngleDelta",fSmearingAngleDelta);
            params->SetParameter("IdealKalmanFitter/fSmearingVertexDelta",fSmearingVertexDelta);
        }
    }
    
    params->SetParameter("IdealKalmanFitter/FitterOption",fFitterOption);
    
    params->SetParameter("IdealKalmanFitter/FitterMaxIterations",fFitterMaxIterations);
    params->SetParameter("IdealKalmanFitter/FitterMaxTrials",fFitterMaxTrials);
    
    params->SetParameter("IdealKalmanFitter/StoreTracks",fStoreTracks);
}

//________________________________________________________________________________________
void SpdIdealV0Finder::LoadParametersFrom(SpdBaseParSet* params)
{
    if (!params) return;
    
    fHitRepresentation.clear();
    
    Int_t pi;
    Double_t pd;
    Bool_t pb;
    
    if (params->GetParameter("IdealKalmanFitter/MaterialsOpt",pi)) SetMaterialEffectsOption(pi);
    
    if (params->GetParameter("IdealKalmanFitter/SeedMomentum",pd)) SetSeedMomentum(pd);
    if (params->GetParameter("IdealKalmanFitter/VertexFitMethod",pi)) SetVertexFindingMethod(pi);
    if (params->GetParameter("IdealKalmanFitter/VertexFitAngleCut",pd)) SetVertexFindingAngleCut(pd);
    
    if (params->GetParameter("IdealKalmanFitter/NHRepresentations",pi)) {
        if (pi > 0) {
            Int_t pi2, pi3;
            for (Int_t i(0); i<pi; i++) {
                 params->GetParameter(Form("IdealKalmanFitter/HitRepDetId[%d]",i),pi2);
                 params->GetParameter(Form("IdealKalmanFitter/HitRepDet[%d]",i),pi3);
                 fHitRepresentation[pi2] = pi3;
            }
        }
    }
    
    if (params->GetParameter("IdealKalmanFitter/fStartSeedMethod",pi)) SetStartSeedMethod(pi);
    if (params->GetParameter("IdealKalmanFitter/fSmearSeedValues",pb)) SetSmearSeedValues(pb);
    
    if (fSmearSeedValues) {
        if (params->GetParameter("IdealKalmanFitter/fSmearingMomDelta",pd)) SetSmearingMomDelta(pd);
        if (fStartSeedMethod == 2) {
            if (params->GetParameter("IdealKalmanFitter/fNPointsVertexUse",pi)) SetNPointsVertexUse(pi);
        }
        else {
            if (params->GetParameter("IdealKalmanFitter/fSmearingAngleDelta",pd)) SetSmearingAngleDelta(pd);
            if (params->GetParameter("IdealKalmanFitter/fSmearingVertexDelta",pd)) SetSmearingVertexDelta(pd);
        }
    }
    
    if (params->GetParameter("IdealKalmanFitter/FitterOption",pi)) SetFitterOption(pi);
    
    if (params->GetParameter("IdealKalmanFitter/FitterMaxIterations",pi)) SetFitterMaxIterations(pi);
    if (params->GetParameter("IdealKalmanFitter/FitterMaxTrials",pi)) SetFitterMaxTrials(pi);
      
    if (params->GetParameter("IdealKalmanFitter/StoreTracks",pb)) SetStoreTracks(pb);
}

//________________________________________________________________________________________
void SpdIdealV0Finder::InitV0nTuple(TString name)
{
    fV0file = new TFile(name, "RECREATE");
    fV0tree = new TTree("DecayV0Tree", "DecayV0Tree");

    fV0tree->Branch("v0_mass",    &v0_mass,    "v0_mass/F"   );
    fV0tree->Branch("v0_massErr", &v0_massErr, "v0_massErr/F");
    fV0tree->Branch("v0_pp",      &v0_pp,      "v0_pp/F"     );
    fV0tree->Branch("v0_energy",  &v0_energy,  "v0_energy/F" );
    fV0tree->Branch("v0_eta",     &v0_eta,     "v0_eta/F"    );
    fV0tree->Branch("v0_theta",   &v0_theta,   "v0_theta/F"  );
    fV0tree->Branch("v0_phi",     &v0_phi,     "v0_phi/F"    );
    fV0tree->Branch("v0_length",  &v0_length,  "v0_length/F" );
}

//________________________________________________________________________________________
void SpdIdealV0Finder::CloseOutputV0file()
{
    fV0file->Write();
    fV0file->Close();
}



