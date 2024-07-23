
void check_pars();
void check_headers();
void check_points();
void check_tracks();

void analyze();

TTree* t = 0;

TString Datafname = "test_data.root";
TString Parsfname = "test_params.root";

//TString Datafname = "run_tor.root";
//TString Parsfname = "params_tor.root";

//________________________________________________________________________________
void CheckTrackerEvents() {
  //
  TFile* f = new TFile(Datafname);
  if (!f) return;
  
  t = (TTree*)f->Get("cbmsim");  
  if (!t) return;
  
  cout << "FILE: Ok! (" << Datafname << ") " << endl;
  
  check_pars();
  check_headers();
  
  check_points(); 
  check_tracks();
  
  analyze();
}

//______________________________________________________________________________
void check_pars() {
   //       
   cout << "\n+++++++++++++++++ <check_pars> ++++++++++++++++++"; 
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl; 
   
   TFile* fp = new TFile(Parsfname);
   if (!fp) return;
   
   SpdFieldPar* field_pars = (SpdFieldPar*)fp->Get("SpdFieldPar");
   if (field_pars) field_pars->print(1);  
   
   SpdPrimGenParSet *gen_pars = (SpdPrimGenParSet*)fp->Get("PrimGenParSet");
   gen_pars->print(1);
   
   SpdParSet* pars = (SpdParSet*)fp->Get("TrackerParSet");
   if (pars) pars->print(1);
   
   fp->Close();
}

//______________________________________________________________________________
void check_headers() {
   //       
   cout << "\n+++++++++++++++++ <check_headers> ++++++++++++++++++"; 
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl; 
   
   if (!t) return;  
   
   TBranch* branch = t->GetBranch("MCEventHeader.");
   if (!branch) return;
   
   int entries = branch->GetEntries();
   
   cout << "Total number of HEADERS in the TREE = " << entries << endl;    
   if (entries == 0) return;
   
   SpdMCEventHeader* header = 0;
   t->SetBranchAddress("MCEventHeader.",&header) ;       
        
   for (int j(0); j < entries; j++)   {
        branch->GetEntry(j);  
        if (j == 0) {
            cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j << endl;
            header->Print(1);
        }
        if (j == entries-1) {
            cout << ">>>>>>>>>>>>>>>>> Print LAST entry:  " << j << endl;
            header->Print(1);
        }
   }
   
   cout << "Total number of HEADERS in the TREE = " << entries << endl;    
}

//______________________________________________________________________________
void check_points() 
{
   cout << "\n+++++++++++++++++ <check_points> ++++++++++++++++++";
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;  
   
   if (!t) return;  
    
   TBranch* branch = t->GetBranch("TrackerPoint");
   if (!branch) return;
   
   TClonesArray *fT = 0; 
   t->SetBranchAddress("TrackerPoint",&fT) ;
   
   int entries = branch->GetEntries();
  
   cout << "Total number of POINTS COLLECTIONS in the TREE = " << entries << endl;    
   
   if (entries == 0) return;
   
   TrackerPoint* point;
   Int_t ne;

   for (int j(0); j <entries; j++) {
     
       branch->GetEntry(j);      
        
       ne = fT->GetEntriesFast();
        
       cout << "entry: " << j << " points = " << ne << endl;
        
        if (j == 0) {
            cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j << endl;
            
            for (int i(0); i < ne; i++) {
                 point = (TrackerPoint*)fT->At(i);            
                 if (i < 3) point->Print("");   
            }
            
        }
        if (j == entries-1) {
            cout << ">>>>>>>>>>>>>>>>> Print LAST entry:  " << j << endl;
          
            for (int i(0); i < ne; i++) {
                point = (TrackerPoint*)fT->At(i);            
                if (i < 3) point->Print("");   
            }
        }
   }
   
   cout << "Total number of POINTS COLLECTIONS in the TREE = " << entries << endl;    
}

//______________________________________________________________________________
void check_tracks() 
{
   cout << "\n+++++++++++++++++ <check_tracks> ++++++++++++++++++"; 
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl; 
   
   if (!t) return; 
   
   TBranch* branch = t->GetBranch("GeoTracks");
   if (!branch) return;
   
   int entries = branch->GetEntries();
  
   cout << "Total number of TRACK COLLECTIONS in the TREE = " << entries << endl;    
   if (entries == 0) return;
   
   TClonesArray* tracks = 0;
   branch->SetAddress(&tracks) ;

   for (int j(0); j<entries; j++) {
        branch->GetEntry(j);
        if (j == 0) {
            cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j 
                 << ",  tracks = " << tracks->GetEntriesFast() << endl;
        }
        if (j == entries-1) {
            cout << ">>>>>>>>>>>>>>>>> Print LAST  entry: " << j 
                 << ",  tracks = " << tracks->GetEntriesFast() << endl;
        }
   }
  
   cout << "Total number of TRACK COLLECTIONS in the TREE = " << entries << endl; 
}

//______________________________________________________________________________
void analyze() 
{
   cout << "\n+++++++++++++++++ <analyze> ++++++++++++++++++";
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl; 

   Int_t geotype = 0;
   Double_t layer_width = 0;
   
   TFile* fp = new TFile(Parsfname);
   if (!fp) return;
   
   SpdParSet* pars = (SpdParSet*)fp->Get("TrackerParSet");
      
   pars->GetParameter("GeoType",geotype);
   pars->GetParameter("LayerWidth",layer_width);
   
   fp->Close();
   
   cout << "Geometry type: " << geotype << endl;
   cout << "Layer width:   " << layer_width << endl;
   
   if (!t) return;  


}

