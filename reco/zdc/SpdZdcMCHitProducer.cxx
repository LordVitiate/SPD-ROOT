// $Id$
// Author: artur   2021/09/10

//_____________________________________________________________________________
//
// SpdZdcMCHitProducer
//_____________________________________________________________________________

#include "SpdZdcMCHitProducer.h"

#include "SpdSetParSet.h"

#include "SpdRunAna.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include "SpdSetParSet.h"
#include "SpdParSet.h"

#include "SpdZdcGeoMapper.h"

#include "SpdMCEvent.h"
#include "SpdMCParticle.h"
#include "SpdZdcMCHit.h"

#include "SpdZdcPoint.h"

#include "SpdMCEventHelper.h"
#include "SpdParticlesAndProcesses.h"
#include "SpdGeopathParser.h"
 
#include <TStatistic.h>
#include <TGeoManager.h>

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdZdcMCHitProducer)

//_____________________________________________________________________________
SpdZdcMCHitProducer::SpdZdcMCHitProducer():
fEvent(0),fParticles(0),fIdTable(0),
fHits(0),
fSaveHits(true),
fNFailedHits(0),fNTotalHits(0),
fVerboseLevel(1)
{
   fGeoMapper  = 0;           
   fParsName   = "ZdcParSet";
   fParameters = 0;           
   fSaveDetIdOption = 0;      
   fSaveEmptyHits = 0;        
   fDataType = "";            
   fPointsArray = 0;          
}

//_____________________________________________________________________________
SpdZdcMCHitProducer::~SpdZdcMCHitProducer() 
{
   if (fGeoMapper) delete fGeoMapper; 
   if (fHits) { fHits->Delete(); delete fHits; }
   
   //SpdMCEventHelper::Instance()->SetZdcHits(0);
}

//_____________________________________________________________________________
void SpdZdcMCHitProducer::LoadGeometry()
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance(); 
      
   if (!gGeoManager) run->GetParameters("FairGeoParSet");

   if (!gGeoManager) {
       cout << "-F- <SpdZdcMCHitProducer::LoadGeometry> Geometry is not defined " << endl;
       exit(1);
   }

   fParameters = dynamic_cast<SpdParSet*>(run->GetParameters(fParsName));
   
   if (!fParameters) {
       SpdSetParSet* pars = (SpdSetParSet*)run->GetParameters("SpdSetParSet");
       if (pars) fParameters = pars->GetActiveGeoPars(fParsName);
   }
   
   if (!fParameters) {
       cout << "-F- <SpdZdcMCHitProducer::LoadGeometry> No geometry parameters " 
            << "(\"" << fParsName << "\")" << endl;
       exit(1); 
   }
   
   //fParameters->print(1);
    
   TString mapper;
    
   fParameters->GetParameter("Mapper",mapper); 
   fGeoMapper = SpdGeoFactory::Instance()->Mapper(mapper);
    
   if (fGeoMapper) {
       fGeoMapper->LoadParametersFrom(fParameters);
       fGeoMapper->InitGeometry();
       fGeoMapper->LockGeometry();
       //fGeoMapper->Print("");
   }
   
   fParameters->GetParameter("Detector/SaveDetIdOption",fSaveDetIdOption);
   fParameters->GetParameter("Detector/SaveEmptyHits",fSaveEmptyHits);
   fParameters->GetParameter("OutData_1",fDataType);
}

//_____________________________________________________________________________
InitStatus SpdZdcMCHitProducer::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdZdcMCHitProducer::Init> RootManager not instantiated " << endl;
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
   
   // get barrel Points array
   fPointsArray = (TClonesArray*)ioman->GetObject(fDataType);
    
   if (!fPointsArray) {
       cout << "-F- <SpdZdcMCHitProducer::Init> No input data " << endl;
       return kFATAL; 
   }
 
   //============== CREATE AND REGISTER OUTPUT DATA OBJECTS ================/
   
   fHits = new TClonesArray("SpdZdcMCHit");
   ioman->Register(SpdDataBranchName(kMCZdcHits),"SpdZdcMCHit", fHits, fSaveHits);  
   
   //============== PASS DATA TO EVENTHELPER ================================/   
   
   helper->SetZdcHits(fHits);
 
   if (fVerboseLevel > -1) cout << "-I- <SpdZdcMCHitProducer::Init> Intialization successfull " << endl;
   
   return kSUCCESS;
}

//________________________________________________________________________________________
void SpdZdcMCHitProducer::Exec(Option_t* opt) 
{
   assert(fEvent);
   assert(fParticles);
   assert(fIdTable);
   assert(fPointsArray); 
   
   if (fVerboseLevel > 0) {
       cout << "\n-I- <SpdZdcMCHitProducer::Exec> Process event ... " 
            << fEvent->GetEventId(); 
       if (fVerboseLevel > 1) cout << "\n";         
   }
   
   fHits->Delete();
                
   // >>>>> Fill hits <<<<< 
         
   Int_t npoints = fPointsArray->GetEntriesFast();
   Int_t napoints(0), nhits(0), nhfailed(0);
   
   SpdZdcPoint* point;
   SpdZdcMCHit* hit;
 
   for (Int_t i(0); i<npoints; i++) 
   {
        point = dynamic_cast<SpdZdcPoint*>(fPointsArray->At(i));
        if (!AcceptPoint(point)) continue;
        
        napoints++;
        
        //create new hit
        hit = AddHit();
        
        hit->SetModId(point->GetDetectorID());
        hit->SetDetId(point->GetModuleId());
        hit->SetWeight(1);
         
        nhits++;
        
        // set hit data 
        hit->SetHitTime(point->GetTimeIn());
        hit->SetHitTimeError(0);
        hit->SetHitPos(point->GetPosIn());
        hit->SetTrackMom(point->GetMomIn());
        hit->SetTrackLen(point->GetLength());
        hit->SetTrackSegLen(point->GetSegmentLength());
        
        // add hit deposit
        hit->SetResp(point->GetEnergyLoss());

        // FIXME add mc-truth FIXME
        AddMCTruth(point->GetTrackID(),hit);
   }
       
   fNFailedHits += nhfailed; 
   fNTotalHits += fHits->GetEntriesFast();
   
   Int_t part_id;
   SpdMCParticle* part;

   for (Int_t i(0); i < fHits->GetEntriesFast(); i++)
   {
        hit = (SpdZdcMCHit*)fHits->At(i);
        part_id = hit->GetParticleId();
        if (part_id < 0) continue;
        part = (SpdMCParticle*)fParticles->At(part_id);
        if (!part) continue;
        
        if (part->GetZdcParticleId() < 0) part->SetZdcParticleId(i);
        else {
            SpdZdcMCHit* hhit = (SpdZdcMCHit*)fHits->At(part->GetZdcParticleId());
            if (hit->GetHitTime() < hhit->GetHitTime()) part->SetZdcParticleId(i);
        }

   }


   if (fVerboseLevel > 1) 
   {
        cout << "\n-I- <SpdZdcMCHitProducer::Exec> Print ... " << endl; 
       
        if (fVerboseLevel > 2 && fHits->GetEntriesFast() > 0) 
        {
            cout << "Print hits [first]: " << endl; 
            if (nhits > 0) {
                hit = dynamic_cast<SpdZdcMCHit*>(fHits->At(0));
                if (hit) hit->Print();
            }
        }
        
        //for (Int_t i(0); i<fHits->GetEntriesFast(); i++)
        //{
        //    hit = dynamic_cast<SpdZdcMCHit*>(fHits->At(i));
        //    if (hit) hit->Print();
        //}
        
        cout << "Points(total,rejected):   " << npoints << "/" << npoints-napoints << endl;
        cout << "Hits(total,failed):       " << nhits << "/" << nhfailed << endl;
   }
}

//________________________________________________________________________________________  
template <class T> Bool_t SpdZdcMCHitProducer::AcceptPoint(const T* point) const
{
   if (!point) return false;
   return true;   
}

//________________________________________________________________________________________
void SpdZdcMCHitProducer::AddMCTruth(Int_t itrack, SpdZdcMCHit* hit)
{
   Int_t ipart = SpdMCEventHelper::Instance()->GetParticleId(itrack); 
   if (ipart < 0) return; 
   hit->AddMCTruth(ipart); 
}

//________________________________________________________________________________________  
SpdZdcMCHit* SpdZdcMCHitProducer::AddHit()
{
   if (!fHits) return 0;
   Int_t size = fHits->GetEntriesFast();
   SpdZdcMCHit* hit = new ((*fHits)[size]) SpdZdcMCHit();
   hit->SetId(size);
   return hit;
}

//________________________________________________________________________________________  
void SpdZdcMCHitProducer::Finish()
{
   if (fVerboseLevel > -1) {
       cout << "\n-I- <SpdZdcMCHitProducer::Finish> [Hits] "
            << "total: " << fNTotalHits << ", failed: " << fNFailedHits;// << endl;
   }
}

//________________________________________________________________________________________
void SpdZdcMCHitProducer::AddParameters()
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance();
   SpdSetParSet* params = run->GetParSetAssembly();
   FairParGenericSet* pars = run->GetParameters(fParsName);
   params->AddActiveGeoPars(pars);
}


