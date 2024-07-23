// $Id$
// Author: artur   2021/02/01

//_____________________________________________________________________________
//
// SpdTsMCHitProducer
//_____________________________________________________________________________

#include "SpdTsMCHitProducer.h"
#include "SpdSetParSet.h"

#include "SpdRunAna.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include "SpdSetParSet.h"
#include "SpdParSet.h"

#include "SpdTsTBGeoMapper.h"
#include "SpdTsTBGeoBuilder.h"

#include "SpdTsTECGeoMapper.h"
#include "SpdTsTECGeoBuilder.h"

#include "SpdMCEvent.h"
#include "SpdMCParticle.h"
#include "SpdMCStrawHit1D.h"
#include "SpdMCStrawHit2D.h"

#include "SpdTsTBPoint.h"
#include "SpdTsTECPoint.h"

#include "SpdMCEventHelper.h"
#include "SpdParticlesAndProcesses.h"

#include <TGeoManager.h>
#include <TRandom3.h>

#include <iostream>

using std::cout;
using std::endl;

# define mkm_ 1.e-4 // mkm -> cm 

ClassImp(SpdTsMCHitProducer)

//_____________________________________________________________________________
SpdTsMCHitProducer::SpdTsMCHitProducer():
fEvent(0),fParticles(0),fHits(0),
fHitType('v'),
fSaveHits(true),
fNFailedHits(0),fNTotalHits(0),
fVerboseLevel(1)
{
   fGeoMapper[0]  = 0;            fGeoMapper[1]  = 0; 
   fGeoBuilder[0] = 0;            fGeoBuilder[1] = 0;
   fParsName[0]   = "TsTBParSet"; fParsName[1]   = "TsTECParSet";
   fParameters[0] = 0;            fParameters[1] = 0;
   fSaveDetIdOption[0] = 0;       fSaveDetIdOption[1] = 0; 
   fSaveEmptyHits[0] = 0;         fSaveEmptyHits[1] = 0;
   fDataType[0] = "";             fDataType[1] = "";
   fPointsArray[0] = 0;           fPointsArray[1] = 0;
}

//_____________________________________________________________________________
SpdTsMCHitProducer::~SpdTsMCHitProducer() 
{
   for (Int_t i(0); i<2; i++) {
        if (fGeoMapper[i])  delete fGeoMapper[i];
        if (fGeoBuilder[i]) delete fGeoBuilder[i];
   }
   if (fHits) { fHits->Delete(); delete fHits; }
   
   //SpdMCEventHelper::Instance()->SetTsHits(0);
}

//_____________________________________________________________________________
void SpdTsMCHitProducer::LoadGeometry()
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance(); 
      
   if (!gGeoManager) run->GetParameters("FairGeoParSet");

   if (!gGeoManager) {
       cout << "-F- <SpdTsMCHitProducer::LoadGeometry> Geometry is not defined " << endl;
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
            cout << "-F- <SpdTsMCHitProducer::LoadGeometry> No geometry parameters " 
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
        
        fParameters[i]->GetParameter("Builder",builder); 
        fGeoBuilder[i] = SpdGeoFactory::Instance()->Builder(builder);
        
        if (fGeoBuilder[i]) {
            fGeoBuilder[i]->LoadParametersFrom(fParameters[i]);
            fGeoBuilder[i]->SetMapper(fGeoMapper[i]);   
            fGeoBuilder[i]->LockGeometry();
            //fGeoBuilder[i]->Print("");  
        }
        
        fParameters[i]->GetParameter("Detector/SaveDetIdOption",fSaveDetIdOption[i]);
        fParameters[i]->GetParameter("Detector/SaveEmptyHits",fSaveEmptyHits[i]);
        fParameters[i]->GetParameter("OutData_1",fDataType[i]);
   }
}

//_____________________________________________________________________________
void SpdTsMCHitProducer::SetHitType(Char_t spec)
{
    if (spec == 'v' || spec == 'w') fHitType = spec;
    else {
        if (fVerboseLevel > -2) {
            cout << "-W- <SpdTsMCHitProducer::SetHitType> Unknown hit specification: " 
                 << spec << endl;
        }
    }
}

//_____________________________________________________________________________
void SpdTsMCHitProducer::SetHitResolution(Int_t detPart, Char_t spec, Double_t resR, Double_t resZ)
{
    resR = TMath::Abs(resR);
    resZ = TMath::Abs(resZ);
    TString key = DetResKey(detPart,spec);
    if (spec == 'p' || spec == 'n') fResolution[key] = TVector3(resR,resZ,0.);
    else fResolution[key] = TVector3(resR,resZ,0.);
}

//_____________________________________________________________________________
Double_t SpdTsMCHitProducer::GetHitResolutionR(Int_t detPart, Char_t spec)
{
    TString key = DetResKey(detPart,spec);
    auto x = fResolution.find(key);
    return (x != fResolution.end()) ? (x->second)[0] : 150*mkm_; //ATTENTION
}

//_____________________________________________________________________________
Double_t SpdTsMCHitProducer::GetHitResolutionZ(Int_t detPart, Char_t spec)
{
    TString key = DetResKey(detPart,spec);
    auto x = fResolution.find(key);
    return (x != fResolution.end()) ? (x->second)[1] : 2.e4*mkm_; //ATTENTION
}

//_____________________________________________________________________________
InitStatus SpdTsMCHitProducer::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdTsMCHitProducer::Init> RootManager not instantiated " << endl;
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
   
   Bool_t init_input = false; 
   
   // get barrel Points array
   fPointsArray[0] = (TClonesArray*)ioman->GetObject(fDataType[0]);
    
   if (!fPointsArray[0]) {
       if (fVerboseLevel > -2) {
           cout << "-W- <SpdTsMCHitProducer::Init> No MC-POINTS: " << fDataType[0] << endl;
       }
   }
   else init_input = true;
   
   // get endcaps Points array
   fPointsArray[1] = (TClonesArray*)ioman->GetObject(fDataType[1]);
    
   if (!fPointsArray[1]) {
       if (fVerboseLevel > -2) {
           cout << "-W- <SpdTsMCHitProducer::Init> No MC-POINTS: " << fDataType[1] << endl;
       }
   }
   else init_input = true;
   
   if (!init_input) {
       cout << "-F- <SpdTsMCHitProducer::Init> No input data " << endl;
       return kFATAL; 
   }
 
   //============== CREATE AND REGISTER OUTPUT DATA OBJECTS ================/
   
   if (fHitType == 'v') {
       fHits = new TClonesArray("SpdMCStrawHit1D");
       ioman->Register(SpdDataBranchName(kMCTsHits),"SpdMCStrawHit1D", fHits, fSaveHits);
   }
   else if (fHitType == 'w') {
       fHits = new TClonesArray("SpdMCStrawHit2D");
       ioman->Register(SpdDataBranchName(kMCTsHits),"SpdMCStrawHit2D", fHits, fSaveHits);  
   }
   else {
       cout << "-F- <SpdTsMCHitProducer::Init> Unknown hit specification: " 
            << fHitType << endl;
       return kFATAL;
   }
   
   //============== PASS DATA TO EVENTHELPER ================================/   
   
   helper->SetTsHits(fHits);
   
   if (!helper->GetEvent()) helper->SetEvent(fEvent);
   if (!helper->GetParticles()) helper->SetParticles(fParticles);
 
   if (fVerboseLevel > -1) cout << "-I- <SpdTsMCHitProducer::Init> Intialization successfull " << endl;
   
   return kSUCCESS;
}

//________________________________________________________________________________________
void SpdTsMCHitProducer::Exec(Option_t* opt) 
{
   assert(fEvent);
   assert(fParticles);
   
   if(!fPointsArray[0]) assert(fPointsArray[1]);
   else assert(fPointsArray[0]); 
   
   if (fVerboseLevel > 0) {
       cout << "\n-I- <SpdTsMCHitProducer::Exec> Process event ... " 
            << fEvent->GetEventId(); 
       if (fVerboseLevel > 1) cout << "\n";         
   }
   
   fHits->Delete();
          
   // >>>>> Fill hits <<<<< 
         
   Int_t npoints, nhits[2] = {0,0};
   SpdMCTrackHit* hit;
    
   if (fPointsArray[0]) 
   {
       SpdTsTBPoint* point;
       npoints = fPointsArray[0]->GetEntriesFast();
       for (Int_t i(0); i<npoints; i++) 
       {
            point = dynamic_cast<SpdTsTBPoint*>(fPointsArray[0]->At(i));
            
            if (!AcceptPointB(point)) continue;
            
                 if (fHitType == 'v') hit = CreateTsHit1D(point,0);
            else if (fHitType == 'w') hit = CreateTsHit2D(point,0);
            else continue; 
             
            if (!hit) continue;
        
            nhits[0]++;
            
            // set mc-point id
            hit->SetMCPointId(i);
            
            // add mc-truth
            AddMCTruth(point->GetTrackID(),hit);
       }
   }
   
   if (fPointsArray[1]) 
   {
       SpdTsTECPoint* point;
       npoints = fPointsArray[1]->GetEntriesFast();
       for (Int_t i(0); i<npoints; i++) 
       {
            point = dynamic_cast<SpdTsTECPoint*>(fPointsArray[1]->At(i));
            
            if (!AcceptPointE(point)) continue;
            
                 if (fHitType == 'v') hit = CreateTsHit1D(point,1);
            else if (fHitType == 'w') hit = CreateTsHit2D(point,1);
            else continue; 
             
            if (!hit) continue;
        
            nhits[1]++;
            
            // set mc-point id
            hit->SetMCPointId(i);
            
            // add mc-truth
            AddMCTruth(point->GetTrackID(),hit);
       }
   }
   
   fNTotalHits += fHits->GetEntriesFast();
   
   if (fVerboseLevel > 1) 
   {
        cout << "\n-I- <SpdItsMCHitProducer::Exec> Print ... " << endl; 
       
        if (fVerboseLevel > 2 && fHits->GetEntriesFast() > 0) 
        {
            cout << "Print hits [first(barrel,endcap)]: " << endl; 
            if (nhits[0] > 0) {
                hit = dynamic_cast<SpdMCTrackHit*>(fHits->At(0));
                if (hit) hit->Print();
            }
            if (nhits[1] > 0) {
                hit = dynamic_cast<SpdMCTrackHit*>(fHits->At(nhits[0]));
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
        
        cout << "Points/hits (total):    " << nn << "/" << fHits->GetEntriesFast()<< endl; 
        cout << "Points/hits (barrel):   " << np1 << "/" << nhits[0] << endl;
        cout << "Points/hits (endcaps):  " << np2 << "/" << nhits[1] 
             << endl;
   }
}

//________________________________________________________________________________________
template <class T> SpdMCStrawHit1D* SpdTsMCHitProducer::CreateTsHit1D(T* p, Int_t detPart)
{
   if (!p) return 0;
   
   if (!dynamic_cast<SpdTsTBPoint*>(p) && 
       !dynamic_cast<SpdTsTECPoint*>(p)) return 0;

   SpdGeoBuilder* builder = fGeoBuilder[detPart];
   if (!builder) return 0;
    
   Long_t id = p->GetDetectorTID();
   
   if (!builder->SetNodePath(id)) return 0;
   
   // module and layer
   //----------------
   TString node_path = builder->GetNodePath();  // get chip "path"
   SpdGeopathParser parser;

   parser.ParsePath(node_path);

   Int_t module_copy_number = parser.Num(2);
   Int_t module_vol_number  = parser.VNum(2);

   Int_t layer_copy_number = parser.Num(3);
   Int_t layer_vol_number  = parser.VNum(3);

   SpdMCStrawHit1D* hit = AddHit1D();
   
   //---------------------------------
   // FILL DATA
   //
   
   // set hit "weight" 
   hit->SetWeight(1);
   
   // set specifity 
   hit->SetSpecifity(fHitType);

   TVector3 mc_point;  // mc-point position
   Double_t t;         // hit time
   
   p->GetPosTime(mc_point,t);

   // set mc-point position
   hit->SetMCPoint(mc_point); 
   
   // set hit time & time error
   hit->SetHitTime(t);
   hit->SetHitTimeError(0.);
   
   // set total signal value FIXME
   hit->SetResp(p->GetEnergyLoss());
   
   // segment length & track direction in the mc-point
   TVector3 dir; // track direction in the mc-point 
   Double_t s = p->GetPointDir(dir);
   
   hit->SetDirInPoint(dir);   
   hit->SetSegLength(s);
   
   //----------------------------------------------------------
   // set DETECTOR parameters
    
   // set chip zero-level id (super module id) 
   hit->SetModId(p->GetDetectorID());
   
   // set chip 1st-level id: backsward(=-1)/forward(=1) endcap or barrel(=0)
   if (detPart == 1) { (mc_point.Z() < 0) ? hit->SetDetId(0,-1) : hit->SetDetId(0,1); }
   else hit->SetDetId(0,0); 
   
   // set chip 2nd-level id 
   hit->SetDetId(1,id); // barrel
   
   Double_t x[3];
   TVector3 pos, size;
   
   // WARNING set straw position, axis (z) and sizes WARNING
   if (builder->GetDetectorPosition(x)) pos.SetXYZ(x[0],x[1],x[2]);
   if (builder->GetDetectorOrientation(x,'z')) dir.SetXYZ(x[0],x[1],x[2]);
   if (builder->GetNodeSize(x)) size.SetXYZ(x[0],x[1],x[2]);
   
   dir.SetMag(1);

   hit->SetMaxRadius(size.X());
   hit->SetWire(pos,dir,size.Z());
        
   // set hit resolution
   hit->SetResolutionR(GetHitResolutionR(detPart,fHitType));

   // set module number
   if (dynamic_cast<SpdTsTBPoint*>(p)) hit->SetDetModule(module_copy_number);
   else                                hit->SetDetModule(module_vol_number);
   
   // set layer number
   if (dynamic_cast<SpdTsTBPoint*>(p)) hit->SetDetLayer(layer_vol_number);
   else                                hit->SetDetLayer(layer_copy_number);
   
   // Checking
   
   Double_t rmax = hit->GetMaxRadius();
   Double_t R = hit->GetPointRdrift(); 

   if ((R < 0) || (R > rmax)) 
   {
       hit->SetResolutionR(GetHitResolutionR(detPart,'n'));
       hit->SetSpecifity('n');
       hit->SetWeight(0);
       fNFailedHits++;   
       //hit->Print(); p->Print("");
       return hit;
   }
   
   //--------------------------------------------
   // ATTENTION >>>> create hit position >>>>  ATTENTION

   Double_t r(0);

   Double_t resR = hit->GetResolutionR();
      
   const Double_t ktol = 0.99999;
   const Int_t nmaxit  = 100;
   Bool_t is_ok = true;

   Int_t nit(0);        
   if (resR < rmax/TMath::Sqrt(12)) {
       while (true) {
          r = gRandom->Gaus(R,resR);
          if (r > 0 && r < rmax) break;
          if (++nit == nmaxit) { is_ok = false; break; }
       }
   }
   else r = gRandom->Uniform(0,ktol*rmax);
   
   if (!is_ok) {
       r = R;
       hit->SetWeight(0);
       fNFailedHits++;
       //hit->Print(); p->Print("");
   }
   
   hit->SetR(r);

   //cout << " resolution (r): " << resR << "  R = " << r << "\n\n";
            
   //p->Print(""); 
   //hit->PrintHit();
   
   return hit;   
}

//________________________________________________________________________________________
template <class T> SpdMCStrawHit2D* SpdTsMCHitProducer::CreateTsHit2D(T* p, Int_t detPart)
{
   if (!p) return 0;
   
   if (!dynamic_cast<SpdTsTBPoint*>(p) && 
       !dynamic_cast<SpdTsTECPoint*>(p)) return 0;

   SpdGeoBuilder* builder = fGeoBuilder[detPart];
   if (!builder) return 0;
    
   Long_t id = p->GetDetectorTID();
   
   if (!builder->SetNodePath(id)) return 0;

   SpdMCStrawHit2D* hit = AddHit2D();
   
   //---------------------------------
   // FILL DATA
   //
   
   // set hit "weight" 
   hit->SetWeight(1);
   
   // set specifity 
   hit->SetSpecifity(fHitType);

   TVector3 mc_point;  // mc-point position
   Double_t t;         // hit time
   
   p->GetPosTime(mc_point,t);

   // set mc-point position
   hit->SetMCPoint(mc_point); 
   
   // set hit time & time error
   hit->SetHitTime(t);
   hit->SetHitTimeError(0.);
   
   // set total signal value FIXME
   hit->SetResp(p->GetEnergyLoss());
   
   // segment length & track direction in the mc-point
   TVector3 dir; // track direction in the mc-point 
   Double_t s = p->GetPointDir(dir);
   
   hit->SetDirInPoint(dir);   
   hit->SetSegLength(s);
   
   //----------------------------------------------------------
   // set DETECTOR parameters
    
   // set chip zero-level id (super module id) 
   hit->SetModId(p->GetDetectorID());
   
   // set chip 1st-level id: backsward(=-1)/forward(=1) endcap or barrel(=0)
   if (detPart == 1) { (mc_point.Z() < 0) ? hit->SetDetId(0,-1) : hit->SetDetId(0,1); }
   else hit->SetDetId(0,0); 
   
   // set chip 2nd-level id 
   hit->SetDetId(1,id); // barrel
   
   Double_t x[3];
   TVector3 pos, size;
   
   // WARNING set straw position, axis (z) and sizes WARNING
   if (builder->GetDetectorPosition(x)) pos.SetXYZ(x[0],x[1],x[2]);
   if (builder->GetDetectorOrientation(x,'z')) dir.SetXYZ(x[0],x[1],x[2]);
   if (builder->GetNodeSize(x)) size.SetXYZ(x[0],x[1],x[2]);
   
   dir.SetMag(1);

   hit->SetMaxRadius(size.X());
   hit->SetWire(pos,dir,size.Z());
        
   // set hit resolution
   hit->SetResolutionR(GetHitResolutionR(detPart,fHitType));
   hit->SetResolutionZ(GetHitResolutionZ(detPart,fHitType));
   
   // Checking
   
   Double_t rmax = hit->GetMaxRadius();
   Double_t zmax = hit->GetWireLength();   
   
   Double_t R = hit->GetPointRdrift(); 
   Double_t Z = hit->GetPointZwire();

   if ((Z < 0) || (Z > zmax) || (R < 0) || (R > rmax)) 
   {
       hit->SetResolutionR(GetHitResolutionR(detPart,'n'));
       hit->SetResolutionZ(GetHitResolutionZ(detPart,'n'));
       hit->SetSpecifity('n');
       hit->SetWeight(0);
       fNFailedHits++;   
       //hit->Print(); p->Print("");
       return hit;
   }
   
   //--------------------------------------------
   // ATTENTION >>>> create hit position >>>>  ATTENTION

   Double_t r(0), z(0);

   Double_t resR = hit->GetResolutionR();
   Double_t resZ = hit->GetResolutionZ();
   
   const Double_t ktol = 0.99999;
   const Int_t nmaxit  = 100;
   Bool_t is_ok = true;

   Int_t nit(0);        
   if (resR < rmax/TMath::Sqrt(12)) {
       while (true) {
          r = gRandom->Gaus(R,resR);
          if (r > 0 && r < rmax) break;
          if (++nit == nmaxit) { is_ok = false; break; }
       }
   }
   else r = gRandom->Uniform(0,ktol*rmax);
   
   if (is_ok) {
       nit = 0;
       if (resZ < zmax/TMath::Sqrt(12)) {
           while (true) {
              z = gRandom->Gaus(Z,resZ);
              if (z > 0 && z < zmax) break;
              if (++nit == nmaxit) { is_ok = false; break; }
           }
       }
       else z = gRandom->Uniform(0,ktol*zmax); 
   }
   
   if (!is_ok) {
       r = R; z = Z;
       hit->SetWeight(0);
       fNFailedHits++;
       //hit->Print(); p->Print("");
   }
   
   hit->SetR(r);
   hit->SetZ(z);
   
   //cout << " resolution (r,z): " << resR << "," << resZ << " R,Z = " << r << ", " << z << "\n\n";
            
   //p->Print(""); 
   //hit->PrintHit();
   
   return hit;   
}

//________________________________________________________________________________________  
Bool_t SpdTsMCHitProducer::AcceptPointB(const SpdTsTBPoint* point) const
{
   if (!point) return false;
   return true;   
}

//________________________________________________________________________________________  
Bool_t SpdTsMCHitProducer::AcceptPointE(const SpdTsTECPoint* point) const
{
   if (!point) return false;
   return true;   
}

//________________________________________________________________________________________
void SpdTsMCHitProducer::AddMCTruth(Int_t itrack, SpdMCTrackHit* hit)
{
   Int_t ipart = SpdMCEventHelper::Instance()->GetParticleId(itrack); 
   if (ipart < 0) return; 
   hit->AddMCTruth(ipart); 
   SpdMCParticle* part = (SpdMCParticle*)fParticles->At(ipart);
   part->SetChar('+');
}

//________________________________________________________________________________________  
SpdMCStrawHit1D* SpdTsMCHitProducer::AddHit1D()
{
   if (!fHits) return 0;
   Int_t size = fHits->GetEntriesFast();
   SpdMCStrawHit1D* hit = new ((*fHits)[size]) SpdMCStrawHit1D();
   hit->SetId(size);
   return hit;
}

//________________________________________________________________________________________  
SpdMCStrawHit2D* SpdTsMCHitProducer::AddHit2D()
{
   if (!fHits) return 0;
   Int_t size = fHits->GetEntriesFast();
   SpdMCStrawHit2D* hit = new ((*fHits)[size]) SpdMCStrawHit2D();
   hit->SetId(size);
   return hit;
}

//________________________________________________________________________________________  
void SpdTsMCHitProducer::Finish()
{
   if (fVerboseLevel > -1) {
       cout << "\n-I- <SpdTsMCHitProducer::Finish> [Hits] "
            << "total: " << fNTotalHits << " failed: " << fNFailedHits;// << endl;
   }
}

//________________________________________________________________________________________
void SpdTsMCHitProducer::AddParameters()
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance();
   SpdSetParSet* params = run->GetParSetAssembly();
   FairParGenericSet* pars;
   pars = run->GetParameters(fParsName[0]);
   params->AddActiveGeoPars(pars);
   pars = run->GetParameters(fParsName[1]);
   params->AddActiveGeoPars(pars);
}


