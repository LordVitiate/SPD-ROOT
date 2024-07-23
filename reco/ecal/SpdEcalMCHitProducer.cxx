// $Id$
// Author: artur   2021/03/02


//_____________________________________________________________________________
//
// SpdEcalMCHitProducer
//_____________________________________________________________________________

#include "SpdEcalMCHitProducer.h"
#include "SpdEcalCommon.h"

#include "SpdSetParSet.h"

#include "SpdRunAna.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include "SpdSetParSet.h"
#include "SpdParSet.h"

#include "SpdEcalBGeoMapper.h"
#include "SpdEcalECGeoMapper.h"

#include "SpdMCEvent.h"
#include "SpdMCParticle.h"
#include "SpdEcalMCHit.h"

#include "SpdEcalBPoint.h"
#include "SpdEcalECPoint.h"

#include "SpdMCEventHelper.h"
#include "SpdParticlesAndProcesses.h"

#include <TStatistic.h>
#include <TGeoManager.h>

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalMCHitProducer)

//_____________________________________________________________________________
SpdEcalMCHitProducer::SpdEcalMCHitProducer():
fEvent(0),fParticles(0),fIdTable(0),
fHits(0),
fSaveHits(true),
fNFailedHits(0),fNTotalHits(0),
fVerboseLevel(1)
{
   fGeoMapper[0]  = 0;              fGeoMapper[1]  = 0; 
   fParsName[0]   = "EcalTBParSet"; fParsName[1]   = "EcalTECParSet";
   fParameters[0] = 0;              fParameters[1] = 0;
   fSaveDetIdOption[0] = 0;         fSaveDetIdOption[1] = 0; 
   fSaveEmptyHits[0] = 0;           fSaveEmptyHits[1] = 0;
   fDataType[0] = "";               fDataType[1] = "";
   fPointsArray[0] = 0;             fPointsArray[1] = 0;
   fAbsorberVolName[0] = "";        fAbsorberVolName[1] = ""; 
   fScintVolName[0] = "";           fScintVolName[1] = "";
}

//_____________________________________________________________________________
SpdEcalMCHitProducer::~SpdEcalMCHitProducer() 
{
   for (Int_t i(0); i<2; i++) { 
        if (fGeoMapper[i]) delete fGeoMapper[i]; 
   }
   
   if (fHits) { fHits->Delete(); delete fHits; }
   
   //SpdMCEventHelper::Instance()->SetTsHits(0);
}

//_____________________________________________________________________________
void SpdEcalMCHitProducer::LoadGeometry()
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance(); 
      
   if (!gGeoManager) run->GetParameters("FairGeoParSet");

   if (!gGeoManager) {
       cout << "-F- <SpdEcalMCHitProducer::LoadGeometry> Geometry is not defined " << endl;
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
            cout << "-F- <SpdEcalMCHitProducer::LoadGeometry> No geometry parameters " 
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
        
        if (i == 0) {
            SpdEcalBGeoMapper* m = dynamic_cast<SpdEcalBGeoMapper*>(fGeoMapper[i]);
            if (m) {
                fAbsorberVolName[i] = m->GetAbsorberVolName();
                fScintVolName[i]    = m->GetScintVolName();
            }
        }
        else if (i == 1) {    
            SpdEcalECGeoMapper* m = dynamic_cast<SpdEcalECGeoMapper*>(fGeoMapper[i]);
            if (m) {
                fAbsorberVolName[i] = m->GetAbsorberVolName();
                fScintVolName[i]    = m->GetScintVolName();
            }
        }
        
        fParameters[i]->GetParameter("Detector/SaveDetIdOption",fSaveDetIdOption[i]);
        fParameters[i]->GetParameter("Detector/SaveEmptyHits",fSaveEmptyHits[i]);
        fParameters[i]->GetParameter("OutData_1",fDataType[i]);
   }
}

//_____________________________________________________________________________
InitStatus SpdEcalMCHitProducer::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdEcalMCHitProducer::Init> RootManager not instantiated " << endl;
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
           cout << "-W- <SpdEcalMCHitProducer::Init> No MC-POINTS: " << fDataType[0] << endl;
       }
   }
   else init_input = true;
   
   // get endcaps Points array
   fPointsArray[1] = (TClonesArray*)ioman->GetObject(fDataType[1]);
    
   if (!fPointsArray[1]) {
       if (fVerboseLevel > -2) {
           cout << "-W- <SpdEcalMCHitProducer::Init> No MC-POINTS: " << fDataType[1] << endl;
       }
   }
   else init_input = true;
   
   if (!init_input) {
       cout << "-F- <SpdEcalMCHitProducer::Init> No input data " << endl;
       return kFATAL; 
   }
 
   //============== CREATE AND REGISTER OUTPUT DATA OBJECTS ================/
   
   fHits = new TClonesArray("SpdEcalMCHit");
   ioman->Register(SpdDataBranchName(kMCEcalHits),"SpdEcalMCHit", fHits, fSaveHits);  
   
   //============== PASS DATA TO EVENTHELPER ================================/   
   
   helper->SetEcalHits(fHits);
 
   if (fVerboseLevel > -1) cout << "-I- <SpdEcalMCHitProducer::Init> Intialization successfull " << endl;
   
   return kSUCCESS;
}

//________________________________________________________________________________________
void SpdEcalMCHitProducer::Exec(Option_t* opt) 
{
   assert(fEvent);
   assert(fParticles);
   assert(fIdTable);
   
   if(!fPointsArray[0]) assert(fPointsArray[1]);
   else assert(fPointsArray[0]); 
   
   if (fVerboseLevel > 0) {
       cout << "\n-I- <SpdEcalMCHitProducer::Exec> Process event ... " 
            << fEvent->GetEventId(); 
       if (fVerboseLevel > 1) cout << "\n";         
   }
   
   fHits->Delete();
          
   // >>>>> Fill hits <<<<< 
   Int_t npoints;
   Int_t napoints[2] = {0,0}, nhits[2] = {0,0}, nhfailed[2] = {0,0};

   SpdEcalMCHit* hit;

   TString pathToCell;   // cell geopath
   TVector3 cell_pos;    // hit cell position in global CS
   Double_t cell_edep;   // cell energy deposit
   
   std::map<TString,SpdEcalMCHit*> HitsMap;
   std::map<TString,SpdEcalMCHit*>::const_iterator it;
   
   // >>>>>>>>>>>  Barrel >>>>>>>>>>>  
   
   if (fPointsArray[0]) 
   {
       SpdEcalBPoint* point;
       npoints = fPointsArray[0]->GetEntriesFast();
       
       for (Int_t i(0); i<npoints; i++) 
       {
            point = dynamic_cast<SpdEcalBPoint*>(fPointsArray[0]->At(i));
            if (!AcceptPointB(point)) continue;
        
            napoints[0]++;
    
            pathToCell = point->GetNodePathToCell(fAbsorberVolName[0],fScintVolName[0]);
            
            it = HitsMap.find(pathToCell);    
            
            if (it == HitsMap.end()) 
            {
                // search for cell
                if (!SpdEcalCommon::GetCellPosition(pathToCell,cell_pos)) {
                    nhfailed[0]++;
                    continue;
                }
                
                // add hit
                hit = AddHit();
                
                hit->SetModId(point->GetDetectorID());
                hit->SetWeight(1);
                
                // set cell position
                hit->SetPosition(cell_pos); 
                
                // set detector id
                hit->SetBasketID(point->GetBasketID());
                hit->SetModuleZID(point->GetModuleZID());
                hit->SetModulePhiID(point->GetModulePhiID());
                hit->SetCellID(point->GetCellID());
                    
                // add hit to the table
                HitsMap.insert(std::pair<TString,SpdEcalMCHit*>(pathToCell,hit));
        
                nhits[0]++;
            }
            else hit = it->second;
            
            // add cell energy deposit
            cell_edep = point->GetEnergyLoss();
            if (point->GetMaterialID() == fScintVolName[0]) hit->AddEnergy(cell_edep);
            hit->AddMCEnergy(cell_edep);
                        
            // add mc-truth
            AddMCTruth(point->GetTrackID(),hit,cell_edep);
       }
       
       fNFailedHits += nhfailed[0]; 
       
       HitsMap.clear();
   }
   
   // >>>>>>>>>>>  Endcaps >>>>>>>>>>>  
   
   if (fPointsArray[1]) 
   {
       SpdEcalECPoint* point;
       npoints = fPointsArray[1]->GetEntriesFast();
       
       for (Int_t i(0); i<npoints; i++) 
       {
            point = dynamic_cast<SpdEcalECPoint*>(fPointsArray[1]->At(i));
            if (!AcceptPointE(point)) continue;
            
            napoints[1]++;
            
            pathToCell = point->GetNodePathToCell(fAbsorberVolName[1],fScintVolName[1]);
            
            it = HitsMap.find(pathToCell);    
            
            if (it == HitsMap.end()) 
            {
                // search for cell
                if (!SpdEcalCommon::GetCellPosition(pathToCell,cell_pos)) {
                    nhfailed[0]++;
                    continue;
                }
                
                // add hit
                hit = AddHit();
                hit->SetModId(point->GetDetectorID());
                hit->SetWeight(1);
                
                // set cell position
                hit->SetPosition(cell_pos); 
                
                // set detector id
                hit->SetEndcapID(point->GetEndcapID());
                hit->SetModuleID(point->GetModuleID());
                hit->SetCellID(point->GetCellID());
                    
                // add hit to the table
                HitsMap.insert(std::pair<TString,SpdEcalMCHit*>(pathToCell,hit));
        
                nhits[1]++;
            }
            else hit = it->second;
            
            // add cell energy deposit
            cell_edep = point->GetEnergyLoss();
            if (point->GetMaterialID() == fScintVolName[1]) hit->AddEnergy(cell_edep);
            hit->AddMCEnergy(cell_edep);
                        
            // add mc-truth
            AddMCTruth(point->GetTrackID(),hit,cell_edep);
       }
       
       fNFailedHits += nhfailed[1]; 
       
       HitsMap.clear();
   }
   
   Int_t nnh = fHits->GetEntriesFast();
   
   // calibrate cell energy deposit
   for (Int_t i(0); i<nnh; i++) {
        hit = (SpdEcalMCHit*)fHits->At(i);
        cell_edep = CalibrateEnergy(hit->GetEnergy());
        hit->SetEnergy(cell_edep);
   }
   
   fNTotalHits += nnh;
   
   if (fVerboseLevel > 1) 
   {
        cout << "\n-I- <SpdEcalMCHitProducer::Exec> Print ... " << endl; 
       
        if (fVerboseLevel > 2 && fHits->GetEntriesFast() > 0) 
        {
            cout << "Print hits [first(barrel,endcap)]: " << endl; 
            if (nhits[0] > 0) {
                hit = dynamic_cast<SpdEcalMCHit*>(fHits->At(0));
                if (hit) hit->Print();
            }
            if (nhits[1] > 0) {
                hit = dynamic_cast<SpdEcalMCHit*>(fHits->At(nhits[0]));
                if (hit) hit->Print();
            }
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
        
        cout << "Points/hits (total):             " << nn << "/" << fHits->GetEntriesFast()<< endl; 
        cout << "Points(barrel|total,rejected):   " << np1 << "/" << np1-napoints[0] << endl;
        cout << "Points(endcaps|total,rejected):  " << np2 << "/" << np2-napoints[1] << endl;
        cout << "Hits(barrel|total,failed):       " << nhits[0] << "/" << nhfailed[0] << endl;
        cout << "Hits(endcaps|total,failed):      " << nhits[1] << "/" << nhfailed[1] << endl;     
   }
}

//________________________________________________________________________________________
Double_t SpdEcalMCHitProducer::CalibrateEnergy(Double_t e) const 
{
   if (e < 0.33) return e/(0.244638 + 0.0740879*e);
   else return e/0.269;
}

//________________________________________________________________________________________  
Bool_t SpdEcalMCHitProducer::AcceptPointB(const SpdEcalBPoint* point) const
{
   if (!point) return false;
   return true;   
}

//________________________________________________________________________________________  
Bool_t SpdEcalMCHitProducer::AcceptPointE(const SpdEcalECPoint* point) const
{
   if (!point) return false;
   return true;   
}

//________________________________________________________________________________________
void SpdEcalMCHitProducer::AddMCTruth(Int_t itrack, SpdEcalMCHit* hit, Double_t edep)
{
   Int_t ipart = SpdMCEventHelper::Instance()->GetParticleId(itrack); 
   hit->AddPoint();
   if (ipart < 0) return;
   hit->AddMCTruth(ipart,edep); 
   //SpdMCParticle* part = (SpdMCParticle*)fParticles->At(ipart);
   //part->SetChar('+');
}

//________________________________________________________________________________________  
SpdEcalMCHit* SpdEcalMCHitProducer::AddHit()
{
   if (!fHits) return 0;
   Int_t size = fHits->GetEntriesFast();
   SpdEcalMCHit* hit = new ((*fHits)[size]) SpdEcalMCHit();
   hit->SetId(size);
   return hit;
}

//________________________________________________________________________________________  
void SpdEcalMCHitProducer::Finish()
{
   if (fVerboseLevel > -1) {
       cout << "\n-I- <SpdEcalMCHitProducer::Finish> [Hits] "
            << "total: " << fNTotalHits << ", failed: " << fNFailedHits;// << endl;
   }
}

//________________________________________________________________________________________
void SpdEcalMCHitProducer::AddParameters()
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance();
   SpdSetParSet* params = run->GetParSetAssembly();
   FairParGenericSet* pars;
   pars = run->GetParameters(fParsName[0]);
   params->AddActiveGeoPars(pars);
   pars = run->GetParameters(fParsName[1]);
   params->AddActiveGeoPars(pars);
}
