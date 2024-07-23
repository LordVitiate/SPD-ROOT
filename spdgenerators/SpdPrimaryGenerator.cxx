// $Id$
// Author: artur   2017/11/15


//_____________________________________________________________________________
//
// SpdPrimaryGenerator
//_____________________________________________________________________________

#include <TDatabasePDG.h>
#include <TParticle.h>
#include <TMCParticle.h>
#include <TRandom3.h>
#include <TMath.h>

#include "FairRuntimeDb.h"
#include "FairRunSim.h"

#include "SpdStack.h"  
#include "SpdMCEventHeader.h"
#include "SpdPrimaryGenerator.h"
#include "SpdPrimGenParSet.h"
#include "SpdPrimGenData.h"
#include "SpdPrimGenFactory.h"
#include "SpdGenerator.h"
#include "SpdDecayer.h"
#include "SpdPythia6Generator.h"
#include "SpdPythia8Generator.h"

#include <iostream>

using std::cout;
using std::endl;

TString SpdPrimaryGenerator::fPrimGenParSet = "PrimGenParSet";

void    SpdPrimaryGenerator::SetPrimParSet(TString parset) { fPrimGenParSet = parset; }
TString SpdPrimaryGenerator::GetPrimParSet() { return fPrimGenParSet; }

ClassImp(SpdPrimaryGenerator)

//_____________________________________________________________________________
SpdPrimaryGenerator::SpdPrimaryGenerator():
FairPrimaryGenerator("Primary Generator","Spd Primary Generator"),
fVerboseLevel(1),fFairMode(kFALSE),fNGenerators(0),
fExternalDecayer(0),
fInit(kFALSE),
fCurrentGenData(0),fCurrentTrackId(-1)
{
   //if (!gRandom) gRandom = new TRandom3();
   //fGen = gRandom;
   fGen = new TRandom3();
   //fGen->SetSeed(0);
}

//_____________________________________________________________________________
SpdPrimaryGenerator::SpdPrimaryGenerator(const char *name, const char *title):
FairPrimaryGenerator(name,title),
fVerboseLevel(1),fFairMode(kFALSE),fNGenerators(0),
fExternalDecayer(0),
fInit(kFALSE),
fCurrentGenData(0),fCurrentTrackId(-1)
{
   //if (!gRandom) gRandom = new TRandom3();
   //fGen = gRandom;
   fGen = new TRandom3();
   //fGen->SetSeed(0);
}

//_____________________________________________________________________________
SpdPrimaryGenerator::~SpdPrimaryGenerator() 
{

}

//____________________________________________________________________________
void SpdPrimaryGenerator::Reset()
{
  fFairMode = kFALSE;
  
  fNGenerators = 0;
  
  fInit = kFALSE;
  
  fCurrentGenData = 0;
  fCurrentTrackId = -1;
  
  /* Reset FairPrimaryGenerator memebers */
  
  fBeamX0 = 0.;
  fBeamY0 = 0.;
  fBeamSigmaX = 0.;
  fBeamSigmaY = 0.;
  
  fTargetZ[0] = 0;
  fTargetDz = 0; 
  
  fVertex.SetXYZ(0., 0., 0.);
  
  fSmearVertexZ = kFALSE; 
  fSmearGausVertexZ = kFALSE;
  fSmearVertexXY = kFALSE;
  fSmearGausVertexXY = kFALSE; 
  
  fBeamAngleX0 = 0.;
  fBeamAngleY0 = 0.;
  fBeamAngleX  = 0.;
  fBeamAngleY  = 0.;
  fBeamAngleSigmaX = 0.;
  fBeamAngleSigmaY = 0.;
  
  fBeamDirection.SetXYZ(0., 0., 1.);
  
  fPhiMin = 0.;
  fPhiMax = 0.;
  fPhi = 0.;
  
  fBeamAngle = kFALSE; 
  fEventPlane = kFALSE;
   
  fGenList->Delete();
  fListIter->Reset();
    
  fNTracks = 0;
  fEventNr = 0;
  
  fMCIndexOffset = 0; 

  fdoTracking = kTRUE;
  
  if (fGenList && fGenList->GetEntriesFast() > 0) {
      fGenList->Delete();
  }
}

//_____________________________________________________________________________
Bool_t SpdPrimaryGenerator::Init() 
{
  if (fInit) return kTRUE;
  if (fVerboseLevel > -1) cout << "-I- <SpdPrimaryGenerator::Init>" << endl;
  
  if (!fGenList) return fInit; // return kFALSE
  
  Int_t ngen = fGenList->GetEntries();
  if (ngen == 0) return fInit; // return kFALSE
 
  FairGenerator* genf;
  SpdGenerator*  gen;
   
  /* ATTENTION Check initialization mode ATTENTION */

  Int_t nspdgen = 0;
  for (Int_t i = 0; i < ngen; i++) { 
       genf = (FairGenerator*)fGenList->At(i);
       if (genf) {
           gen = dynamic_cast<SpdGenerator*>(genf);
           if (gen) nspdgen++;
       }
  }
  
  if (!fFairMode && nspdgen != ngen) {
      cout << "-W- <SpdPrimaryGenerator::Init> Fair generators are detected (spd/fair): "
           << nspdgen << "/" << ngen-nspdgen 
           << "\n\t\t\t in the list of generators, fair mode is assigned" << endl;
      fFairMode = kTRUE;     
  }
  
  /* ATTENTION Set decayer ATTENTION */
 
  if (!fExternalDecayer) fExternalDecayer = SpdDecayer::Decayer();
  if (fExternalDecayer) {
      if (!fExternalDecayer->GetEventHeader()) fExternalDecayer->SetEventHeader(fEvent);
  }
        
  /* ATTENTION Init generators ATTENTION */
   
  fNGenerators = 0;
   
  if (fFairMode) {
      fInit = FairPrimaryGenerator::Init();
      for (Int_t i = 0; i < ngen; i++) {
           genf = (FairGenerator*)fGenList->At(i);
           if (genf) {
               if (!genf->Init()) {
                   cout << "-E- <SpdPrimaryGenerator::Initialize> Initialization failed" 
                        << " for generator (type/name): " 
                        << genf->ClassName() << "/" << genf->GetName() << endl;
                   return fInit; // return kFALSE
               }   
           }
      }
  }
  else {
      for (Int_t i = 0; i < ngen; i++) {
           gen = dynamic_cast<SpdGenerator*>(fGenList->At(i));
           if (gen) {
               gen->SetGenIndex(++fNGenerators);   
               if (!gen->Init()) {
                   cout << "-E- <SpdPrimaryGenerator::Initialize> Initialization failed" 
                        << " for generator (type/index): " 
                        << gen->ClassName() << "/" << gen->GetGenIndex() << endl;
                   return fInit;  // return kFALSE
               } 
           }
      }
  }
 
  /* ATTENTION Init decayer ATTENTION */
  
  if (fExternalDecayer && !fExternalDecayer->IsInit()) fExternalDecayer->Init();
  
  /* ATTENTION Save parameters ATTENTION */
   
  SpdPrimGenParSet* pars = GetParameters();
  
  if (pars) {
    
      FillParsIn(pars);
      
      if (fExternalDecayer) fExternalDecayer->FillParsIn(pars);
      
      FairRunSim* run = FairRunSim::Instance();
    
      if (run) {
          pars->setInputVersion(run->GetRunId(),1);
          pars->setChanged();
      }
  }
    
  fInit = kTRUE;
 
  return fInit;
}

//_____________________________________________________________________________
void SpdPrimaryGenerator::SetVerboseLevel(Int_t level)
{
  if (!fGenList) return;
  if (fGenList->GetEntriesFast() == 0) return;
    
  SpdGenerator* gen = 0;
 
  Bool_t init = kTRUE;
  for (Int_t i = 0; i < fGenList->GetEntries(); i++) {
       gen = dynamic_cast<SpdGenerator*>(fGenList->At(i));
       if (gen) gen->SetVerboseLevel(level);
  }
}

//_____________________________________________________________________________
Bool_t SpdPrimaryGenerator::GenerateEvent(FairGenericStack *pStack) 
{
  if (!fInit) Init();
  
  // Check for MCEventHeader
  if (!fEvent) {
      cout << "-F- <SpdPrimaryGenerator::ReadEvent> No MCEventHeader " << endl;
      return kFALSE;
  } 
  
  SpdMCEventHeader* eheader = dynamic_cast<SpdMCEventHeader*>(fEvent);
  (eheader) ? eheader->Reset() : fEvent->Reset();
  
  fCurrentTrackId = -1;
  
  SpdGenerator::ResetTracksCounter();
  
  // Save primary generator data 
  FillPrimGenData();
  
  /* ATTENTION Save decayer data ATTENTION */
  if (fExternalDecayer) fExternalDecayer->FillEventData();
  
  if (fFairMode) return FairPrimaryGenerator::GenerateEvent(pStack);
      
  fStack = pStack;
  fNTracks = 0;
  fCurrentGenData = 0;
    
  // Create event vertex (!disable multitarget option!)
  
  Int_t ntargets = fNrTargets;
  fNrTargets = 1; 
  
  MakeVertex();
  
  fNrTargets = ntargets;
  
  fEvent->SetVertex(fVertex);

  // Create beam angle
  if (fBeamAngle) MakeBeamAngle();

  // Create event plane
  if (fEventPlane) MakeEventPlane();
  
  /* Call the ReadEvent methods from all registered generators */
  
  SpdGenerator* gen = 0;
  for (Int_t i = 0; i < fGenList->GetEntries(); i++) {
    
       gen = dynamic_cast<SpdGenerator*>(fGenList->At(i));
       if (gen) {
             
           // number of tracks before generator is called
                   
           if (!fCurrentGenData) fMCIndexOffset = 0;
           else fMCIndexOffset += fCurrentGenData->GetNParticles();
           
           if (eheader) {
               fCurrentGenData = eheader->AddGenData(gen->ClassName());
               fCurrentGenData->SetGeneratorIndex(gen->GetGenIndex());
               gen->SetGenData(fCurrentGenData); 
           }
         
           if (!gen->ReadEvent(this)) {
               cout << "-E- <SpdPrimaryGenerator::ReadEvent> Failed for generator " << gen->GetName() << endl;
               return kFALSE;
           }          
       }
  }

  fTotPrim += fNTracks;
  
  // Set the event number if not set already by one of the dedicated generators
  if (fEvent->GetEventID() == -1) {
      fEventNr++;
      fEvent->SetEventID(fEventNr);
  }
  
  if (fVerboseLevel > -1) {
      cout << "-I- <SpdPrimaryGenerator::ReadEvent> Event: " << fEvent->GetEventID() 
           << "   Particles: " << fNTracks
           << "   Vertex: (" << fVertex.X() << ", "
           << fVertex.Y() << ", " << fVertex.Z() << ") "<< endl;
  }

  fEvent->SetNPrim(fNTracks);
  
  if (fVerboseLevel > 0) ((SpdMCEventHeader*)fEvent)->Print(1);
  if (fVerboseLevel > 1) ((SpdStack*)fStack)->PrintPrimaries();

  return kTRUE;
}

#if OLD_FAIRROOT_PRIMGEN_VERSION
//_____________________________________________________________________________
void SpdPrimaryGenerator::AddTrack(Int_t pdgid, 
                                   Double_t px_raw, Double_t py_raw, Double_t pz_raw,
                                   Double_t vx, Double_t vy, Double_t vz,
                                   Int_t secondParent, Bool_t wanttracking,
                                   Double_t e, Double_t tof, 
                                   Double_t weight)
{
  if (fFairMode) {
      FairPrimaryGenerator::AddTrack(pdgid, px_raw, py_raw, pz_raw,
                                     vx, vy, vz, secondParent, wanttracking,
                                     e, tof, weight);
      return;
  }
  
  AddTrackX(pdgid, px_raw, py_raw, pz_raw, vx, vy, vz, 
            secondParent, wanttracking, e, tof, weight);
}
#else
//_____________________________________________________________________________
void SpdPrimaryGenerator::AddTrack(Int_t pdgid, 
                                   Double_t px_raw, Double_t py_raw, Double_t pz_raw,
                                   Double_t vx, Double_t vy, Double_t vz,
                                   Int_t secondParent, Bool_t wanttracking,
                                   Double_t e, Double_t tof, 
                                   Double_t weight, TMCProcess proc) 
{
  if (fFairMode) {
      FairPrimaryGenerator::AddTrack(pdgid, px_raw, py_raw, pz_raw,
                                     vx, vy, vz, secondParent, wanttracking,
                                     e, tof, weight, proc);
      return;
  }
  
  AddTrackX(pdgid, px_raw, py_raw, pz_raw, vx, vy, vz, 
            secondParent, wanttracking, e, tof, weight, proc);
}
#endif   

//_____________________________________________________________________________
void SpdPrimaryGenerator::AddTrackX(Int_t pdgid, 
                                    Double_t px_raw, Double_t py_raw, Double_t pz_raw,
                                    Double_t vx, Double_t vy, Double_t vz,
                                    Int_t secondParent, Bool_t wanttracking,
                                    Double_t e, Double_t tof, 
                                    Double_t weight, TMCProcess proc) 
{
  // Add event vertex to track vertex
  
  vx += fVertex.X();
  vy += fVertex.Y();
  vz += fVertex.Z();

  TVector3 mom(px_raw, py_raw, pz_raw);

  // Rotate the track (event) by the randomly generated angle which is fixed 
  // for the complete event.
  
  if (fEventPlane) mom.RotateZ(fPhi);
  
  // Rotate the track (event) from the rotated beam direction system into
  // the fixed global experiment coordinate system
  
  if (fBeamAngle) mom.RotateUz(fBeamDirection.Unit());

  // ATTENTION Convert K0 and K0_bar into K0S and K0L ATTENTION
  
  if (pdgid == 311 || pdgid == -311) 
  {
      Double_t test = fGen->Uniform(0.,1.);
      if (test >= 0.5) pdgid = 310; // K0S
      else pdgid = 130; // K0L
  }

  // Check whether particle type is in PDG Database
  
  TDatabasePDG *pdgBase = TDatabasePDG::Instance();
  if (!pdgBase) {    
      cout << "-F- <FairPrimaryGenerator::AddTrack> No TDatabasePDG instantiated " << endl;
      return;
  } 
  
  TParticlePDG *pdgPart = pdgBase->GetParticle(pdgid);
  
   if (!pdgPart) {
      if (e < 0) {
          cout << "-E- <FairPrimaryGenerator::AddTrack> PDG code " << pdgid 
               << " is not found in database. Discarding the particle. " << endl;
          return;
      } 
      else {
          cout << "-W- <FairPrimaryGenerator::AddTrack> PDG code " << pdgid
               << " not found in database. This warning can be safely ignored " << endl;
      }
  }
  
  // Get mass and calculate energy of particle
  
  if (e < 0) {
      Double_t mass = pdgBase->GetParticle(pdgid)->Mass();
      e = TMath::Sqrt(mom.Mag2() + mass * mass);
  } 

  //  Set all other parameters required by PushTrack
  
  //Int_t doTracking = 0; // Go to tracking
  //if (fdoTracking && wanttracking) doTracking = 1;
  
  Int_t doTracking = 1;
  
  Int_t       dummyparent = -1;               // Primary particle (now the value is -1 by default)
  Double_t    polx(0.), poly(0.), polz(0.);   // Polarisation
  Int_t       status = 0;                     // Generation status

  // Correct for tracks which are in list before generator is called
  
  if (secondParent != -1) secondParent += fMCIndexOffset;
 
  // Add track to stack
 
  fStack->PushTrack(doTracking, dummyparent, pdgid, mom.X(), mom.Y(), mom.Z(),
                    e, vx, vy, vz, tof, polx, poly, polz, proc, fCurrentTrackId,
                    weight, status, secondParent);
 
  //cout << "<SpdPrimaryGenerator::AddTrack> pdg/trackId: " << pdgid << "/" << fCurrentTrackId << endl;
   
  fNTracks++;
}

//_____________________________________________________________________________
void SpdPrimaryGenerator::AddPrimGenDataParticle(TParticle* part)
{
  if (!fEvent) return; 
  
  SpdMCEventHeader* eheader = dynamic_cast<SpdMCEventHeader*>(fEvent);
  if (!eheader) return;
  
  SpdPrimGenData* data = eheader->GetPrimData();
  if (!data) return;
  data->AddParticle(part);
}

//_____________________________________________________________________________
SpdGenerator* SpdPrimaryGenerator::Generator(Int_t index) const
{
  if (!fGenList) return 0;
  SpdGenerator* gen = dynamic_cast<SpdGenerator*>(fGenList->At(index-1));
  if (gen && index == gen->GetGenIndex()) return gen;
 
  FairGenerator* fgen;
  TIter next(fGenList);
  while ((fgen = (FairGenerator*)next())) {
      if (!fgen) continue;
      gen = dynamic_cast<SpdGenerator*>(fgen);
      if (gen && index == gen->GetGenIndex()) return gen;
  }
  
  return 0;
}

//_____________________________________________________________________________
Bool_t SpdPrimaryGenerator::FillParsIn(SpdPrimGenParSet* params)
{
  if (!params) return kFALSE;
  
  params->SetParameter("PrimaryGenerator",this->ClassName(),0);
  params->SetParameter("Name",fName,0);
  params->SetParameter("Title",fTitle,0);
  
  params->SetParameter("VerboseLevel",fVerboseLevel,0);
  
  params->SetParameter("Parameters",fPrimGenParSet,0);
  
  params->SetParameter("FairMode",fFairMode,0);
    
  params->SetParameter("NGenerators",fNGenerators,0);
     
  params->SetParameter("SmearVertexZ",fSmearVertexZ,0);
  params->SetParameter("SmearGausVertexZ",fSmearGausVertexZ,0);
  params->SetParameter("SmearVertexXY",fSmearVertexXY,0);
  params->SetParameter("SmearGausVertexXY",fSmearGausVertexXY,0);
  
  params->SetParameter("BeamAngle",fBeamAngle,0);
  params->SetParameter("EventPlane",fEventPlane,0);
    
  if (fSmearVertexZ || fSmearGausVertexZ) 
  {
      params->SetParameter("Vertex.TargetZ0.cm",fTargetZ[0],0);
      params->SetParameter("Vertex.TargetDz.cm",fTargetDz,0);
  }
  
  if (fSmearVertexXY || fSmearGausVertexXY) 
  {
      params->SetParameter("Vertex.BeamX0.cm",fBeamX0,0);
      params->SetParameter("Vertex.BeamY0.cm",fBeamY0,0);
      params->SetParameter("Vertex.BeamSigmaX.cm",fBeamSigmaX,0);
      params->SetParameter("Vertex.BeamSigmaY.cm",fBeamSigmaY,0);
  } 
    
  if (fBeamAngle) {
      params->SetParameter("Angle.BeamAngleX0.rad",fBeamAngleX0,0);
      params->SetParameter("Angle.BeamAngleY0.rad",fBeamAngleY0,0);
      params->SetParameter("Angle.BeamAngleSigmaX.rad",fBeamAngleSigmaX,0);
      params->SetParameter("Angle.BeamAngleSigmaY.rad",fBeamAngleSigmaY,0);
  }
  
  if (fEventPlane) {
      params->SetParameter("Angle.PhiMin.rad",fPhiMin,0);
      params->SetParameter("Angle.PhiMax.rad",fPhiMax,0);
  }
  
  SpdGenerator* gen;
  for (Int_t i = 0; i < fNGenerators; i++) {
      gen = dynamic_cast<SpdGenerator*>(fGenList->At(i));
      if (!gen) continue;
      params->SetParameter(Form("Generator_%d",gen->GetGenIndex()),gen->ClassName(),0);       
      params->SetParameter(Form("Generator_%d.name",gen->GetGenIndex()),gen->GetName(),0);      
      gen->FillParsIn(params);
  }
  
  // ATTENTION register external decayer storage index ATTENTION
  
  if (fExternalDecayer) params->SetParameter("DecayerStorageIndex",
                                             fExternalDecayer->GetStorageIndex(),0);

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdPrimaryGenerator::LoadParsFrom(SpdPrimGenParSet* params)
{
  Reset();
  
  if (!params) return kFALSE;
  
  TString parameters;
  
  params->GetParameter("Parameters",parameters,0);
  
  if (parameters != fPrimGenParSet) {
      cout << "-W- <SpdPrimaryGenerator::LoadParsFrom> "
           << " Change current parameters type " << fPrimGenParSet 
           << " to " << parameters << endl;
      fPrimGenParSet = parameters;
  }
  
  params->GetParameter("Name",fName,0);
  params->GetParameter("Title",fTitle,0);
  
  params->GetParameter("VerboseLevel",fVerboseLevel,0);
  
  Bool_t fmode;
  params->GetParameter("FairMode",fmode,0);
  
  SetFairMode(fmode);
      
  Double_t v[4];
   
  Bool_t aSmearVertexZ(kFALSE), aSmearGausVertexZ(kFALSE);
  
  params->GetParameter("SmearVertexZ",aSmearVertexZ,0);
  params->GetParameter("SmearGausVertexZ",aSmearGausVertexZ,0);
   
  SmearVertexZ(aSmearVertexZ);
  SmearGausVertexZ(aSmearGausVertexZ);
  
  if (aSmearVertexZ || aSmearGausVertexZ) 
  {
      params->GetParameter("Vertex.TargetZ0.cm",v[0],0);
      params->GetParameter("Vertex.TargetDz.cm",v[1],0);
      
      SetTarget(v[0],v[1]);
  }
  
  Bool_t aSmearVertexXY(kFALSE), aSmearGausVertexXY(kFALSE);
   
  params->GetParameter("SmearVertexXY",aSmearVertexXY,0);
  params->GetParameter("SmearGausVertexXY",aSmearGausVertexXY,0);
  
  SmearVertexXY(aSmearVertexXY);
  SmearGausVertexXY(aSmearGausVertexXY);
  
  if (fSmearVertexXY || fSmearGausVertexXY) 
  {
      params->GetParameter("Vertex.BeamX0.cm",v[0],0);
      params->GetParameter("Vertex.BeamY0.cm",v[1],0);
      params->GetParameter("Vertex.BeamSigmaX.cm",v[2],0);
      params->GetParameter("Vertex.BeamSigmaY.cm",v[3],0);
      
      SetBeam(v[0],v[1],v[2],v[3]);
  } 
    
  Bool_t BeamAngle = kFALSE;   
  params->GetParameter("BeamAngle",BeamAngle,0);
    
  if (BeamAngle) {
      params->GetParameter("Angle.BeamAngleX0.rad",v[0],0);
      params->GetParameter("Angle.BeamAngleY0.rad",v[1],0);
      params->GetParameter("Angle.BeamAngleSigmaX.rad",v[2],0);
      params->GetParameter("Angle.BeamAngleSigmaY.rad",v[3],0);
      
      SetBeamAngle(v[0],v[1],v[2],v[3]);
  }
  
  Bool_t EventPlane = kFALSE;
  params->GetParameter("EventPlane",EventPlane,0);
  
  if (EventPlane) {
      params->GetParameter("Angle.PhiMin.rad",v[0],0);
      params->GetParameter("Angle.PhiMax.rad",v[1],0);
      
      SetEventPlane(v[0],v[1]);
  }
  
  Int_t NGenerators;
  params->GetParameter("NGenerators",NGenerators,0);
  
  if (NGenerators < 1) return kTRUE;
  
  SpdPrimGenFactory* genfact = SpdPrimGenFactory::Instance();
  
  SpdGenerator* gen = 0;
  TString gentype, genname;
  
  for (Int_t i = 1; i <= NGenerators; i++) {
    
      if (!params->GetParameter(Form("Generator_%d",i),gentype,0)) continue;      
      if (!params->GetParameter(Form("Generator_%d.name",i),genname,0)) continue;   
      
      gen = genfact->CreateGenerator(gentype, genname);
   
      if (gen) {
          
          gen->SetGenIndex(i);
          
          if (gen->LoadParsFrom(params)) AddGenerator(gen);
          else {
              cout << "-F- <SpdPrimaryGenerator::LoadParsFrom> "
                   << "Intialization failed for generator (type/name): "
                   << gentype << "/" << genname << endl;
              delete gen;     
              exit(1);
          }
      }
      else {
          cout << "-F- <SpdPrimaryGenerator::LoadParsFrom> Unknown generator type: "
               << gentype << endl;
          exit(1);    
      }
  }
  
  return kTRUE;
}

//_____________________________________________________________________________
void SpdPrimaryGenerator::LoadPythia6Parameters(SpdPrimGenParSet* params)
{
  if (!params) return;
    
  SpdGenerator* gen;
  SpdPythia6Generator* sgen;
  
  TIter next(fGenList);
  while ((gen = (SpdGenerator*)next())) {
      if (!gen) continue;
      sgen = dynamic_cast<SpdPythia6Generator*>(gen);
      if (!sgen) continue;
      cout << "-I- <SpdPrimaryGenerator::LoadPyhia6Parameters> "
           << "Pythia6Generator with index: " << sgen->GetGenIndex() 
           << " was found " << endl;     
      SpdPythia6* pythia6 = sgen->GetGenerator();
      if (pythia6->GetParSetNumber() != 0) {
          cout << "-W- <SpdPrimaryGenerator::LoadPyhia6Parameters> "
               << "Some settings were alredy performed "
               << "(current set number: " << pythia6->GetParSetNumber() << ") "<< endl;
      }
      pythia6->SetStorageIndex(sgen->GetGenIndex());
      pythia6->LoadParsFrom(params); // load parameters & init Pythia6      
  }
}

//_____________________________________________________________________________
void SpdPrimaryGenerator::LoadPythia8Parameters(SpdPrimGenParSet* params)
{
  if (!params) return;
    
  SpdGenerator* gen;
  SpdPythia8Generator* sgen;
  
  TIter next(fGenList);
  while ((gen = (SpdGenerator*)next())) {
      if (!gen) continue;
      sgen = dynamic_cast<SpdPythia8Generator*>(gen);
      if (!sgen) continue;
      cout << "-I- <SpdPrimaryGenerator::LoadPyhia8Parameters> "
           << "Pythia6Generator with index: " << sgen->GetGenIndex() 
           << " was found " << endl;     
      SpdPythia8* pythia8 = sgen->GetGenerator();
//       if (pythia8->GetParSetNumber() != 0) {
//           cout << "-W- <SpdPrimaryGenerator::LoadPyhia8Parameters> "
//                << "Some settings were alredy performed "
//                << "(current set number: " << pythia8->GetParSetNumber() << ") "<< endl;
//       }
      pythia8->SetStorageIndex(sgen->GetGenIndex());
      pythia8->LoadParsFrom(params); // load parameters & init Pythia6      
  }
}

//_____________________________________________________________________________
void SpdPrimaryGenerator::FillPrimGenData()
{
  if (!fEvent) return;
   
  SpdMCEventHeader* eheader = dynamic_cast<SpdMCEventHeader*>(fEvent);
  if (!eheader) return;
 
  SpdPrimGenData* data = eheader->GetPrimData();
  if (!data) return;
  
  std::vector<UInt_t>& seeds = data->GetGenSeeds();
  
  (fGen) ? seeds.push_back(fGen->GetSeed()) : seeds.push_back(0);
}

//_____________________________________________________________________________
SpdPrimGenParSet* SpdPrimaryGenerator::GetParameters()
{
  FairRun* run = FairRun::Instance();
  if (!run) return 0;
  
  FairRuntimeDb* rtdb = run->GetRuntimeDb(); 
  if (!rtdb) return 0;
  
  SpdPrimGenParSet* pars = (SpdPrimGenParSet*)(rtdb->getContainer(fPrimGenParSet));
  
  return pars;
}

//_____________________________________________________________________________
void SpdPrimaryGenerator::Print(Option_t* opt) const
{
  cout << "<SpdPrimaryGenerator::Print>\n" << endl;
  
  cout << "Name/Title:        " << GetName() << " / " << GetTitle() << endl;
  cout << "Parameters:        " << fPrimGenParSet << endl;
  cout << "Is Init:           " << fInit << endl; 
  cout << "Fair mode:         " << fFairMode << endl;
  cout << "Seeds:             " << fGen->GetSeed() << endl;
  cout << "Generators:        " << fNGenerators << endl;
  
  if (fEvent) cout << "Event header:      " << "yes" << endl;  
  else cout << "Event header:      " << "no" << endl; 
 
  cout << endl;
  
  cout << "SmearVertexXY:     " << fSmearVertexXY << endl;
  cout << "SmearGausVertexXY: " << fSmearGausVertexXY << endl;
  cout << "BeamAngle:         " << fBeamAngle << endl;
  cout << "EventPlane:        " << fEventPlane << endl;
  cout << "SmearVertexXY:     " << fSmearVertexXY << endl;
  cout << "SmearGausVertexXY: " << fSmearGausVertexXY << endl;
  
  cout << endl;
  
  if (fExternalDecayer) fExternalDecayer->Print();
  else cout << "External decayer:  " << "no" << "\n\n"; 
   
  if (!fGenList) return;
  
  SpdGenerator* gen;
  for (Int_t i = 0; i < fGenList->GetEntriesFast(); i++) {
      gen = dynamic_cast<SpdGenerator*>(fGenList->At(i));
      if (gen) gen->Print("");
  }
  
  cout << endl;
}





