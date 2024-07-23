
// $Id$
// Author: vladimir   2022/07/01

//_____________________________________________________________________________
//
// SpdRCTracksFinder
//_____________________________________________________________________________

#include "SpdRunAna.h"

#include "SpdRCTracksFinder.h"

#include "SpdTrackPropagatorGF.h"
#include "Exception.h"

#include "SpdTrackFitterGF.h"
#include "SpdRCTrackFitterGF.h"

#include "SpdTrackMC.h"
#include "SpdTrackRC.h"
 
#include "SpdMCEvent.h"

#include "SpdHit.h"
#include "SpdMCSiliconHit.h"
#include "SpdMCStrawHit1D.h"

#include "SpdMCTrackHit.h"

#include "SpdSetParSet.h"
#include "SpdParSet.h"
#include "SpdTrackFitPar.h"
#include "SpdTrackState.h"

#include "SpdField.h"

#include "SpdMCEventHelper.h"

#include <TRandom3.h>

#include <map>
#include <set>

#include <time.h>

using std::map;
using std::set;
using std::pair;

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdRCTracksFinder)

//_____________________________________________________________________________
SpdRCTracksFinder::SpdRCTracksFinder():
// track fitter
fFitter(0),               // track fitter
fRCFitter(0),             // reco track fitter  
fPropagator(0),           // track propagator

// input
fEvent(0),
fItsHits(0),
fTsHits(0),
fSeeds(0),

// output
fTracks(0),

// settings
fSaveTracks(true),

// checking & counters 
fNHitsAccepted(0),        // accepted Ts hits
fNHitsUsed(0),            // used Ts hits

// geometry 
fMinRadBar(27.515),       // min radius of 1-st layer in Barrel
fMinDistEC(121.0),        // min distance (z) of Endcap
fMinRadEC(10.0),          // min radius of Endcap
fMaxRadEC(85.0),          // max radius of Endcap

// set selection
fMaxLayer(59),            // max number of layers in Barrel 
fMaxEcLayer(59),           // max number of layers in Endcap
fMaxDist(1.10),           // max distance of hit to wire for selection
fMaxChi2(3.1),            // max chi2 for hit selection
fMaxDchi2(20.),           // max chi2 increment for Ts hit 
fMaxFitTsHits(3),         // max number of Ts hits for full Kalman fit 

// compare 2 tracks
fMaxTsDiff(0.4),          // max difference of Ts hits for track cand
fQuality(1.0),            // coefficient for track quality 
fMaxDtheta(0.025),        // max delta theta between two track cand's

// track finder method
fKalmanTree(false),       // use Kalman tree method 
fFullTracking(false),     // use full Kalman fit method 
fUsedTsHits(false),       // remove already used Ts hits 

fTrkCodeFit(211),          // particle code (pions) for track fit

// verbosity
fVerboseLevel(1)
{
   SetDefaultCuts();
}

//_____________________________________________________________________________
SpdRCTracksFinder::~SpdRCTracksFinder() 
{
   if (fFitter)   delete fFitter;
   if (fTracks)   delete fTracks;
   if (fRCFitter) delete fRCFitter;

   if (fPropagator) delete fPropagator;
   
   //SpdMCEventHelper::Instance()->SetTracks(0);
}

//_____________________________________________________________________________
SpdTrackFitterGF* SpdRCTracksFinder::Fitter()
{
   if (fFitter) return fFitter;
   fFitter = new SpdTrackFitterGF();
   return fFitter;
}

//_____________________________________________________________________________
SpdRCTrackFitterGF* SpdRCTracksFinder::RCFitter()
{
   if (fRCFitter) return fRCFitter;
   fRCFitter = new SpdRCTrackFitterGF();
   return fRCFitter;
}

//_____________________________________________________________________________
void SpdRCTracksFinder::SetDefaultCuts()
{
   // min. number of hits in vertex detector (ITS)
   fCheckMinItsHits = true;
   fMinItsHits = 2;
   
   // min. number of hits in straw tracker detector (TS)
   fCheckMinTsHits = true;
   fMinTsHits = 0;
   
   // min. number of hits in ITS+TS
   fCheckMinHits = true;
   fMinHits = 3;

   // max. track generation for Kalman tree method 
   fCheckMaxTrkGeneration = true; 
   fMaxGen = 2;    

   // Ts hits efficiency 
   fCheckTsHitsEfficiency = false;
   fEffTsHits = 0.98;
}

//_____________________________________________________________________________
InitStatus SpdRCTracksFinder::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdRCTrackFinder::Init> RootManager not instantiated " << endl;
       return kFATAL;
   }
   
   //===================== LOAD INPUT DATA OBJECTS =========================/
   
   // get Event 
   
   fEvent = (SpdMCEvent*)ioman->GetObject(SpdDataBranchName(kMCEvent));
   if (!fEvent) return kFATAL;
   
   // get reco track-seed array
   
   fSeeds = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kRCTrackSeeds));
   if (!fSeeds) {
       cout << "-F- <SpdRCTracksFinder::Init> No track seed data " << endl;
      return kFATAL;
   }
      
   Bool_t init_input = false; 
      
   // --- get ItsHits array ---
    
   fItsHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCItsHits));
   if (fItsHits) init_input = true;
   
   // --- get TsHits array ---
    
   fTsHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCTsHits));
   if (fTsHits) init_input = true;
    
   if (!init_input) {
       cout << "-F- <SpdTsMCHitProducer::Init> No input data " << endl;
       return kFATAL; 
   }

   //============== INIT TRACK PROPAGATOR ===================================/
   
   fPropagator = new SpdTrackPropagatorGF();
   if (!fPropagator->Init()) {
       cout << "-F- <SpdRCTracksFinder::Init> "
            << "Track propagator not initialized properly" << endl;
       return kFATAL;
   }
   
   //============== CREATE AND REGISTER OUTPUT DATA OBJECTS ================/
   
   fSaveTracks = true;

   fTracks = new TClonesArray("SpdTrackRC");
   ioman->Register(SpdDataBranchName(kRCTracks),"SpdTrackRC", fTracks, fSaveTracks);

   //============== PASS DATA TO EVENTHELPER ================================/   

   SpdMCEventHelper* helper = SpdMCEventHelper::Instance();
   helper->SetTracksRC(fTracks);

   if (!helper->GetEvent()) helper->SetEvent(fEvent);

   //============== CREATE AND INITIALIZE FITTER ============================/
   
   if (!fFitter) fFitter = new SpdTrackFitterGF();
   if (!fFitter->Init()) {
      cout << "-F- <SpdRCTrackFinder::Init> Fitter not initalized properly" << endl;
      return kFATAL;
   }
   else {
      if (fVerboseLevel > -1) {
         cout << "-I- <SpdRCTrackFinder::Init> Fitter created and initalized " << endl;
      }
   }

   if (!fRCFitter) fRCFitter = new SpdRCTrackFitterGF();
   if (!fRCFitter->Init()) {
      cout << "-F- <SpdRCTrackFinder::Init> RCFitter not initalized properly" << endl;
      return kFATAL;
   }
   else {
      if (fVerboseLevel > -1) {
         cout << "-I- <SpdRCTrackFinder::Init> RCFitter created and initalized " << endl;
      }
   }
   
   if (fVerboseLevel > -1) cout << "-I- <SpdRCTrackFinder::Init> Intialization successfull " << endl;
   
   return kSUCCESS;
}

//________________________________________________________________________________________
void SpdRCTracksFinder::Exec(Option_t* opt) 
{
   assert(fEvent);
   assert(fSeeds);

   if(!fItsHits) assert(fTsHits);
   else assert(fItsHits); 
   
   if (fVerboseLevel > 0) {
       cout << "\n-I- <SpdRCTrackFinder::Exec> Process event ... " << fEvent->GetEventId(); 
       if (fVerboseLevel > 1) cout << "\n";          
   }

   // init
   fTracks->Delete();                             

   Int_t ncount[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };   // set counters

   ClearAll();                                     // clear arrays

   if (fSeeds) FillPrimaryTrackSeeds();            // fill primary track seeds
   ncount[0] = fTrkCand.size();                    

   if (fVerboseLevel > 1)  
      cout << "\n-I- <SpdRCSeedsFinder::Exec> Primary number of track seeds: " << fTrkCand.size();

   // fill arrays with Ts hits
   //---------------------------
   if (fTsHits) FillTsHitsArrays();                // fill Ts hits 

   // init
   //------
   SpdMCTrackHit*   hit;
   SpdMCStrawHit1D* tsHit;

   std::vector<spd_its_point> fHits;

   fFitter->InitFitter();                          // init fitter
   fRCFitter->InitFitter();                        // init RCfitter

   // create reco track and track's fit parameters
   //----------------------------------------------
   rctrack = AddTrack();                     
   fitpars = rctrack->FitParameters();      

   // find track candidates
   //----------------------
   if (fKalmanTree)      FindTrackKalmanTree();      // Kalnan tree method 
   else {
      if (fFullTracking) FindTrackFullKalmanFit();   // full track fit with new Ts hit
      else               FindTrackFastKalmanFit();   // track update using only last Ts hit
   }

   if (fVerboseLevel > 1)
      cout << "\n-I- <SpdRCTracksFinder::Exec> Total number of track candidate: " 
           << fTrkCand.size();

   ncount[1] = fTrkCand.size();                      // number of track candidates

   if (fVerboseLevel > 2) 
      for (auto& itrk : fTrkCand) PrintTrkCand(itrk);  // print

   CleanTrackCandidate();                            // clean track candidate

   if (fVerboseLevel > 2) 
     for (auto& itrk : fTrkCand) PrintTrkCand(itrk);  // print

   if (fVerboseLevel > 1)
       cout << "\n <SpdRCTracksFinder::Exec> Final number of track cand's: " << fTrkCand.size();

   ncount[2] = fTrkCand.size();                      // track cand after cleaning

   fTracks->Delete();

   // create tracks for final fit
   //-----------------------------
   Int_t itrk = -1;
   for (auto trk : fTrkCand)
   {
      if (trk.nts() < fMinTsHits) continue;          // min number of Ts hits
      if (trk.n()   < fMinHits)   continue;          // min number of Its+Ts hits

      ++itrk;                                        // track number after selecton

      rctrack = AddTrack();

      rctrack->SetId(itrk);
      rctrack->SetParticleId(trk.itrk);
      rctrack->SetParticlePdg(fTrkCodeFit*trk.iq);     // use pions for fit   
      rctrack->SetVertexId(0); 

      rctrack->SetNHitsIts(trk.nits);
      rctrack->SetNHitsTsB(trk.nts_b);
      rctrack->SetNHitsTsEC(trk.nts_ec);
 
      for (auto& ihit : trk.itsHits) rctrack->AddPoint(ihit);   // add Its hits
      for (auto& ihit : trk.tsHits)  rctrack->AddPoint(ihit);   // add Ts hits

      if (fVerboseLevel > 2) PrintTrkCand(trk);                 // print track cand
   } 

   if (fVerboseLevel > 1)
       cout << "\n Number of recoTracks candidates = " << fTracks->GetEntriesFast() << endl;

   Int_t ntracks = fTracks->GetEntriesFast();

   ntracks = (fTrkCand.size() > 0) ? ntracks : 0;
   ncount[3] = ntracks;                                  // final number of track candidates

   // final track's fit
   //-------------------
   for (Int_t i(0); i<ntracks; i++)
   {
       rctrack = dynamic_cast<SpdTrackRC*>(fTracks->At(i));
       std::vector<Int_t>& ids = fFitter->GetTrackPointIds();
            
       // create array of track hits
       std::vector<SpdHit*> track;
       PrepareTrack(rctrack,track,ids);
       
       // create output fit parameters
       fitpars = rctrack->FitParameters();
       fitpars->SetTrackId(i);

       Int_t indx = rctrack->GetId(); 

       fFitter->SetPdg(rctrack->GetParticlePdg());
       fFitter->SetMomentum(fTrkCand[indx].pMom);
       fFitter->SetVertex(  fTrkCand[indx].pVtx);
       fFitter->SetTime(0.);

       // fit track
       if (!fFitter->FitTrack(track,fitpars)) 
       {
          rctrack->DeleteFitPars();
          if (fVerboseLevel > 2)          
              cout << "\n track = " << i << " fit not successful ";
       }
       else 
       {
                                          ++ncount[4];           // fitted track
          if (fitpars->GetIsGood())       ++ncount[5];           // good-fitted track 
          if (fitpars->GetIsAcceptable()) ++ncount[6];           // acceptable track
       }
   }

   if (fVerboseLevel > 0) 
   { 
       cout << "\n-I- <SpdRCTracksFinder::Exec> Print ... " << endl; 

       for (auto& ix : fTsUsed_hits) if (ix.second > 0) ++fNHitsUsed; 

       cout << endl;
       cout << " Ts hits (total):                         " << fTsHits->GetEntriesFast() << endl;
       cout << " Ts hits (accepted):                      " << fNHitsAccepted << endl;
       cout << " Ts hits (used):                          " << fNHitsUsed << endl;
       cout << endl;
       
       cout << " Primary number of track seeds:           " << ncount[0] << endl;
       cout << " Number of track candidates:              " << ncount[1] << endl;
       cout << " Number of track cand (after cleaning):   " << ncount[2] << endl;  
       cout << " Final number of track candidates:        " << ncount[3] << endl;  
       cout << " Number of fitted tracks:                 " << ncount[4] << endl;  
       cout << " Number of \"Good-fitted\" tracks:          " << ncount[5] << endl;  
       cout << " \"Acceptable\" tracks (chi2/ndf < 2):      " << ncount[6] << endl;  
       cout << endl;
       
       SpdMCEventHelper::Instance()->PrintTracksRC();  

       SpdMCEventHelper::Instance()->PrintTracks();   
   }
	
}

//________________________________________________________________________________________ 
void SpdRCTracksFinder::FindTrackFullKalmanFit()
{
   //-----------------------------------------------
   // Find track candidate applying full Kalman fit 
   // procedure for each new Ts hit
   //-----------------------------------------------

   if (fVerboseLevel > 2)
      cout << "\n-I- <SpdRCTracksFinder::FindTrackFullKalmanFit> ";

   Int_t inMod, outMod;                         // input, output modules 
   Int_t iLay;                                  // current layer

   TVector3 outPos;                             // hit position on plane
   TVector3 outCov;                             // hit cov matrix on plane

   std::vector<Int_t> layHits;                  // hits in layer

   SpdMCStrawHit1D* tsHit;

   inMod = outMod = 0;
   for ( auto& trk : fTrkCand  )                // loop over track cand 
   {
      fitpars->ClearPars();                     // clear track parameters
      rctrack->RemovePoints();                  // clear points

      FillTrackWithSeed(trk);                   // fill seed parameters 

      if ( !FitTrackCand(rctrack, fitpars) ) continue;             // fit track cand

      if (trk.imod==0) trk.imod = FindTrackerModule(fitpars);      // find module

      if (trk.imod==0 || trk.imod > 10)     continue;              // bad module 

      trk.Update(true, 0, fitpars, nullptr, true);                 // update track cand

      inMod = trk.imod;             
      iLay  = trk.ilay;

      ++iLay;
      while ( iLay < fMaxLayer )                                   // loop over layers
      {
         if (inMod > 8 && iLay > fMaxEcLayer) break;         //  special check for Endcap       

         if( !CheckTrackerModule(fitpars, inMod, iLay, outMod, outPos, outCov) ) 
                                                  { ++iLay; continue; }    // next layer

         if ( inMod != outMod )  {                                 // change module
             inMod = outMod;
             if (outMod > 8) iLay = 1;                             // 1-st layer in endcap 
             trk.imod = outMod;
         }

         if (fTsHits_map[{inMod,iLay}].size() <= 0) { ++iLay; continue; } // no Ts hits

         if (!CheckTsHitsInLayer(inMod, iLay, outPos, outCov, layHits))
                                                  { ++iLay; continue; } // next layer

         for (auto ihit : layHits)                                      // loop over hits
         {
            tsHit = dynamic_cast<SpdMCStrawHit1D*>(fTsHits->At(ihit));

            rctrack->AddPoint( spd_ts_point(ihit,kMCTsHits) );

            if (!FitTrackCand(rctrack, fitpars)) { RemoveLastHit(trk); continue; }  // last hit

            trk.Update(true, iLay, fitpars, tsHit, true);               // uodate track cand

            fTsUsed_hits[ihit] = 1;                                     // hit is used
         }
         ++iLay;                                                        // next layer
      }                                                   
   }                                                      

}

//________________________________________________________________________________________ 
void SpdRCTracksFinder::FindTrackFastKalmanFit()
{
   //---------------------------------------------------------
   // Find track candidate using fast Kalman fit method
   // when track parameters are updated with Kalman procedure 
   // using only 2 points:
   // - track position on plane after extrapolation
   // - hit position on plane
   //---------------------------------------------------------

   if (fVerboseLevel > 2)
      cout << "\n-I- <SpdRCTracksFinder::FindTrackKalmanFastFit> ";

   // loop over primary seeds
   //-------------------------
   Int_t inMod, outMod;                            // input, output modules
   Int_t iLay;                                     // current tracks

   TVector3 outPos;                                // hit position on plane
   TVector3 outCov;                                // hit cov on plane

   std::vector<Int_t> layHits;                     // hits on layer
   std::vector<SpdMCStrawHit1D*> vHits;         

   SpdMCStrawHit1D* tsHit;

   inMod = outMod = 0;
   for ( auto& trk : fTrkCand  )                   // loop over track cand
   {
      fitpars->ClearPars();                        // clear parameters
      rctrack->RemovePoints();                     // clear points

      FillTrackWithSeed(trk);                                       // fill seed parameters 

      if ( !FitTrackCand(rctrack, fitpars) ) continue;              // fit track cand

      if ( trk.imod == 0 ) trk.imod = FindTrackerModule(fitpars);   // find module

      if ( trk.imod == 0 || trk.imod > 10 )    continue;            // bad module 

      trk.Update(true, 0, fitpars, nullptr, true);                  // update parameters

      inMod = trk.imod;             
      iLay  = trk.ilay;

      ++iLay;
      while ( iLay < fMaxLayer )                                    // loop over layers
      {
         if (inMod > 8 && iLay > fMaxEcLayer) break;     // special check for Endcap             

         if(!CheckTrackerModule(fitpars, inMod, iLay, outMod, outPos, outCov))
                                                  { ++iLay; continue; }  // next layer

         if ( inMod != outMod )  {                                  // change module
             inMod = outMod;
             if (outMod > 8) iLay = 1;                              // 1-st layer in Endcap 
             trk.imod = outMod;
         }

         if (fTsHits_map[{inMod,iLay}].size() <= 0) { ++iLay; continue; } // no Ts hits

         if (!CheckTsHitsInLayer(inMod, iLay, outPos, outCov, layHits))
                                                    { ++iLay; continue; } // next layer

         Bool_t full = (trk.tsHits.size() <= fMaxFitTsHits) ? true : false;   // switch

         for (auto ihit : layHits)                                    // loop over hits  
         {
             tsHit = dynamic_cast<SpdMCStrawHit1D*>(fTsHits->At(ihit));

             if ( full )                                              // full fit
             {
                rctrack->AddPoint( spd_ts_point(ihit,kMCTsHits) );

                if (!FitTrackCand(rctrack, fitpars)) { RemoveLastHit(trk); continue; } // last hit
             }
             else                                                     // fast fit
             {
                vHits.clear(); 
                vHits.push_back(tsHit);

                SpdTrackState* finalState = fitpars->GetLastState();
                fRCFitter->SetPdg(rctrack->GetParticlePdg());

                if (!fRCFitter->FitPoints(vHits, finalState, fitpars)) continue;

                if (fRCFitter->GetChi2() > fMaxDchi2) continue;      // too much chi2   

                rctrack->AddPoint( spd_ts_point(ihit,kMCTsHits) );
             }

             trk.Update(true, iLay, fitpars, tsHit, full);       // update parameters

             fTsUsed_hits[ihit] = 1;                             // hit is used
         }

         ++iLay;                                                 // next layer
      }                                                   

   }                                                             // next track       

}

//________________________________________________________________________________________ 
void SpdRCTracksFinder::FindTrackKalmanTree()
{
   //---------------------------------------------------------
   // Track finding method with Kalman tree procedure, 
   // if 2 good hits are found on plane then new track
   // candidate is created and finding procedure is repeated
   // for each new track candidate. Creation of new track
   // candidate is limited by fMaxGen - parameter.
   //----------------------------------------------------------   

   if (fVerboseLevel > 2)
      cout << "\n-I- <SpdRCTracksFinder::FindTrackKalmanTree> ";

   // loop over primary seeds
   //-------------------------
   Int_t inMod, outMod;                       // input, output modules
   Int_t iTrk, maxTracks, iLay;               // track, layer

   TVector3 outPos;                           // hit position on plane
   TVector3 outCov;                           // hit cov on plane

   std::vector<Int_t> layHits;                // hits in layer
   std::vector<SpdMCStrawHit1D*> vHits;

   SpdMCStrawHit1D* tsHit;

   inMod = outMod = iTrk = 0;
   maxTracks = fTrkCand.size();               // max number of tracks to be evaluated

   while ( iTrk < maxTracks )                  // loop over track cand
   {
      fitpars->ClearPars();                    // clear track parameters
      rctrack->RemovePoints();                 // clear points

      FillTrackWithSeed(fTrkCand[iTrk]);       // fill seed parameters   

      if ( !FitTrackCand(rctrack, fitpars) ) { ++iTrk; continue; }  // fit track cand 

      if ( fTrkCand[iTrk].imoth == -1 ) fTrkCand[iTrk].imod = FindTrackerModule(fitpars);   

      if ( fTrkCand[iTrk].imod==0 || fTrkCand[iTrk].imod > 10 ) { ++iTrk; continue; }  

      inMod = fTrkCand[iTrk].imod;               
      iLay  = fTrkCand[iTrk].ilay;

      fTrkCand[iTrk].Update(true, iLay, fitpars, nullptr, true);      // update track cand
 
      ++iLay;
      while ( iLay < fMaxLayer )                           // loop over layers
      {
         if (inMod > 8 && iLay > fMaxEcLayer) break;     // special check for Endcap            

         if( !CheckTrackerModule(fitpars, inMod, iLay, outMod, outPos, outCov) )
                                                           { ++iLay; continue; } // next layer

         if (inMod != outMod)                               // change module
         {
             inMod = outMod;
             fTrkCand[iTrk].imod = outMod;
             if (outMod > 8) iLay = 1;                      // 1-st layer in Endcap 
         }

         if (!CheckTsHitsInLayer(inMod, iLay, outPos, outCov, layHits))
                                                  { ++iLay; continue; } // next layer

         if (layHits.size()>1 && fTrkCand[iTrk].igen < fMaxGen )      // new track cand
         {
            Int_t ihit = layHits.back();

            AddCloneTrackCand(iTrk, iLay, ihit);    // clone  

            fTsUsed_hits[ihit] = 1;

            ++maxTracks;                            // max number of track cand
         }     

         // add 1-st hit or all hits to current track cand 
         //--------------------------------------------------
         Int_t maxHits = (fTrkCand[iTrk].igen < fMaxGen) ? 1 : layHits.size();

         Bool_t full = (fTrkCand[iTrk].tsHits.size() <= fMaxFitTsHits) ? true : false;   // switch
      
         for (Int_t it(0); it<maxHits; ++it)
         {
             Int_t ihit = layHits[it];
             tsHit = dynamic_cast<SpdMCStrawHit1D*>(fTsHits->At(ihit));

             if ( full ) 
             {
                rctrack->AddPoint( spd_ts_point(ihit,kMCTsHits) );

                if (!FitTrackCand(rctrack, fitpars)) { RemoveLastHit(fTrkCand[iTrk]); continue; }
             }
             else
             {
                vHits.clear(); 
                vHits.push_back(tsHit);

                SpdTrackState* finalState = fitpars->GetLastState();
                fRCFitter->SetPdg(rctrack->GetParticlePdg());

                if (!fRCFitter->FitPoints(vHits, finalState, fitpars)) continue;

                if (fRCFitter->GetChi2() > fMaxDchi2) continue;

                rctrack->AddPoint( spd_ts_point(ihit,kMCTsHits) );
             }

             fTrkCand[iTrk].Update(true, iLay, fitpars, tsHit, full);      // uodate track cand
             fTsUsed_hits[ihit] = 1;                        // used hit
         }

         ++iLay;                                        // next layer
      }                                                 // end loop over layers

      ++iTrk;                                           // next track
   }                                                    // end while statement

}

//________________________________________________________________________________________ 
void SpdRCTracksFinder::FillPrimaryTrackSeeds()
{
     if (fVerboseLevel > 3)
         cout << "\n-I- <SpdRCTracksFinder::FillPrimaryTrackSeeds> ";

     SpdMCTrackHit*     hit;
     SpdTrackRCSeed*    seed;
     SpdTrackRCSeedPar* seedPars;   

     std::vector<spd_its_point> fHits;      

     // init field
     Float_t fieldBz = 10.;
     SpdField* field = fPropagator->GetField();
     if (field) fieldBz = field->GetBz(0.,0.,0.);       // set field

     Int_t ns = fSeeds->GetEntriesFast(); 

     if (fVerboseLevel > 2) 
        cout << "\n-I- <SpdRCTracksFinder::FillPrimaryTrackSeeds> Number of track seeds in event: " 
             << ns; 

     Int_t iTrk = -1;
     for (Int_t is(0); is<ns; is++)                    // loop over primary seeds
     {
        seed = dynamic_cast<SpdTrackRCSeed*>(fSeeds->At(is));
        if (!seed) continue;

        fHits = seed->GetSeedHits();                  // get Its hits in seed
        seedPars = seed->GetSeedPars();               // seed parameters

        if (fHits.size() < fMinItsHits) continue;     // check Its hits

        ++iTrk;                                       // new track number

        // fill track cand structure
        //--------------------------
        trkcand x;

        Double_t seedQ  = seedPars->GetCharge();
        seedQ = (seedQ*fieldBz >= 0) ? -1. : 1.;

        Double_t seedRad   = seedPars->GetRadius();
        Double_t seedTheta = seedPars->GetTheta();
        Double_t seedPt    = TMath::Abs(0.0003*fieldBz*seedRad); // pt[GeV/c]=0.3*B[T]*R[m]
        Double_t seedMom   = seedPt / TMath::Sin(seedTheta);
        Double_t seedPhi   = seedPars->GetPhi();
        Double_t seedZvtx  = seedPars->GetZvtx();

        x.itrk  = iTrk;                          // track's number      
        x.iseed = is;                            // seed's number      
        x.iq    = seedQ;                         // charge  

        x.pMom.SetMag(seedMom);                  // primary momentum
        x.pMom.SetTheta(seedTheta);              // primary theta
        x.pMom.SetPhi(seedPhi);                  // primary phi
        x.pVtx.SetXYZ(0.,0.,seedZvtx);           // z-coordinate of PV 

        for (Int_t j=0; j<fHits.size(); ++j)     // add Its hits 
        {
            hit = dynamic_cast<SpdMCTrackHit*>(fItsHits->At(fHits[j].first));
            x.itsHits.push_back(spd_its_point(hit->GetId(),kMCItsHits));
            x.nits++;
        }

        fTrkCand.push_back(x);                   // add structure in vector

        if (fVerboseLevel > 2) PrintTrkCand(x);  // print 

     }      // end loop over seeds

}

//________________________________________________________________________________________ 
void SpdRCTracksFinder::FillTsHitsArrays()
{
     if (fVerboseLevel > 3)
        cout << "\n-I- <SpdRCTracksFinder::FillTsHitsArrays>  \n";

     TVector3 xp;     
     Int_t  imod, ilay;

     SpdMCStrawHit1D* tsHit;
   
     for (Int_t i(0); i<fTsHits->GetEntriesFast(); i++)             // loop over Ts hits  
     {
        tsHit = dynamic_cast<SpdMCStrawHit1D*>(fTsHits->At(i));
        assert(tsHit);

        if (fCheckTsHitsEfficiency && gRandom->Uniform(0.,1.) > fEffTsHits) continue;

        ++fNHitsAccepted;
        fTsUsed_hits.insert( std::pair<Int_t,Int_t>(i,0) );

        if (tsHit->GetModId() == kSpdTsTB)                          // hit in Barrel
        {          
           imod = tsHit->GetDetModule();
           ilay = tsHit->GetDetLayer();

           fTsHits_map[{imod,ilay}].push_back(i);
        }
        else if (tsHit->GetModId() == kSpdTsTEC)                    // hit in Endcap
        {
           xp = tsHit->GetWirePosition();                           // wire position

           imod = (tsHit->GetDetModule()==1) ? 9 : 10;              // module
           ilay = (Int_t)(TMath::Abs(xp.Z())-fMinDistEC+0.1) + 1;   // lay = z-position-117

           fTsHits_map[{imod,ilay}].push_back(i);
        }
        else 
           cout << "\n-I- <SpdRCTracksFinder::FillTsHitsArrays> Wrong type of Ts hit: " << i;  

     }   // end loop over Ts hits

     // find maximum layer number in Endcaap
     //--------------------------------------
     Int_t lay1, lay2;
     lay1 = lay2 = 0;
     for (Int_t jlay(1); jlay<fMaxLayer; ++jlay) 
     {
        if (fTsHits_map[{9, jlay}].size() != 0) lay1 = jlay;
        if (fTsHits_map[{10,jlay}].size() != 0) lay2 = jlay;
     }

     fMaxEcLayer = TMath::Max(lay1,lay2);
 
}

//________________________________________________________________________________________ 
Bool_t SpdRCTracksFinder::FitTrackCand(SpdTrackRC* trk, SpdTrackFitPar* pars)
{
     if (fVerboseLevel > 3)
        cout << "\n-I- <SpdRCTracksFinder::FitTrackCand>";

     // prepare track fit
     //-------------------
     std::vector<Int_t>& ids = fFitter->GetTrackPointIds();
     std::vector<SpdHit*> track;

     PrepareTrack(trk,track,ids);

     // create output fit parameters
     //-----------------------------
     Int_t iTrk = trk->GetId();

     pars = trk->FitParameters();
     pars->SetTrackId(iTrk);

     // set primary fitter parameters (from seed)
     //------------------------------------------
     if (fTrkCand[iTrk].nits == 2) fFitter->SetFitterMaxTrials(4);  // special fot Its==2     

     fFitter->SetPdg(trk->GetParticlePdg());             // PDG of track 
     fFitter->SetMomentum(fTrkCand[iTrk].pMom);          // momentum
     fFitter->SetVertex(  fTrkCand[iTrk].pVtx);          // primary vertex  
     fFitter->SetTime(0.0);                              // time

     // fit track 
     //----------
     if ( !fFitter->FitTrack(track,pars) ) return false;
     else                                  return true;

}

//________________________________________________________________________________________ 
Int_t SpdRCTracksFinder::FindTrackerModule(SpdTrackFitPar* pars)
{
     if (fVerboseLevel > 3)
         cout << "\n-I- <SpdRCTracksFinder::FindTrackerModule> ";
   
     std::vector<Double_t> vPhi {0.3927, 1.1781, 1.9635, 2.7489, 3.2415 };     

     Int_t imod = 0;                                       // undefined imod
     Double_t dist; 
     TVector3 outPos;                                      // output position

     SpdTrackState  stateOut;
     SpdTrackState* finalstate = pars->GetLastState();     // fit parameters in the last point
     fPropagator->InitTrack(pars->GetTrackPdgCode(),1);    // propagation

     // 1-st step => track extrapolate to virtual cylinder with radius = cylRad
     //--------------------------------------------------------------------------
     Double_t minRad = 1.082*fMinRadBar;                   // min radius of barrel tracker
     try   { dist = fPropagator->ExtrapolateToCylinder(minRad, *finalstate, stateOut); }
     catch (genfit::Exception& e) { return 0; }            // bad extrapolation 

//     dist <= 0.0 -> no track extrapolation to virtual cylinder, try to plane

     if (dist <= 0.0) // track extrapolation to Endcap (2-nd step, if dist == 0)
     {
        Int_t inMod       = (finalstate->GetMomentum().Theta() < 1.57) ? 9 : 10;
        TVector3 planeDir = GetPlaneNormal(inMod);
        TVector3 planePos = GetPlanePos(inMod, 1);

        try { dist = fPropagator->ExtrapolateToPlane(planePos, planeDir, *finalstate, stateOut); }
        catch (genfit::Exception& e) { return 0; }                      // bad extrapolation

        if (dist <= 0.0) return 0;                             // imod = 0, no extrapolation  

        if (stateOut.GetPosition().Perp() >= fMinRadEC && 
            stateOut.GetPosition().Perp() <= fMaxRadEC) return inMod;
        else                                            
            return 0;
     }

     TVector3 pPos = stateOut.GetPosition();               // position on virtual cylinder

     // endcup
     if (TMath::Abs(pPos.Z()) > fMinDistEC) return imod = (pPos.Z() > 0) ? 9 : 10; 

     // barrel
     auto   it = std::upper_bound(vPhi.begin(), vPhi.end(), TMath::Abs(pPos.Phi()));
     imod = it - vPhi.begin() + 1;

     return imod = (pPos.Phi()>0.0) ? imod : ( (imod==1) ? 1 : 10-imod );      
}

//________________________________________________________________________________________ 
TVector3 SpdRCTracksFinder::GetPlaneNormal(Int_t inMod)
{
     if (fVerboseLevel > 3)
        cout << "\n <SpdRCTracksFinder::GetPlaneNormal>  \n";

     Double_t nDir[10][3] = { { 1.0,  0.0, 0.0}, { 0.7071,  0.7071, 0.0},
                              { 0.0,  1.0, 0.0}, {-0.7071,  0.7071, 0.0},
                              {-1.0,  0.0, 0.0}, {-0.7071, -0.7071, 0.0},
                              { 0.0, -1.0, 0.0}, { 0.7071, -0.7071, 0.0},
                              { 0.0,  0.0, 1.0}, { 0.0,     0.0,   -1.0} };

    TVector3 planeN (nDir[inMod-1][0], nDir[inMod-1][1], nDir[inMod-1][2]);

    return planeN;
}

//______________________________________________________________________________________________ 
TVector3 SpdRCTracksFinder::GetPlanePos(Int_t imod, Int_t ilay)
{
     if (fVerboseLevel > 3)
        cout << "\n <SpdRCTracksFinder::GetPlanePos>  \n";

     TVector3 dir  = GetPlaneNormal(imod);
     Double_t play = (imod < 9) ? (fMinRadBar + (Double_t)(ilay-1)) : 
                                  (fMinDistEC + (Double_t)(ilay-1) + 0.5);
     TVector3 pos = play * dir;

     return pos;
}

//____________________________________________________________________________________________ 
Bool_t SpdRCTracksFinder::CheckTrackerModule(SpdTrackFitPar* pars, Int_t inMod,  Int_t ilay,
                                          Int_t &outMod, TVector3 &outPos, TVector3& outCov)
{
     if (fVerboseLevel > 3)
         cout << "\n-I- <SpdRCTracksFinder::CheckTrackerModule> ";

     Double_t modEdge = 0.41421 * (fMinRadBar + (Double_t)(ilay-1));  // range on plane

     outMod = 0;
     outPos.SetXYZ( 0., 0., 0.);
     outCov.SetXYZ(10., 10., 10.);
     Double_t planeV = 100.; 

     if ( !ExtrapToPlane(pars, inMod, ilay, outPos, outCov, planeV) )  return false;

     if (inMod > 8)                                                    // endcap 
     {
        if (outPos.Perp() < fMinRadEC || outPos.Perp() > fMaxRadEC)    return false;    
        else                                        { outMod = inMod;  return true;  }
     }
     else                                                              // barrel
     {
        if (TMath::Abs(outPos.Z()) > fMinDistEC)                       // barrel -> endcap
        {
           if (outPos.Perp() < fMinRadEC || outPos.Perp() > fMaxRadEC) return false;  
           else                                              
           {
              outMod = (outPos.Z()>0) ? 9 : 10;                  
              ilay = 1; 

              if (!ExtrapToPlane(pars, outMod, ilay, outPos, outCov, planeV)) return false;
              if (outPos.Perp() < fMinRadEC || outPos.Perp() > fMaxRadEC)     return false; 

              return true;
           }                                                   
        }
        else                                                           // hit in barrel 
        {
           if (TMath::Abs(planeV) <= modEdge )  { outMod = inMod; return true; } 
           else                                                        // change module
           {
              outMod = (planeV > 0.) ? ((inMod>1) ? inMod-1 : 8) : ((inMod<8) ? inMod+1 : 1);

              if (!ExtrapToPlane(pars, outMod, ilay, outPos, outCov, planeV)) return false;
              if (TMath::Abs(planeV) >  modEdge )                             return false; 

              return true;
           }
        }            
     }              

}

//____________________________________________________________________________________________ 
Bool_t SpdRCTracksFinder::ExtrapToPlane(SpdTrackFitPar* pars, Int_t inMod,  Int_t ilay,
                                        TVector3 &outPos, TVector3& outCov, Double_t &planeV)
{
     if (fVerboseLevel > 3)
         cout << "\n-I- <SpdRCTracksFinder::ExtrapToPlane> ";

     planeV = 100.;
     outPos.SetXYZ( 0.,  0.,  0.);
     outCov.SetXYZ(10., 10., 10.);

     // track parameters in the last track point
     //------------------------------------------
     SpdTrackState  stateOut;
     SpdTrackState* stateIni = pars->GetLastState();
     fPropagator->InitTrack(pars->GetTrackPdgCode(),1);

     // plane position and direction
     //-------------------------------
     Double_t dist;
     TVector3 planeDir = GetPlaneNormal(inMod);
     TVector3 planePos = GetPlanePos   (inMod, ilay);

     // track extrapolation to plane inMod
     //------------------------------------
     try { dist = fPropagator->ExtrapolateToPlane(planePos, planeDir, *stateIni, stateOut); }
     catch (genfit::Exception& e) { return false; }             // bad extrapolation

     if ( dist <= 0.0) return false;

     const TVectorD&  state5 = fPropagator->GetState5();        // point on plane
     const TMatrixDSym& cov5 = fPropagator->GetStateCov5x5();   // Cov on plane

     outPos = stateOut.GetPosition();                           // point in GS
     outCov.SetXYZ(cov5(3,3), cov5(3,4), cov5(4,4));            // only diagonal elements

     planeV = state5[3];                                        // v-coordinate on plane

     return true;
}

//______________________________________________________________________________________________ 
Bool_t SpdRCTracksFinder::CheckTsHitsInLayer(Int_t iMod, Int_t iLay, TVector3 pos, TVector3 cov,
                                                                      std::vector<Int_t>& iHits)
{
    if (fVerboseLevel > 3)
        cout << "\n-I- <new SpdRCTracksFinder::CheckTsHitInLayer> \n";

    iHits.clear();
    std::vector<Long_t> hitId;

    TVector3 wp, wd;
    SpdMCStrawHit1D* hit;

    auto& vhits = fTsHits_map[{iMod,iLay}];
    if (vhits.size() <= 0) return false;

    Double_t dist1, dist2, chi2;

    for (auto ihit : vhits) 
    {
       hit = dynamic_cast<SpdMCStrawHit1D*>(fTsHits->At(ihit));

       wp   = hit->GetWirePosition(); 
       wd   = hit->GetWireDirection(); 

       if (fUsedTsHits && fTsUsed_hits[ihit] != 0) continue;

       // hit distance to wire
       if ( ((pos-wp).Cross(wd)).Mag() > fMaxDist ) continue;  

       // hit projection on wire
       if ( TMath::Abs((pos-wp)*wd) > 0.5*hit->GetWireLength()+0.5 ) continue; 

       dist1 = ((pos-wp).Cross(wd)).Mag();
       dist2 = (dist1 > 0.5) ? dist1 - 0.5 : dist1;
       chi2  = dist2/TMath::Sqrt(cov.X() + cov.Z());

       if ( dist1 > 0.5 && chi2 > fMaxChi2 ) continue;

       // use only unique hits (long number)
       Long_t idHit = hit->GetDetId(1);
       if ( std::find(hitId.begin(),hitId.end(),idHit) == hitId.end() )
       {    
          hitId.push_back( idHit );
          if  (iHits.size() > 0 && dist1 < 0.5) iHits.insert(iHits.begin(), hit->GetId());
          else iHits.push_back( ihit );
       }

    }  

    if (iHits.size() > 0) return true;
    else                  return false;    

}

//______________________________________________________________________________________________ 
void  SpdRCTracksFinder::FillTrackWithSeed(trkcand& trk)
{
      if (fVerboseLevel > 3) {
          cout << "\n-I- <SpdRCTracksFinder::FillTrackWithSeed> :  \n";
      }

     // fill track with primary seed's parameter
      //------------------------------------------
      rctrack->SetId(trk.itrk);                                  // set seed Id 
      rctrack->SetParticleId(trk.iseed);                          // set seed Id 

      rctrack->SetParticlePdg(fTrkCodeFit*trk.iq);  // set PDG (pion) 
      rctrack->SetVertexId(0);                                   // primary vertex

      rctrack->SetNHitsIts(trk.nits);
      rctrack->SetNHitsTsB(trk.nts_b);
      rctrack->SetNHitsTsEC(trk.nts_ec);

      for (auto& ihit : trk.itsHits) rctrack->AddPoint(ihit);  // add Its hits in track

      for (auto& ihit : trk.tsHits)  rctrack->AddPoint(ihit);  // add Ts hits in track

}

//______________________________________________________________________________________________ 
void  SpdRCTracksFinder::RemoveLastHit(trkcand& trk)
{
    if (fVerboseLevel > 3) {
        cout << "\n-I- <SpdRCTracksFinder::RemoveLastHit> :  \n";
    }

    rctrack->RemovePoints();             // remove all hits in current track

    // add all previous hits
    rctrack->SetNHitsIts( trk.nits  );
    rctrack->SetNHitsTsB( trk.nts_b );
    rctrack->SetNHitsTsEC(trk.nts_ec);

    for (auto& ihit : trk.itsHits) rctrack->AddPoint(ihit);  // add Its hits in track
    for (auto& ihit : trk.tsHits ) rctrack->AddPoint(ihit);  // add Ts hits in track

}

//______________________________________________________________________________________________ 
void  SpdRCTracksFinder::AddCloneTrackCand(Int_t iTrk, Int_t iLay, Int_t ihit)
{
    if (fVerboseLevel > 3) {
        cout << "\n-I- <SpdRCTracksFinder::AddCloneTrackCand> :  \n";
    }

    Int_t nTrk = (Int_t)fTrkCand.size();

    trkcand x = fTrkCand[iTrk];
    fTrkCand.push_back(x);

    fTrkCand[nTrk].itrk  = nTrk;               // track number             
    fTrkCand[nTrk].imoth = iTrk;               // mother track             
    fTrkCand[nTrk].ifit  = false;              // no fit with this hit             
    fTrkCand[nTrk].ilay  = iLay;               // layer         

    ++fTrkCand[nTrk].igen;                        

    SpdMCStrawHit1D* tsHit = dynamic_cast<SpdMCStrawHit1D*>(fTsHits->At(ihit));

    fTrkCand[nTrk].tsHits.push_back(spd_ts_point(tsHit->GetId(),kMCTsHits));
             
         if (tsHit->GetModId() == kSpdTsTB)  fTrkCand[nTrk].nts_b++;
    else if (tsHit->GetModId() == kSpdTsTEC) fTrkCand[nTrk].nts_ec++;
}

//________________________________________________________________________________________ 
void SpdRCTracksFinder::CleanTrackCandidate()
{
    if (fVerboseLevel > 3) {
       cout << "\n-I- <SpdRCTracksFinder::CleanTrackCandidate> : \n";
    }

    std::vector<Int_t> blackList;

   // compare track candidate from the same seed
   //---------------------------------------------
   blackList = CompareTwoTracksCand(true);          // special for 2 Its track candidate
   std::sort(blackList.begin(), blackList.end()); 

   if (fVerboseLevel > 2) 
   {
      cout << "\n-I- <SpdRCTracksFinder::CleanTrackCandidate> Black list of track cand's (1-st cleaning): " 
           << blackList.size() << " => ";
      for (Int_t i(0); i < blackList.size(); ++i)  cout << blackList[i] << " "; 
      cout << endl;
   }

   // remove track cand from list (after 1-st cleaning)
   //--------------------------------------------------
   for (auto it = blackList.rbegin(); it != blackList.rend(); ++it)
       fTrkCand.erase( fTrkCand.begin()+(*it) );

   if (fVerboseLevel > 2) 
      cout << "\n <SpdRCTracksFinder::CleanTrackCandidate> Number of track cand's (after 1-d cleaning): " 
           << fTrkCand.size();

   // 2-nd cleaning
   //---------------
   blackList.clear();
   blackList = CompareTwoTracksCand(false);
   std::sort(blackList.begin(), blackList.end()); 

   if (fVerboseLevel > 2) 
   {
      cout << "\n-I- <SpdRCTracksFinder::CleanTrackCandidate> Black list of track cand's (2-d cleaning): "
           << blackList.size() << " => ";
      for (Int_t i(0); i < blackList.size(); ++i)  cout << blackList[i] << " ";
          cout << endl;
   }

   // remove track cand from list (after 2-nd cleaning)
   //---------------------------------------------------
   for (auto it = blackList.rbegin(); it != blackList.rend(); ++it)
       fTrkCand.erase( fTrkCand.begin()+(*it) );

   if (fVerboseLevel > 2) for (auto& itrk : fTrkCand) PrintTrkCand(itrk);  // print

   if (fVerboseLevel > 1)
       cout << "\n <SpdRCTracksFinder::Exec> Final number of track cand's: " << fTrkCand.size();

}

//________________________________________________________________________________________ 
std::vector<Int_t> SpdRCTracksFinder::CompareTwoTracksCand(Bool_t twoHits)
{
    if (fVerboseLevel > 3) {
       cout << "\n-I- <SpdRCTracksFinder::CompareTwoTracksCand> : \n";
    }

    std::vector<Int_t> list;

    if (fTrkCand.size() < 2 ) return list;
    Int_t nTracks = fTrkCand.size();                  // total number of track cand

    std::vector<Int_t> vIts1;
    std::vector<Int_t> vTs1;

    std::vector<Int_t> vIts2;
    std::vector<Int_t> vTs2;

    Double_t ratio, dTheta;

    for (Int_t i(0); i<nTracks-1; ++i)                                  // 1-st track     
    {
       if (twoHits && fTrkCand[i].nits != 2)                                  continue;  
       if (std::find(list.begin(),list.end(),i) != list.end())                continue;  
       if (twoHits && fTrkCand[i].nts() < fMaxFitTsHits) { list.push_back(i); continue; }  

       vIts1.clear();
       for (auto& ip : fTrkCand[i].itsHits ) vIts1.push_back(ip.first); // Its hits       

       vTs1.clear();
       for (auto& ip : fTrkCand[i].tsHits ) vTs1.push_back(ip.first);   // Ts hits    

       for (Int_t j=i+1; j<nTracks; ++j)                                //  2-nd track 
       {
          if (twoHits && fTrkCand[j].nits != 2)                                  continue;  
          if (std::find(list.begin(),list.end(),j) != list.end())                continue;
          if (twoHits && fTrkCand[j].nts() < fMaxFitTsHits) { list.push_back(j); continue; } 

          vIts2.clear();                                                // common Its hits
          for (auto& ip : fTrkCand[j].itsHits)  {
              if (std::find(vIts1.begin(),vIts1.end(),ip.first) != vIts1.end()) 
                                                      vIts2.push_back(ip.first);
          }

          vTs2.clear();                                                 // common Ts hits
          for (auto& ip : fTrkCand[j].tsHits)  {
              if (std::find(vTs1.begin(),vTs1.end(),ip.first) != vTs1.end())
                                                    vTs2.push_back(ip.first);
          }

          if (vIts2.size() < 2 && vTs2.size() == 0 ) continue;   // no common Its and Ts hits

          ratio = 0.0;
          if ( TMath::Min(fTrkCand[i].nts(),fTrkCand[j].nts()) != 0)           
             ratio = (Double_t)vTs2.size() / TMath::Min(fTrkCand[i].nts(),fTrkCand[j].nts());
 
          if ( ratio < fMaxTsDiff ) continue;                     // low number of common Ts hits

          dTheta = TMath::Abs( fTrkCand[i].pMom.Theta()-fTrkCand[j].pMom.Theta() );
          
          if (twoHits && dTheta > fMaxDtheta) continue;           // delta theta (only for 2 Its) 

          // track quality => n_hits - coeff*(chi2/ndf) 
          //--------------------------------------------
          Double_t qTrk1 = fTrkCand[i].n() - fQuality * (fTrkCand[i].chi2 / fTrkCand[i].n());  
          Double_t qTrk2 = fTrkCand[j].n() - fQuality * (fTrkCand[j].chi2 / fTrkCand[j].n());  

          if ( qTrk1 >= qTrk2 ) list.push_back(j);
          else                { list.push_back(i); break; }

       }          // end loop over 2-nd track (j)
    }             // end loop over 1-st track (i)

   return list;

}

//________________________________________________________________________________________ 
SpdTrackRC* SpdRCTracksFinder::AddTrack() {

   if (!fTracks) return 0;

   Int_t size = fTracks->GetEntriesFast();
   SpdTrackRC* track = new ((*fTracks)[size]) SpdTrackRC();
   track->SetId(size);
 
   return track; 
}

//________________________________________________________________________________________ 
void SpdRCTracksFinder::PrepareTrack(SpdTrackRC* rtrack, std::vector<SpdHit*>& track, 
                                                         std::vector<Int_t>& ids) const
{
    ids.clear();
    track.clear();
    
    const std::vector<spd_rctrack_point_>& points = rtrack->GetTrackPoints();

    SpdHit* hit;
    Int_t   n(0);
    ids.reserve(points.size());

    for (auto x : points) {
        if (x.second == kMCItsHits) 
        {
            track.push_back((SpdHit*)fItsHits->At(x.first));
            ids.push_back(n);
        }
        else if (x.second == kMCTsHits) {
            track.push_back((SpdHit*)fTsHits->At(x.first));
            ids.push_back(n);
        }
        n++;
    }
}

//________________________________________________________________________________________  
void SpdRCTracksFinder::ClearAll()
{
   if (fVerboseLevel > 2)
       cout << "\n-I- <SpdRCTracksFinder::ClearAll> ... ";

   fNHitsAccepted = 0;
   fNHitsUsed     = 0;

   fTrkCand.clear();
   fTsHits_map.clear();
   fTsUsed_hits.clear();

}

//________________________________________________________________________________________  
void SpdRCTracksFinder::PrintTrkCand( trkcand& trk) 
{

   if (fVerboseLevel > 2)
       cout << "\n-I- <SpdRCTracksFinder::PrintTrkCand> ... ";

    cout << endl;
    cout << "\n track candidate = " << trk.itrk << endl;
    cout << " iseed = "    << trk.iseed
         << " imoth = "    << trk.imoth
         << " q = "        << trk.iq
         << " pt = "       << trk.pMom.Mag()*TMath::Sin(trk.pMom.Theta()) 
         << " mom  = "     << trk.pMom.Mag()
         << " theta = "    << trk.pMom.Theta()
         << " zvtx = "    <<  trk.pVtx.Z();

    cout << "\n Its hits: " << trk.itsHits.size() << " => ";
       for (auto& ihit : trk.itsHits)  cout << ihit.first << " ";

    if (trk.tsHits.size() > 0) {
       cout << "\n Ts hits: " << trk.tsHits.size() << " => ";
       for (auto& ihit : trk.tsHits)   cout << ihit.first  << " ";
    }

}

//________________________________________________________________________________________  
void SpdRCTracksFinder::Finish()
{
   if (fVerboseLevel > -1) 
   { 
       cout << "\n-I- <SpdRCTracksFinder::Finish> ... ";

       PrintCuts();

       if (fFitter) fFitter->PrintCounter();
     
   }

}

//________________________________________________________________________________________ 
void SpdRCTracksFinder::PrintCuts() const
{
   cout << "\n\t-I- <SpdRCTracksFinder::PrintCuts> \n\n"; 

   cout << "\tmin. number of Its hits (value):       " << ((fCheckMinItsHits) ? "yes" : "no") 
        << " (" << fMinItsHits << ") " << endl;
   cout << "\tmin. number of Ts hits (value):        " << ((fCheckMinTsHits) ? "yes" : "no") 
        << " (" << fMinTsHits << ") " << endl;
   cout << "\tmin. number of Its+Ts hits (value):    " << ((fCheckMinHits) ? "yes" : "no") 
        << " (" << fMinHits << ") " << endl;
   cout << "\tmax. number of Ts hits for full fit:   " << ((fMaxFitTsHits>0) ? "yes" : "no") 
        << " (" << fMaxFitTsHits << ") " << endl;
   cout << "\tmax. efficiency of Ts hits:            " << ((fCheckTsHitsEfficiency) ? "yes" : "no") 
        << " (" << fEffTsHits << ") " << endl;
   cout << "\tremove already used Ts hits:           " << ((fUsedTsHits) ? "yes" : "no") 
        << endl;

   cout << endl;

   cout << "\tuse Kalman tree method:                " << ((fKalmanTree) ? "yes" : "no") << endl; 
   cout << "\tuse Kalman full fitting method:        " << ((fFullTracking) ? "yes" : "no") << endl;
   cout << "\tuse Kalman fast fitting method:        " << ((!fFullTracking) ? "yes" : "no") << endl;
   cout << endl;

   cout << "\tmax. Kalman tree generation (value):   " << ((fMaxGen) ? "yes" : "no")
                                                       << " (" << fMaxGen << ") " << endl;
   cout << "\tmax. number of layers:                 " << fMaxLayer << endl;
   cout << "\tmax. distance hit to wire:             " << fMaxDist << " (cm)" << endl; 
   cout << "\tmax. chi2 for accepted Ts hits:        " << fMaxChi2 << endl;
   cout << "\tmax. hits difference of 2 tracks cand: " << fMaxTsDiff << endl;
   cout << "\ttrack quality coefficient:             " << fQuality << endl;

//   cout << "\n";
}


