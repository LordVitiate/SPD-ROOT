// $Id$
// Author: artur   2018/01/31


//_____________________________________________________________________________
//
// SpdDetector
//_____________________________________________________________________________

#include "SpdDetector.h"
#include "SpdCommonGeoMapper.h"

#include "FairRuntimeDb.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairGeoBuilder.h"
#include "FairGeoMedia.h"

#include <TGeoManager.h>
#include <TObjString.h>

#include <iostream>
using std::cout;
using std::endl;

using std::map;
using std::string;

ClassImp(SpdDetector)
ClassImp(SpdDetector::DetVolume) 

//_____________________________________________________________________________
SpdDetector::SpdDetector():FairDetector("SpdDetector",kFALSE),
fGeoMapper(0),fGeoBuilder(0),fMasterVolume(0),fNDataOut(0),
fSaveEmptyHits(kFALSE),fSaveDetIdOption(2),
fNodesIdTable(0),fNodesIdTableIsOwn(kFALSE)
{
   SetParametersType("not defined");
   SpdCommonGeoMapper::Instance()->AddDetector(this); 
}

//_____________________________________________________________________________
SpdDetector::SpdDetector(const char* Name, Int_t Det_ID, Bool_t Is_Active):
FairDetector(Name,Is_Active,Det_ID),
fGeoMapper(0),fGeoBuilder(0),fMasterVolume(0),fNDataOut(0),
fSaveEmptyHits(kFALSE),fSaveDetIdOption(2),
fNodesIdTable(0),fNodesIdTableIsOwn(kFALSE)
{
   SetParametersType("not defined");
   SpdCommonGeoMapper::Instance()->AddDetector(this); 
}

//_____________________________________________________________________________
SpdDetector::~SpdDetector() 
{
   if (fGeoMapper) {
       delete fGeoMapper;
       fGeoMapper = 0;
   }
   
   if (fGeoBuilder) {
       delete fGeoBuilder;
       fGeoBuilder = 0;
   }
   
   if (fNodesIdTableIsOwn) {
       if (fNodesIdTable) delete fNodesIdTable;
       fNodesIdTable = 0;
   }
}

//_____________________________________________________________________________
void SpdDetector::Initialize() 
{ 
   FairDetector::Initialize(); 
   
   FillParsIn(GetParameters()); 
}

//_____________________________________________________________________________
void SpdDetector::FinishRun()
{
   //FillNodesTableIn(GetParameters());
}

//_____________________________________________________________________________
Bool_t SpdDetector::SetNodesIdTable(SpdNodesIdTable* table)
{
   if (fNodesIdTable) return kFALSE;
   
   if (!table) {
       fNodesIdTable = new SpdNodesIdTable();
       fNodesIdTableIsOwn = kTRUE;
       return kTRUE;
   }
   
   fNodesIdTable = table;
   fNodesIdTableIsOwn = kFALSE;
   return kTRUE;
}

//_____________________________________________________________________________
void SpdDetector::RemoveNodesIdTable()
{
   if (fNodesIdTable) {
       delete fNodesIdTable;
       fNodesIdTable = 0;
   }
   fNodesIdTableIsOwn = kFALSE;
}

//_____________________________________________________________________________
Int_t SpdDetector::RegisterMediums()
{
   if (!fGeoMapper) return 0;
    
   TObjArray* meds = fGeoMapper->GetMediums();
   if (!meds) return 0;
   
   //cout << "<SpdDetector::RegisterMediums> [1]: " << meds->GetEntriesFast() << endl;
   
   FairGeoLoader* geoLoad = FairGeoLoader::Instance();   
   if (!geoLoad) return 0;     
            
   FairGeoInterface* geoFace = geoLoad->getGeoInterface();
   if (!geoFace) return 0;     
  
   FairGeoMedia* Media = geoFace->getMedia();
   if (!Media) return 0;    
  
   FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder(); 
   if (!geoBuild) return 0;
   
   Int_t n = meds->GetEntriesFast();
   TObjString* medname;
   TGeoMedium* med;
   
   Int_t nn(0);
   for (Int_t i(0); i<n; i++) {
        medname = (TObjString*)meds->At(i);
        if (!medname) continue; 
        med = gGeoManager->GetMedium(medname->GetName());
        if (med) { nn++; continue; }
        geoBuild->createMedium(Media->getMedium(medname->GetName()));
        if (gGeoManager->GetMedium(medname->GetName())) nn++;
   }
   
   TString umedia = fGeoMapper->GetUnsetMedia();
   if (!umedia.IsWhitespace()) {
       med = gGeoManager->GetMedium(umedia);
       if (med) nn++;
       else {
          geoBuild->createMedium(Media->getMedium(umedia));
          if (gGeoManager->GetMedium(umedia.Data())) nn++;
       }
   }
   
   //cout << "<SpdDetector::RegisterMediums> [2]: " << nn << endl;
   
   return nn;
}

//_____________________________________________________________________________
void SpdDetector::RegisterActiveVolumes()
{
   if (!fGeoMapper || !fGeoBuilder) return;
   
   TObjArray* vols = fGeoMapper->GetVolPars();
   if (!vols) return;
   
   fGeoBuilder->CheckActiveVolumes();
   
   SpdGeoVolPars* pars;
   TGeoVolume* vol;
  
   for (Int_t i(0); i<vols->GetSize(); i++) {
     
        pars = (SpdGeoVolPars*)vols->At(i);
        if (!pars) continue;
        
        if (pars->GetActivity() < 1) continue;
        
        vol = fGeoBuilder->GetVolume(pars->GetName());       
        if (vol) AddSensitiveVolume(vol);  
        
        //cout << "<SpdDetector::RegisterActiveVolumes>"
        //     << " Register sensitive volume: " << pars->GetName() 
        //     << " activity: " << pars->GetActivity() << endl; 
   }
}

//_____________________________________________________________________________________
SpdParSet* SpdDetector::GetParameters() 
{
   FairRun* run = FairRun::Instance();
   if (!run) return 0;
  
   FairRuntimeDb* rtdb = run->GetRuntimeDb(); 
   if (!rtdb) return 0;
  
   SpdParSet* pars = (SpdParSet*)(rtdb->getContainer(GetParametersType()));
   
   return pars;
}

//_____________________________________________________________________________
Long_t SpdDetector::GetNodeId(TString nodepath)
{
   if (fSaveDetIdOption < 1 || fSaveDetIdOption > 3) return -1;
   if (nodepath.IsWhitespace()) return -1;
   
   if (fSaveDetIdOption == 2) {
       if (fGeoMapper) return fGeoMapper->GetNodeId(nodepath);
       fSaveDetIdOption = 1;
       cout << "-E- <" << this->ClassName() << "::GetNodeId>" 
            << " No mapper, option (2) is changed to " 
            << fSaveDetIdOption << endl;
   }
   
   if (!fNodesIdTable) SetNodesIdTable();
   
   return fNodesIdTable->GetTableNodeId(nodepath.Data());
}

//_____________________________________________________________________________
Bool_t SpdDetector::FillParsIn(SpdParSet* params) 
{
   if (!params) return kFALSE; 
   
   params->SetParameter("Detector/Id",fDetId,-2);  
   params->SetParameter("Detector/Name",fName,-2);  
   
   params->SetParameter("Detector/NOutData",fNDataOut,-2);
   
   for (Int_t i(0); i<fNDataOut; i++) {
        params->SetParameter(Form("OutData_%d",i+1),GetDataOut(i),-1); 
   }

   // save mapper type & parameters
   if (fGeoMapper) {
       params->SetParameter("Mapper",fGeoMapper->ClassName(),-1);
       fGeoMapper->FillParametersIn(params);
   }
   
   // save builder type & parameters
   if (fGeoBuilder) {
       params->SetParameter("Builder",fGeoBuilder->ClassName(),-1);
       fGeoBuilder->FillParametersIn(params);
   }
   
   params->SetParameter("Detector/SaveEmptyHits",fSaveEmptyHits,-2);
    
   //ATTENTION  check and save actual SaveDetIdOption and NodesIdTable ATTENTION 
   if (fMasterVolume) {
       TString nodepath("/");
       nodepath += fMasterVolume->GetName();
       nodepath += "_1";
       GetNodeId(nodepath);
   }
   
   params->SetParameter("Detector/SaveDetIdOption",fSaveDetIdOption,-2);
    
   // save nodes table type
   if (fNodesIdTable) {
       params->SetParameter("NodesIdTable",fNodesIdTable->ClassName(),-1);
   }
     
   params->setChanged();
   params->setInputVersion(FairRun::Instance()->GetRunId(),1);
   
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdDetector::LoadParsFrom(SpdParSet* params) 
{
   if (!params) return kFALSE;
   
   Int_t x;
 
   if (!params->GetParameter("Detector/Id",x)) return kFALSE;
   
   if (x != fDetId) {
       cout << "-E- <" << this->ClassName() 
            << ">::LoadParsFrom> Different detector's id (actual/loaded): "
            << fDetId << "/" << x << endl;
       return kFALSE;
   }
   
   //cout << "-I- <" << this->ClassName() << ">::LoadParsFrom> Dectector : " << fDetId << endl;
   
   params->GetParameter("Detector/NOutData",fNDataOut);
   
   params->GetParameter("Detector/SaveEmptyHits",fSaveEmptyHits);
   params->GetParameter("Detector/SaveDetIdOption",fSaveDetIdOption);
   
   CreateNodeIdTable(params);

   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdDetector::FillNodesTableIn(SpdParSet* params)
{
   if (!params) return kFALSE;
   
   TObjArray* nodes = params->GetNodes();
   if (!nodes) return kFALSE;
   
   if (!nodes->IsEmpty()) nodes->Delete();
   
   if (fNodesIdTable) fNodesIdTable->FillNodesTableIn(nodes);
   
   return kTRUE;
}
 
//_____________________________________________________________________________ 
Bool_t SpdDetector::LoadNodesTableFrom(SpdParSet* params)
{
   if (fNodesIdTable) fNodesIdTable->Clear();
   
   if (!params) return kFALSE;
   
   TObjArray* nodes = params->GetNodes();
   if (!nodes) return kFALSE;
   
   if (nodes->IsEmpty()) return kTRUE;
   
   if (fNodesIdTable) {
       fNodesIdTable->LoadNodesTableFrom(nodes);
       return kTRUE;
   }
   
   if (CreateNodeIdTable(params)) {
       fNodesIdTable->LoadNodesTableFrom(nodes);
       return kTRUE;
   }
          
   return kFALSE;
}

//_____________________________________________________________________________
Bool_t SpdDetector::CreateNodeIdTable(SpdParSet* params)
{
   TString nodestable("");
   
   if (!params->GetParameter("NodesIdTable",nodestable)) return kFALSE;
   
   if (nodestable == "SpdNodesIdTable") {
       fNodesIdTable = new SpdNodesIdTable();
       return kTRUE;
   }
   cout << "-W- <" << this->ClassName() << ">::CreateNodesTable> "
        << "Unknown table type: "<< nodestable << endl;
  
   return kFALSE; 
}

//_____________________________________________________________________________
void SpdDetector::Print(Option_t*) const
{
   cout << "<" << this->ClassName() << "::Print>" << endl;
   
   cout << "    Name:         " << fName << endl;
   cout << "    Unique Id:    " << fDetId << endl;
   cout << "    Parameters:   " << fTitle << endl;
   cout << "    Is active:    " << fActive << endl;
   cout << "    Out data (N): " << fNDataOut << endl;
   
   if (fGeoMapper) cout << "    Mapper:       " << fGeoMapper->ClassName() << endl;
   else cout << "    Mapper:       not defined "<< endl;
   
   if (fGeoBuilder) cout << "    Builder:      " << fGeoBuilder->ClassName() << endl;
   else cout << "    Builder:      not defined " << endl;
   
   cout << "\n    Parameters/ " << endl;
   cout << "    Save empty hits:   " << fSaveEmptyHits << endl;
   
   cout <<  endl;
}

//_____________________________________________________________________________
Double_t SpdDetector::GetDensity() const 
{ 
   Double_t capacity = GetCapacity();
   return (capacity > 1e-9) ? GetMass()/capacity : 0;
}

//_____________________________________________________________________________
TGeoMedium* SpdDetector::FindMedium(TString& medname, TString default_med) 
{ 
   if (medname.IsWhitespace()) return 0;
   if (!gGeoManager) return 0;
   
   TGeoMedium* med = 0;
   
   med = gGeoManager->GetMedium(medname);
   if (med) return med;
   
   FairGeoLoader* geoLoad = FairGeoLoader::Instance();   
   if (!geoLoad) return 0;     
            
   FairGeoInterface* geoFace = geoLoad->getGeoInterface();
   if (!geoFace) return 0;     
  
   FairGeoMedia* Media = geoFace->getMedia();
   if (!Media) return 0;    
  
   FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder(); 
   if (!geoBuild) return 0;
   
   if (!Media->getMedium(medname)) return 0;
          
   Int_t nmed = geoBuild->createMedium(Media->getMedium(medname));
   
   med = gGeoManager->GetMedium(medname);
   if (med) return med;
   
   medname = default_med;
   return FindMedium(medname,"");
}

