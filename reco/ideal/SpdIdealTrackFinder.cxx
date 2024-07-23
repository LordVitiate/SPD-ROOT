// $Id$
// Author: artur   2019/09/16

//_____________________________________________________________________________
//
// SpdIdealTrackFinder
//_____________________________________________________________________________

#include "SpdIdealTrackFinder.h"

#include <TRandom.h>
#include <TClonesArray.h>
#include <TGeoManager.h>
#include <TDatabasePDG.h>
#include "TFile.h"

#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairRun.h"

#include "SpdDetector.h"
#include "SpdMCEventHeader.h"
#include "SpdMCTrack.h"
#include "SpdGeoFactory.h"

#include "SpdItsPoint.h"
#include "SpdItsGeoMapperX.h"
#include "SpdItsGeoBuilder.h"

#include "SpdTsTBPoint.h"
#include "SpdTsTBGeoMapper.h"
#include "SpdTsTBGeoBuilder.h"

#include "SpdTsTECPoint.h"
#include "SpdTsTECGeoMapper.h"
#include "SpdTsTECGeoBuilder.h"

#include "SpdIdealTrackData.hh"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdIdealTrackFinder)

//_____________________________________________________________________________
SpdIdealTrackFinder::SpdIdealTrackFinder():
FairTask("SPD Ideal track finder"),
fOwnGeoLoader(false),
fGeoLoader(0),
fEventHeader(0),
fMCTracksArray(0),
fEvent(0),
fParameters(0),
fPrimVertexOnly(true),
fNErrors(0),
fNFailedHits(0)
{
    fMinMomentum[0]     = 4e-3; fMinMomentum[1]     = 1e-2; // pmin = [4,10] MeV
    fMinGammaBeta[0]    = 0.05; fMinGammaBeta[1]    = 0.5;  // p/m
    fMinVertexNHits[0]  = 0;    fMinVertexNHits[1]  = 1;    
    fMinTrackerNHits[0] = 2;    fMinTrackerNHits[1] = 2;
    
    Double_t mkm = 1e-4;
    
    // set hits resolution (undefined type): x, y, z
    SetHitResolution(kSpdIts,  'n', 100*mkm, 100*mkm, 100*mkm); 
    SetHitResolution(kSpdTsTB, 'n', 150*mkm, 150*mkm, 150*mkm); 
    SetHitResolution(kSpdTsTEC,'n', 150*mkm, 150*mkm, 150*mkm); 
    
    // set hits resolution default point-like hits   
    SetHitResolution(kSpdTsTB,  'p', 150*mkm, 150*mkm, 150*mkm);  
    SetHitResolution(kSpdTsTEC, 'p', 150*mkm, 150*mkm, 150*mkm); 
    
    fParameters = new SpdBaseParSet();
}

//_____________________________________________________________________________
SpdIdealTrackFinder::~SpdIdealTrackFinder() 
{
}

//_____________________________________________________________________________
void SpdIdealTrackFinder::DeleteGeoLoader()
{
    if (fOwnGeoLoader && fGeoLoader) {
        cout << "-I- <SpdIdealTrackFinder::DeleteGeoLoader> Remove geometry loader" << endl;
        delete fGeoLoader;
        fGeoLoader = 0;
        fOwnGeoLoader = false;
    }
}

//_____________________________________________________________________________
void SpdIdealTrackFinder::SetHitResolution(Int_t det, Char_t spec, Double_t res1, Double_t res2, Double_t res3)
{
    TVector3 res(res1,res2,res3);
    
    if (res2 < 0) res.SetY(res1);
    if (res3 < 0) res.SetZ(TMath::Max(res1,res2));
    
    fResolution[DetResKey(det,spec)] = res;
    
    //cout << fResolution.size() << " "<< fResolution[DetResKey(det,spec)].X() << " " 
    //     << fResolution[DetResKey(det,spec)].Y() << " " 
    //     << fResolution[DetResKey(det,spec)].Z() << "\n";
}

//_____________________________________________________________________________
TVector3 SpdIdealTrackFinder::GetHitResolution(Int_t det, Char_t spec)
{
    TString reskey = DetResKey(det,spec);
    std::map< TString, TVector3 >::const_iterator it = fResolution.find(reskey);
    return (it != fResolution.end()) ? it->second : TVector3(0.01,0.01,0.01);
}

//_____________________________________________________________________________
void SpdIdealTrackFinder::SetTrackingGeoModules(Bool_t add_passives)
{
    fGeoModules.clear(); 
    if (add_passives) fGeoModules.push_back("Passives");
    
    fGeoModules.push_back("Inner tracker system");
    fGeoModules.push_back("TS barrel (tor)");
    fGeoModules.push_back("TS endcaps (tor)");
}

//_____________________________________________________________________________
void SpdIdealTrackFinder::LoadGeometry() 
{ 
    fGeoLoader = SpdGeoLoader::Instance();

    if (!fGeoLoader) { 
        fGeoLoader = new SpdGeoLoader(); 
        fOwnGeoLoader = true; 
    }
    else fOwnGeoLoader = false;
    
    fGeoLoader->LoadGeometry();    // load TOP level only (cave)
    
    Int_t n = fGeoModules.size();
    
    if (n < 1) return;
    
    for (Int_t i(0); i<n; i++) {
         if (fGeoModules[i].IsDigit()) fGeoLoader->LoadModule(fGeoModules[i].Atoi());
         else fGeoLoader->LoadModule(fGeoModules[i]);
    }

    ITF_info* data;
    std::set<Int_t>::const_iterator it = fGeoLoader->GetActualModules().begin();
    
    for (; it != fGeoLoader->GetActualModules().end(); it++) 
    {
        if (!fGeoLoader->IsModuleActive(*it)) continue;
        data = new ITF_info();
        data->mapper = fGeoLoader->GetMapper(*it);
        data->builder = fGeoLoader->GetBuilder(*it);
        data->htype = kUndefined; //kSpacepoint;
        fInputData.insert(std::pair<Int_t,ITF_info*>(*it,data)); 
    }
    
    cout << "-i- <SpdIdealTrackFinder::LoadGeometry> add digits collections: " << fInputData.size() << endl;
    
    fGeoLoader->PrintActualGeometry();
}

//________________________________________________________________________________________
Bool_t SpdIdealTrackFinder::FindSelectedTrack(Int_t eid, Int_t tid) const 
{
    if (fTracksSelected.empty()) return kFALSE;
    std::map< Int_t, std::set<Int_t> >::const_iterator it = fTracksSelected.find(eid);
    if (it == fTracksSelected.end()) return kFALSE;
    return (it->second.find(tid) != it->second.end());
}

//________________________________________________________________________________________
void SpdIdealTrackFinder::AddTrackAsSelected(Int_t event /* >=1 */, Int_t track /*>= 0*/)
{
     std::map< Int_t, std::set<Int_t> >::iterator it = fTracksSelected.find(event);
     if (it == fTracksSelected.end()) {
         std::set<Int_t> tracks;
         tracks.insert(track);
         fTracksSelected.insert(std::pair< Int_t, std::set<Int_t> >(event,tracks));
     }
     else it->second.insert(track);
}

//_____________________________________________________________________________
InitStatus SpdIdealTrackFinder::Init() 
{   
    // get RootManager
    
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- <SpdIdealTrackFinder::Init> "
             << "RootManager not instantiated!" << endl;
        return kFATAL;
    }
    
    // Load Geometry 
    
    LoadGeometry();
    
    // get Event header
    
    fEventHeader = (SpdMCEventHeader*)ioman->GetObject("MCEventHeader.");
   
    if (!fEventHeader) {
        cout << "-E- <SpdIdealTrackFinder::Init>" << "No HEADER object inside " << endl;
        return kERROR;
    }
    
    // get Tracks array
    
    fMCTracksArray = (TClonesArray*)ioman->GetObject("SpdMCTrack");
    
    if (!fMCTracksArray) {
        cout << "-W- <SpdIdealTrackFinder::Init> " << "No MCTrack array!" << endl;
        return kERROR;
    }
    
    // Create and register output data object
    
    fEvent = new SpdIdealTrackColl();
    ioman->Register("IdealTrackColl.", "IdealEvent", fEvent, kTRUE);
    
    // get Points arrays
    
    SpdParSet* pars;
    SpdParameter* par;
    TString datatype;
    TClonesArray* points;
    Int_t NDataOut(0);
    
    SpdIdealDataInfo* dinfo;
    Int_t dinfo_num;
    
    std::map<Int_t,ITF_info*>::iterator it = fInputData.begin();
    for (; it != fInputData.end(); it++) {
         
         pars = fGeoLoader->GetActiveParameters(it->first);
         
         pars->GetParameter("Detector/NOutData",NDataOut);
         if (NDataOut < 1) {
             cout << "-W- <SpdIdealTrackFinder::Init>  No Points array for detector of id = " 
                  << it->first<< endl;
             return kERROR;
         }
         
         pars->GetParameter("OutData_1",datatype);
         
         points = (TClonesArray*)ioman->GetObject(datatype);
         it->second->points = points;
         
         dinfo = new SpdIdealDataInfo(datatype,pars->GetName(),it->first);
         dinfo->SetHitType(it->second->htype);
         
         dinfo_num = fEvent->AddDataInfo(dinfo);
         it->second->datainfo = dinfo_num;
        
         //cout << " >>>> data: " << datatype << "  params: " << pars->GetName() 
         //     << " detector: " << it->first << " info: " << dinfo_num << endl;
    }
    
    // save parameters list
   
    FillParametersIn(fParameters);
    
    TFile* f = gFile;
    TFile* ff = ioman->GetOutFile();
    ff->cd();
    fParameters->Write("IdealTrackFinderParams",TObject::kOverwrite);
    gFile = f;
    
    cout << "-I- <SpdIdealTrackFinder:Init> Intialization successfull " << endl;
    
    fNErrors = 0;
    fNFailedHits = 0;
    
    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdIdealTrackFinder::Exec(Option_t* opt) 
{
    if (!fEventHeader) return;
    if (!fMCTracksArray) return;
    
    fEvent->ClearData();
    
    Int_t nfailedhits = fNFailedHits;
    
    // >>>>> Fill event <<<<< 
    
    fEvent->SetRunId(fEventHeader->GetRunID());
    fEvent->SetEventId(fEventHeader->GetEventID());
    
    fEvent->SetVertex(fEventHeader->GetX(),fEventHeader->GetY(),fEventHeader->GetZ());
    fEvent->SetVertexTime(fEventHeader->GetT());
    
    // +++++ +++++ SELECT TRACKS +++++ +++++
    
    SpdMCTrack* mctrack;
    SpdIdealTrack* track;
    
    // selected mc-tracks: 
    std::map< Int_t, std::map<Double_t, SpdIdealHit*> > htracks;  // [track id <-> [hit time <-> hit] ]
    std::map< Int_t, Int_t > htrnums;                             // [track id <-> track number in the output Event)]
    
    Int_t Nt = 0;
    Int_t Nmc = fMCTracksArray->GetEntriesFast();
    
    Bool_t accept;
    Int_t  label;
   
    for (Int_t i(0); i<Nmc; i++) {
        
         mctrack = (SpdMCTrack*)fMCTracksArray->At(i);
        
         track = CreateTrack(i,mctrack,accept,label);
        
         if (accept) fEvent->AddTrackMark(label);
         else continue;
             
         if (!track) continue;
         
         htracks.insert(std::pair< Int_t, std::map<Double_t,SpdIdealHit*> >(i,std::map<Double_t, SpdIdealHit*>()));
         htrnums.insert(std::pair< Int_t, Int_t >(i,Nt));
    
         fEvent->AddTrack(track);
    
         Nt++;
    }
    
    if (Nt == 0) {
        cout << "<SpdIdealTrackFinder::Exec> No accepted data for the event: " << fEventHeader->GetEventID() << endl;
        fEventHeader->Reset();
        return;
    }
    
    //cout << "<SpdIdealTrackFinder::Exec> event: " << fEventHeader->GetEventID()
    //     << "; add tracks: " << Nt << "/" << Nmc << endl;
    
    fEvent->SetInfoNTracks(Nt);  
    
    // +++++ +++++ CREATE HITS +++++ +++++
    
    TClonesArray* points;
    SpdIdealDataInfo* dinfo;
    FairMCPoint* mcpoint;
    SpdIdealHit* hit;
    Int_t dinfonum;
    
    std::map<Int_t,ITF_info*>::const_iterator it = fInputData.begin();
    
    for ( ; it != fInputData.end(); it++) {
        
         points = it->second->points;
         dinfonum = it->second->datainfo;
         
         dinfo = fEvent->GetDataInfo(dinfonum);
         
         Int_t np = points->GetEntriesFast();
         Int_t tid;
         for (Int_t i(0); i<np; i++) {
             
              mcpoint = (FairMCPoint*)points->At(i);
              tid = mcpoint->GetTrackID();
              
              if (htracks.find(tid) == htracks.end()) continue;
              
              hit = CreateHit(mcpoint,it->first,it->second);
              if (!hit) continue; // see "Checking" below ATTENTION
              
              hit->SetDetId(0,it->first);
              hit->SetEdep(mcpoint->GetEnergyLoss());
              hit->SetInfoNum(dinfonum);
              hit->SetMotherPointNum(i);
        
              dinfo->SetHitType(it->second->htype); //ATTENTION 
              
              htracks[tid].insert(std::pair<Double_t,SpdIdealHit*>(hit->GetTimeStamp(),hit));
              dinfo->AddTrackHits(htrnums[tid],1);
         }
    }

    // +++++ +++++ ADD HITS TO TRACKS +++++ +++++

    std::map< Int_t,std::map<Double_t, SpdIdealHit*> >::iterator itx = htracks.begin();
    Int_t n(0); 
    Int_t hit_uid(0);
    for ( ; itx != htracks.end(); itx++) {
         track = fEvent->GetTrack(n);
         std::map<Double_t, SpdIdealHit*>::iterator itxx = itx->second.begin();
         for (; itxx != itx->second.end(); itxx++) {
              itxx->second->AddTrackId(n);
              itxx->second->SetId(hit_uid++);
              track->AddHit(itxx->second);
         }
         n++;
    }
     
    // ATTENTION-------------------  Checking   -----------------ATTENTION
    Int_t nerrrors;
    
    for (Int_t i(0); i<Nmc; i++) {
        
        if (htrnums.find(i) == htrnums.end()) continue;
        
        mctrack = (SpdMCTrack*)fMCTracksArray->At(i);
        Int_t np1, np2, nn(0);
        
        nerrrors = 0;
        
        for (it = fInputData.begin(); it != fInputData.end(); it++) {
            
             dinfo = fEvent->GetDataInfo(it->second->datainfo);
             track = fEvent->GetTrack(htrnums[i]);
             
             np1 = mctrack->GetNPoints(DetectorId(it->first));
             np2 = dinfo->GetTrackHits(htrnums[i]); 
            
             nn += np1;
             
             if (np1 != np2) { 
                 cout << "-W- <SpdIdealTrackFinder::Exec> Number of hits: " 
                      << np1 << "/"<< np2 << endl;
                 nerrrors++;    
             }
         }
         
         // Set 
         track = fEvent->GetTrack(htrnums[i]); 
         track->SetNPoints(nn);
         
         if (nerrrors > 0) {
             track->SetErrorFlag(1);
             fEvent->SetErrorFlag(1);
         }
    }
 
    if (fEvent->GetErrorFlag() != 0) fNErrors++;
    
    if (fNFailedHits > nfailedhits) {
        if (fEvent->GetErrorFlag() == 0) fEvent->SetErrorFlag(2);
        else fEvent->SetErrorFlag(3);
    }
 
    //fEvent->PrintData();
    
    fEventHeader->Reset();
}

//________________________________________________________________________________________
SpdIdealTrack* SpdIdealTrackFinder::CreateTrack(Int_t id, SpdMCTrack* mctrack, Bool_t& accept_track, Int_t& selmark)
{
    selmark = -1;
    accept_track = kFALSE; //ATTENTION 
    
    if (!mctrack) return 0;
    
    selmark = kDefined; // =0
    accept_track = kTRUE;
    
    // check track number
    if (fTracksSelected.empty()) {
        if (fPrimVertexOnly && mctrack->GetFirstMotherId() != -1) 
        {
            accept_track = kFALSE; //ATTENTION 
            selmark = kVertexParticle;
            return 0;
        }
    }
    else { /* FIXME */}
    
    // check particle info 
    Int_t pdg = mctrack->GetPdgCode();
    TParticlePDG* part = TDatabasePDG::Instance()->GetParticle(pdg);
    if (!part) { selmark = kIdentified; return 0; }
    
    // check if particle is charged
    Double_t q = part->Charge();
    if (q == 0) { selmark = kCharged; return 0; }
    
    // check particle momentum
    Double_t mom = mctrack->GetP();
    if (mom < GetMinMomentum()) { selmark = kMomentumCut; return 0; }
    
    // check hits number
    Int_t np = 0;
    std::map<Int_t,ITF_info*>::const_iterator it = fInputData.begin();
   
    // check vertex hits number
    np = mctrack->GetNPoints(kSpdIts);
    if (np < GetMinVertexNHits()) { selmark = kVDHitsLim; return 0; }
    
    // check tracker hits number
    np  = mctrack->GetNPoints(kSpdTsTB);
    np += mctrack->GetNPoints(kSpdTsTEC);
    if (np < GetMinTrackerNHits()) { selmark = kTHitsLim; return 0; }
    
    //cout << "-I- <SpdIdealTrackFinder::AcceptTrack> Add track: " << id << " points: " << np << endl; 
    
    SpdIdealTrack* track = new SpdIdealTrack();
    
    track->SetTrackId(id);
    track->SetMother1TrackId(mctrack->GetFirstMotherId());
    track->SetMother2TrackId(mctrack->GetSecondMotherId());
    
    track->SetPdgCode(pdg);
    track->SetCharge(q/3.); 
    track->SetVProcess(mctrack->GetStartProcess());
    track->SetStartTime(mctrack->GetStartT());
    
    track->SetPrimVertex(mctrack->GetStartVertex());
    track->SetStartMomentum(mctrack->GetMomentum());
    
    selmark = kAccepted;
    
    return track;
}

//________________________________________________________________________________________
SpdIdealHit* SpdIdealTrackFinder::CreateHit(FairMCPoint* point, Int_t DetId, ITF_info* info)
{
    if (!info) return 0;
    
    switch(DetId) 
    {
        case kSpdIts:   return CreateItsHit(point,info);
        case kSpdTsTB:  return CreateTsTBHit(point,info);
        case kSpdTsTEC: return CreateTsTECHit(point,info);
        default: break;
    }
    
    cout << "-W- <SpdIdealTrackFinder::MakeHit> "
         << " Hit cannot be created for the detector of id = " << DetId << endl;
    
    return 0;
}

//________________________________________________________________________________________
SpdIdealHit* SpdIdealTrackFinder::CreateItsHit(FairMCPoint* point, ITF_info* info)
{
    info->htype = kSpacepoint;
    
    if (!fGeoLoader) return 0;
        
    SpdDetector* det = fGeoLoader->GetActive(kSpdIts);
    if (!det) return 0;
    
    SpdItsPoint* p = dynamic_cast<SpdItsPoint*>(point);
    if (!p) return 0;
    
    SpdItsGeoBuilder* builder = dynamic_cast<SpdItsGeoBuilder*>(info->builder);
    if (!builder) return 0;
    
    Int_t id = p->GetNodeId();
    
    if (!builder->SetNodePath(id)) return 0;
        
    SpdIdealSpacepointHit* hit = new SpdIdealSpacepointHit();
    
    hit->SetDetId(1,id);
    
    TVector3 v;  // MC-hit position
    Double_t t;  // MC-hit time
    
    p->GetPosTime(v,t);

    hit->SetPoint(v);   // ATTENTION set MC-POINT position
    hit->SetTimeStamp(t);
   
    TString path = builder->GetNodePath();
    
    // check detector part (barrel or endcap) ATTENTION
    Bool_t barrel_part = true;
    if (path.Contains("EClayer")) barrel_part = false; 
    
    Double_t x[3];
    
    // set detector position
    if (builder->GetDetectorPosition(x)) hit->SetDetPos(x[0],x[1],x[2]);
        
    // set chip local axis
    if (barrel_part) {
        // BARREL: (z->u, x->v, y->n)
        if (builder->GetDetectorOrientation(x,'z')) hit->SetU(TVector3(x[0],x[1],x[2]));
        if (builder->GetDetectorOrientation(x,'x')) hit->SetV(TVector3(x[0],x[1],x[2]));
        if (builder->GetNodeSize(x)) hit->SetL(x[2],x[0],x[1]); //u,v,n-sizes 
    }
    else {
        // ENDCAPS: (x->u, y->v, z->n)
        if (builder->GetDetectorOrientation(x,'x')) hit->SetU(TVector3(x[0],x[1],x[2]));
        if (builder->GetDetectorOrientation(x,'y')) hit->SetV(TVector3(x[0],x[1],x[2]));
        if (builder->GetNodeSize(x)) hit->SetL(x[0],x[1],x[2]); //u,v,n-sizes  
    }
    
    //===============================================
    // searching for HIT position and specification
    //===============================================
    
    //cout << "\n<SpdIdealTrackFinder::CreateItsHit> Check specifity, find hit position " << "\n\n";
    
    SpdGeoVVolumeBox2D* vvol = dynamic_cast<SpdGeoVVolumeBox2D*>(builder->GetNodeVVolume()); 
    
    //if (vvol) vvol->PrintVolume();
    //p->Print("");
    
    Bool_t nehits = !det->GetSaveEmptyHits();
    
    Char_t spec = 'n';    // hit specification    
    Int_t  submod_id(0);  // submodule id for vertex detector: 0 = barrel part, 1 = endcaps  
    
    // check submodule id 
    if (!barrel_part) {
        submod_id = 1;
        hit->SetDetId(2,submod_id);
        hit->SetSpecifity('p'); // specification = "point"
        
        //hit->SetHit(v); // ATTENTION set HIT position = MC-POINT for ITS endcaps
        hit->SetHit(v[0],v[1],hit->GetDetPosZ()); // ATTENTION set HIT position = MC-POINT(X,Y)+DET_POS(Z) for ITS endcaps
        
        hit->SetResolution(GetHitResolution(kSpdIts,hit->GetSpecifity())); // set hit resolution
        
        // ATTENTION
        if (TMath::Abs(hit->GetHitN()) > 1e-14) {
            hit->SetSpecifity('n');
            fNFailedHits++;
            //cout << "(1) spec  "<< hit->GetSpecifity() << " U, V, N: " 
            //     << hit->GetHitU() << " " << hit->GetHitV() << " " << hit->GetHitN() << endl;            
        }
        return hit;     
    }
    
    // check if hit is empty
    if (nehits && !(point->GetEnergyLoss() > 0)) {
        
        hit->SetDetId(2,submod_id);
        hit->SetSpecifity('n');  // specification = "undefined"
        hit->SetHit(v); // ATTENTION set HIT position = MC-POINT 
        hit->SetResolution(GetHitResolution(kSpdIts,hit->GetSpecifity())); // set hit resolution
        fNFailedHits++;
        return hit;      
    }
        
    // check hit specification
    if (vvol) {
        TString vvolname = vvol->GetName();
        if (vvolname.Contains("DSSD")) spec = 's';
        else if (vvolname.Contains("MAPS")) spec = 'm';
    }
    
    Double_t hit_pos[3] = {0, 0, 0};    // chip channel global position (x,y,z)
    
    //cout << "Hit production; spec = " << hit->GetSpecifity() << " " << vvol << " " << path << endl;
    
    // standard hit specifications
    if (spec == 's' || spec == 'm') 
    {
        Int_t nchan = p->GetVDataSize();   // number of hitted channels in the chip
        Int_t vid;                         // chip channel id 
        if (nchan < 1) spec = 'n';
        else if (nchan == 1) {
            if (nehits && !(p->GetVed(0) > 0)) spec = 'n';
            else {
                vid = p->GetVid(0); 
                if (vid < 0) spec = 'n';
                else {
                    builder->SetVid(vid); // set channel C.S.
                    if (!builder->GetDetectorPosition(hit_pos)) spec = 'n';
                }
            }
        }
        else {
            Int_t iz_ch, iphi_ch;  // chip channel id's: iz_ch = id1, iphi_ch = id2
            Int_t nc(0), az_ch(0), aphi_ch(0);
            for (Int_t j(0); j < nchan; j++) {
                 vid = p->GetVid(j);
                 if (vid < 0) continue;
                 if (nehits && !(p->GetVed(j) > 0)) continue;
                 vvol->CellId(vid, iz_ch, iphi_ch);
                 az_ch += iz_ch;
                 aphi_ch += iphi_ch;
                 nc++;
                 //cout << j << "  (z,phi):  " << iz_ch << "  " << iphi_ch << endl;
            }  
            //cout << "  a(z,phi):  " << az_ch/Double_t(nc) << "  " << aphi_ch/Double_t(nc) << " nc = " << nc << endl;
            if (nc == 0) {
                Double_t dep = 0;
                if (p->IsSpecialPoint(vid,dep)) { // special case: very short (~several mkm) track segment 
                    if (nehits && !(dep > 0)) spec = 'n';
                    else {
                        builder->SetVid(vid); // set channel C.S.
                        if (!builder->GetDetectorPosition(hit_pos)) spec = 'n';
                        //cout << "spec: " << spec << " vid = " << vid << " dep =  " << dep << endl;
                    }
                }
                else spec = 'n';
            }
            else if (nc == 1) {
                vid = vvol->CellId(az_ch,iphi_ch);
                builder->SetVid(vid); // set channel C.S.
                if (!builder->GetDetectorPosition(hit_pos)) spec = 'n';
            }
            else {
                Double_t pos[3];
                vvol->GetPos(az_ch/Double_t(nc),aphi_ch/Double_t(nc),pos);
                builder->SetVid(-1);  // set chip C.S.
                if (!builder->LocalToGlobalP(pos,hit_pos)) spec = 'n'; 
            }
        }
        
//         cout << "\t vol.name = " << vvol->GetName() << ";  specifity: " << spec << ";  sub_mod: " << submod_id 
//              << ";  nchannels: " << nchan << ";  " << builder->GetNodePath() << "\n\n";
//         printf("\t its point (1): %14.8f  %14.8f  %14.8f\n",v.X(),v.Y(),v.Z());
//         printf("\t its point (2): %14.8f  %14.8f  %14.8f\n",hit_pos[0],hit_pos[1],hit_pos[2]);
//         printf("\n");

    }
 
    if (spec == 'n') {
        hit->SetDetId(2,submod_id);
        hit->SetSpecifity('n');   // specification = "undefined"
        hit->SetHit(v); // ATTENTION set HIT position = MC-POINT 
        hit->SetResolution(GetHitResolution(kSpdIts,hit->GetSpecifity()));
        fNFailedHits++;
        //p->Print("");
        return hit;          
    }
    
    hit->SetDetId(2,submod_id);
    hit->SetSpecifity(spec); // specification = 'm' (MAPS) or 's' (DSSD)
    hit->SetHit(hit_pos[0],hit_pos[1],hit_pos[2]); // ATTENTION set hit position
    hit->SetResolution(GetHitResolution(kSpdIts,hit->GetSpecifity()));
    
    // ATTENTION
    if (TMath::Abs(hit->GetHitN()) > 1e-14) {
        hit->SetSpecifity('n');
        fNFailedHits++;
        //cout << "(2) spec  "<< hit->GetSpecifity() << " U, V, N: " 
        //       << hit->GetHitU() << " " << hit->GetHitV() << " " << hit->GetHitN() << endl;
    }
    
    return hit; 
    
    // calculate resolution "on the fly" 
//     if (spec == 'm') 
//     {
//         static Int_t nr = 0; 
//         static Double_t rsum = 0;
//         static Double_t rsum2 = 0;
//         static Double_t dd = -1.;
//         
//         static Double_t xsum2[3] = {0,0,0};
//         static Double_t xsum[3]  = {0,0,0};
//         static Double_t xd[3]    = {-1,-1,-1};
//   
//         Double_t r2 = (v.X()-hit_pos[0])*(v.X()-hit_pos[0])+
//                       (v.Y()-hit_pos[1])*(v.Y()-hit_pos[1])+
//                       (v.Z()-hit_pos[2])*(v.Z()-hit_pos[2]);
//         Double_t r = TMath::Sqrt(r2);
//         
//         Double_t du = TMath::Abs(hit->GetHitU()-hit->GetPointU());
//         Double_t dv = TMath::Abs(hit->GetHitV()-hit->GetPointV());
//         Double_t dn = TMath::Abs(hit->GetHitN()-hit->GetPointN());
//        
//         nr++;
//         rsum2 += r2;
//         rsum  += r;
//         if (r > dd) dd = r;
//         
//         xsum[0] += du;
//         xsum[1] += dv;
//         xsum[2] += dv;
//         
//         xsum2[0] += du*du;
//         xsum2[1] += dv*dv;
//         xsum2[2] += dn*dn;
//         
//         if (du > xd[0]) xd[0] = du;
//         if (dv > xd[1]) xd[1] = dv;
//         if (dn > xd[2]) xd[2] = dn;
//         
//         cout << "[ITS] specifity: " << spec
//              << "  diff. distance (current, max, ave) = " << r*1e4 
//              << "  " << dd*1e4 << "  " << 1e4*rsum/nr << " [mkm] " 
//              << " sigma = " << 1e4*TMath::Sqrt(TMath::Abs(rsum2/nr-rsum*rsum/(nr*nr))) << " [mkm] " << endl;
//              
//         cout << "[ITS] specifity: " << spec
//              << "  diff. distance (current, max, ave) = " << r*1e4 
//              << "  " << dd*1e4 << "  " << 1e4*rsum/nr << " [mkm] " 
//              << " sigma = " << 1e4*TMath::Sqrt(TMath::Abs(rsum2/nr-rsum*rsum/(nr*nr))) << " [mkm] " 
//              << "\n\t (du,dv,dn) = " << du*1e4 << ", " << dv*1e4 << ", " << dn*1e4 << " [mkm] "
//              << "  max(du,dv,dn) = " << xd[0]*1e4 << "," << xd[1]*1e4 << "," << xd[2]*1e4 << " [mkm] "
//              << " sigma(du,dv,dn) = " << 1e4*TMath::Sqrt(TMath::Abs(xsum2[0]/nr-xsum[0]*xsum[0]/(nr*nr)))
//                               << ", " << 1e4*TMath::Sqrt(TMath::Abs(xsum2[1]/nr-xsum[1]*xsum[1]/(nr*nr)))
//                               << ", " << 1e4*TMath::Sqrt(TMath::Abs(xsum2[2]/nr-xsum[2]*xsum[2]/(nr*nr)))
//              << endl;      
//         
//         //printf("\t its point (1): %14.8f  %14.8f  %14.8f\n",v.X(),v.Y(),v.Z());
//         //printf("\t its point (2): %14.8f  %14.8f  %14.8f\n",hit_pos[0],hit_pos[1],hit_pos[2]);
//         //p->Print("");            
//     }
//     else if (spec == 's') 
//     {
//         static Int_t nr = 0; 
//         static Double_t rsum = 0;
//         static Double_t rsum2 = 0;
//         static Double_t dd = -1.;
//         
//         static Double_t xsum2[3] = {0,0,0};
//         static Double_t xsum[3]  = {0,0,0};
//         static Double_t xd[3]    = {-1,-1,-1};
//   
//         Double_t r2 = (v.X()-hit_pos[0])*(v.X()-hit_pos[0])+
//                       (v.Y()-hit_pos[1])*(v.Y()-hit_pos[1])+
//                       (v.Z()-hit_pos[2])*(v.Z()-hit_pos[2]);
//         Double_t r = TMath::Sqrt(r2);
//         
//         Double_t du = TMath::Abs(hit->GetHitU()-hit->GetPointU());
//         Double_t dv = TMath::Abs(hit->GetHitV()-hit->GetPointV());
//         Double_t dn = TMath::Abs(hit->GetHitN()-hit->GetPointN());
//         
//         nr++;
//         
//         rsum2 += r2;
//         rsum  += r;
//         if (r > dd) dd = r;
//        
//         xsum[0] += du;
//         xsum[1] += dv;
//         xsum[2] += dv;
//         
//         xsum2[0] += du*du;
//         xsum2[1] += dv*dv;
//         xsum2[2] += dn*dn;
//         
//         if (du > xd[0]) xd[0] = du;
//         if (dv > xd[1]) xd[1] = dv;
//         if (dn > xd[2]) xd[2] = dn;
//          
//         cout << "[ITS] specifity: " << spec
//              << "  diff. distance (current, max, ave) = " << r*1e4 
//              << "  " << dd*1e4 << "  " << 1e4*rsum/nr << " [mkm] " 
//              << " sigma = " << 1e4*TMath::Sqrt(TMath::Abs(rsum2/nr-rsum*rsum/(nr*nr))) << " [mkm] " 
//              << "\n\t (du,dv,dn) = " << du*1e4 << ", " << dv*1e4 << ", " << dn*1e4 << " [mkm] "
//              << "  max(du,dv,dn) = " << xd[0]*1e4 << "," << xd[1]*1e4 << "," << xd[2]*1e4 << " [mkm] "
//              << " sigma(du,dv,dn) = " << 1e4*TMath::Sqrt(TMath::Abs(xsum2[0]/nr-xsum[0]*xsum[0]/(nr*nr)))
//                               << ", " << 1e4*TMath::Sqrt(TMath::Abs(xsum2[1]/nr-xsum[1]*xsum[1]/(nr*nr)))
//                               << ", " << 1e4*TMath::Sqrt(TMath::Abs(xsum2[2]/nr-xsum[2]*xsum[2]/(nr*nr)))
//              << endl;
//              
//         //printf("\t its point (1): %14.8f  %14.8f  %14.8f\n",v.X(),v.Y(),v.Z());
//         //printf("\t its point (2): %14.8f  %14.8f  %14.8f\n",hit_pos[0],hit_pos[1],hit_pos[2]);
//         //cout << hit->GetHitN() << " " << hit->GetPointN() << endl;
//     }
//     
//     //p->Print(""); 
//     //hit->PrintHit();
//        
//     return hit;   
}

//________________________________________________________________________________________
SpdIdealHit* SpdIdealTrackFinder::CreateTsTBHit(FairMCPoint* point, ITF_info* info)
{
    info->htype = kWirepoint;
    
    SpdTsTBPoint* p = dynamic_cast<SpdTsTBPoint*>(point);
    if (!p) return 0;
    
    SpdGeoBuilder* builder = info->builder;
    if (!builder) return 0;
    
    Long_t id = p->GetDetectorTID();
    
    if (!builder->SetNodePath(id)) return 0;

    SpdIdealWirepointHit* hit = new SpdIdealWirepointHit();
    
    hit->SetSpecifity('w');
    
    hit->SetDetId(1,id);
    hit->SetDetId(2,0); // barrel
    
    Double_t x[3], s, t, rmax, zmax, resR, resZ, R(0), Z(0);
    TVector3 pos, dir, size, res, Lr, Lz;
    
    // --- detector 
    if (builder->GetDetectorPosition(x)) pos.SetXYZ(x[0],x[1],x[2]);
    if (builder->GetDetectorOrientation(x,'z')) dir.SetXYZ(x[0],x[1],x[2]);
    if (builder->GetNodeSize(x)) size.SetXYZ(x[0],x[1],x[2]);
    
    dir.SetMag(1);
    
    hit->SetWireMaxRadius(size.X());
    hit->SetWire(pos,dir,size.Z());
    
    rmax = hit->GetWireMaxRadius();
    zmax = hit->GetWireLength();
    
    // ---
    p->GetPosTime(pos,t);        // point position and time
    s = p->GetPointDir(dir);     // s = segment length, dir = track direction in the point
    
    // --- point 
    hit->SetPoint(pos);          // position
    hit->SetDirInPoint(dir,s);   // track direction in the point, segment length
  
    // --- hit resolution
    hit->SetResolution(GetHitResolution(kSpdTsTB,hit->GetSpecifity()));
    
    // --- hit time & position
    hit->SetTimeStamp(t);  // time
    hit->SetHit(pos);      // position FIXME
    
    //--------------------------------------------
    // ATTENTION >>>> create hit position >>>>  ATTENTION
    resR = hit->GetResolutionR();
    resZ = hit->GetResolutionZ();
    
    Lr = hit->GetDriftDirR(); 
    Lz = hit->GetWireDirZ();
    
    R = Lr.Mag();
    Z = Lz.Mag();
    
    Double_t ktol = 0.99999;
    if (resR > 0) {
        if (resR < rmax/TMath::Sqrt(12)) {
            R += gRandom->Gaus(0,resR);
            if (R < 0) R = 0;
            else if (!(R < rmax)) R = ktol*rmax;
        }
        else R = gRandom->Uniform(0,ktol*rmax); 
        if (R > 0) Lr.SetMag(R);
    }
    
    if (resZ > 0) {
        if (resZ < zmax/TMath::Sqrt(12)) {
            Z += gRandom->Gaus(0,resZ);
            if (Z < 0) Z = 0;
            else if (!(Z < zmax)) Z = ktol*zmax;
        }
        else Z = gRandom->Uniform(0,ktol*zmax); 
        if (Z > 0) Lz.SetMag(Z);
    }
    
    pos = hit->GetWirePoint1();
    if (Z > 0) pos += Lz;
    if (R > 0) pos += Lr;
    //  ATTENTION <<<< create hit position <<<<<  ATTENTION
    //--------------------------------------------
    
    hit->SetHit(pos);  // new position FIXME
   
    // ATTENTION
    if (hit->GetRdrift() > rmax || 
        hit->GetZwire() > zmax ) {
        hit->SetSpecifity('n');
        fNFailedHits++;  
        //cout << "<TsTB> R drift = " << hit->GetRdrift() << " Max. R = " << rmax
        //     << " z = " << hit->GetZwire() << " Length = " << zmax << endl;
    } 
    
    //cout << "<TsTB> R drift = " << hit->GetRdrift() << " / " <<  hit->GetDriftDirR().Mag() 
    //     << " resolution (r,z): " << resR << "," << resZ << " R,Z = " << R << ", " << Z << "\n\n";
             
    //p->Print(""); 
    //hit->PrintHit();
    
    return hit;   
}

//________________________________________________________________________________________
SpdIdealHit* SpdIdealTrackFinder::CreateTsTECHit(FairMCPoint* point, ITF_info* info)
{
    info->htype = kWirepoint;
    
    SpdTsTECPoint* p = dynamic_cast<SpdTsTECPoint*>(point);
    if (!p) return 0;
    
    SpdGeoBuilder* builder = info->builder;
    if (!builder) return 0;
    
    Long_t id = p->GetDetectorTID();
    
    if (!builder->SetNodePath(id)) return 0;
        
    SpdIdealWirepointHit* hit = new SpdIdealWirepointHit();
    
    hit->SetSpecifity('w');
    
    hit->SetDetId(1,id);
    hit->SetDetId(2,1); // endcap 
  
    Double_t x[3], s, t, rmax, zmax, resR, resZ, R(0), Z(0);
    TVector3 pos, dir, size, res, Lr, Lz;
    
    // --- detector 
    if (builder->GetDetectorPosition(x)) pos.SetXYZ(x[0],x[1],x[2]);
    if (builder->GetDetectorOrientation(x,'z')) dir.SetXYZ(x[0],x[1],x[2]);
    if (builder->GetNodeSize(x)) size.SetXYZ(x[0],x[1],x[2]);
    
    hit->SetWireMaxRadius(size.X());
    hit->SetWire(pos,dir,size.Z());
    
    rmax = hit->GetWireMaxRadius();
    zmax = hit->GetWireLength();
    
    // ---
    p->GetPosTime(pos,t);        // point position and time
    s = p->GetPointDir(dir);     // s = segment length, dir = track direction in the point
    
    // --- point 
    hit->SetPoint(pos);          // position 
    hit->SetDirInPoint(dir,s);   // track direction in the point, segment length

    // --- hit resolution
    hit->SetResolution(GetHitResolution(kSpdTsTEC,hit->GetSpecifity()));
    
    // --- hit time & position
    hit->SetTimeStamp(t);        // time
    hit->SetHit(pos);            // position FIXME
  
    //--------------------------------------------
    // ATTENTION >>>> create hit position >>>>  ATTENTION
    resR = hit->GetResolutionR();
    resZ = hit->GetResolutionZ();
    
    Lr = hit->GetDriftDirR(); 
    Lz = hit->GetWireDirZ();
    
    R = Lr.Mag();
    Z = Lz.Mag();
    
    Double_t ktol = 0.99999;
    if (resR > 0) {
        if (resR < rmax/TMath::Sqrt(12)) {
            R += gRandom->Gaus(0,resR);
            if (R < 0) R = 0;
            else if (!(R < rmax)) R = ktol*rmax;
        }
        else R = gRandom->Uniform(0,ktol*rmax); 
        if (R > 0) Lr.SetMag(R);
    }
    
    if (resZ > 0) {
        if (resZ < zmax/TMath::Sqrt(12)) {
            Z += gRandom->Gaus(0,resZ);
            if (Z < 0) Z = 0;
            else if (!(Z < zmax)) Z = ktol*zmax;
        }
        else Z = gRandom->Uniform(0,ktol*zmax); 
        if (Z > 0) Lz.SetMag(Z);
    }
    
    pos = hit->GetWirePoint1();
    if (Z > 0) pos += Lz;
    if (R > 0) pos += Lr;
    //  ATTENTION <<<< create hit position <<<<<  ATTENTION
    //--------------------------------------------
    
    hit->SetHit(pos);  // new position FIXME

    // ATTENTION
    if (hit->GetRdrift() > hit->GetWireMaxRadius() || 
        hit->GetZwire() > hit->GetWireLength() ) {
        hit->SetSpecifity('n');
        fNFailedHits++;  
        //cout << "<TsTEC> R drift = " << hit->GetRdrift() << " Max. R = " << hit->GetWireMaxRadius() << " z = " 
        //     << hit->GetZwire() << " Length = " << hit->GetWireLength() << endl;
    }
             
    //p->Print(""); 
    //hit->PrintHit();
    
    return hit;   
}

//________________________________________________________________________________________
SpdIdealTrackFinder::ITF_info* SpdIdealTrackFinder::FindInfo(Int_t id) // protected
{
    std::map<Int_t,ITF_info*>::iterator it = fInputData.find(id);
    return (it != fInputData.end()) ? it->second : 0;
}

//________________________________________________________________________________________
void SpdIdealTrackFinder::Finish() 
{
    cout << "-I- <SpdIdealTrackFinder::Finish> errors: " << fNErrors 
         << "; failed hits: " << fNFailedHits << endl;
}

//________________________________________________________________________________________
void SpdIdealTrackFinder::PrintInput() const
{
    cout << "-I- <SpdIdealTrackFinder::PrintInput>" << endl;
}

//________________________________________________________________________________________
void SpdIdealTrackFinder::FillParametersIn(SpdBaseParSet* params)
{
    if (!params) return;
    
    params->SetParameter("IdealTrackFinder/MinMomentum",fMinMomentum[1]);
    params->SetParameter("IdealTrackFinder/MinGammaBeta",fMinGammaBeta[1]);
    params->SetParameter("IdealTrackFinder/MinVertexNHits",fMinVertexNHits[1]);
    params->SetParameter("IdealTrackFinder/MinTrackerNHits",fMinTrackerNHits[1]);
    
    if (!fTracksSelected.empty()) params->SetParameter("IdealTrackFinder/TrackSelection","selected");
    else {
        if (fPrimVertexOnly) params->SetParameter("IdealTrackFinder/TrackSelection","prim");
        else params->SetParameter("IdealTrackFinder/TrackSelection","all");
    }
}

//________________________________________________________________________________________
void SpdIdealTrackFinder::LoadParametersFrom(SpdBaseParSet* params)
{
    if (!params) return;
    
    Double_t p1;
    if (params->GetParameter("IdealTrackFinder/MinMomentum",p1))  SetMinMomentum(p1);
    if (params->GetParameter("IdealTrackFinder/MinGammaBeta",p1)) SetMinGammaBeta(p1);
    
    Int_t p2;
    if (params->GetParameter("IdealTrackFinder/MinVertexNHits",p2))  SetMinVertexNHits(p2);
    if (params->GetParameter("IdealTrackFinder/MinTrackerNHits",p2)) SetMinTrackerNHits(p2); 
}


