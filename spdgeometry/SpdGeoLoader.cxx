// $Id$
// Author: artur   2019/09/16


//_____________________________________________________________________________
//
// SpdGeoLoader
//_____________________________________________________________________________

#include <TGeoManager.h>
#include <TSystem.h>
#include <TFile.h>

#include "FairBaseParSet.h"
#include "SpdPassiveGeoParSet.h"

#include "SpdGeoLoader.h"
#include "SpdGeoFactory.h"
#include "SpdCommonGeoMapper.h"

#include "SpdPassives.hh"
#include "SpdDetector.h"

#include <iostream>

using std::cout;
using std::endl;

TString SpdGeoLoader::fTopGeoFile = "";
TString SpdGeoLoader::fMediaFile  = "";
Bool_t  SpdGeoLoader::fUnsetMaterials = false;
Bool_t  SpdGeoLoader::fResetMediaFromPars = true;
Bool_t  SpdGeoLoader::fResetPassivesMedia = false;

//------------------------------------------------------ 
void SpdGeoLoader::ForceTopGeoFile(TString fname) { fTopGeoFile = fname; }
//------------------------------------------------------ 
void SpdGeoLoader::ForceMediaFile(TString fname) { fMediaFile = fname; }
//------------------------------------------------------
void SpdGeoLoader::UnsetMaterials(Bool_t m) { fUnsetMaterials = m; }
//------------------------------------------------------ 
void SpdGeoLoader::ResetMediaFromParams(Bool_t r) { fResetMediaFromPars = r; } 
//------------------------------------------------------ 
void SpdGeoLoader::ResetPassivesMedia(Bool_t r) { fResetPassivesMedia = r; }

SpdGeoLoader* SpdGeoLoader::fInstance = 0;

ClassImp(SpdGeoLoader)

//_____________________________________________________________________________
SpdGeoLoader::SpdGeoLoader():fIsInit(false),fLockGeom(false),fParFile(0)
{
    if (fInstance) { 
        cout << "-F- <SpdGeoLoader::SpdGeoLoader> Fatal error. " 
             << "Singleton object has already been created." << endl;
        exit;
    }
    fInstance = this;
}

//_____________________________________________________________________________
SpdGeoLoader::~SpdGeoLoader() 
{
   if (fParFile) {
       fParFile->Close();
       delete fParFile;
       fParFile = 0;
   }
   
   fTopGeoFile = "";
   fMediaFile  = "";
   fUnsetMaterials = false;
   fResetMediaFromPars = true;
   fResetPassivesMedia = false;

   fInstance = 0;
}

//_____________________________________________________________________________
Bool_t SpdGeoLoader::LoadGeometry()
{         
   if (fIsInit) {
       cout << "-W- <SpdGeoLoader::LoadGeometry> Geometry is already initialized" << endl;
       return fIsInit;
   }
   
   Int_t nmodules = 0;
   nmodules += SpdCommonGeoMapper::Instance()->GetNDetectors();
   nmodules += SpdCommonGeoMapper::Instance()->GetNPassives();
   
   if (nmodules == 0) {
       cout << "-W- <SpdGeoLoader::LoadBaseGeometry> No modules have been loaded " << endl;
       return fIsInit;
   }
   
   cout << "-I- <SpdGeoLoader::LoadGeometry> Load SPD geometry; number of modules: " << nmodules << endl;
 
   if (gGeoManager) delete gGeoManager; /*!ATTENTION! DELETE CURRENT GEOMETRY !ATTENTION!*/
   
   SpdPassiveGeoParSet* paspars = GetPassiveParameters();
   
   if (!fMediaFile.IsWhitespace()) SpdCommonGeoMapper::Instance()->OpenGeometry(fMediaFile);
   else {
       if (paspars->GetParameter("Global/MediaFileName",fMediaFile)) SpdCommonGeoMapper::Instance()->OpenGeometry(fMediaFile);
       else SpdCommonGeoMapper::Instance()->OpenGeometry();
   }
   fMediaFile = SpdCommonGeoMapper::Instance()->GetActualMediaFileName();
   
   SpdCave* Cave = new SpdCave();
   Cave->LoadParsFrom(paspars);
   if (!fTopGeoFile.IsWhitespace()) Cave->SetGeometryFileName(fTopGeoFile);
   fTopGeoFile = Cave->GetActualGeometryFileName();
   Cave->ConstructGeometry();
   
   Int_t nmod, nm(0);
   
   printf("--------------------------------------------------\n");
   printf("%5s | %5s | \"%s\"\n","N","Id","Name");
   printf("--------------------------------------------------\n");
    
   // searching for list of passives 
   SpdPassiveModule** mod = SpdCommonGeoMapper::Instance()->GetListOfPassives(nmod);
   for (Int_t i(0); i<nmod; i++) {
        fModulesId.insert(std::pair<TString,Int_t>(mod[i]->GetName(),mod[i]->GetId()));  
        printf("%5d | %5d | \"%s\"\n",++nm,mod[i]->GetId(),mod[i]->GetName());
   } 
   delete [] mod;
   
   // searching for list of detectors
   SpdDetector** det = SpdCommonGeoMapper::Instance()->GetListOfDetectors(nmod);
   for (Int_t i(0); i<nmod; i++) {
        fModulesId.insert(std::pair<TString,Int_t>(det[i]->GetName(),det[i]->GetDetId()));
        printf("%5d | %5d | \"%s\"\n",++nm,det[i]->GetDetId(),det[i]->GetName());
   }
   delete [] det;
   
   printf("--------------------------------------------------\n");
   
   fIsInit = kTRUE;
   
   return fIsInit;
}

//_____________________________________________________________________________
Bool_t SpdGeoLoader::LoadGeometry(TString parfile)
{
   if (fIsInit) {
       cout << "-W- <SpdGeoLoader::LoadGeometry> Geometry is already initialized" << endl;
       return fIsInit;
   }
    
   if (parfile.IsWhitespace()) {
       cout << "-W- <SpdGeoLoader::LoadGeometry> Geometry is not defined " << endl; 
       return fIsInit; 
   }
   
   if (gSystem->AccessPathName(parfile)) {
       cout << "-E- <SpdGeoLoader::LoadGeometry> File not found: " << parfile << endl;
       return fIsInit;
   }
   
   fParFile = new TFile(parfile);
   
   if (!fParFile) {
       cout << "-E- <SpdGeoLoader::LoadGeometry> File cannot be opened: " << parfile << endl;
       return fIsInit;
   }

   cout << "-I- <SpdGeoLoader::LoadGeometry> Load SPD geometry from: " << parfile << endl;
   
   if (gGeoManager) delete gGeoManager; /*!ATTENTION! DELETE CURRENT GEOMETRY !ATTENTION!*/
   
   SpdPassiveGeoParSet* paspars = GetPassiveParameters();
   
   if (!fMediaFile.IsWhitespace()) SpdCommonGeoMapper::Instance()->OpenGeometry(fMediaFile);
   else {
       if (paspars->GetParameter("Global/MediaFileName",fMediaFile)) SpdCommonGeoMapper::Instance()->OpenGeometry(fMediaFile);
       else SpdCommonGeoMapper::Instance()->OpenGeometry();
   }
   fMediaFile = SpdCommonGeoMapper::Instance()->GetActualMediaFileName();
   
   SpdCave* Cave = new SpdCave();
   Cave->LoadParsFrom(paspars);
   if (!fTopGeoFile.IsWhitespace()) Cave->SetGeometryFileName(fTopGeoFile);
   fTopGeoFile = Cave->GetActualGeometryFileName();
   Cave->ConstructGeometry();

   // load full set of modules (passive and active)
   FairBaseParSet* geoset = (FairBaseParSet*)fParFile->Get("FairBaseParSet");

   Int_t nmod, nm(0);
   
   printf("--------------------------------------------------\n");
   printf("%5s | %5s | \"%s\"\n","N","Id","Name");
   printf("--------------------------------------------------\n");
    
   // searching for list of passives 
   SpdPassiveModule** mod = SpdCommonGeoMapper::Instance()->GetListOfPassives(nmod);
   for (Int_t i(0); i<nmod; i++) {
        fModulesId.insert(std::pair<TString,Int_t>(mod[i]->GetName(),mod[i]->GetId()));  
        printf("%5d | %5d | \"%s\"\n",++nm,mod[i]->GetId(),mod[i]->GetName());
   } 
   delete [] mod;
   
   // searching for list of detectors
   SpdDetector** det = SpdCommonGeoMapper::Instance()->GetListOfDetectors(nmod);
   for (Int_t i(0); i<nmod; i++) {
        fModulesId.insert(std::pair<TString,Int_t>(det[i]->GetName(),det[i]->GetDetId()));
        printf("%5d | %5d | \"%s\"\n",++nm,det[i]->GetDetId(),det[i]->GetName());
   }
   delete [] det;
   
   printf("--------------------------------------------------\n");
   
   fIsInit = kTRUE;
   
   return fIsInit;
}

//_____________________________________________________________________________
 Bool_t SpdGeoLoader::LoadModule(Int_t id, Bool_t unsetmat)
{
   if (!fIsInit) {
       cout << "-W- <SpdGeoLoader::LoadModule> Geometry is not initialized" << endl;
       return kFALSE;
   }
   
   if (fLockGeom) {
       cout << "-W- <SpdGeoLoader::LoadModule> Geomery is locked, disable protection to add module " << endl;
       return kFALSE; 
   }
   
   if (id == Int_t(kSpdUndefined)) {
       cout << "-W- <SpdGeoLoader::LoadModule> Module is undefined" << endl;
       return kFALSE;
   }
       
   std::set<Int_t>::const_iterator it = fModules.find(id);
   
   if (it != fModules.end()) {
       cout << "-W- <SpdGeoLoader::LoadModule> Module of id = " << id << " is already built " << endl;
       return kTRUE;
   }
   
   FairModule* module = SpdCommonGeoMapper::Instance()->SearchForModule(id);
   if (!module) {
       cout << "-W- <SpdGeoLoader::LoadModule> No module of id = " << id << endl;
       return kFALSE;
   }

   if (SpdCommonGeoMapper::IsActive(id)) {
       SpdParSet* pars = GetActiveParameters(id);
       SpdDetector* active = (SpdDetector*)module;
       active->LoadParsFrom(pars); 
       SpdGeoMapper* mapper = active->GetMapper();
       if (fResetMediaFromPars) mapper->ResetMediaFromParams();
       if (unsetmat || fUnsetMaterials) mapper->UnsetMaterials(false,"base");
   }
   else {
       SpdPassiveGeoParSet* pars = GetPassiveParameters();
       SpdPassiveModule* passive = (SpdPassiveModule*)module;
       if (fResetPassivesMedia) passive->ResetMaterials();
       if (unsetmat || fUnsetMaterials) passive->UnsetMaterials("vacuum");
       passive->Print("");
   }

   cout << "-I- <SpdGeoLoader::LoadModule> Construct module of id: " << id << endl;
   
   module->ConstructGeometry();
   
   fModules.insert(id);
   
   return kTRUE;
}
 
//_____________________________________________________________________________
Bool_t SpdGeoLoader::LoadModule(TString name, Bool_t unsetmat)
{
   if (!fIsInit) {
       cout << "-W- <SpdGeoLoader::LoadModule> Geometry is not initialized" << endl;
       return kFALSE;
   }
   
   if (fLockGeom) {
       cout << "-W- <SpdGeoLoader::LoadModule> Geomery is locked, disable protection to add module " << endl;
       return kFALSE; 
   }
    
   std::map<TString,Int_t>::const_iterator it = fModulesId.find(name);
   if (it != fModulesId.end()) return LoadModule(it->second);
  
   TString mname = name;
   mname.ToLower();
   
   Bool_t load = kFALSE;
   
   if (mname == "all" || mname == "total" || mname == "passives" || mname == "passive") {
       Int_t nmod;
       SpdPassiveModule** mod = SpdCommonGeoMapper::Instance()->GetListOfPassives(nmod);
       for (Int_t i(0); i<nmod; i++) load = LoadModule(mod[i]->GetId(),unsetmat);
       delete [] mod;
   }
   
   if (mname == "all" || mname == "total" || mname == "actives" || mname == "active") {
       Int_t ndet;
       SpdDetector** det = SpdCommonGeoMapper::Instance()->GetListOfDetectors(ndet);
       for (Int_t i(0); i<ndet; i++) load = LoadModule(det[i]->GetDetId(),unsetmat);
       delete [] det;
   }
     
   if (!load) cout << "-I- <SpdGeoLoader::LoadModule> No module(-es): " << name << endl;
   
   return load;
}

//_____________________________________________________________________________
Int_t SpdGeoLoader::GetModuleId(TString mname) const
{
   std::map<TString,Int_t>::const_iterator it = fModulesId.find(mname);
   return (it != fModulesId.end()) ? it->second : Int_t(kSpdUndefined);
}

//_____________________________________________________________________________   
Bool_t SpdGeoLoader::IsGeometryModule(Int_t id) const 
{
   std::map<TString,Int_t>::const_iterator it = fModulesId.begin();
   for (; it != fModulesId.end(); it++) {
        if (it->second == id) return kTRUE;   
   }
   return kFALSE;
}

//_____________________________________________________________________________   
Bool_t SpdGeoLoader::IsModuleActual(Int_t id) const
{
   return (fModules.find(id) != fModules.end());
}
//_____________________________________________________________________________  
Bool_t SpdGeoLoader::IsModuleActive(Int_t id) const
{
   return SpdCommonGeoMapper::IsActive(id);  
}

//_____________________________________________________________________________  
SpdPassiveGeoParSet* SpdGeoLoader::GetPassiveParameters()
{
   TFile* g = gFile;
   
   SpdPassiveGeoParSet* pars  = 0;
   if (fParFile) {
       pars = (SpdPassiveGeoParSet*)fParFile->Get("PassiveGeoParSet");
   }
   else {
       FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
       pars = (SpdPassiveGeoParSet*)rtdb->getContainer("PassiveGeoParSet");
       ((FairParSet*)pars)->init();
   }

   gFile = g;
   if (g) g->cd();
       
   return pars; 
}

//_____________________________________________________________________________  
SpdParSet* SpdGeoLoader::GetActiveParameters(Int_t id)
{
   if (!fIsInit) {
       cout << "-W- <SpdGeoLoader::GetActiveParameters> Geometry is not initialized" << endl;
       return 0;
   }
   
   if (!IsGeometryModule(id)) {
       cout << "-W- <SpdGeoLoader::GetActiveParameters> Module of id = " << id << " is not actual " << endl;
       return 0;
   }
   
   if (SpdCommonGeoMapper::IsPassive(id)) {
       cout << "-W- <SpdGeoLoader::GetActiveParameters> This module is \"passive\": " << id << endl;
       return 0;
   }
   
   SpdDetector* det = SpdCommonGeoMapper::Instance()->SearchForActive(id);
   if (!det) {
       cout << "-W- <SpdGeoLoader::GetActiveParameters> No module of id = " << id << endl;
       return 0;
   }
   
   TFile* g = gFile;
   
   SpdParSet* pars = 0;
   
   if (fParFile) {
       pars = (SpdParSet*)fParFile->Get(det->GetParametersType());
   }
   else {
       FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
       pars = (SpdParSet*)rtdb->getContainer(det->GetParametersType());
       ((FairParSet*)pars)->init();
   }
   
   gFile = g;
   if (g) g->cd();  
   
   return pars;
}

//_____________________________________________________________________________
FairModule* SpdGeoLoader::GetModule(Int_t id)
{
   if (!IsModuleActual(id)) {
       cout << "-W- <SpdGeoLoader::GetModule> Module of id = " << id << " is not actual " << endl;
       return 0;
   }
   return SpdCommonGeoMapper::Instance()->SearchForModule(id);
}

//_____________________________________________________________________________
SpdDetector* SpdGeoLoader::GetActive(Int_t id)
{
   if (!IsModuleActual(id)) {
       cout << "-W- <SpdGeoLoader::GetActive> Module of id = " << id << " is not actual " << endl;
       return 0;
   }
   return SpdCommonGeoMapper::Instance()->SearchForActive(id);
}

//_____________________________________________________________________________
SpdPassiveModule* SpdGeoLoader::GetPassive(Int_t id)
{
   if (!IsModuleActual(id)) {
       cout << "-W- <SpdGeoLoader::GetPassive> Module of id = " << id << " is not actual " << endl;
       return 0;
   }
   return SpdCommonGeoMapper::Instance()->SearchForPassive(id);
}

//_____________________________________________________________________________
SpdGeoMapper* SpdGeoLoader::GetMapper(Int_t id)
{
   if (SpdCommonGeoMapper::IsPassive(id)) return 0;
   SpdDetector* det = (SpdDetector*)GetModule(id);
   return (det) ? det->GetMapper() : 0;
}

//_____________________________________________________________________________
SpdGeoBuilder* SpdGeoLoader::GetBuilder(Int_t id)
{
   if (SpdCommonGeoMapper::IsPassive(id)) return 0;
   SpdDetector* det = (SpdDetector*)GetModule(id);
   return (det) ? det->GetBuilder() : 0;
}

//_____________________________________________________________________________
void SpdGeoLoader::DrawGeometry(Int_t level, TString option)
{
   if (!gGeoManager) return;
   if (!gGeoManager->GetMasterVolume()) return;
   
   gGeoManager->SetVisLevel(level);
   
   if (option.IsWhitespace()) gGeoManager->GetMasterVolume()->Draw();
   else gGeoManager->GetMasterVolume()->Draw(option);
}

//_____________________________________________________________________________
void SpdGeoLoader::PrintGeometry() const
{
   cout << "-I- <SpdGeoLoader::PrintGeometry> Modules [actual/total]: " 
        << GetNActualModules()  << "/" << GetNTotalModules() << endl; 
        
   cout << endl;  
   cout << "Cave geometry:    " <<  fTopGeoFile << endl;
   cout << "Media file:        " << fMediaFile << endl; 
   cout << "Unset materials:  " << ((fUnsetMaterials) ? "true" : "false") << endl;
   cout << "Pars. materials:  " << ((fResetMediaFromPars) ? "true" : "false") << endl;
   cout << endl;
   
   if (GetNTotalModules() < 1) return;     
   
   printf("--------------------------------------------------\n");
   printf("%5s | %5s | \"%s\"\n","N","Id","Name");
   printf("--------------------------------------------------\n");
   
   Int_t n(0);
   std::map<TString,Int_t>::const_iterator it = fModulesId.begin();
   
   for (; it != fModulesId.end(); it++) {
        printf("%5d | %5d | \"%s\"\n",++n,it->second,it->first.Data());
   }
   printf("--------------------------------------------------\n");
}

//_____________________________________________________________________________   
void SpdGeoLoader::PrintActualGeometry() const
{
   cout << "-I- <SpdGeoLoader::PrintActualGeometry> Modules [actual/total]: " 
        << GetNActualModules() << "/" << GetNTotalModules() << endl; 

   cout << endl;  
   cout << "Cave geometry:      " << fTopGeoFile << endl;
   cout << "Media file:         " << fMediaFile << endl; 
   cout << "Is geometry locked: " << fLockGeom << endl;
   cout << "Unset materials:    " << ((fUnsetMaterials) ? "true" : "false") << endl;
   cout << "Reset materials:    " << ((fResetMediaFromPars) ? "true" : "false") << endl;
   cout << endl;
   
   printf("--------------------------------------------------\n");
   printf("%5s | %5s | \"%s\"\n","N","Id","Name");
   printf("--------------------------------------------------\n");
  
   if (GetNActualModules() < 1) return;     
    
   Int_t n(0);
   std::map<TString,Int_t>::const_iterator it = fModulesId.begin();
   
   for (; it != fModulesId.end(); it++) {
        if (!IsModuleActual(it->second)) continue;
        printf("%5d | %5d | \"%s\"\n",++n,it->second,it->first.Data());
   }
   printf("--------------------------------------------------\n");      
}


