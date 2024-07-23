// #include <TFile.h>
// #include <TTree.h>
// #include <TGeoManager.h>
// #include <TClonesArray.h>
// #include "FairGeoParSet.h"
// #include "SpdKFSimpleRes.h"
// #include "GFCore.hh"
// #include "EventDisplay.h"
// #include "SpdGFTrackW.h"
// #include <vector>

TTree* Tree_ = 0;

void check_tracks(Int_t nevent = 0);

TString inFile, inParFile;

//______________________________________________________________________________
void CheckGFTracks(TString path = "") 
{
  if (!path.IsWhitespace() && !path.EndsWith("/")) path += "/";
    
  inFile    = path + "IdealFittedTracks.root";   
  inParFile = path + "params.root";
  
  TFile* f = new TFile(inFile);
  if (!f) return;
  
  Tree_ = (TTree*)f->Get("spdsim");  
  if (!Tree_) return;
  
  cout << "\n >>>>>>>>>>>>>>>> DATA FILE & TREE: Ok! <<<<<<<<<<<<<<<<< " << endl;  
  
  check_tracks(0);
  
  //SpdBaseParSet* pars = (SpdBaseParSet*)f->Get("IdealKalmanFitterParams");
  //if (pars) pars->print(1);
}

//______________________________________________________________________________
void check_tracks(Int_t nevent)
{
   if (!Tree_) return;  
   
   TBranch* branch = Tree_->GetBranch("SpdGFTrackW.");
   if (!branch) { cout << " no \"GF tracks\" branch " << endl; return; }
   
   Int_t entries = branch->GetEntries();
   
   cout << "Total entries in the \"GF tracks\" branch: " << entries << endl; 
   
   if (nevent < 0 || nevent >= entries) return;
  
   TObjArray *fT = 0; 
   Tree_->SetBranchAddress("SpdGFTrackW.",&fT) ; 
   
   //------------------------
   
   TBranch* data_branch = Tree_->GetBranch("SpdKFSimpleRes.");
   if (!data_branch) { cout << " no \"KF data\" branch " << endl; return; }
   
   SpdKFSimpleRes* Res = 0;
   Tree_->SetBranchAddress("SpdKFSimpleRes.",&Res) ;  
   
   //------------------------
      
   branch->GetEntry(nevent);  
   data_branch->GetEntry(nevent);  
   
   if (Res) Res->PrintData(2);
         
   SpdGFTrackW* wtrack = 0;
   genfit::Track* track = 0;
   
   Int_t ntr = fT->GetEntriesFast();
   
   cout << "\nevent = " << nevent << " tracks = " << ntr << endl;
  
   for (Int_t i(0); i<ntr; i++) {
       wtrack = (SpdGFTrackW*)fT->At(i);
       if (!wtrack) continue;
       cout << "wtrack " << i << " data: " << wtrack->GetDataID() << endl;
       track = wtrack->GetTrack();
       if (!track) continue;
       
       //if (track && i == 4) 
       //    track->Print();
           
       cout<< "------ track: " << i+1 << "/" << ntr << endl;   
   }
   
   //genfit::EventDisplay* display = genfit::EventDisplay::getInstance();
   
   //genfit::Track* trackX = new genfit::Track();
   //*trackX = *(genfit::Track*)fT->At(3);
   
   //track = (genfit::Track*)fT->At(0);
   //track->fillPointsWithMeasurement();
   //track->checkConsistency();
   
   //track->Print();
   
   //display->addEvent(track);
   
   //display->open();
}


