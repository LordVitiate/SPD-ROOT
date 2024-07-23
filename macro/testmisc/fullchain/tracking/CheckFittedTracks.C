// #include <TFile.h>
// #include <TTree.h>
// #include <TGeoManager.h>
// #include <TClonesArray.h>
// #include "FairGeoParSet.h"
// #include "SpdKFSimpleRes.h"
// #include "GFCore.hh"
// #include "EventDisplay.h"
// #include <vector>

TTree* Tree_ = 0;

void check_KFres();
void check_geometry();

TString inFile, inParFile;

//______________________________________________________________________________
void CheckFittedTracks(TString path = "") 
{
  if (!path.IsWhitespace() && !path.EndsWith("/")) path += "/";
    
  inFile    = path + "IdealFittedTracks.root";   
  inParFile = path + "params.root";
  
  TFile* f = new TFile(inFile);
  if (!f) return;
  
  Tree_ = (TTree*)f->Get("spdsim");  
  if (!Tree_) return;
  
  cout << "\n >>>>>>>>>>>>>>>> DATA FILE & TREE: Ok! <<<<<<<<<<<<<<<<< " << endl;  
  
  check_KFres();
  //check_geometry();
  
  SpdBaseParSet* pars = (SpdBaseParSet*)f->Get("IdealKalmanFitterParams");
  if (pars) pars->print(1);
}

//______________________________________________________________________________
void check_KFres() {
   //       
   cout << "\n+++++++++++++++++ <check_events> ++++++++++++++++++"; 
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl; 
   
   if (!Tree_) return;  
   
   TBranch* branch = Tree_->GetBranch("SpdKFSimpleRes.");
   if (!branch) return;
   
   int entries = branch->GetEntries();
   
   cout << "Total number of ENTRIES in the TREE = " << entries << endl;    
   
   if (entries == 0) return;
   
   SpdKFSimpleRes* Res = 0;
   
   Tree_->SetBranchAddress("SpdKFSimpleRes.",&Res) ;       

   Int_t NWellFittedEvents = 0;
   
   for (int j(0); j < entries; j++)   {
        branch->GetEntry(j);
        cout << "\n------------------------------------------------------------------------------\n";
        Res->PrintData(2);  // 0(default), 1, 2
        if (Res->IsWellFittedEvent()) NWellFittedEvents++;
        Res->ClearData();
   }
   
   cout << "\nEVENTS [fully_well_fitted/total]: " << NWellFittedEvents << "/" << entries 
        << "  (" << NWellFittedEvents/Double_t(entries) << ")" << "\n\n";
        
}

//______________________________________________________________________________
void check_geometry()
{
   TFile* fp = new TFile(inParFile);
   if (!fp) return;
   
   FairGeoParSet* geoset = (FairGeoParSet*)fp->Get("FairGeoParSet");

   gGeoManager->GetMasterVolume()->Draw("ogl");  
}

