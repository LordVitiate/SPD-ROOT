// $Id$
// Author: artur   2021/11/19


//_____________________________________________________________________________
//
// SpdMCTsParticleProducer
//_____________________________________________________________________________

#include "SpdMCTsParticleProducer.h"

#include <TString.h>
#include <TVector3.h>
#include <TClonesArray.h>
#include <TGeoManager.h>
#include <TDatabasePDG.h>

#include "SpdRunAna.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"
#include "SpdTsTBGeoMapper.h"
#include "SpdTsTECGeoMapper.h"

#include "SpdSetParSet.h"
#include "SpdParSet.h"

#include "SpdMCEvent.h"
#include "SpdMCParticle.h"
#include "SpdTrackMC.h"
#include "SpdTsParticle.h"

#include "SpdMCSiliconHit.h"
#include "SpdMCStrawHit1D.h"
#include "SpdMCStrawHit2D.h"

#include "SpdMCEventHelper.h"
#include "SpdParticlesAndProcesses.h"
#include "SpdGeopathParser.h"
 
#include "SpdTrackPropagatorGF.h"
 
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdMCTsParticleProducer)

const Int_t SpdMCTsParticleProducer::kPDG[kNTYPES] = {211, 321, 2212};

//_____________________________________________________________________________
SpdMCTsParticleProducer::SpdMCTsParticleProducer():
fPropagator(0),
fEvent(0),fParticles(0),fHits(0),fTracks(0),
fTsParticles(0),
fSaveParticles(true),
fVerboseLevel(1) 
{
   fGeoMapper[0]  = 0;            fGeoMapper[1]  = 0; 
   fParsName[0]   = "TsTBParSet"; fParsName[1]   = "TsTECParSet";
   fParameters[0] = 0;            fParameters[1] = 0;
}

//_____________________________________________________________________________
SpdMCTsParticleProducer::~SpdMCTsParticleProducer() 
{
   if (fPropagator) delete fPropagator;
}

//_____________________________________________________________________________
Bool_t SpdMCTsParticleProducer::LoadGeometry() // private
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance(); 
   if (!gGeoManager && run) run->GetParameters("FairGeoParSet");
   if (!gGeoManager) return false; 
   
   SpdSetParSet* pars;
   TString mapper;
   
   for (Int_t i(0); i<2; i++) 
   {
        fParameters[i] = dynamic_cast<SpdParSet*>(run->GetParameters(fParsName[i]));
        
        if (!fParameters[i]) {
            pars = (SpdSetParSet*)run->GetParameters("SpdSetParSet");
            if (pars) fParameters[i] = pars->GetActiveGeoPars(fParsName[i]);
        }
        
        if (!fParameters[i]) {
            cout << "-W- <SpdMCTofParticleProducer::LoadGeometry> No geometry parameters " 
                 << "(\"" << fParsName[i] << "\")" << endl;
            return true;
        }
        
        //fParameters[i]->print(1);
        
        fParameters[i]->GetParameter("Mapper",mapper); 
        fGeoMapper[i] = SpdGeoFactory::Instance()->SearchForMapper(mapper);
         
        if (!fGeoMapper[i]) {
            cout << "-W- <SpdMCTofParticleProducer::LoadGeometry> Mapper not found " 
                 << "(\"" << mapper << "\")" << endl;
            return true;     
        }
        
        //fGeoMapper[i]->Print("");
   }
   
   return true;     
}

//_____________________________________________________________________________
InitStatus SpdMCTsParticleProducer::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdMCTsParticleProducer::Init> RootManager not instantiated " << endl;
       return kFATAL;
   }
   
   //========================== LOAD GEOMETRY ==============================/
   
   if (!LoadGeometry()) {
       cout << "-W- <SpdMCTsParticleProducer::Init> Geometry not defined " << endl;
   }
   
   //===================== INIT FIELD PROPAGATOR ===========================/
   
   fPropagator = new SpdTrackPropagatorGF();
   if (!fPropagator->Init()) {
       cout << "-F- <SpdMCTsParticleProducer::Init> "
            << "Track propagator not initialized properly" << endl;
       return kFATAL;
   }
   
   //===================== LOAD INPUT DATA OBJECTS =========================/
   
   // get Event 
   
   fEvent = (SpdMCEvent*)ioman->GetObject(SpdDataBranchName(kMCEvent));
   if (!fEvent) return kFATAL;
   
   // get Particles array
   
   fParticles = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCParticles));
   if (!fParticles) return kFATAL;
   
   // get TS hits array
   
   fHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCTsHits));
   if (!fHits) return kFATAL;
   
   // --- get Tracks array ---
    
   fTracks = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCTracks));
   if (!fTracks) return kFATAL; 
      
   //============== CREATE AND REGISTER OUTPUT DATA OBJECTS ================/
   
   fTsParticles = new TClonesArray("SpdTsParticle");
   ioman->Register(SpdDataBranchName(kTsParticles),"SpdTsParticle", fTsParticles, fSaveParticles);  
   
   //============== PASS DATA TO EVENTHELPER ================================/   

   SpdMCEventHelper* helper = SpdMCEventHelper::Instance();
 
   if (!helper->GetEvent())     helper->SetEvent(fEvent);
   if (!helper->GetParticles()) helper->SetParticles(fParticles);
   if (!helper->GetTsHits())    helper->SetTsHits(fHits);
   if (!helper->GetTracks())    helper->SetTracks(fTracks);
 
   helper->SetTsParticles(fTsParticles);
   
   if (fVerboseLevel > -1) cout << "-I- <SpdMCTsParticleProducer::Init> Intialization successfull " << endl;
   
   return kSUCCESS;
}

//________________________________________________________________________________________
void SpdMCTsParticleProducer::Exec(Option_t* opt) 
{
   assert(fEvent);
   assert(fParticles);
   assert(fHits);
   assert(fTracks);
      
   if (fVerboseLevel > 0) {
       cout << "\n-I- <SpdMCTsParticleProducer::Exec> Process event ... " 
            << fEvent->GetEventId(); 
       if (fVerboseLevel > 1) cout << "\n";         
   }
   
   fTsParticles->Delete();
                
   // >>>>> Create particles <<<<< 
   
   SpdTrackMC* track;
   SpdTrackFitPar* fitpar;
   SpdTsParticle* part;
   
   SpdMCParticle* mcpart;
   
   std::vector<Double_t> dEdx_values;
   
   Int_t ntracks = fTracks->GetEntriesFast();
   
   for (Int_t i(0); i<ntracks; i++) 
   {
        track = (SpdTrackMC*)fTracks->At(i);
        if (!track) continue;
        
        mcpart = (SpdMCParticle*)fParticles->At(track->GetParticleId());
        if (!mcpart) continue;

        fitpar = track->GetFitPars();
        if (!fitpar) continue;
        
        Int_t track_id = track->GetId();
        
        if (!(track->GetNHitsTsB() + track->GetNHitsTsEC() > 0)) {
            if (fVerboseLevel > 1) {
                cout << "\n-I- <SpdMCTsParticleProducer::Exec> Track " << track_id << " has no hits in TS. ";
            }
            continue;
        }
        
        if (!(fitpar->GetIsFittedOk())) {
            if (fVerboseLevel > 1) {
                cout << "\n-I- <SpdMCTsParticleProducer::Exec> Track " << track_id << " is not fitted OK. ";
            }
            continue;
        }
        
        // Find average momentum of the track
        Double_t momentum(0);
        Bool_t mom_defined(false);
        if (fMomentumOption < 0 || fMomentumOption > 1) {
            fMomentumOption = 0;
        }
        if (fMomentumOption == 0) {  
            // Average of values in first and last points of the track
            SpdTrackState* first_state = fitpar->GetFirstState();
            SpdTrackState* last_state = fitpar->GetLastState();
            Double_t mom_first = first_state->GetMomentum().Mag();
            Double_t mom_last = last_state->GetMomentum().Mag();
            momentum = 0.5*(mom_first + mom_last);
            mom_defined = true;
        }
        else if (fMomentumOption == 1) {
            // Average over all states of the track belonging to TS
            Int_t n_states_ts(0);
            for(int istate = 0; istate < fitpar->GetNStates(); istate++) {
                Int_t point_id = fitpar->GetPointId(istate);
                spd_mctrack_point_ point = track->GetTrackPoint(point_id);
                if (point.second != kMCTsHits) continue;
                n_states_ts++;
                momentum += fitpar->GetMomentum(istate)->Mag();
            }
            if (n_states_ts > 0) {
                momentum /= n_states_ts;
                mom_defined = true;
            }
            else {
                if (fVerboseLevel > 1) {
                    cout << "\n-I- <SpdMCTsParticleProducer::Exec> Track " << track_id << " has no states in TS. ";
                }
            }
        }
        if (!mom_defined) {
            continue;
        }
        
        // Find dE/dx values in the points of the track
        dEdx_values.clear();
        for(Int_t ipoint = 0; ipoint < track->GetNPoints(); ipoint++) {
            spd_mctrack_point_ point = track->GetTrackPoint(ipoint);
            Int_t id = point.first;
            Int_t data_id = point.second;
            if (data_id != kMCTsHits) continue;
            
            SpdMCTrackHit* hit = (SpdMCTrackHit*) fHits->At(id);   // hit can be either SpdMCStrawHit1D or SpdMCStrawHit2D
            if (!hit) continue;
            //int modid = hit->GetModId();    // module (detector) id
            //if (!(modid == kSpdTsTB || modid == kSpdTsTEC)) continue;
            
            Double_t de(0), dx(0);
            de = hit->GetResp();
            if (hit->GetSpecifity() == 'v') {
                SpdMCStrawHit1D* hit1d = (SpdMCStrawHit1D*) hit;
                dx = hit1d->GetSegLen();
            }
            if (hit->GetSpecifity() == 'w') {
                SpdMCStrawHit2D* hit2d = (SpdMCStrawHit2D*) hit;
                dx = hit2d->GetSegLen();
            }
            if (dx == 0) continue;
            dEdx_values.push_back(de/dx);
        }
        if (dEdx_values.size() < 1) {
            if (fVerboseLevel > 1) {
                cout << "\n-I- <SpdMCTsParticleProducer::Exec> Track " << track_id << " has no points with valid dE/dx values. ";
            }
            continue;
        }
        
        Int_t nhits = dEdx_values.size();
        if (nhits < 3) continue;
        
        // Calculate truncated mean of dE/dx
        const Double_t truncationValue = 0.35;
        std::sort(dEdx_values.begin(), dEdx_values.end());
        Int_t ntr = round((1.-truncationValue)*dEdx_values.size());
        if (ntr < 1) {
            continue;
        }
        Double_t truncmeandedx(0);
        for(int j = 0; j < ntr; j++) {
            truncmeandedx += dEdx_values[j];
        }
        truncmeandedx /= ntr;
        
        // create TS-particle
        part = AddParticle();
        
        part->SetTrackId(track->GetId());
        
        // ATTENTION mc-particle info  ATTENTION
        mcpart->SetTsParticleId(part->GetId());
        
        part->SetMomentum(momentum);
        part->SetTruncMeandEdx(truncmeandedx);
        
        CalculateLikelihoodsAndDistances(part, momentum, truncmeandedx, nhits);
        
        if(fVerboseLevel > 2) part->Print();
   }
   
   cout << "\n >>>>> -I- <SpdMCTsParticleProducer::Exec> Particles: " << fTsParticles->GetEntriesFast() << endl;
}

//________________________________________________________________________________________  
void SpdMCTsParticleProducer::Finish()
{
   if (fVerboseLevel > -1) {
       cout << "\n-I- <SpdMCTsParticleProducer::Finish>  ";// << endl;
   }
}

//________________________________________________________________________________________  
SpdTsParticle* SpdMCTsParticleProducer::AddParticle()
{
   if (!fTsParticles) return 0;
   Int_t size = fTsParticles->GetEntriesFast();
   SpdTsParticle* particle = new ((*fTsParticles)[size]) SpdTsParticle();
   particle->SetId(size);
   return particle;
}

//________________________________________________________________________________________  
// Parameterization of mean of distribution of truncated mean dE/dx
Double_t SpdMCTsParticleProducer::MeanParameterization(Int_t itype, Double_t mom)
{
    const int NPAR = 3;
    const Double_t c[kNTYPES][NPAR] = {
        {1.17144, 0.0779687, 1.09661},  // pions
        {1.19942, 0.0775301, 1.09758},  // kaons
        {1.20439, 0.0775639, 1.09703}   // protons
    };
    //const double mass_[kNTYPES] = {0.13957000, 0.49367700, 0.93827200}
    Double_t mass = TDatabasePDG::Instance()->GetParticle(kPDG[itype])->Mass();
    Double_t c0 = c[itype][0];
    Double_t c1 = c[itype][1];
    Double_t c2 = c[itype][2];
    Double_t p_over_m = mom/mass;
    Double_t sqr_beta = pow(p_over_m,2)/(1. + pow(p_over_m,2));  // beta^2
    Double_t f = c0/sqr_beta + c1/sqr_beta*2.*log(p_over_m) - (c0-c2)*(1. + log(sqr_beta)/sqr_beta);
    return f*1e-6;
}

// Parameterization of relative sigma of distribution of truncated mean dE/dx
Double_t SpdMCTsParticleProducer::RelativeSigmaParameterization(Int_t itype, Double_t mom, Int_t nhits)
{
    const Double_t r[kNTYPES] = {
        0.0837143,   // pions
        0.0835451,   // kaons
        0.0831169    // protons
    };
    const Int_t n0 = 44;
    Double_t k = pow(double(n0)/double(nhits), 0.47);
    return r[itype]*k;
}

Double_t SpdMCTsParticleProducer::SigmaParameterization(Int_t itype, Double_t mom, Int_t nhits)
{
    return MeanParameterization(itype, mom)*RelativeSigmaParameterization(itype, mom, nhits);
}


void SpdMCTsParticleProducer::CalculateLikelihoodsAndDistances(SpdTsParticle* part, Double_t momentum, Double_t value, Int_t nhits)
{
    Double_t mean, sigma;
    std::vector<Double_t> lh(kNTYPES);
    std::vector<Double_t> dist(kNTYPES);
    for(int itype = 0; itype < kNTYPES; itype++) {
        mean = MeanParameterization(itype, momentum);
        sigma = mean*RelativeSigmaParameterization(itype, momentum, nhits);
        lh[itype] = 1./sqrt(2.*TMath::Pi())/sigma*exp(-0.5*pow((value-mean)/sigma,2));
        dist[itype] = (value-mean)/sigma; 
    }
    part->SetLikelihoods(lh);
    part->SetDistances(dist);
}
