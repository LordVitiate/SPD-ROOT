// $Id$
// Author: artur   2021/03/02

//_____________________________________________________________________________
//
// SpdRsMCHitProducer
//_____________________________________________________________________________

#include "SpdRsMCHitProducer.h"

#include "SpdSetParSet.h"

#include "SpdRunAna.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include "SpdSetParSet.h"
#include "SpdParSet.h"

#include "SpdRsTB2GeoMapper.h"
#include "SpdRsTEC2GeoMapper.h"

#include "SpdMCEvent.h"
#include "SpdMCParticle.h"
#include "SpdRsMCHit.h"

#include "SpdRsTB2Point.h"
#include "SpdRsTEC2Point.h"

#include "SpdMCEventHelper.h"
#include "SpdParticlesAndProcesses.h"
#include "SpdGeopathParser.h"
 
#include <TStatistic.h>
#include <TGeoManager.h>

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdRsMCHitProducer)

//_____________________________________________________________________________
SpdRsMCHitProducer::SpdRsMCHitProducer():
fEvent(0),fParticles(0),fIdTable(0),
fHits(0),
fCellMinTrackLen(0.2),fCellMinTrackDep(0.2),
fSaveHits(true),
fMakeStripHits(false),
fNFailedHits(0),fNTotalHits(0),
fVerboseLevel(1)
{
   fGeoMapper[0]  = 0;            fGeoMapper[1]  = 0; 
   fParsName[0]   = "RsTBParSet"; fParsName[1]   = "RsTECParSet";
   fParameters[0] = 0;            fParameters[1] = 0;
   fSaveDetIdOption[0] = 0;       fSaveDetIdOption[1] = 0; 
   fSaveEmptyHits[0] = 0;         fSaveEmptyHits[1] = 0;
   fDataType[0] = "";             fDataType[1] = "";
   fPointsArray[0] = 0;           fPointsArray[1] = 0;
}

//_____________________________________________________________________________
SpdRsMCHitProducer::~SpdRsMCHitProducer() 
{
   for (Int_t i(0); i<2; i++) { 
        if (fGeoMapper[i]) delete fGeoMapper[i]; 
   }
   
   if (fHits) { fHits->Delete(); delete fHits; }
   
   //SpdMCEventHelper::Instance()->SetTsHits(0);
}

//_____________________________________________________________________________
void SpdRsMCHitProducer::LoadGeometry()
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance(); 
      
   if (!gGeoManager) run->GetParameters("FairGeoParSet");

   if (!gGeoManager) {
       cout << "-F- <SpdRsMCHitProducer::LoadGeometry> Geometry is not defined " << endl;
       exit(1);
   }

   SpdSetParSet* pars;
   TString mapper, builder;
   
   for (Int_t i(0); i<2; i++) 
   {
        fParameters[i] = dynamic_cast<SpdParSet*>(run->GetParameters(fParsName[i]));
        
        if (!fParameters[i]) {
            pars = (SpdSetParSet*)run->GetParameters("SpdSetParSet");
            if (pars) fParameters[i] = pars->GetActiveGeoPars(fParsName[i]);
        }
        
        if (!fParameters[i]) {
            cout << "-F- <SpdRsMCHitProducer::LoadGeometry> No geometry parameters " 
                 << "(\"" << fParsName[i] << "\")" << endl;
            exit(1); 
        }
        
        //fParameters[i]->print(1);
        
        fParameters[i]->GetParameter("Mapper",mapper); 
        fGeoMapper[i] = SpdGeoFactory::Instance()->Mapper(mapper);
         
        if (fGeoMapper[i]) {
            fGeoMapper[i]->LoadParametersFrom(fParameters[i]);
            fGeoMapper[i]->InitGeometry();
            fGeoMapper[i]->LockGeometry();
            //fGeoMapper[i]->Print("");
        }
        
        fParameters[i]->GetParameter("Detector/SaveDetIdOption",fSaveDetIdOption[i]);
        fParameters[i]->GetParameter("Detector/SaveEmptyHits",fSaveEmptyHits[i]);
        fParameters[i]->GetParameter("OutData_1",fDataType[i]);
   }
}

//_____________________________________________________________________________
InitStatus SpdRsMCHitProducer::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdRsMCHitProducer::Init> RootManager not instantiated " << endl;
       return kFATAL;
   }
   
   //========================== LOAD GEOMETRY ==============================/
   
   LoadGeometry();
   
   //========================== ADD PARAMETERS =============================/
   
   AddParameters();
   
   //===================== LOAD INPUT DATA OBJECTS =========================/
   
   // get Event 
   
   fEvent = (SpdMCEvent*)ioman->GetObject(SpdDataBranchName(kMCEvent));
   if (!fEvent) return kFATAL;
   
   // get Particles array
   
   fParticles = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCParticles));
   if (!fParticles) return kFATAL;
   
   SpdMCEventHelper* helper = SpdMCEventHelper::Instance();
   
   // get ID-table 
   
   fIdTable = helper->GetIdTable();
   if (!fIdTable) return kFATAL;
   
   Bool_t init_input = false; 
   
   // get barrel Points array
   fPointsArray[0] = (TClonesArray*)ioman->GetObject(fDataType[0]);
    
   if (!fPointsArray[0]) {
       if (fVerboseLevel > -2) {
           cout << "-W- <SpdRsMCHitProducer::Init> No MC-POINTS: " << fDataType[0] << endl;
       }
   }
   else init_input = true;
   
   // get endcaps Points array
   fPointsArray[1] = (TClonesArray*)ioman->GetObject(fDataType[1]);
    
   if (!fPointsArray[1]) {
       if (fVerboseLevel > -2) {
           cout << "-W- <SpdRsMCHitProducer::Init> No MC-POINTS: " << fDataType[1] << endl;
       }
   }
   else init_input = true;
   
   if (!init_input) {
       cout << "-F- <SpdRsMCHitProducer::Init> No input data " << endl;
       return kFATAL; 
   }
 
   //============== CREATE AND REGISTER OUTPUT DATA OBJECTS ================/
   
   fHits = new TClonesArray("SpdRsMCHit");
   ioman->Register(SpdDataBranchName(kMCRsHits),"SpdRsMCHit", fHits, fSaveHits);  
   
   //============== PASS DATA TO EVENTHELPER ================================/   
   
   helper->SetRsHits(fHits);
 
   if (fVerboseLevel > -1) cout << "-I- <SpdRsMCHitProducer::Init> Intialization successfull " << endl;
   
   return kSUCCESS;
}

//________________________________________________________________________________________
void SpdRsMCHitProducer::Exec(Option_t* opt) 
{
   assert(fEvent);
   assert(fParticles);
   assert(fIdTable);
   
   if(!fPointsArray[0]) assert(fPointsArray[1]);
   else assert(fPointsArray[0]); 
   
   if (fVerboseLevel > 0) {
       cout << "\n-I- <SpdRsMCHitProducer::Exec> Process event ... " 
            << fEvent->GetEventId(); 
       if (fVerboseLevel > 1) cout << "\n";         
   }
   
   fHits->Delete();
          
   // geometry navigator
   TGeoNavigator* GeoNavigator = gGeoManager->GetCurrentNavigator();  
   assert(GeoNavigator); 
      
   // >>>>> Fill hits <<<<< 
         
   Int_t npoints;
   Int_t napoints[2] = {0,0}, nhits[2] = {0,0}, nhfailed[2] = {0,0};
   
   Int_t detId;
   Int_t stripId;
   Int_t SortId;
   SpdRsMCHit* hit;
 
   TString node_path;
   SpdGeopathParser ppath;
   Double_t ploc[3] = {0,0,0};
   Double_t detpos[3];          // hit node position in global CS
   Double_t StripShift = 1; 	// strip board is 1cm above MDT wires

   std::map<Int_t,SpdRsMCHit*> HitsMap;
   std::map<Int_t,SpdRsMCHit*>::const_iterator it;
   
   // hit time finding
   Double_t tt;
   TStatistic* stat;
   std::map<Int_t,TStatistic*> stats;
   std::map<Int_t,TStatistic*>::iterator it_stats;
   
   Bool_t add_hit_time = true; // FIXME
   Bool_t add_strip = false;  
   
   // >>>>>>>>>>>  Barrel >>>>>>>>>>>  
   
   if (fPointsArray[0]) 
   {
       SpdRsTB2Point* point;
       npoints = fPointsArray[0]->GetEntriesFast();
       for (Int_t i(0); i<npoints; i++) 
       {
            point = dynamic_cast<SpdRsTB2Point*>(fPointsArray[0]->At(i));
            if (!AcceptPoint(point)) continue;
            
            napoints[0]++;
            
            // check node path
            node_path = point->GetHitGeoPath();
            
            ppath.ParsePath(node_path);
            if (ppath.GetCurrentPathLevel() < 6) {
                cout << "-E- <SpdRsMCHitProducer::Exec> [B] Geometry level (<6!): " << ppath.GetCurrentPathLevel() 
                         << " for path: " << node_path << endl;
                nhfailed[0]++;
                continue; 
            }
         
            // get detector ID
            detId = SpdRsTID::DetectorId(ppath.Num(2),ppath.Num(3),ppath.Num(4),ppath.Num(6)); 
            
            // simple way to define a strip
            stripId = TMath::Nint( (point->GetPosOut().Z() - 0.5*(point->GetPosOut()-point->GetPosIn()).Z() + 240 - 1.5)/3 ) + 1;
            
            // SortId - var is used for combining a few points in one hit (in the same detector)
            if (fMakeStripHits) {
                // modified detId for sum points to hit in case of strip hits
                SortId = 1000*detId + stripId;
            }
            else {
                // detId in case of no strip hits
                SortId = detId;
            }
            
            if (fVerboseLevel > 3) 
            {  
                cout << "\n\t [B] " << node_path << ", level " << ppath.GetCurrentPathLevel() 
                     << "; detId: " << detId << " = "
                     << ppath.Num(2) << ", " << ppath.Num(3) << ", " 
                     << ppath.Num(4) << ", " << ppath.Num(6);
            }
            
            it = HitsMap.find(SortId);    
            
            if (it == HitsMap.end()) 
            {
                // search for detector
                if (!GeoNavigator->cd(node_path)) {
                    nhfailed[0]++;
                    cout << "-E- <SpdRsMCHitProducer::Exec> [B] Node not found: " << node_path << endl;
                    continue;
                }
            
                // create new wire hit
                hit = AddHit();
                
                hit->SetModId(point->GetDetectorID());
                hit->SetWeight(1);
               
                // set detector ID
                hit->SetDetectorId(detId);
                hit->SetHitType('w');

                if (fMakeStripHits) 
                {
                    hit->SetAssociatedHit(hit->GetId()+1);
                    add_strip = true;
                }
                else hit->SetAssociatedHit(-1);

                // set detector position
                GeoNavigator->LocalToMaster(ploc,detpos);
                hit->SetHitPos(detpos[0],detpos[1],detpos[2]); //center of wire-detector

                // add hit to the table
                HitsMap.insert(std::pair<Int_t,SpdRsMCHit*>(SortId,hit));
        
                nhits[0]++;
                        
                if (add_hit_time) {
                    stat = new TStatistic();
                    tt = 0.5*(point->GetTimeIn()+point->GetTimeOut());
                    stat->Fill(tt);
                    stats.insert(std::pair<Int_t,TStatistic*>(SortId,stat));
                }
            }
            else 
            {
                hit = it->second;
                
                if (add_hit_time) {
                    it_stats = stats.find(SortId);
                    tt = 0.5*(point->GetTimeIn()+point->GetTimeOut());
                    it_stats->second->Fill(tt);
                }
                add_strip = false;
            }
            
            // add hit deposit
            hit->AddResp(point->GetEnergyLoss());

            // FIXME add mc-truth FIXME
            AddMCTruth(point->GetTrackID(),hit);

            // --------------------------------
            // add corresponding to wire a strip-hit
            if (fMakeStripHits && add_strip)
            {
                // create new hit
                hit = AddHit();

                hit->SetModId(point->GetDetectorID());
                hit->SetWeight(1);

                // set detector ID = strip ID
                hit->SetDetectorId(stripId);
                hit->SetHitType('s');
                hit->SetAssociatedHit(hit->GetId()-1);
                
                // set strip position (x, y coordinates are "center" of strip)
                // a unit vector
                Double_t nx = 0.;
                Double_t ny = 1.;
                Int_t SecId = ppath.Num(2); // number of Sector
                Double_t angle = (SecId-1)*TMath::Pi()/4;

//                 Double_t l = 1; // a half of wire plane thickness
//                 hit->SetHitPos(detpos[0] + l*(nx*TMath::Cos(angle) - ny*TMath::Sin(angle)),
//                                detpos[1] + l*(nx*TMath::Sin(angle) + ny*TMath::Cos(angle)),
//                                -240+(stripId-1)*3+1.5 );
// 
//                 if (fVerboseLevel > 3) {
//                     cout << "\n\t [B] X: " << detpos[0] << " Y: " << detpos[1];
//                     cout << "\n\t [B] Sector: " << SecId << " angle: " << angle*TMath::DegToRad()
//                          << " Sin: " << TMath::Sin(angle) << "  Cos: " << TMath::Cos(angle);
//                     cout << "\n\t [B] shift X: " << detpos[0] + x*TMath::Cos(angle) - y*TMath::Sin(angle);
//                     cout << "\n\t [B] shift Y: " << detpos[1] + x*TMath::Sin(angle) + y*TMath::Cos(angle);
//                 }
                
                // transverse (i.e. x-y) distance from point (0,0) to wire position
                Double_t Lhit = TMath::Sqrt(detpos[0]*detpos[0]+detpos[1]*detpos[1]);
                
                if (Lhit < 1e-9) hit->SetHitPos(0, 0, -240+(stripId-1)*3+1.5 ); // ??? wire plane is on the Z-axis 
                else {
                
                    // normalized transverse direction to wire (hit) position
                    Double_t nx_hit = detpos[0]/Lhit;
                    Double_t ny_hit = detpos[1]/Lhit;
        
                    // normalized transverse direction to wire plane (according to octant)
                    Double_t nx_plane = nx*TMath::Cos(angle) - ny*TMath::Sin(angle);
                    Double_t ny_plane = nx*TMath::Sin(angle) + ny*TMath::Cos(angle);

                    // transverse distance to wire plane from point (0,0)
                    Double_t L = Lhit * (nx_plane*nx_hit + ny_plane*ny_hit);
                    
                    // set strip (hit) position
                    hit->SetHitPos( L*nx_plane, L*ny_plane, -240+(stripId-1)*3+1.5 );
                }                
                
                // add hit to the table
                HitsMap.insert(std::pair<Int_t,SpdRsMCHit*>(SortId,hit));

                nhits[0]++;
            }
       }
       
       fNFailedHits += nhfailed[0]; 
       
       if (add_hit_time) {
           for (auto v : stats) {    
                it = HitsMap.find(v.first);
                stat = v.second;
                hit = it->second;
                hit->SetHitTime(stat->GetMean());
                hit->SetHitTimeError(stat->GetRMS());
                delete v.second;
           }
           stats.clear(); 
        } 
        
        HitsMap.clear();
   }
   
   // >>>>>>>>>>>  Endcaps >>>>>>>>>>>  
   
   if (fPointsArray[1]) 
   {
       SpdRsTEC2Point* point;
       npoints = fPointsArray[1]->GetEntriesFast();
       for (Int_t i(0); i<npoints; i++) 
       {
            point = dynamic_cast<SpdRsTEC2Point*>(fPointsArray[1]->At(i));
            if (!AcceptPoint(point)) continue;
            
            napoints[1]++;
            
            // check node path
            node_path = point->GetHitGeoPath();
            
            ppath.ParsePath(node_path);
            if (ppath.GetCurrentPathLevel() < 6) {
                cout << "-E- <SpdRsMCHitProducer::Exec> [EC] Geometry level (<6!): " << ppath.GetCurrentPathLevel() 
                         << " for path: " << node_path << endl;
                nhfailed[0]++;
                continue; 
            }
         
            // get detector ID (EC = 10 and 20)
            detId = SpdRsTID::DetectorId(10*ppath.Num(2),ppath.Num(3),ppath.Num(4),ppath.Num(6));
            stripId = TMath::Nint( (point->GetPosOut().X() - 0.5*(point->GetPosOut()-point->GetPosIn()).X() + 240 - 1.5 )/3 ) + 1;

            // SortId - var is used for combining a few points in one hit (in the same detector)
            if (fMakeStripHits) {
                // modified detId for sum points to hit in case of strip hits
                SortId = 1000*detId + stripId;
            }
            else {
                // detId in case of no strip hits
                SortId = detId;
            } 
    
            if (fVerboseLevel > 3) {  
                cout << "\n\t [EC] " << node_path << ", level " << ppath.GetCurrentPathLevel() 
                     << "; detId: " << detId << " = "
                     << ppath.Num(2) << ", " << ppath.Num(3) << ", " 
                     << ppath.Num(4) << ", " << ppath.Num(6);
            }
            
            it = HitsMap.find(SortId);    
            
            if (it == HitsMap.end()) 
            {
                // search for detector
                if (!GeoNavigator->cd(node_path)) {
                    cout << "-E- <SpdRsMCHitProducer::Exec> [EC] Node not found: " << node_path << endl;
                    nhfailed[0]++;
                    continue;
                }
            
                // create new hit
                hit = AddHit();
                
                hit->SetModId(point->GetDetectorID());
                hit->SetWeight(1);
               
                // set detector ID
                hit->SetDetectorId(detId);
                hit->SetHitType('w');
                
                if (fMakeStripHits)
                {
                   hit->SetAssociatedHit(hit->GetId()+1);
                   add_strip = true;
                }
                else hit->SetAssociatedHit(-1);
                
                // set detector position
                GeoNavigator->LocalToMaster(ploc,detpos);
                hit->SetHitPos(detpos[0],detpos[1],detpos[2]);
               
                // add hit to the table
                HitsMap.insert(std::pair<Int_t,SpdRsMCHit*>(SortId,hit));
                
                nhits[1]++;
                
                if (add_hit_time) {
                    stat = new TStatistic();
                    tt = 0.5*(point->GetTimeIn()+point->GetTimeOut());
                    stat->Fill(tt);
                    stats.insert(std::pair<Int_t,TStatistic*>(SortId,stat));
                }
            }
            else 
            {
                hit = it->second;
             
                if (add_hit_time) {
                    it_stats = stats.find(SortId);
                    tt = 0.5*(point->GetTimeIn()+point->GetTimeOut());
                    it_stats->second->Fill(tt);
                }
                add_strip = false;
            }
            
            // add hit deposit
            hit->AddResp(point->GetEnergyLoss());

            // FIXME add mc-truth FIXME
            AddMCTruth(point->GetTrackID(),hit);

            // --------------------------------
            // add corresponding hit of strip
            if (fMakeStripHits && add_strip)
            {
                // create new hit
                hit = AddHit();

                hit->SetModId(point->GetDetectorID());
                hit->SetWeight(1);

                // set detector ID = strip ID
                hit->SetDetectorId((Int_t)stripId);
                hit->SetHitType('s');
                hit->SetAssociatedHit(hit->GetId()-1);

                // set strip position (y, z coordinates from wire)
                //Double_t Strip_EC_Z = 9999;
                //if ( detpos[2] > 0 ) Strip_EC_Z = detpos[2] + StripShift;
                //else Strip_EC_Z = detpos[2] - StripShift;
                //hit->SetHitPos(-240+(stripId-1)*3+1.5, detpos[1], Strip_EC_Z);
                
                // set strip position (y, z coordinates from wire)
                hit->SetHitPos(-240+(stripId-1)*3+1.5, 0, detpos[2]); //center of a stip
                
                // add hit to the table
                HitsMap.insert(std::pair<Int_t,SpdRsMCHit*>(SortId,hit));

                nhits[1]++;
            }
       }
       
       fNFailedHits += nhfailed[1];  
       
       if (add_hit_time) {
           for (auto v : stats) {    
                it = HitsMap.find(v.first);
                stat = v.second;
                hit = it->second;
                hit->SetHitTime(stat->GetMean());
                hit->SetHitTimeError(stat->GetRMS());
                delete v.second;
           }
           stats.clear(); 
        } 
        
        HitsMap.clear();
   }
   
   fNTotalHits += fHits->GetEntriesFast();
   
   if (fVerboseLevel > 1) 
   {
        cout << "\n-I- <SpdRsMCHitProducer::Exec> Print ... " << endl; 
       
        if (fVerboseLevel > 2 && fHits->GetEntriesFast() > 0) 
        {
            cout << "Print hits [first(barrel,endcap)]: " << endl; 
            if (nhits[0] > 0) {
                hit = dynamic_cast<SpdRsMCHit*>(fHits->At(0));
                if (hit) hit->Print();
            }
            if (nhits[1] > 0) {
                hit = dynamic_cast<SpdRsMCHit*>(fHits->At(nhits[0]));
                if (hit) hit->Print();
            }
        }
        
        for (Int_t i(0); i<fHits->GetEntriesFast(); i++)
        {
            hit = dynamic_cast<SpdRsMCHit*>(fHits->At(i));
            if (hit) hit->Print();
        }
        
        Int_t np1(0), np2(0), nn(0);
        if (fPointsArray[0]) {
            np1 = fPointsArray[0]->GetEntriesFast();
            nn += np1;
        }
        if (fPointsArray[1]) {
            np2 = fPointsArray[1]->GetEntriesFast();
            nn += np2;
        }
        
        cout << "Points/hits (total):             " << nn << "/" << fHits->GetEntriesFast() << endl; 
        cout << "Points(barrel|total,rejected):   " << np1 << "/" << np1-napoints[0] << endl;
        cout << "Points(endcaps|total,rejected):  " << np2 << "/" << np2-napoints[1] << endl;
        cout << "Hits(barrel|total,failed):       " << nhits[0] << "/" << nhfailed[0] << endl;
        cout << "Hits(endcaps|total,failed):      " << nhits[1] << "/" << nhfailed[1] << endl;
   }
}

//________________________________________________________________________________________  
template <class T> Bool_t SpdRsMCHitProducer::AcceptPoint(const T* point) const
{
   if (!point) return false;
   
   Double_t dist  = (point->GetPosIn()-point->GetPosOut()).Mag();
   Double_t eloss = point->GetEnergyLoss()*1.0e6;  // GeV -> keV
   
   if ( (dist < fCellMinTrackLen) && (eloss < fCellMinTrackDep) ) return false;	
   
   return true;   
}

//________________________________________________________________________________________
void SpdRsMCHitProducer::AddMCTruth(Int_t itrack, SpdRsMCHit* hit)
{
   Int_t ipart = SpdMCEventHelper::Instance()->GetParticleId(itrack); 
   hit->AddPoint();
   if (ipart < 0) return; 
   hit->AddMCTruth(ipart); 
   //SpdMCParticle* part = (SpdMCParticle*)fParticles->At(ipart);
   //part->SetChar('+');
}

//________________________________________________________________________________________  
SpdRsMCHit* SpdRsMCHitProducer::AddHit()
{
   if (!fHits) return 0;
   Int_t size = fHits->GetEntriesFast();
   SpdRsMCHit* hit = new ((*fHits)[size]) SpdRsMCHit();
   hit->SetId(size);
   return hit;
}

//________________________________________________________________________________________  
void SpdRsMCHitProducer::Finish()
{
   if (fVerboseLevel > -1) {
       cout << "\n-I- <SpdRsMCHitProducer::Finish> [Hits] "
            << "total: " << fNTotalHits << " failed: " << fNFailedHits;// << endl;
   }
}

//________________________________________________________________________________________
void SpdRsMCHitProducer::AddParameters()
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance();
   SpdSetParSet* params = run->GetParSetAssembly();
   FairParGenericSet* pars;
   pars = run->GetParameters(fParsName[0]);
   params->AddActiveGeoPars(pars);
   pars = run->GetParameters(fParsName[1]);
   params->AddActiveGeoPars(pars);
}


