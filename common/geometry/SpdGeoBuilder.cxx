// $Id$
// Author: artur   2018/05/07

//_____________________________________________________________________________
//
// SpdGeoBuilder
//_____________________________________________________________________________

#include "SpdGeoBuilder.h"

#include <TGeoManager.h>
#include <TGeoNavigator.h>
#include <TMath.h>
#include <TGeoBBox.h>

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdGeoBuilder)

//_____________________________________________________________________________
SpdGeoBuilder::SpdGeoBuilder():fMasterVolume(0),fLockGeometry(kFALSE),
fMaxBuildGeoLevel(100)
{
 
}

//_____________________________________________________________________________
SpdGeoBuilder::~SpdGeoBuilder() 
{
 
}

//_____________________________________________________________________________
void SpdGeoBuilder::SetMasterVolume(TGeoVolume* vol) 
{ 
   if (!fLockGeometry) fMasterVolume = vol; 
}

//_____________________________________________________________________________
Int_t SpdGeoBuilder::GetActualGeoLevel() const
{
   const SpdGeoTable* table = GetTable();
   if (!table) return 0;
   Int_t level = table->GetMaxGeoLevel();
   return (fMaxBuildGeoLevel < level) ? fMaxBuildGeoLevel : level;
}

//_____________________________________________________________________________
SpdGeoTable* SpdGeoBuilder::GetTable() const
{
   SpdGeoMapper* mapper = GetMapper();
   if (!mapper) return 0;
   return mapper->GetGeoTable();
}

//_____________________________________________________________________________
TGeoVolume* SpdGeoBuilder::GetVolume(TString volname)
{
   if (volname == fMasterVolume->GetName()) return fMasterVolume; 
   if (fVolumes.empty()) return 0;
   
   std::map<TString,TGeoVolume*>::const_iterator it = fVolumes.find(volname);
   
   TGeoVolume* vol = 0;
   if (it != fVolumes.end()) { vol = it->second; }
  
   return vol;
}

//_____________________________________________________________________________
TGeoMedium* SpdGeoBuilder::GetMedia(TString medname)
{
   if (!gGeoManager) return 0;
   
   TGeoMedium* med = 0;
   
   SpdGeoMapper* mapper = GetMapper();
   
   if (mapper) {
       TString umed = mapper->GetUnsetMedia();
       if (!umed.IsWhitespace()) {
           med = gGeoManager->GetMedium(umed);  
           if (med) return med;
       }
   }
   
   if (!medname.IsWhitespace()) {
       med = gGeoManager->GetMedium(medname);
       if (med) return med;
   }
  
   med = gGeoManager->GetMedium(0);
   if (med) return med;
  
   med = gGeoManager->GetMedium("xvacuum");
   if (med) return med;
   
   // create default
   
   if (!gGeoManager->GetMaterial("XVACUUM")) {
       gGeoManager->Material("XVACUUM", 1e-16,   0.,  0.000,  0,   0.,  0.); 
   }
   med = new TGeoMedium("xvacuum",0,gGeoManager->GetMaterial("XVACUUM"));
 
   return med;
}

//_____________________________________________________________________________
void SpdGeoBuilder::ConstructVolumes(const SpdGeoTable* geotable)
{
   if (!geotable) return;
   
   if (fLockGeometry) {
       cout << "<SpdGeoBuilder::ConstructVolumes> Geometry is locked" << endl;  
       return;
   }
   
   fVolumes.clear();
   
   const SpdGeoMapper* mapper = GetMapper();
   if (!mapper) return;
   
   Int_t n(geotable->GetMaxGeoLevel());
   Int_t nn;
   
   if (n > fMaxBuildGeoLevel) n = fMaxBuildGeoLevel;
        
   const SpdGeoTableRec* rec;
   const SpdGeoVolPars*  vpars;
   TString vname;
   TGeoVolume* vol;
   
   for (Int_t i(0); i < n; i++) {
        
        nn = geotable->GetNRec(i);
        
        for (Int_t j(0); j < nn; j++) 
        {
             rec = geotable->GetRec(i,j);
             vname = rec->GetVolName();
             vpars = mapper->GetVolPars(vname);
             
             if (!vpars) continue;
             
             if (fVolumes.find(vname) != fVolumes.end()) continue;
             
             vol = BuildVolume(vpars);
             if (!vol) continue;
                            
             if (vname != vol->GetName()) {
                 cout << "-E - <SpdGeoBuilder::ConstructVolumes> "
                      << "Error is occured for the volume: " 
                      << vname << "/" <<  vol->GetName() << endl;
                 continue;
             }
             
             fVolumes.insert(std::pair<TString,TGeoVolume*>(vname,vol));
        }
   }
}

//_____________________________________________________________________________    
void SpdGeoBuilder::FillParametersIn(SpdParSet* params) 
{ 
   if (!params) return;
   params->SetParameter("Builder/MaxBuildGeoLevel",fMaxBuildGeoLevel,-3);
}
    
//_____________________________________________________________________________        
void SpdGeoBuilder::LoadParametersFrom(SpdParSet* params) 
{
   if (!params) return;
   params->GetParameter("Builder/MaxBuildGeoLevel",fMaxBuildGeoLevel);
}

//_____________________________________________________________________________    
void SpdGeoBuilder::Print(Option_t*) const
{ 
   cout << "<" << this->ClassName() << "::Print>" << endl;
   cout << endl;
   cout << "\tMax. build geometry level: " << fMaxBuildGeoLevel << endl;
   cout << endl;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*/////////////////////////////////////////////////////////////////////////////
//////////////////////// CHECK ACTIVE VOLUMES ///////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//_____________________________________________________________________________
void SpdGeoBuilder::CheckActiveVolumes()
{
  //cout << "-I- <" << this->ClassName() << "::CheckActiveVolumes> " << endl;
   
  if (!fMasterVolume) return;
  
  SpdGeoMapper* mapper = GetMapper();
  if (!mapper) return;
  
  SpdGeoTable* table = mapper->GetGeoTable();
  if (!table) return;
  
  TObjArray* vpars = mapper->GetVolPars();
  if (!vpars) return;
  
  const SpdGeoTableRec* rec;
       
  TString volname;
  
  TObjArray*  nodes;
  TGeoVolume* vol;
  TGeoNode*   node;
  
  std::set<TString> actvols;
  
  for (Int_t i(0); i<table->GetNRec(0); i++) 
  {
       rec = table->GetRec(0,i);
       volname = rec->GetVolName();
     
       vol = gGeoManager->GetVolume(volname);
       CheckActivity(vol,actvols);
  }
  
  //cout << "-I- <SpdGeoBuilder::CheckActiveVolumes> Active volumes: " << actvols.size() << endl;
  
  Int_t nact(0), ndeact(0);
  
  Int_t np = vpars->GetEntriesFast();
  SpdGeoVolPars* pars; 
  for (Int_t i(0); i<np; i++) {
       pars = (SpdGeoVolPars*)vpars->At(i);
       if (pars->GetActivity() > 0) {
           if (actvols.find(pars->GetName()) == actvols.end()) 
           {
               //cout << "-I- <SpdGeoBuilder::CheckActiveVolumes> Inactivate volume: " 
               //     << pars->GetName() << endl; 
               pars->SetActivity(0);  
               ndeact++;
           }
           nact++;
       }
  }
  cout << "-I- <" << this->ClassName() << "::CheckActiveVolumes> "
       << "Deactivate volumes: " << ndeact << "/" << nact << endl;
}

//_____________________________________________________________________________
void SpdGeoBuilder::CheckActivity(TGeoVolume* vol, std::set<TString>& actvols)
{
  if (!vol) return;
  
  if (GetMapper()->GetVolActivity(vol->GetName())) 
  {
      actvols.insert(TString(vol->GetName()));
      //cout << "<SpdGeoBuilder::CheckActivity> add (new?) active: " 
      //     << vol->GetName() << " " <<  actvols.size() << endl;
  }
  
  TObjArray* nodes = vol->GetNodes();
  if (!nodes) return;
 
  Int_t nv = nodes->GetEntriesFast();
  if (nv < 1) return;
  
  //cout << "\t <SpdGeoBuilder::CheckActivity> volume: " << vol->GetName() << " activity: " 
  //     << fGeoMapper->GetVolActivity(vol->GetName()) 
  //     << " nodes: " << nv << endl; 
  
  TGeoNode*  node;
  TGeoVolume *volume(0), *cv;
 
  for (Int_t j(0); j<nv; j++) {
       node = (TGeoNode*)nodes->At(j);
       if (!node) continue;
       cv = node->GetVolume();
       if (cv == volume) continue;
       volume = cv;
       if (volume) CheckActivity(volume,actvols);            
  }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*/////////////////////////////////////////////////////////////////////////////
//////////////////////// GEOMETRY NAVIGATION ////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//_____________________________________________________________________________      
TGeoNavigator* SpdGeoBuilder::GetNavigator()
{
  return (gGeoManager) ? gGeoManager->GetCurrentNavigator() : 0;
}

//_____________________________________________________________________________      
TGeoNavigator* SpdGeoBuilder::SetNodePath(const TString& path) 
{
  if (!gGeoManager) return 0;
  
  if (path == "MASTER") return 0;
  
  TGeoNavigator* ntor = gGeoManager->GetCurrentNavigator();
  if (!ntor) return 0;

  if (!ntor->cd(path)) return 0;  
  return ntor;
}

//_____________________________________________________________________________      
TGeoNavigator* SpdGeoBuilder::SetNodePath(Long_t detTID) 
{
  if (detTID < 1) return 0;
  if (!gGeoManager) return 0;
  
  TGeoNavigator* ntor = gGeoManager->GetCurrentNavigator();
  if (!ntor) return 0;
  
  SpdGeoTable* geotable = GetTable();
  if (!geotable) return 0;
  
  TString path = geotable->GetNodeGeoPath(detTID);
  if (path == "MASTER") return 0;
    
  if (!ntor->cd(path)) return 0;
  
  return ntor;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~ GEOMETRY NAVIGATION ~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

//_____________________________________________________________________________    
TString SpdGeoBuilder::GetNodePath()
{
  TGeoNavigator* ntor = gGeoManager->GetCurrentNavigator();
  return (ntor) ? ntor->GetPath() : "";
}

//_____________________________________________________________________________    
TGeoVolume* SpdGeoBuilder::GetNodeVolume()
{
  TGeoNavigator* ntor = gGeoManager->GetCurrentNavigator();
  return (ntor) ? ntor->GetCurrentVolume() : 0;
}

//_____________________________________________________________________________    
Bool_t SpdGeoBuilder::GetNodeSize(Double_t* l)
{
  TGeoVolume* vol =  GetNodeVolume();
  if (!vol) return kFALSE;
  TGeoBBox* box = (TGeoBBox*)vol->GetShape();
  l[0] = box->GetDX(); l[1] = box->GetDY(); l[2] = box->GetDZ();
  return kTRUE;
}
 
//_____________________________________________________________________________    
TString SpdGeoBuilder::GetNodeVolumeName()
{
  TGeoNavigator* ntor = gGeoManager->GetCurrentNavigator();
  return (ntor) ? ntor->GetCurrentVolume()->GetName() : "";
}

//_____________________________________________________________________________
Bool_t SpdGeoBuilder::GetDetectorPosition(Double_t* p)
{
  TGeoNavigator* ntor = gGeoManager->GetCurrentNavigator();
  if (!ntor) return kFALSE;
  
  Double_t ploc[3] = {0, 0, 0};
  ntor->LocalToMaster(ploc,p);
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdGeoBuilder::GetDetectorPositionXYZ(Double_t* p)
{
  return GetDetectorPosition(p);
}

//_____________________________________________________________________________
Bool_t SpdGeoBuilder::GetDetectorPositionRPZ(Double_t* p)
{
  if (!GetDetectorPosition(p)) return kFALSE;
  
  Double_t r   = TMath::Sqrt(p[0]*p[0] + p[1]*p[1]);
  Double_t phi = TMath::ATan2(p[1],p[0])*TMath::RadToDeg();
  
  p[0] = r;
  p[1] = phi;
  
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdGeoBuilder::GetDetectorPositionRTP(Double_t* p)
{
  if (!GetDetectorPosition(p)) return kFALSE;
  
  Double_t r     = TMath::Sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
  Double_t theta = TMath::ACos(p[2]/r)*TMath::RadToDeg();
  Double_t phi   = TMath::ATan2(p[1],p[0])*TMath::RadToDeg();
  
  p[0] = r;
  p[1] = theta;
  p[2] = phi;
  
  return kTRUE;
}
  
//_____________________________________________________________________________ 
Bool_t SpdGeoBuilder::GetDetectorOrientation(Double_t* v, Char_t dir)
{
  TGeoNavigator* ntor = gGeoManager->GetCurrentNavigator();
  if (!ntor) return kFALSE;
  
  Double_t vloc[3];
  
       if (dir == 'x' || dir == 'X') { vloc[0] = 1.; vloc[1] = 0.; vloc[2] = 0.; }
  else if (dir == 'y' || dir == 'Y') { vloc[0] = 0.; vloc[1] = 1.; vloc[2] = 0.; }
  else if (dir == 'z' || dir == 'Z') { vloc[0] = 0.; vloc[1] = 0.; vloc[2] = 1.; }
  else return kFALSE;
  
  ntor->LocalToMasterVect(vloc,v);
  
  if (TMath::Abs(v[0]) < 1e-15) v[0] = 0.;
  if (TMath::Abs(v[1]) < 1e-15) v[1] = 0.;
  if (TMath::Abs(v[2]) < 1e-15) v[2] = 0.;
  
  return kTRUE;
}
    
//_____________________________________________________________________________
Bool_t SpdGeoBuilder::GlobalToLocalP(const Double_t* gpnt, Double_t* lpnt)
{
  TGeoNavigator* ntor = gGeoManager->GetCurrentNavigator();
  if (!ntor) return kFALSE;
  
  ntor->MasterToLocal(gpnt,lpnt);
  return kTRUE;
}

//_____________________________________________________________________________   
Bool_t SpdGeoBuilder::LocalToGlobalP(const Double_t* lpnt, Double_t* gpnt) 
{ 
  TGeoNavigator* ntor = gGeoManager->GetCurrentNavigator();
  if (!ntor) return kFALSE;
  
  ntor->LocalToMaster(lpnt,gpnt);
  return kTRUE;
}

//_____________________________________________________________________________   
Bool_t SpdGeoBuilder::GlobalToLocalV(const Double_t* gvct, Double_t* lvct)
{  
  TGeoNavigator* ntor = gGeoManager->GetCurrentNavigator();
  if (!ntor) return kFALSE;
  
  ntor->MasterToLocalVect(gvct,lvct);
  return kTRUE;
}

//_____________________________________________________________________________      
Bool_t SpdGeoBuilder::LocalToGlobalV(const Double_t* lvct, Double_t* gvct)
{
  TGeoNavigator* ntor = gGeoManager->GetCurrentNavigator();
  if (!ntor) return kFALSE;
  
  ntor->LocalToMasterVect(lvct,gvct);
  return kTRUE;
}

//_____________________________________________________________________________      
void SpdGeoBuilder::PrintGeoVolume(const TGeoVolume* vol, Int_t option)
{
  if (!vol) {
      cout << "<SpdGeoBuilder::PrintGeoVolume> No volume to print" << endl;
      return;
  }
  cout << "<SpdGeoBuilder::PrintVolume>" << endl;
  
  cout << "Type:       " << vol->ClassName() << endl;
  cout << "Name:       " << vol->GetName() << endl;
  cout << "Material:   " << vol->GetMaterial()->GetName() << endl;
  cout << "Shape:      " << vol->GetShape()->ClassName() << endl;
  
  cout << endl;
}
