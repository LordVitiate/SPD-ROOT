// $Id$
// Author: artur   2021/09/10

//_____________________________________________________________________________
//
// SpdTofMCHitProducer
//_____________________________________________________________________________

#include "SpdTofMCHitProducer.h"

#include "SpdSetParSet.h"

#include "SpdRunAna.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include "SpdSetParSet.h"
#include "SpdParSet.h"

#include "SpdTofBGeoMapper.h"
#include "SpdTofECGeoMapper.h"

#include "SpdMCEvent.h"
#include "SpdMCParticle.h"
#include "SpdTofMCHit.h"

#include "SpdTofBPoint.h"
#include "SpdTofECPoint.h"

#include "SpdMCEventHelper.h"
#include "SpdParticlesAndProcesses.h"
#include "SpdGeopathParser.h"
 
#include <TStatistic.h>
#include <TGeoManager.h>

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdTofMCHitProducer)

//_____________________________________________________________________________
SpdTofMCHitProducer::SpdTofMCHitProducer():
fEvent(0),fParticles(0),fIdTable(0),
fHits(0),
fSaveHits(true),
fNFailedHits(0),fNTotalHits(0),
fVerboseLevel(1)
{
   fGeoMapper[0]  = 0;            fGeoMapper[1]  = 0; 
   fParsName[0]   = "TofBParSet"; fParsName[1]   = "TofECParSet";
   fParameters[0] = 0;            fParameters[1] = 0;
   fSaveDetIdOption[0] = 0;       fSaveDetIdOption[1] = 0; 
   fSaveEmptyHits[0] = 0;         fSaveEmptyHits[1] = 0;
   fDataType[0] = "";             fDataType[1] = "";
   fPointsArray[0] = 0;           fPointsArray[1] = 0;
}

//_____________________________________________________________________________
SpdTofMCHitProducer::~SpdTofMCHitProducer() 
{
   for (Int_t i(0); i<2; i++) { 
        if (fGeoMapper[i]) delete fGeoMapper[i]; 
   }
   
   if (fHits) { fHits->Delete(); delete fHits; }
   
   //SpdMCEventHelper::Instance()->SetTofHits(0);
}

//_____________________________________________________________________________
void SpdTofMCHitProducer::LoadGeometry()
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance(); 
      
   if (!gGeoManager) run->GetParameters("FairGeoParSet");

   if (!gGeoManager) {
       cout << "-F- <SpdTofMCHitProducer::LoadGeometry> Geometry is not defined " << endl;
       exit(1);
   }

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
            cout << "-F- <SpdTofMCHitProducer::LoadGeometry> No geometry parameters " 
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
InitStatus SpdTofMCHitProducer::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdTofMCHitProducer::Init> RootManager not instantiated " << endl;
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
           cout << "-W- <SpdTofMCHitProducer::Init> No MC-POINTS: " << fDataType[0] << endl;
       }
   }
   else init_input = true;
   
   // get endcaps Points array
   fPointsArray[1] = (TClonesArray*)ioman->GetObject(fDataType[1]);
    
   if (!fPointsArray[1]) {
       if (fVerboseLevel > -2) {
           cout << "-W- <SpdTofMCHitProducer::Init> No MC-POINTS: " << fDataType[1] << endl;
       }
   }
   else init_input = true;
   
   if (!init_input) {
       cout << "-F- <SpdTofMCHitProducer::Init> No input data " << endl;
       return kFATAL; 
   }
 
   //============== CREATE AND REGISTER OUTPUT DATA OBJECTS ================/
   
   fHits = new TClonesArray("SpdTofMCHit");
   ioman->Register(SpdDataBranchName(kMCTofHits),"SpdTofMCHit", fHits, fSaveHits);  
   
   //============== PASS DATA TO EVENTHELPER ================================/   
   
   helper->SetTofHits(fHits);
 
   if (fVerboseLevel > -1) cout << "-I- <SpdTofMCHitProducer::Init> Intialization successfull " << endl;
   
   return kSUCCESS;
}

//________________________________________________________________________________________
void SpdTofMCHitProducer::Exec(Option_t* opt) 
{
   assert(fEvent);
   assert(fParticles);
   assert(fIdTable);
   
   if (!fPointsArray[0]) assert(fPointsArray[1]);
   else assert(fPointsArray[0]); 
   
   if (fVerboseLevel > 0) {
       cout << "\n-I- <SpdTofMCHitProducer::Exec> Process event ... " 
            << fEvent->GetEventId(); 
       if (fVerboseLevel > 1) cout << "\n";         
   }
   
   fHits->Delete();
                
   // >>>>> Fill hits <<<<< 
         
   Int_t npoints;
   Int_t napoints[2] = {0,0}, nhits[2] = {0,0}, nhfailed[2] = {0,0};
   
   SpdTofMCHit* hit;
 
   // >>>>>>>>>>>  Barrel >>>>>>>>>>>  
   
   if (fPointsArray[0]) 
   {
       SpdTofBPoint* point;
       npoints = fPointsArray[0]->GetEntriesFast();
       for (Int_t i(0); i<npoints; i++) 
       {
            point = dynamic_cast<SpdTofBPoint*>(fPointsArray[0]->At(i));
            if (!AcceptPoint(point)) continue;
            
            napoints[0]++;
            
            //create new hit
            hit = AddHit();
            
            hit->SetModId(point->GetDetectorID());
            hit->SetWeight(1);
             
            nhits[0]++;
            
            // set hit data 
            hit->SetHitTime(point->GetTimeIn());
            hit->SetHitTimeError(0);
            hit->SetHitPos(point->GetPosIn());
            hit->SetTrackMom(point->GetMomIn());
            hit->SetTrackLen(point->GetLength());
            hit->SetNodePath(point->GetNodePath());

            // add hit deposit
            hit->SetResp(point->GetEnergyLoss());

            // FIXME add mc-truth FIXME
            AddMCTruth(point->GetTrackID(),hit);
       }
       
       fNFailedHits += nhfailed[0]; 
   }
   
   // >>>>>>>>>>>  Endcaps >>>>>>>>>>>  
   
   if (fPointsArray[1]) 
   {
       SpdTofECPoint* point;
       npoints = fPointsArray[1]->GetEntriesFast();
       for (Int_t i(0); i<npoints; i++) 
       {
            point = dynamic_cast<SpdTofECPoint*>(fPointsArray[1]->At(i));
            if (!AcceptPoint(point)) continue;
            
            napoints[1]++;
         
            // create new hit
            hit = AddHit();
            
            hit->SetModId(point->GetDetectorID());
            hit->SetWeight(1);
            
            // set hit data 
            hit->SetHitTime(point->GetTimeIn());
            hit->SetHitTimeError(0);
            hit->SetHitPos(point->GetPosIn());
            hit->SetTrackMom(point->GetMomIn());
            hit->SetTrackLen(point->GetLength());
            hit->SetNodePath(point->GetNodePath());

            // add hit deposit
            hit->SetResp(point->GetEnergyLoss());
            
            nhits[1]++;

            // FIXME add mc-truth FIXME
            AddMCTruth(point->GetTrackID(),hit);
       }
       
       fNFailedHits += nhfailed[1];  
   }
   
   fNTotalHits += fHits->GetEntriesFast();
   
   /*ATTENTION +++ set TOF_HIT_ID as TOF_PARTICLE_ID (MC-PARTICLE-INFO) +++ ATTENTION */ 
   Int_t part_id;
   SpdMCParticle* part;

   for (Int_t i(0); i<fHits->GetEntriesFast(); i++) 
   {
        hit = (SpdTofMCHit*)fHits->At(i);
        part_id = hit->GetParticleId();
        if (part_id < 0) continue;
        part = (SpdMCParticle*)fParticles->At(part_id);
        if (!part) continue;
        
        if (part->GetTofParticleId() < 0) part->SetTofParticleId(i);
        else {
            SpdTofMCHit* hhit = (SpdTofMCHit*)fHits->At(part->GetTofParticleId());
            if (hit->GetHitTime() < hhit->GetHitTime()) part->SetTofParticleId(i);
        }
    
        /* !!! particle type (PDG) !!! */
        hit->SetPdg(part->GetPdgCode()); 
   }
   /*ATTENTION +++ ++++++++++++++++++++++++++++++++++++++++++++++++ +++ ATTENTION */
   
   if (fVerboseLevel > 1) 
   {
        cout << "\n-I- <SpdTofMCHitProducer::Exec> Print ... " << endl; 
       
        if (fVerboseLevel > 2 && fHits->GetEntriesFast() > 0) 
        {
            cout << "Print hits [first(barrel,endcap)]: " << endl; 
            if (nhits[0] > 0) {
                hit = dynamic_cast<SpdTofMCHit*>(fHits->At(0));
                if (hit) hit->Print();
            }
            if (nhits[1] > 0) {
                hit = dynamic_cast<SpdTofMCHit*>(fHits->At(nhits[0]));
                if (hit) hit->Print();
            }
        }
        
        //for (Int_t i(0); i<fHits->GetEntriesFast(); i++)
        //{
        //    hit = dynamic_cast<SpdTofMCHit*>(fHits->At(i));
        //    if (hit) hit->Print();
        //}
        
        Int_t np1(0), np2(0), nn(0);
        if (fPointsArray[0]) {
            np1 = fPointsArray[0]->GetEntriesFast();
            nn += np1;
        }
        if (fPointsArray[1]) {
            np2 = fPointsArray[1]->GetEntriesFast();
            nn += np2;
        }
        
        cout << "Points/hits (total):             " << nn << "/" << fHits->GetEntriesFast()<< endl; 
        cout << "Points(barrel|total,rejected):   " << np1 << "/" << np1-napoints[0] << endl;
        cout << "Points(endcaps|total,rejected):  " << np2 << "/" << np2-napoints[1] << endl;
        cout << "Hits(barrel|total,failed):       " << nhits[0] << "/" << nhfailed[0] << endl;
        cout << "Hits(endcaps|total,failed):      " << nhits[1] << "/" << nhfailed[1] << endl;
   }
}

//________________________________________________________________________________________  
template <class T> Bool_t SpdTofMCHitProducer::AcceptPoint(const T* point) const
{
   if (!point) return false;
   return true;   
}

//________________________________________________________________________________________
void SpdTofMCHitProducer::AddMCTruth(Int_t itrack, SpdTofMCHit* hit)
{
   Int_t ipart = SpdMCEventHelper::Instance()->GetParticleId(itrack); 
   if (ipart < 0) return; 
   hit->AddMCTruth(ipart); 
}

//________________________________________________________________________________________  
SpdTofMCHit* SpdTofMCHitProducer::AddHit()
{
   if (!fHits) return 0;
   Int_t size = fHits->GetEntriesFast();
   SpdTofMCHit* hit = new ((*fHits)[size]) SpdTofMCHit();
   hit->SetId(size);
   return hit;
}

//________________________________________________________________________________________  
void SpdTofMCHitProducer::Finish()
{
   if (fVerboseLevel > -1) {
       cout << "\n-I- <SpdTofMCHitProducer::Finish> [Hits] "
            << "total: " << fNTotalHits << ", failed: " << fNFailedHits;// << endl;
   }
}

//________________________________________________________________________________________
void SpdTofMCHitProducer::AddParameters()
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance();
   SpdSetParSet* params = run->GetParSetAssembly();
   FairParGenericSet* pars;
   pars = run->GetParameters(fParsName[0]);
   params->AddActiveGeoPars(pars);
   pars = run->GetParameters(fParsName[1]);
   params->AddActiveGeoPars(pars);
}


