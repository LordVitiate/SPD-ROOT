// $Id$
// Author: artur   2021/11/19


//_____________________________________________________________________________
//
// SpdMCAegParticleProducer
//_____________________________________________________________________________

#include "SpdMCAegParticleProducer.h"

#include "TRandom.h"
#include <TString.h>
#include <TVector3.h>
#include <TClonesArray.h>
#include <TGeoManager.h>
#include <TParticlePDG.h>
#include <TDatabasePDG.h>
#include "SpdRunAna.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"
#include "SpdTofBGeoMapper.h"
#include "SpdTofECGeoMapper.h"

#include "SpdSetParSet.h"
#include "SpdParSet.h"

#include "SpdMCEvent.h"
#include "SpdMCParticle.h"
#include "SpdTrackMC.h"
#include "SpdAegMCHit.h"
#include "SpdAegParticle.h"

#include "SpdMCEventHelper.h"
#include "SpdParticlesAndProcesses.h"
#include "SpdGeopathParser.h"
 
#include "SpdTrackPropagatorGF.h"


#include <iostream>


using std::cout;
using std::endl;

ClassImp(SpdMCAegParticleProducer)

//_____________________________________________________________________________
SpdMCAegParticleProducer::SpdMCAegParticleProducer():
fPropagator(0),
fDistanceToEndcap(-1.0),
fEvent(0),fParticles(0),fHits(0),fTracks(0),
fAegParticles(0),
fSaveParticles(true),
fVerboseLevel(1)
{
    fGeoMapper[0]  = 0; 
    fParsName[0]   = "AegParSet";
    fParameters[0] = 0;
    n = 1.02;

}

//_____________________________________________________________________________
SpdMCAegParticleProducer::~SpdMCAegParticleProducer() 
{
   if (fPropagator) delete fPropagator;
}

//_____________________________________________________________________________
Bool_t SpdMCAegParticleProducer::LoadGeometry() // private
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance(); 
   if (!gGeoManager && run) run->GetParameters("FairGeoParSet");
   if (!gGeoManager) return false; 
   
   SpdSetParSet* pars;
   TString mapper;
   
   for (Int_t i(0); i<1; i++) 
   {
        fParameters[i] = dynamic_cast<SpdParSet*>(run->GetParameters(fParsName[i]));
        
        if (!fParameters[i]) {
            pars = (SpdSetParSet*)run->GetParameters("SpdSetParSet");
            if (pars) fParameters[i] = pars->GetActiveGeoPars(fParsName[i]);
        }
        
        if (!fParameters[i]) {
            cout << "-W- <SpdMCAegParticleProducer::LoadGeometry> No geometry parameters " 
                 << "(\"" << fParsName[i] << "\")" << endl;
            return true;
        }
        
        //fParameters[i]->print(1);
        
        fParameters[i]->GetParameter("Mapper",mapper); 
        fGeoMapper[i] = SpdGeoFactory::Instance()->SearchForMapper(mapper);
         
        if (!fGeoMapper[i]) {
            cout << "-W- <SpdMCAegParticleProducer::LoadGeometry> Mapper not found " 
                 << "(\"" << mapper << "\")" << endl;
            return true;     
        }
        
        //fGeoMapper[i]->Print("");
   }
   
   fParameters[0]->GetParameter("AegMinDist",fDistanceToEndcap);
   
   return true;
}

//_____________________________________________________________________________
InitStatus SpdMCAegParticleProducer::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdMCAegParticleProducer::Init> RootManager not instantiated " << endl;
       return kFATAL;
   }
   
   //========================== LOAD GEOMETRY ==============================/
   
   if (!LoadGeometry()) {
       cout << "-W- <SpdMCAegParticleProducer::Init> Geometry not defined " << endl;
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
   
   // get AEG hits array
   
   fHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCAegHits));
   if (!fHits) return kFATAL;
   
   // --- get Tracks array ---
    
   fTracks = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCTracks));
   if (!fTracks) return kFATAL; 
      
   //============== CREATE AND REGISTER OUTPUT DATA OBJECTS ================/
   
   fAegParticles = new TClonesArray("SpdAegParticle");
   ioman->Register(SpdDataBranchName(kAegParticles),"SpdAegParticle", fAegParticles, fSaveParticles);  
   
   //============== PASS DATA TO EVENTHELPER ================================/   

   SpdMCEventHelper* helper = SpdMCEventHelper::Instance();
 
   if (!helper->GetEvent())     helper->SetEvent(fEvent);
   if (!helper->GetParticles()) helper->SetParticles(fParticles);
   if (!helper->GetAegHits())   helper->SetAegHits(fHits);
   if (!helper->GetTracks())    helper->SetTracks(fTracks);
 
   helper->SetAegParticles(fAegParticles);
   
   if (fVerboseLevel > -1) cout << "-I- <SpdMCAegParticleProducer::Init> Intialization successfull " << endl;
   
   return kSUCCESS;
}

//________________________________________________________________________________________
void SpdMCAegParticleProducer::Exec(Option_t* opt) 
{
   assert(fEvent);
   assert(fParticles);
   assert(fHits);
   assert(fTracks);
      
   if (fVerboseLevel > 0) {
       cout << "\n-I- <SpdMCAegParticleProducer::Exec> Process event ... " 
            << fEvent->GetEventId(); 
       if (fVerboseLevel > 1) cout << "\n";         
   }
   
   fAegParticles->Delete();
                
   // >>>>> Create paricles <<<<< 
   
   SpdTrackMC* track;
   SpdTrackFitPar* fitpar;
   SpdAegMCHit* hit;
   SpdAegParticle* part;
   
   SpdMCParticle* mcpart;
   Int_t  hitId;
   
   Int_t ntracks = fTracks->GetEntriesFast();
   for (Int_t i(0); i<ntracks; i++) 
   {
        track = (SpdTrackMC*)fTracks->At(i);
        if (!track) continue;
        
        fitpar = track->GetFitPars();
        if (!fitpar) continue;

        SpdTrackState *trklaststate = fitpar->GetLastState();
        if (!trklaststate) continue;

        SpdTrackState *trkfirststate = fitpar->GetFirstState();
        if (!trkfirststate) continue;


        // searching for Aeg hit which corresponds to the track using mc-info
        mcpart = (SpdMCParticle*)fParticles->At(track->GetParticleId());
        hitId =  mcpart->GetAegParticleId(); 
        if (hitId < 0) continue;
    
        hit = (SpdAegMCHit*)fHits->At(hitId);
        
       

        // create TS-particle
        part = AddParticle();
        
        part->SetHitId(hit->GetId());  // same as hitId
        part->SetTrackId(track->GetId());
        
        // ATTENTION mc-particle info redefinition (hit_id -> particle_id) ATTENTION
        
        mcpart->SetAegParticleId(part->GetId());


        TVector3 lastStateMom = trklaststate->GetMomentum();
        TVector3 firstStateMom = trkfirststate->GetMomentum();
        double p_mean = (lastStateMom.Mag() + firstStateMom.Mag()) / 2.;


       
        part->SetMomentum(p_mean);
        
        part->SetLikelihoods(CalculateLikelihoods(p_mean));

        part->SetPth(CalculatePth());

        



        //CheckParticle(part); //FIXME
   }
   
   //cout << "\n >>>>> -I- <SpdMCAegParticleProducer::Exec> Particles: " << fAegParticles->GetEntriesFast() << endl;
}





std::vector< Double_t> SpdMCAegParticleProducer::CalculateLikelihoods( Double_t momentum )
{


    std::vector<Double_t> vprob;

    double pth;
    for (auto &ipdg : pdg_particles)
    {
        TParticlePDG *partPDG = TDatabasePDG::Instance()->GetParticle(ipdg);
        pth = partPDG->Mass() / sqrt(n * n - 1);
        if (momentum >= pth)
            vprob.push_back(1.0);
        else
            vprob.push_back(0.0);
    }

    return vprob;
}




std::vector< Double_t> SpdMCAegParticleProducer::CalculatePth()
{


    std::vector<Double_t> vpth;

    double pth;
    for (auto &ipdg : pdg_particles)
    {
        TParticlePDG *partPDG = TDatabasePDG::Instance()->GetParticle(ipdg);
        pth = partPDG->Mass() / sqrt(n * n - 1);

        vpth.push_back(pth);
    }

    return vpth;
}


//________________________________________________________________________________________  
void SpdMCAegParticleProducer::CheckParticle(SpdAegParticle* part)
{
 
}

//________________________________________________________________________________________  
void SpdMCAegParticleProducer::Finish()
{
   if (fVerboseLevel > -1) {
       cout << "\n-I- <SpdMCAegParticleProducer::Finish>  ";// << endl;
   }
}

//________________________________________________________________________________________  
SpdAegParticle* SpdMCAegParticleProducer::AddParticle()
{
   if (!fAegParticles) return 0;
   Int_t size = fAegParticles->GetEntriesFast();
   SpdAegParticle* particle = new ((*fAegParticles)[size]) SpdAegParticle();
   particle->SetId(size);
   return particle;
}

