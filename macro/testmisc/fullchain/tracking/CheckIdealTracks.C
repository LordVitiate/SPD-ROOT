

TTree* Tree_ = 0;

void check_headers();
void check_events();
void check_params();

TString inFile, inParFile;

//______________________________________________________________________________
void CheckIdealTracks(TString path = "") 
{
  if (!path.IsWhitespace() && !path.EndsWith("/")) path += "/";

  inFile    = path + "IdealTracks.root";   
  inParFile = path + "params.root";
  
  TFile* f = new TFile(inFile);
  if (!f) return;
  
  Tree_ = (TTree*)f->Get("spdsim");  
  if (!Tree_) return;
  
  cout << "\n >>>>>>>>>>>>>>>> DATA FILE & TREE: Ok! <<<<<<<<<<<<<<<<< " << endl;  
  
  check_headers();
  check_params();
  check_events();
  
  SpdBaseParSet* pars = (SpdBaseParSet*)f->Get("IdealTrackFinderParams");
  if (pars) pars->print(1);
}

//______________________________________________________________________________
void check_headers() {
   //       
   cout << "\n+++++++++++++++++ <check_headers> ++++++++++++++++++"; 
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl; 
   
   if (!Tree_) return;  
 
   TBranch* branch = Tree_->GetBranch("EventHeader.");
   if (!branch) return;

   int entries = branch->GetEntries();
   
   cout << "Total number of HEADERS in the TREE = " << entries << endl;    
   if (entries == 0) return;
   
   FairEventHeader* header = 0;
   Tree_->SetBranchAddress("EventHeader.",&header) ;       
        
   for (int j(0); j < entries; j++)   {
        branch->GetEntry(j);  
        if (j == 0) {
            cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j << endl;
            cout << "Run ID: " << header->GetRunId() << endl;
        }
        if (j == entries-1) {
            cout << ">>>>>>>>>>>>>>>>> Print LAST entry:  " << j << endl;
            cout << "Run ID: " << header->GetRunId() << endl;
        }
   }
   
   cout << "Total number of HEADERS in the TREE = " << entries << endl;    
}
//______________________________________________________________________________
void check_events() {
   //       
   cout << "\n+++++++++++++++++ <check_events> ++++++++++++++++++"; 
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl; 
   
   if (!Tree_) return;  
   
   TBranch* branch = Tree_->GetBranch("IdealTrackColl.");
   if (!branch) return;
   
   int entries = branch->GetEntries();
   
   cout << "Total number of EVENTS in the TREE = " << entries << endl;    
   
   if (entries == 0) return;
   
   SpdIdealTrackColl* Event = 0;
  
   Tree_->SetBranchAddress("IdealTrackColl.",&Event) ;       
  
   SpdIdealTrack* track;
   
   //entries = 1;
   
   for (int j(0); j < entries; j++)   {
        branch->GetEntry(j);  
      
        //if (j != 0) continue;
        Event->PrintData();
        
        if (Event->GetNTracks() > 0) {
          track = (SpdIdealTrack*)Event->GetTrack(0);
          if (track) track->PrintTrack(true);
          //if (track) track->PrintTrack(false);
        }
        
        Event->DeleteAll(); // clear event
   }
}

//______________________________________________________________________________
void check_params()
{
   TFile* fp = new TFile(inParFile);
   if (!fp) return;
   
   FairGeoParSet* geoset = (FairGeoParSet*)fp->Get("FairGeoParSet");

   gGeoManager->GetMasterVolume()->Draw("ogl"); 
 
}
