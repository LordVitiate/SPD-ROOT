// $Id$
// Author: artur   2021/12/26

//_____________________________________________________________________________
//
// SpdMvdMCHitProducer
//_____________________________________________________________________________

#include "SpdMvdMCHitProducer.h"
#include "SpdSetParSet.h"

#include "SpdRunAna.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include "SpdSetParSet.h"
#include "SpdParSet.h"

#include "SpdMvdGeoMapper.h"

#include "SpdMCEvent.h"
#include "SpdMCParticle.h"
#include "SpdMCSiliconHit.h"
#include "SpdMvdPoint.h"

#include "SpdMCEventHelper.h"
#include "SpdParticlesAndProcesses.h"

#include <TGeoManager.h>
#include <TRandom3.h>

#include <iostream>

using std::cout;
using std::endl;

# define mkm_ 1.e-4 // mkm -> cm 

ClassImp(SpdMvdMCHitProducer)

//_____________________________________________________________________________
SpdMvdMCHitProducer::SpdMvdMCHitProducer():
fGeoMapper(0),
fParsName("MvdParSet"),fParameters(0),
fSaveDetIdOption(0),fSaveEmptyHits(0),fDataType(""),
fEvent(0),fParticles(0),
fSaveHits(true),
fPointsArray(0),fHits(0),fNFailedHits(0),fNTotalHits(0),
fVerboseLevel(1)
{
   SpdParticlesAndProcesses::AddParticlesToDatabasePDG();
   
   // set hit default resolution
   SetHitResolution(1, 800*mkm_, 90*mkm_); 
   SetHitResolution(2, 800*mkm_, 90*mkm_); 
   SetHitResolution(3, 800*mkm_, 90*mkm_); 
   SetHitResolution(4, 800*mkm_, 90*mkm_);
   SetHitResolution(5, 800*mkm_, 90*mkm_); 
   SetHitResolution(6, 800*mkm_, 90*mkm_);
   SetHitResolution(7, 800*mkm_, 90*mkm_); 
   SetHitResolution(8, 800*mkm_, 90*mkm_); 
   SetHitResolution(9, 800*mkm_, 90*mkm_); 
   SetHitResolution(10, 800*mkm_, 90*mkm_); 
}

//_____________________________________________________________________________
SpdMvdMCHitProducer::~SpdMvdMCHitProducer() 
{
   if (fGeoMapper) delete fGeoMapper;
   if (fHits) { fHits->Delete(); delete fHits; }

   //SpdMCEventHelper::Instance()->SetMvdHits(0);
}

//_____________________________________________________________________________
void SpdMvdMCHitProducer::LoadGeometry()
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance(); 
      
   if (!gGeoManager) run->GetParameters("FairGeoParSet");

   if (!gGeoManager) {
       cout << "-F- <SpdMvdMCHitProducer::LoadGeometry> Geometry is not defined " << endl;
       exit(1);
   }
   
   fParameters = dynamic_cast<SpdParSet*>(run->GetParameters(fParsName));
   
   if (!fParameters) {
       SpdSetParSet* pars = (SpdSetParSet*)run->GetParameters("SpdSetParSet");
       if (pars) fParameters = pars->GetActiveGeoPars(fParsName);
   }
   
   if (!fParameters) {
       cout << "-F- <SpdMvdMCHitProducer::LoadGeometry> No geometry parameters " 
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
void SpdMvdMCHitProducer::SetHitResolution(Int_t layerID, Double_t resU, Double_t resV)
{
    resU = TMath::Abs(resU);
    resV = TMath::Abs(resV);
    fResolution[layerID] = TVector3(resU,resU,0.);
}

//_____________________________________________________________________________
Double_t SpdMvdMCHitProducer::GetHitResolutionU(Int_t layerID)
{
    auto x = fResolution.find(layerID);
    return (x != fResolution.end()) ? (x->second)[0] : 1000*mkm_;
}

//_____________________________________________________________________________
Double_t SpdMvdMCHitProducer::GetHitResolutionV(Int_t layerID)
{
    auto x = fResolution.find(layerID);
    return (x != fResolution.end()) ? (x->second)[1] : 1000*mkm_;
}

//_____________________________________________________________________________
InitStatus SpdMvdMCHitProducer::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdMvdMCHitProducer::Init> RootManager not instantiated " << endl;
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
   
   // searching for ID-table 
   
   if (!helper->GetIdTable()) return kFATAL;
   
   // get Points array
   fPointsArray = (TClonesArray*)ioman->GetObject(fDataType);
    
   if (!fPointsArray) {
       cout << "-F- <SpdMvdMCHitProducer::Init> No MC-POINTS: " << fDataType << endl;
       return kFATAL;
   }
   
   //============== CREATE AND REGISTER OUTPUT DATA OBJECTS ================/
   
   fHits = new TClonesArray("SpdMCSiliconHit");
   ioman->Register(SpdDataBranchName(kMCItsHits),"SpdMCSiliconHit", fHits, fSaveHits);
   
   //============== PASS DATA TO EVENTHELPER ================================/   
   
   helper->SetItsHits(fHits);
   
   if (!helper->GetEvent()) helper->SetEvent(fEvent);
   if (!helper->GetParticles()) helper->SetParticles(fParticles);
   
   if (fVerboseLevel > -1) cout << "-I- <SpdMvdMCHitProducer::Init> Intialization successfull " << endl;
   
   return kSUCCESS;
}

//________________________________________________________________________________________
void SpdMvdMCHitProducer::Exec(Option_t* opt) 
{
   assert(fEvent);
   assert(fParticles);
   assert(fPointsArray);
    
   if (fVerboseLevel > 0) {
       cout << "\n-I- <SpdMvdMCHitProducer::Exec> Process event ... " 
            << fEvent->GetEventId();// << "\n\n";
       if (fVerboseLevel > 1) cout << "\n";    
   }
   
   fHits->Delete();
          
   // >>>>> Fill hits <<<<< 
         
   Int_t npoints = fPointsArray->GetEntriesFast();
   
   SpdMvdPoint*     point;
   SpdMCSiliconHit* hit;
   
   for (Int_t i(0); i<npoints; i++) 
   {
       point = dynamic_cast<SpdMvdPoint*>(fPointsArray->At(i));
        
       if (!AcceptPoint(point)) continue;
        
       //cout << ">>> create hit >>> point = " << i << endl;
        
       //point->Print("");
    
       hit = CreateMvdHit(point);
        
       if (!hit) return;
            
       // set mc-point id
       hit->SetMCPointId(i);
        
       // add mc-truth
       AddMCTruth(point,hit);
    }
    
   fNTotalHits += fHits->GetEntriesFast();
   
   if (fVerboseLevel > 1) 
   {
       cout << "\n-I- <SpdMvdMCHitProducer::Exec> Print ... " << endl; 
       
       if (fVerboseLevel > 2) {
           
           cout << "Print hits [first,last]: " << endl; 
           
           Int_t nn = fHits->GetEntriesFast();
           if (nn > 0) {
               hit = dynamic_cast<SpdMCSiliconHit*>(fHits->At(0));
               if (hit) hit->Print();
               if (nn > 1) {
                   hit = dynamic_cast<SpdMCSiliconHit*>(fHits->At(nn-1));
                   if (hit) hit->Print();
               }
           }
       }
       
       cout << "Points/hits:        " << fPointsArray->GetEntriesFast() << "/" 
                                      << fHits->GetEntriesFast() 
                                      << endl;
   }
}

//________________________________________________________________________________________
SpdMCSiliconHit* SpdMvdMCHitProducer::CreateMvdHit(SpdMvdPoint* p)
{
    if (!p) return 0;
    
    SpdMCSiliconHit* hit = AddHit();
    
    //---------------------------------
    // FILL DATA
    //
    
    // set hit "weight" 
    hit->SetWeight(1);

    TVector3 mc_point;  // mc-point position
    Double_t t;         // hit time
    
    p->GetPosTime(mc_point,t);

    // set mc-point position 
    hit->SetMCPoint(mc_point); //ATTENTION
    
    // set hit time & time error
    hit->SetHitTime(t);
    hit->SetHitTimeError(0.);
    
    // set total signal value FIXME
    hit->SetResp(p->GetEnergyLoss());
     
    // set layer number
    Int_t layerID = p->GetLayerId();
    hit->SetDetLayer(layerID);
    
    // set detector id
    hit->SetDetId(0,p->GetSubdetId());  // id[0] =  0 <=> barrel part
    hit->SetDetId(1,-1);                // id[1] = -1 <=> undefined (strip id)
    
    //----------------------------------------------------------
    // set DETECTOR parameters
    
    // set chip zero-level id (super module id) 
    hit->SetModId(p->GetDetectorID()); // kSpdIts
    
    hit->SetDetPos(mc_point);  //ATTENTION
    
    TVector3 nu(0,0,1);                                       // Z-axis
    TVector3 nn(mc_point.X(),mc_point.Y(),0); nn.SetMag(1);   // radial direction
    TVector3 nv = nn.Cross(nu); nv.SetMag(1);                 // (rphi)-direction
    
    hit->SetU(nu);
    hit->SetV(nv);
    
    //----------------------------------------------------------
    // >>>>>>>>>>>> calculate HIT position <<<<<<<<<<<<<<<
    
    TVector3 hit_pos;
    
    Double_t ures = GetHitResolutionU(layerID);
    Double_t vres = GetHitResolutionV(layerID);
    
    Double_t rmin = p->GetPosInPerp();
    Double_t rmax = p->GetPosOutPerp();
   
    if (rmax < rmin) {
        Double_t rr = rmin;
        rmin = rmax;
        rmax = rr;
    }
    
    Double_t r;
    Int_t n(0);
    while (true) {
       hit_pos = mc_point + gRandom->Gaus(0,vres)*nv;
       r = hit_pos.Perp();
       //cout << "rmax = " << rmax << " rmin = " << rmin << " r = " << r << " >>> " << (r > rmin && r < rmax) << endl;
       if (r > rmin && r < rmax) {
           hit_pos += gRandom->Gaus(0,ures)*nu;
           break;
       }
       if (++n == 10000) { //???
           hit_pos = mc_point;
           break;
       }
    }
    
    //cout << " rr  = " << rmax << " " << rmin << " r = " << hit_pos.Perp()  << endl;
    //cout << " res = " << ures << " " << vres << " r = " << r << "\n\n";
    
    //-----------------------------------------------------------------
    // ATTENTION set hit position, specification & resolution ATTENTION
    //-----------------------------------------------------------------
    hit->SetSpecifity('p'); // specification = 'p' (point) 
    hit->SetHitUV(hit_pos); 
    hit->SetResolutionU(ures);
    hit->SetResolutionV(vres);
    
    Double_t delta = (mc_point-hit_pos).Mag();
    
    hit->SetDelta(delta);
    
    // hit checking ...
    Double_t HitN = (mc_point-hit_pos).Dot(nn);
        
    //cout << "hit position(1): " << hit_pos.X() << " " << hit_pos.Y()  << " " << hit_pos.Z() << "\n";
    //cout << "hit position(2): " << hit->GetHitU() << " " << hit->GetHitV()  << " " << HitN << "\n";
    //cout << "hit delta:    " << delta << endl;
     
    return hit;
}

//________________________________________________________________________________________  
Bool_t SpdMvdMCHitProducer::AcceptPoint(const SpdMvdPoint* point) const
{
   if (!point) return false;
   return true;   
}

//________________________________________________________________________________________
void SpdMvdMCHitProducer::AddMCTruth(SpdMvdPoint* point, SpdMCSiliconHit* hit)
{
   Int_t ipart = SpdMCEventHelper::Instance()->GetParticleId(point->GetTrackID()); 
   if (ipart < 0) return; 
   hit->AddMCTruth(ipart); 
   SpdMCParticle* part = (SpdMCParticle*)fParticles->At(ipart);
   part->SetChar('+');
}

//________________________________________________________________________________________  
SpdMCSiliconHit* SpdMvdMCHitProducer::AddHit()
{
   if (!fHits) return 0;
   Int_t size = fHits->GetEntriesFast();
   SpdMCSiliconHit* hit = new ((*fHits)[size]) SpdMCSiliconHit();
   hit->SetId(size);
   return hit;
}

//________________________________________________________________________________________  
void SpdMvdMCHitProducer::Finish()
{
   if (fVerboseLevel > -1) {
       cout << "\n-I- <SpdMvdMCHitProducer::Finish> [Hits] "
            << "total: " << fNTotalHits << ", failed: " << fNFailedHits;// << endl;
   }
}

//________________________________________________________________________________________
void SpdMvdMCHitProducer::AddParameters()
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance();
   SpdSetParSet* params = run->GetParSetAssembly();
   FairParGenericSet* pars = run->GetParameters(fParsName);
   params->AddActiveGeoPars(pars);
}


