
// $Id$
// Author: vladimir   2022/07/01

//_____________________________________________________________________________
//
// SpdRCTrackSeedsFinder
//
// Find track seed candidates using only Its hits:                            
//                                                                            
// 1) combine 2 hits on different layers (starting from 1-st)                   
// 2) add hit layer by layer using delta theta and phi conditions                   
// 3) estimate track seed parameters                                          
// 4) merge track seed candidates                                             
// 5) cleanup track seed candidates                                             
//_____________________________________________________________________________

#include "SpdRunAna.h"
#include "SpdMCEvent.h"

#include "SpdRCTrackSeedsFinder.h"

#include "SpdHit.h"
#include "SpdMCSiliconHit.h"

#include "SpdSetParSet.h"
#include "SpdParSet.h"

#include "SpdTrackRCSeed.h"
#include "SpdTrackRCSeedPar.h"

#include "SpdMCEventHelper.h"

#include <map>
#include <set>

using std::map;
using std::set;
using std::pair;

#include <iostream>

using std::cout;
using std::endl;

#define PI 3.14159265

ClassImp(SpdRCTrackSeedsFinder)

//_____________________________________________________________________________
SpdRCTrackSeedsFinder::SpdRCTrackSeedsFinder():
// input
fEvent(0),fItsHits(0),

// output
fSeeds(0),

// settings
fMaxItsLayers(5),                // max number of Its layers
fMinLine(0.0001),                // min value for radius estimation
fMaxDtheta(0.02),                // max delta theta for adding new point 
fMaxPVZvtx(90.0),                // max +- z-range (cm) of primary vertex
fMaxRes(0.02),                   // max residual of line fit 
fMaxDrad(0.25),                  // max delta radius for 3-d point fit

fMaxPVIter(1),                   // max number iteration for PV estimation
fMaxPVDist(1.0),                 // max distance (cm) for PV clustering

fSameHits(false),                // use hits only from the same part (barrel or endcap)
fSaveSeeds(true),                // save output seeds

// checking & counters 
fNHitsAccepted(0),               // number of accepted Its hits
fNHitsUsed(0),                   // number of used Its hits
fNSeedsTotal(0),                 // total number created track seeds

// verbosity
fVerboseLevel(0)
{
   SetDefaultCuts();
}

//_____________________________________________________________________________
SpdRCTrackSeedsFinder::~SpdRCTrackSeedsFinder() 
{
   if (fSeeds) delete fSeeds;
}

//_____________________________________________________________________________
void SpdRCTrackSeedsFinder::SetDefaultCuts()
{
   // min. number of hits in vertex detector (ITS)
   fCheckMinItsHits = true;
   fMinItsHits = 2;

   // min Pt of seed candidate
   fCheckMinPtSeed = true;
   fMinPtSeed = 0.025;                    // GeV/c

   // min Mom of seed candidate                                               
   fCheckMinMomSeed = true;
   fMinMomSeed = 0.075;                   // GeV/c
}

//_____________________________________________________________________________
InitStatus SpdRCTrackSeedsFinder::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdRCTrackSeedsFinder::Init> RootManager not instantiated " << endl;
       return kFATAL;
   }
   
   //===================== LOAD INPUT DATA OBJECTS =========================/
   
   // get Event 
   
   fEvent = (SpdMCEvent*)ioman->GetObject(SpdDataBranchName(kMCEvent));
   if (!fEvent) return kFATAL;
   
   Bool_t init_input = false; 
      
   // --- get Its Digi Hits array ---
    
   fItsHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCItsHits));
   if (fItsHits) init_input = true;
   
   if (!init_input) {
       cout << "-F- <SpdRCTrackSeedsFinder::Init> No input data " << endl;
       return kFATAL; 
   }
   
   //============== CREATE AND REGISTER OUTPUT DATA OBJECTS ================/
   
   fSeeds = new TClonesArray("SpdTrackRCSeed");
   ioman->Register(SpdDataBranchName(kRCTrackSeeds),"SpdTrackRCSeed", fSeeds, fSaveSeeds);
   
   //============== PASS DATA TO EVENTHELPER ================================/   

//   SpdMCEventHelper* helper = SpdMCEventHelper::Instance();
   
//   helper->SetSeedsRC(fSeeds);
//   if (!helper->GetEvent()) helper->SetEvent(fEvent);

   //============== CREATE AND INITIALIZE FITTER ============================/
   
   if (fVerboseLevel > -1) cout << "-I- <SpdRCTrackSeedsFinder::Init> Intialization successfull " 
                                << endl;
   
   return kSUCCESS;
}

//________________________________________________________________________________________
void SpdRCTrackSeedsFinder::Exec(Option_t* opt) 
{
   assert(fEvent);
   assert(fItsHits); 
   
   if (fVerboseLevel > -1) {
       cout << "\n-I- <SpdRCTrackSeedsFinder::Exec> Process event ... " 
            << fEvent->GetEventId(); // << "\n\n";
   }
  
   fSeeds->Delete();
   ClearAll();                                              // clear 

   Int_t n = fItsHits->GetEntriesFast();                    
   SpdMCSiliconHit* hit;   

   if (fItsHits && n > 0)                                   // Its hits
   {
      for (Int_t i(0); i < n; i++)                          // loop over Its hits
      {
         hit = dynamic_cast<SpdMCSiliconHit*>(fItsHits->At(i));
         assert(hit);

         fXYZhits.push_back(hit->GetHit());                 // hit space point in GS
         fTypeHits.push_back((Int_t)hit->GetDetId(0));      // barrel or endcap
       
         Int_t iLay = hit->GetDetLayer();                   // detector layer
         fIts_map[iLay].push_back(i);                       // hit in layer (iLay)

         ++fNHitsAccepted;
      }

      if (fVerboseLevel > 1) {
          cout << "\n-I- <SpdRCTrackSeedsFinder::Exec> Number of Its hits: (accepted/total) => " 
               << fNHitsAccepted << " / " << n; 
      }

      if (fVerboseLevel > 1)  PrintItsHits();                // print Its hits

      fMaxItsLayers = fIts_map.rbegin()->first;              // max number of layers    
 
      Create2pointsSeeds();                                  // create 2-points seeds 

      CreateNpointsSeeds();                                  // create n-points seeds  

      MergeSeedCands();                                      // merge seeds

      ZofPrimaryVertexCand();                                // PV estimation 

      CheckNpointsSeeds();                                   // check n-points seeds

      CleanNpointsSeeds();                                   // clean n-points seeds 

   }  // end if of Its hits

   if (fVerboseLevel > 1) PrintSeeds();                      // final print
   
   // fill output seeds starting from 5-points seeds
   //------------------------------------------------
   SpdTrackRCSeed* seed;
   for (auto it = fSeed_map.rbegin(); it != fSeed_map.rend(); ++it)       
   {
      auto& vseed = fSeed_map[it->first];
      for (auto iseeds : vseed)
      { 
         seed = AddSeed();                                 // add new seed         

         SpdTrackRCSeedPar* vpars = dynamic_cast<SpdTrackRCSeedPar*>(seed->SeedPars());
         assert(vpars);

         vpars->SetNHits (iseeds.seedPoint.size());
         vpars->SetCharge(iseeds.charge);
         vpars->SetRadius(iseeds.radius);
         vpars->SetTheta (iseeds.theta);
         vpars->SetPhi   (iseeds.phi);
         vpars->SetZvtx  (iseeds.zvtx);
         vpars->SetRes   (iseeds.res);
//         vpars->SetChi2  (iseeds.chi2);
//         vpars->SetNDF   (iseeds.ndf);

         for (auto ix : iseeds.seedPoint)
         {
            hit = dynamic_cast<SpdMCSiliconHit*>(fItsHits->At(ix));
            assert(hit);

            seed->AddHit(spd_its_point(hit->GetId(),kMCItsHits) );
         }
      }
   }

   // Counters
   //----------
   if (fVerboseLevel > 0) 
   { 
       cout << "\n-I- <SpdRCTrackSeedsFinder::Exec> Print ... " << endl; 

       // total number of seeds 
       for (Int_t is(2); is<fMaxItsLayers+1; ++is) fNSeedsTotal += fSeed_map[is].size();
       
       cout << " Its hits (total):                " << n << endl;
       cout << " Its hits (accepted):             " << fNHitsAccepted << endl;
       cout << " Its hits (used):                 " << fUsedHits.size() << endl;
       cout << endl;
       cout << " Total number of seeds:           " << fNSeedsTotal << endl;
       for (Int_t i(2); i<fMaxItsLayers+1; ++i)
       {
          cout << " Number of " << i << "-points seeds:        " 
                                       << fSeed_map[i].size() << endl;
       }
   }

}

//________________________________________________________________________________________ 
void SpdRCTrackSeedsFinder::Create2pointsSeeds()
{
    //------------------------------------------------------------------
    // create 2-points seeds, loop over layers, starting from 1-st layer
    //------------------------------------------------------------------

    if (fVerboseLevel > 2)
       cout << "-I- <SpdRCTrackSeedsFinder::Create2pointsSeeds>  ";

    for (Int_t is(1); is<fMaxItsLayers; ++is) {                        
       for (Int_t js=is+1; js<fMaxItsLayers+1; ++js) Create2pointsSeed(is,js);  
    }

}

//________________________________________________________________________________________ 
void SpdRCTrackSeedsFinder::Create2pointsSeed(Int_t il1, Int_t il2)
{
    //----------------------------------------------------------------
    // create 2-points seeds (ordered with increasing detector layer):
    // a) loop over possible combinations of hits in two layers
    //    starting from 1-st layer
    // b) il1 -> 1-st layer, il2 -> 2-d layer
    //-----------------------------------------------------------------

    if (fVerboseLevel > 2)
       cout << "-I- <SpdRCTrackSeedsFinder::Create2pointsSeed>  ";
  
    TVector3 tmpRad;                                        // circle parameters 
    Double_t zvtx, theta, res;                              // zvtx, theta, res

    SeedCand fSeed1;

    auto& ihits = fIts_map[il1];                            // Its hits in il1-layer
    auto& jhits = fIts_map[il2];                            // Its hits in il2-layer

    for (auto ip1 : ihits)                                
    {
       for (auto ip2 : jhits)                             
       {
          // use hits only from the same detector part (barrel or endcup)
          //--------------------------------------------------------------
          if (fSameHits && (fTypeHits[ip1] != fTypeHits[ip2]) ) continue;

          fSeed1.Reset();                                   // seed reset

          // update seed
          //------------ 
          fSeed1.seedLay.push_back(il1);                    // add 1-st layer
          fSeed1.seedPoint.push_back(ip1);                  // add hit's number
    
          fSeed1.seedLay.push_back(il2);                    // add 2-nd layer 
          fSeed1.seedPoint.push_back(ip2);                  // add hit's number

          if (!RadiusNPointsSeed(fSeed1, tmpRad)) continue; // circle parameters

          // update seed
          //------------
          fSeed1.seedRad = tmpRad;                          // xc, yc, rc on XY - plane
          fSeed1.charge  = ChargeOfSeedCand(fSeed1);        // charge
          fSeed1.radius  = tmpRad.Z();                      // radius

          if (!ZvtxThetaNpointsSeed(fSeed1, zvtx, theta, res)) continue;

          fSeed1.theta  = theta;                            // theta of line fit
          fSeed1.phi    = fXYZhits[ip1].Phi();              // phi at 1-st hit
          fSeed1.zvtx   = zvtx;                             // zvtx of line fit
          fSeed1.res    = res;                              // res  of line fit

          fSeed_map[2].push_back(fSeed1);                   // add seed in map
       } 
    }

}

//__________________________________________________________________
Double_t SpdRCTrackSeedsFinder::ChargeOfSeedCand(SeedCand& fSeed1)
{
    if (fVerboseLevel > 2) 
         cout << "\n-I- <SpdRCTrackSeedsFinder::ChargeOfSeedCand>  ";

    //--------------------------------------------------------------
    // charge: +1 -> clockwise, -1 -> anticlockwise, 0 -> undefined  
    //--------------------------------------------------------------
    if (fSeed1.seedPoint.size() < 2) return 0.;

    Int_t ip1 = fSeed1.seedPoint.front();              // 1-st hit
    Int_t ip2 = fSeed1.seedPoint.back() ;              // last hit

    TVector3 xp1 = fXYZhits[ip1];                      // xyz in GS
    TVector3 xp2 = fXYZhits[ip2];                      // xyz in GS

    Double_t charge = 0.;
    Double_t dPhi = TMath::Abs(xp2.Phi()-xp1.Phi());

    if (xp2.Phi()*xp1.Phi() > 0.)
    {
       charge = (xp2.Phi()-xp1.Phi() < 0.) ? -1. : 1.;
    }
    else
    {
       if (xp1.Phi() > 0.) charge = (dPhi < PI) ? -1. :  1.;
       else                charge = (dPhi < PI) ?  1. : -1.;  
    }

    return charge;

}

//________________________________________________________________________________________________
Bool_t SpdRCTrackSeedsFinder::ZvtxThetaNpointsSeed(SeedCand& fSeed1, Double_t& zvtx, Double_t& theta, Double_t& res)
{
   //------------------------------------------------------------------
   // Estimation z-coordinate of primary vertex and line slope using 
   // Least Square Method for line on plane z(s) = tanL * s + zvtx;
   //
   // tanL - tangent of deep angle, zvtx - originate vertex,
   // z - z coordinate, s - curvature length
   //   
   // s_i = qTrk * R * (phi_i - phi0);
   // z <->y ; x <-> s;
   //
   // a*Sum(X_i^2) + b*Sum(X_i) = Sum(X_i*Y_i)
   // a*Sum(X_i)   + n*b        = Sum(Y_i)
   //
   // a = ( n*Sum(X_i*Y_i)-Sum(X_i)*Sum(Y_i) ) / ( n*Sum(X_i^2)-(Sum(X_i))^2 )
   // b = ( Sum(Y_i) - a*Sum(X_i) ) / n;
   //--------------------------------------------------------------------------

   if (fVerboseLevel > 2) 
      cout << "\n-I- <SpdRCTrackSeedsFinder::ZvtxThetaNpointsSeed>  ";

   zvtx = theta = res = 0.;                   // zvtx, theta, residual

   if (fSeed1.seedPoint.size() < 2) return false;

   Double_t a, b, n, a1, b1;
   Double_t sx, sy, sx2, sxy, sy2;
   Double_t qTrk, xc, yc, rc;
   TVector3 xp;  

   sx = sy = sxy = sx2 = sy2 = 0.0;

   qTrk = fSeed1.charge;
   xc   = fSeed1.seedRad.X();   
   yc   = fSeed1.seedRad.Y();   
   rc   = fSeed1.seedRad.Z();

   // XY - plane
   //------------
   TVector2 vcr(xc, yc);                               // circle center
   TVector2 v0(0.,0.);                                 // zero point
   Double_t phi0 = (v0 - vcr).Phi();                   // phi of zero point

   // si = qTrk * R * (phi - phi0);
   // z <-> y; x <-> s;
   //-------------------------------
   TVector2 vi;
   Double_t phi, si, dphi;
   for (auto ix : fSeed1.seedPoint)                     // loop over hits
   {
      xp = fXYZhits[ix];

      vi.Set(xp.X(), xp.Y());
      phi = (vi - vcr).Phi();

      if (phi0 < PI ) {
         if ( phi < phi0+PI || phi < phi0 ) dphi = phi - phi0;
         else                               dphi = 2.*PI - phi + phi0;  
      } 
      else {
         if ( phi > phi0-PI) dphi = phi - phi0;
         else                dphi = 2.*PI - phi0 + phi;    
      }

      si  = rc * TMath::Abs(dphi);

      sx  += si;
      sy  += xp.Z();
      sx2 += si * si;
      sxy += xp.Z() * si;
      sy2 += xp.Z() * xp.Z();
   }

   n = fSeed1.seedPoint.size();                        // number of points

   if ((n*sx2-sx*sx) < 0.0001)  return false;          // horizontal line check 
   else
   {
      a = (n*sxy - sx*sy) / (n*sx2 - sx*sx);           // line slope
      b = (   sy -  a*sx) / n;                         // z coordinate

      theta = TMath::ATan(a);
      theta = (theta > 0.0) ? 0.5*PI-theta : 0.5*PI+abs(theta); // in SPD CS
      zvtx = b;
   }

   // residual => res^2 = Sum( (sy - (a * si + b))^2 );
   //---------------------------------------------------
   if (n > 2) res = TMath::Sqrt(TMath::Abs(sy2-2.*a*sxy-2.*b*sy+a*a*sx2+2.*a*b*sx+n*b*b));

   if (TMath::Abs(res) > fMaxRes || TMath::Abs(zvtx) > fMaxPVZvtx) return false; 
   else                                                            return true;

}

//________________________________________________________________________
void SpdRCTrackSeedsFinder::CreateNpointsSeeds()
{
    //--------------------------------------------------
    // create (n+1)-points seeds from n-points seed 
    // adding new hits from layer in increasing order:  
    // it=2 =>(2->3), it=3 =>(3->4), it=4 =>(4->5) )
    //--------------------------------------------------

    if (fVerboseLevel > 2) 
       cout << "\n-I- <SpdRCTrackSeedsFinder::CreateNpointsSeed> ";

    Int_t lastLay,ip2;
    TVector3 xp3, tmpRad;

    Double_t dTheta;
    Double_t r1, r2;
    Double_t zvtx, theta, res;

    SeedCand fSeed1;              
 
    for (Int_t it(2); it<fMaxItsLayers; ++it)                // loop over seeds
    {
       auto& vseed = fSeed_map[it];

       for (auto iseed : vseed)   
       {
          lastLay = iseed.seedLay.back();                    // last layer
          ip2 = iseed.seedPoint.back();                      // last hit in seed

          for (Int_t il3=lastLay+1; il3<fMaxItsLayers+1; ++il3)    // next layer
          {
             auto& ihits = fIts_map[il3];
             for (auto ip3 : ihits)                           // hit in layer
             {
                // use hits only from the same detector part (barrel or endcup)
                //--------------------------------------------------------------
                if (fSameHits && (fTypeHits[ip2] != fTypeHits[ip3]) ) continue;

                xp3 = fXYZhits[ip3];                          // XYZ of new hit in GS

                // theta of new hit should be inside theta range
                //-----------------------------------------------
                TVector3 xpZvtx(0.0, 0.0, iseed.zvtx);        // reference vertex 
                
                dTheta = TMath::Abs((xp3-xpZvtx).Theta() - iseed.theta);

                if (dTheta > fMaxDtheta)  continue;           // hits should be inside dTheta  
 
                fSeed1.Reset();                               // reset seed

                // update seed
                fSeed1 = iseed;                               // copy seed

                fSeed1.seedLay.push_back(il3);                // add layer
                fSeed1.seedPoint.push_back(ip3);              // add hit

                // check delta radius with new hit
                //--------------------------------- 
                r1 = fSeed1.radius;                           // previous radius

                if (!RadiusNPointsSeed(fSeed1, tmpRad))       continue;
                r2 = tmpRad.Z();                              // new radius

                if (TMath::Abs(r1-r2)/TMath::Max(r1,r2) > fMaxDrad)   continue;

                // update seed
                fSeed1.seedRad = tmpRad;                        // radius
                fSeed1.radius  = tmpRad.Z();                    // radius

                if (!ZvtxThetaNpointsSeed(fSeed1, zvtx, theta, res))  continue;

                // update seed
                //-------------
                fSeed1.theta  = theta;                          // theta (line fit)
                fSeed1.zvtx   = zvtx;                           // z of PV (line fit)
                fSeed1.res    = res;                            // residial (line fit)

                fSeed_map[it+1].push_back(fSeed1);              // add new seed in map

             }  // end hits in il3 layer

          }     // end loop over il3 layer

       }        // end loop over seeds

    }           // end loop over fMaxItsLayers

}

//________________________________________________________________________________________
Bool_t SpdRCTrackSeedsFinder::RadiusNPointsSeed(SeedCand& fSeed1, TVector3& tmpVtx)
{
   if (fVerboseLevel > 2) 
       cout << "\n-I- <SpdRCTrackSeedsFinder::RadiusNPointsSeed>  ";

   if (fSeed1.seedPoint.size() < 2) return false;

   //----------------------------------------------------------------
   // Least Square Method for finding circle parameters (xc, yc, rc),
   // use XY(0.,0.) - point as additional hit for 2-ponts seed
   //----------------------------------------------------------------
   Int_t ip;
   TVector3 xp;

   Double_t sx, sx2, sx3; 
   Double_t sy, sy2, sy3;
   Double_t sxy, sxy2, sx2y, sx2y2;

   sx = sx2 = sx3 = 0.0;
   sy = sy2 = sy3 = 0.0;
   sxy = sxy2 = sx2y = sx2y2 = 0.0;

   for (Int_t i=0; i<fSeed1.seedPoint.size(); ++i)
   {
       ip = fSeed1.seedPoint[i];
       xp = fXYZhits[ip];        

       sx  += xp.X();
       sx2 += xp.X()*xp.X();
       sx3 += xp.X()*xp.X()*xp.X();

       sy  += xp.Y();
       sy2 += xp.Y()*xp.Y();
       sy3 += xp.Y()*xp.Y()*xp.Y();

       sxy   += xp.X()*xp.Y();
       sxy2  += xp.X()*xp.Y()*xp.Y();
       sx2y  += xp.X()*xp.X()*xp.Y();
       sx2y2 += xp.X()*xp.X() + xp.Y()*xp.Y();
   }

   Double_t nPoints = (fSeed1.seedPoint.size()==2) ? 3. : fSeed1.seedPoint.size();

   Double_t norm = 1. / nPoints;

   Double_t a1 = 2.*norm*sx*sx - 2.*sx2;
   Double_t b1 = 2.*norm*sx*sy - 2.*sxy;
   Double_t b2 = 2.*norm*sy*sy - 2.*sy2;
   Double_t c1 = sx3 + sxy2 - norm*sx*sx2y2;
   Double_t c2 = sy3 + sx2y - norm*sy*sx2y2;
   Double_t a2 = b1;

   if (TMath::Abs(a1*b2-b1*b1) < fMinLine) return false;   // stright line

   Double_t xr = (b1*c2-c1*b2)/(a1*b2-b1*b1);
   Double_t yr = (b1*c1-a1*c2)/(a1*b2-b1*b1);
   Double_t rr = xr*xr + yr*yr + norm*sx2y2 - 2.*norm*(xr*sx+yr*sy);

   if (rr < 0.0) return false;

   tmpVtx.SetXYZ(xr, yr, sqrt(rr));

   return true;

}

//________________________________________________________________________________________
void SpdRCTrackSeedsFinder::MergeSeedCands()
{
   //---------------------------------------------------------------
   // merge seeds: (2->3, 2->4, 2->5), (3->4, 3->5, 4->5),
   // high-level seed should contains all hits from low-level seed
   //
   // is1 - low-level seed 
   // is2 - high-level seed 
   //----------------------------------------------------------------

   if (fVerboseLevel > 2)
      cout << "\n-I- <SpdRCTrackSeedsFinder::MergeSeedCands>  ";

    Int_t indx;
    std::vector<Int_t> clearList;

    for (Int_t is1(2); is1<fMaxItsLayers; ++is1) 
    {
       for (Int_t is2=is1+1; is2<fMaxItsLayers+1; ++is2)
       {
          auto& v1 = fSeed_map[is1];                        // low-level seed
          auto& v2 = fSeed_map[is2];                        // high-level seed

          clearList.clear();

          indx = -1;
          for (auto ix1 : v1) {                             // loop over low-level seed
             ++indx;
             for (auto ix2 : v2) {                          // loop over high-level seed 
                if(CompareSeedCand(ix1, ix2, 0)) { clearList.push_back(indx);  break; }
             }
          }

          for (auto it1 = clearList.rbegin(); it1 != clearList.rend(); ++it1) {
             fSeed_map[is1].erase( fSeed_map[is1].begin()+(*it1) );
          } 
       }
    } 

}

//________________________________________________________________________________________
Bool_t SpdRCTrackSeedsFinder::CompareSeedCand(SeedCand& fSeed1, SeedCand &fSeed2, Int_t nMax)
{
   if (fVerboseLevel > 2)
      cout << "\n-I- <SpdRCTrackSeedsFinder::CompareSeedCand>  ";

   // check difference of seed1 and seed2 hits
   //-------------------------------------------
   Int_t nDiff = fSeed1.seedPoint.size();

   auto& v1 = fSeed1.seedPoint;                      // low-level seed
   auto& v2 = fSeed2.seedPoint;                      // high-level seed

   for (auto ix1 : v1) {
       if (std::find(v2.begin(), v2.end(), ix1) != v2.end() ) --nDiff; 
   }

  if(nDiff <= nMax) return kTRUE;
  else return kFALSE;

}

//________________________________________________________________________________________
void SpdRCTrackSeedsFinder::CleanNpointsSeeds()
{
   //----------------------------------------------------
   // cleaning procedure for 2, 3, 4 and 5-points seeds
   // (remove duplicates), if seeds are differed only 
   // for one hit and dTheta < fMaxDtheta =>  
   // remove seed with greater residual
   //----------------------------------------------------

   if (fVerboseLevel > 2)
      cout << "\n-I- <SpdRCTrackSeedsFinder::CleanNpointsSeeds>  ";

    Double_t dTheta;
    Int_t ind1, ind2, clear;
    std::vector<Int_t>  clearList;

    for (Int_t is(2); is<fMaxItsLayers+1; ++is)
    {
       auto& vseed = fSeed_map[is];
 
       if (vseed.size() <= 1 ) continue;  

       ind1 = -1;
       clearList.clear();

       for (auto it1 = vseed.begin(); it1 != vseed.end()-1; ++it1) {
          ++ind1;
          ind2 = ind1;   
          for (auto it2 = it1+1; it2 != vseed.end(); ++it2) {
             ++ind2;
             dTheta = TMath::Abs( (*it1).theta-(*it2).theta );

             if (CompareSeedCand(*it1,*it2,1) && dTheta < fMaxDtheta)
             {
                clear = ind2;
                if ((*it1).res > (*it2).res) clear = ind1;

                if (std::find(clearList.begin(),clearList.end(),clear) == clearList.end()) 
                                                                clearList.push_back(clear);
             }
          }
       }

       std::sort(clearList.begin(),clearList.end());
       for (auto it1 = clearList.rbegin(); it1 != clearList.rend(); ++it1) {
          fSeed_map[is].erase( fSeed_map[is].begin()+(*it1) );
       }
    }

    // fill vector fUsedHits with used Its hits in 3,4 and 5-points seeds
    //--------------------------------------------------------------------
    for (Int_t is(2); is<fMaxItsLayers+1; ++is) 
    {
       auto& vseed = fSeed_map[is];
       for (auto iseeds : vseed) {
          for (auto ip : iseeds.seedPoint) {
             if (std::find(fUsedHits.begin(),fUsedHits.end(),ip) != fUsedHits.end()) continue;
             fUsedHits.push_back(ip);
          }
       }
    }
    std::sort(fUsedHits.begin(), fUsedHits.end());

}

//______________________________________________________________________________
void SpdRCTrackSeedsFinder::CheckNpointsSeeds()
{
   if (fVerboseLevel > 2)
      cout << "\n-I- <SpdRCTrackSeedsFinder::CheckNpointsSeed>  ";

   //----------------------------------------
   // next conditions should be correct:
   //
   // 1) check pt > pt_min
   // 2) check momentum > p_min
   // 3) check distance to z of PV
   //----------------------------------------

   if (fVerboseLevel > 2)
   {
      cout << " Number of primary vertex candidates = " << fZprimVtx.size() << endl;

      for (Int_t i(0); i<fZprimVtx.size(); ++i) 
      {
          cout << " vertex = " << i << " z = " << fZprimVtx[i].zVtx << " sigma = " 
               << fZprimVtx[i].sigVtx << " nSeeds = " << fZprimVtx[i].np << endl;
      }  
   }

   // loop over N-points seeds and check distance of z-coordinate to PV
   //------------------------------------------------------------------
   Int_t indx;
   Bool_t accept;
   Double_t pt, mom, dZmax;

   std::vector<Int_t> clearList;

   for (Int_t is(2); is<fMaxItsLayers+1; ++is)
   {
      clearList.clear();
      auto& vseed = fSeed_map[is];

      indx = -1;
      for (auto iseeds : vseed)
      { 
         ++indx;
         accept = false;

         pt = 0.003*iseeds.radius;                     // 10 kG 
         mom = pt / sin(iseeds.theta);

         for (auto iz : fZprimVtx)  {                        // loop over vertexes
//            dZmax = (is == 2) ? 0.5 : TMath::Max(fMaxPVDist, 10.*iz.sigVtx);
            dZmax = (is == 2) ? 0.5 : TMath::Max(fMaxPVDist, 5.*iz.sigVtx);

            if (TMath::Abs(iseeds.zvtx-iz.zVtx) < dZmax) accept = true;
         }

         if (!accept) 
            { clearList.push_back(indx); continue; } 

         if (pt < fMinPtSeed || mom < fMinMomSeed) 
            { clearList.push_back(indx); continue; }
      }

      for (auto it1 = clearList.rbegin(); it1 != clearList.rend(); ++it1) {
         fSeed_map[is].erase( fSeed_map[is].begin()+(*it1) );
      }

   }

}

//___________________________________________________
void SpdRCTrackSeedsFinder::ZofPrimaryVertexCand()
{
   if (fVerboseLevel > 2)
      cout << "-I- <SpdRCTrackSeedsFinder::ZofPrimaryVertexCand>  " << endl;

   //------------------------------------------------------------------
   // find primary vertex candidates using simple clustering procedure:
   // a) use only 5, 4 or 3 - points seeds for primary vertex estimation
   // b) calculate distance between z-coordinates
   // c) do clustering around candidate with minimum distance
   //-------------------------------------------------------------------

   std::vector<Int_t> clearList;
   std::vector<Double_t> distVtx;
   std::vector<std::pair<Int_t, Int_t>> pairVtx;

   Int_t itr, minTrk;   
   Double_t zMean, zSigma;

   // total number of 3-, 4- and 5-points track candidates
   //-----------------------------------------------------
   Int_t ns3 = fSeed_map[3].size();
   Int_t ns4 = fSeed_map[4].size();
   Int_t ns5 = fSeed_map[5].size();

   Int_t is = ((ns3+ns4+ns5) == 0) ? 2 : ((ns4+ns5 > 3) ? 4 : 3);
 
   // primary vector with z-coordinate of PV only for 3,4 and 5-points seeds
   //-----------------------------------------------------------------------
   std::vector<Double_t> zVtx;

   Int_t fMaxSeed = fSeed_map.rbegin()->first;

   for (Int_t i(is); i<fMaxSeed+1; ++i) {
      auto& vseed = fSeed_map[i];
      for (auto iseeds : vseed) zVtx.push_back(iseeds.zvtx);
   }

   minTrk = (zVtx.size() == 2) ? 2 : 3;        // special for 2 candidates  

   zPrimVtxCand fVtx;                          // PV structure

   if (zVtx.size() == 1)                       // if only 1 seed                   
   {
      fVtx.np     = 1;
      fVtx.zVtx   = zVtx[0];
      fVtx.sigVtx = 0.1;

      fZprimVtx.push_back(fVtx);
   }

   itr = 0; 
   while (itr < fMaxPVIter)                     // start iteration
   {
      if (zVtx.size() < minTrk) break;         // < 3

      // distance between two z-points of seed
      //--------------------------------------
      for (Int_t i(0); i<zVtx.size()-1; ++i)   {
         for (Int_t j=i+1; j<zVtx.size(); ++j)   {
              distVtx.push_back( TMath::Abs(zVtx[i]-zVtx[j]) );
              pairVtx.push_back( std::make_pair(i,j) );
         }
      }

      // find index with smallest distance
      //------------------------------------
      auto smallest = std::min_element(std::begin(distVtx), std::end(distVtx));
      Double_t minDist = *smallest;
      Int_t minIndx = std::distance(std::begin(distVtx),smallest);

      if (minDist > fMaxPVDist) break;

      Int_t it1 = pairVtx[minIndx].first;                        // index of 1-st seed
      Int_t it2 = pairVtx[minIndx].second;                       // index of 2-nd seed

      clearList.push_back(it1); 
      clearList.push_back(it2); 

      zMean = 0.5*(zVtx[it1]+zVtx[it2]);                         // mean z of 2 seeds
      Int_t nTrk = 2;
      for (Int_t i(0); i<zVtx.size(); ++i)
      {
         if (i==it1 || i==it2) continue;
         if ( TMath::Abs(zMean-zVtx[i]) > fMaxPVDist) continue;     // delta z < maxDist

         zMean = (nTrk*zMean)/(nTrk+1) + zVtx[i]/(nTrk+1);
         ++nTrk;

         clearList.push_back(i);
      } 

      if (nTrk >= minTrk)                                  // need 3 or more seeds  
      {
         zSigma = 0;
         for (auto iz : zVtx) {
            if ( TMath::Abs(zMean-iz) > fMaxPVDist) continue;
            zSigma += (zMean-iz)*(zMean-iz);
         }
         zSigma = sqrt( zSigma/(nTrk-1) );

         fVtx.np     = nTrk;
         fVtx.zVtx   = zMean;
         fVtx.sigVtx = zSigma;

         fZprimVtx.push_back(fVtx);
      } 

      // clean already used points
      //---------------------------
      std::sort( clearList.begin(), clearList.end() );

      for (auto it = clearList.rbegin(); it != clearList.rend(); ++it) {
           zVtx.erase( zVtx.begin()+(*it) );
      }

      distVtx.clear();
      pairVtx.clear();
      clearList.clear();

      ++itr;

   }  // end while 

  
   if (fVerboseLevel > 2)
   {
      cout << " Number of PV vertex candidates: " << fZprimVtx.size() << " => "; 
      for (Int_t i(0); i<fZprimVtx.size(); ++i)
          cout << " z1 = " << fZprimVtx[i].zVtx << " ";
      cout << endl;
   } 

   if (fZprimVtx.size() <= 0 ) fZprimVtx.push_back(fVtx);

}

//________________________________________________________________________________________  
SpdTrackRCSeed* SpdRCTrackSeedsFinder::AddSeed() 
{
   if (fVerboseLevel > 2)
      cout << "-I- <SpdRCTrackSeedsFinder::AddSeedd>  " << endl;

   if (!fSeeds) return 0;

   Int_t size = fSeeds->GetEntriesFast();
   SpdTrackRCSeed* seed = new ((*fSeeds)[size]) SpdTrackRCSeed();
   seed->SetId(size);

   return seed;
}

//________________________________________________________________________________________  
void SpdRCTrackSeedsFinder::ClearAll()
{
   if (fVerboseLevel > 2)
      cout << "-I- <SpdRCTrackSeedsFinder::ClearAll>  " << endl;

      fNHitsAccepted = 0;                      // accepted Its hits
      fNHitsUsed     = 0;                      // used Its hits
      fNSeedsTotal   = 0;                      // found track seeds

      fXYZhits.clear();                        // hit point in GS
      fTypeHits.clear();                       // type of hit (barrel or endcup) 
      fUsedHits.clear();                       // used  hit (1 or 0) 
      fZprimVtx.clear();                       // primary vetex array

      fIts_map.clear();                        // Its hits
      fSeed_map.clear();                       // track seeds

}

//_______________________________________________________________________
void SpdRCTrackSeedsFinder::PrintSeeds()
{
   if (fVerboseLevel > 2)
      cout << "\n-I- <SpdRCTrackSeedsFinder::PrintSeeds>  " << endl;

   for (Int_t is(2); is<fMaxItsLayers+1; ++is) {
      cout << "\n-I- <SpdRCTrackSeedsFinder::Exec> => " << is << "-points seeds" << endl;
      PrintSeed(is);
   }

}

//________________________________________________________________________________________  
void SpdRCTrackSeedsFinder::PrintSeed(Int_t is)
{
   if (fVerboseLevel > 2)
      cout << "\n-I- <SpdRCTrackSeedsFinder::PrintSeed>  " << endl;

    auto& vseed = fSeed_map[is]; 
    Int_t indx = -1;
    for (auto iseeds : vseed)
    {
        ++indx; 
        cout << " i = " << indx << " => ";
        for (auto ip : iseeds.seedPoint) cout << ip << " "; 

            Double_t pt = 0.003*iseeds.radius;           // 1 T

            cout << " q = "     << iseeds.charge
                 << " pt = "    << pt
                 << " p = "     << pt / sin(iseeds.theta)
                 << " theta = " << iseeds.theta
                 << " phi = "   << iseeds.phi
                 << " zvtx = "  << iseeds.zvtx
                 << " res = "   << iseeds.res;
            cout << endl;
    }

}

//________________________________________________________________________________________ 
void SpdRCTrackSeedsFinder::PrintItsHits() 
{
     cout << "\n-I- <SpdRCTrackSeedsFinder::Exec> Its hits in detector layers" << endl;

     if (fIts_map.size() > 0)
     {
        for (auto& ilay : fIts_map) 
        {
            cout << " Detector layer = " << ilay.first << " => ";
            for (auto ihit : ilay.second) cout << ihit << " ";
            cout << endl;
         }
     }

}

//________________________________________________________________________________________ 
void SpdRCTrackSeedsFinder::PrintCuts() const
{
   cout << "\n\t-I- <SpdRCTrackSeedsFinder::PrintCuts> \n\n"; 

   cout << "\tmin. number of Its hits (value) :      " << ((fCheckMinItsHits) ? "yes" : "no") 
        << " (" << fMinItsHits << ") " << endl;
   cout << "\tmax. number of Its layers (value) :    " << ((true) ? "yes" : "no") 
        << " (" << fMaxItsLayers << ") " << endl;
   cout << "\tmin. transverse momentum (value) :     " << ((fCheckMinPtSeed) ? "yes" : "no") 
        << " (" << fMinPtSeed << ", GeV/c) " << endl;        
   cout << "\tmin. momentum (value) :                " << ((fCheckMinMomSeed) ? "yes" : "no") 
        << " (" << fMinMomSeed << ", GeV/c) " << endl;  
  
   cout << endl;     

   cout << "\thits from same detector part:          " << ((fSameHits) ? "yes" : "no") << endl;
   cout << "\tprimary vertex range:                  " << "yes (+-" << fMaxPVZvtx << ", cm)" 
        << endl;
   cout << "\tmax. iteration for PV estimaton:       " << "yes (" << fMaxPVIter << ")" 
        << endl;
   cout << "\tmax. delta theta range:                " << "yes (+-" << fMaxDtheta << ", radian)" 
        << endl;
   cout << "\tmax. residual of line fit:             " << "yes (+-" << fMaxRes << ", cm)" 
        << endl;
   cout << "\tmax. distance for PV clustering:       " << "yes (+-" << fMaxPVDist << ", cm)" 
        << endl;

   //cout << "\n";
}

//________________________________________________________________________________________  
void SpdRCTrackSeedsFinder::Finish()
{
   if (fVerboseLevel > -1) 
   { 
       cout << "\n-I- <SpdRCTrackSeedsFinder::Finish> ... ";
       
       PrintCuts();
       
   }
}

