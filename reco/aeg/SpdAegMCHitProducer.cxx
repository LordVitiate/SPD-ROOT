// $Id$
// Author: artur   2021/29/11

//_____________________________________________________________________________
//
// SpdAegMCHitProducer
//_____________________________________________________________________________

#include "SpdAegMCHitProducer.h"

#include "SpdSetParSet.h"

#include "SpdRunAna.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include "SpdSetParSet.h"
#include "SpdParSet.h"

#include "SpdAegGeoMapper.h"

#include "SpdMCEvent.h"
#include "SpdMCParticle.h"
#include "SpdAegMCHit.h"

#include "SpdAegPoint.h"

#include "SpdMCEventHelper.h"
#include "SpdParticlesAndProcesses.h"
#include "SpdGeopathParser.h"
 
#include <TStatistic.h>
#include <TGeoManager.h>

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdAegMCHitProducer)

//_____________________________________________________________________________
SpdAegMCHitProducer::SpdAegMCHitProducer():
fEvent(0),fParticles(0),fIdTable(0),
fHits(0),
fSaveHits(true),
fNFailedHits(0),fNTotalHits(0),
fVerboseLevel(1)
{
   fGeoMapper  = 0;           
   fParsName   = "AegParSet";
   fParameters = 0;           
   fSaveDetIdOption = 0;      
   fSaveEmptyHits = 0;        
   fDataType = "";            
   fPointsArray = 0;          
}

//_____________________________________________________________________________
SpdAegMCHitProducer::~SpdAegMCHitProducer() 
{
   if (fGeoMapper) delete fGeoMapper; 
   if (fHits) { fHits->Delete(); delete fHits; }
   
   //SpdMCEventHelper::Instance()->SetAegHits(0);
}

//_____________________________________________________________________________
void SpdAegMCHitProducer::LoadGeometry()
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance(); 
      
   if (!gGeoManager) run->GetParameters("FairGeoParSet");

   if (!gGeoManager) {
       cout << "-F- <SpdAegMCHitProducer::LoadGeometry> Geometry is not defined " << endl;
       exit(1);
   }

   fParameters = dynamic_cast<SpdParSet*>(run->GetParameters(fParsName));
   
   if (!fParameters) {
       SpdSetParSet* pars = (SpdSetParSet*)run->GetParameters("SpdSetParSet");
       if (pars) fParameters = pars->GetActiveGeoPars(fParsName);
   }
   
   if (!fParameters) {
       cout << "-F- <SpdAegMCHitProducer::LoadGeometry> No geometry parameters " 
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
InitStatus SpdAegMCHitProducer::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdAegMCHitProducer::Init> RootManager not instantiated " << endl;
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
       cout << "-F- <SpdAegMCHitProducer::Init> No input data " << endl;
       return kFATAL; 
   }
 
   //============== CREATE AND REGISTER OUTPUT DATA OBJECTS ================/
   
   fHits = new TClonesArray("SpdAegMCHit");
   ioman->Register(SpdDataBranchName(kMCAegHits),"SpdAegMCHit", fHits, fSaveHits);  
   
   //============== PASS DATA TO EVENTHELPER ================================/   
   
   helper->SetAegHits(fHits);
 
   if (fVerboseLevel > -1) cout << "-I- <SpdAegMCHitProducer::Init> Intialization successfull " << endl;
   
   return kSUCCESS;
}

//________________________________________________________________________________________
void SpdAegMCHitProducer::Exec(Option_t* opt) 
{
   assert(fEvent);
   assert(fParticles);
   assert(fIdTable);
   assert(fPointsArray); 
   
   if (fVerboseLevel > 0) {
       cout << "\n-I- <SpdAegMCHitProducer::Exec> Process event ... " 
            << fEvent->GetEventId(); 
       if (fVerboseLevel > 1) cout << "\n";         
   }
   
   fHits->Delete();
                
   // >>>>> Fill hits <<<<< 
         
   Int_t npoints = fPointsArray->GetEntriesFast();
   Int_t napoints(0), nhits(0), nhfailed(0);
   
   SpdAegPoint* point;
   SpdAegMCHit* hit;
 
   for (Int_t i(0); i<npoints; i++) 
   {
        point = dynamic_cast<SpdAegPoint*>(fPointsArray->At(i));
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
   for (Int_t i(0); i<fHits->GetEntriesFast(); i++)
   {
      hit = dynamic_cast<SpdAegMCHit *>(fHits->At(i));
    //  if (hit) hit->Print();
      part_id = hit->GetParticleId();

      if (part_id < 0) continue;
      part = (SpdMCParticle*)fParticles->At(part_id);
      if (!part) continue;

       part->SetAegParticleId(i);
        
   }
        

   if (fVerboseLevel > 1) 
   {
        cout << "\n-I- <SpdAegMCHitProducer::Exec> Print ... " << endl; 
       
        if (fVerboseLevel > 2 && fHits->GetEntriesFast() > 0) 
        {
            cout << "Print hits [first]: " << endl; 
            if (nhits > 0) {
                hit = dynamic_cast<SpdAegMCHit*>(fHits->At(0));
                if (hit) hit->Print();
            }
        }
        
        //for (Int_t i(0); i<fHits->GetEntriesFast(); i++)
        //{
        //    hit = dynamic_cast<SpdAegMCHit*>(fHits->At(i));
        //    if (hit) hit->Print();
        //}
        
        cout << "Points(total,rejected):   " << npoints << "/" << npoints-napoints << endl;
        cout << "Hits(total,failed):       " << nhits << "/" << nhfailed << endl;
   }
}

//________________________________________________________________________________________  
template <class T> Bool_t SpdAegMCHitProducer::AcceptPoint(const T* point) const
{
   if (!point) return false;
   return true;   
}

//________________________________________________________________________________________
void SpdAegMCHitProducer::AddMCTruth(Int_t itrack, SpdAegMCHit* hit)
{
   Int_t ipart = SpdMCEventHelper::Instance()->GetParticleId(itrack); 
   if (ipart < 0) return; 
   hit->AddMCTruth(ipart); 
}

//________________________________________________________________________________________  
SpdAegMCHit* SpdAegMCHitProducer::AddHit()
{
   if (!fHits) return 0;
   Int_t size = fHits->GetEntriesFast();
   SpdAegMCHit* hit = new ((*fHits)[size]) SpdAegMCHit();
   hit->SetId(size);
   return hit;
}

//________________________________________________________________________________________  
void SpdAegMCHitProducer::Finish()
{
   if (fVerboseLevel > -1) {
       cout << "\n-I- <SpdAegMCHitProducer::Finish> [Hits] "
            << "total: " << fNTotalHits << ", failed: " << fNFailedHits;// << endl;
   }
}

//________________________________________________________________________________________
void SpdAegMCHitProducer::AddParameters()
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance();
   SpdSetParSet* params = run->GetParSetAssembly();
   FairParGenericSet* pars = run->GetParameters(fParsName);
   params->AddActiveGeoPars(pars);
}


