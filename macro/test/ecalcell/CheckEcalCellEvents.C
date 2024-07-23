
void check_pars();
void check_headers();
void check_tracks();
void check_hits();
void check_points();

void analyze();

TTree* t = 0;

TString Datafname = "test_data.root";
TString Parsfname = "test_params.root";

//________________________________________________________________________________
void CheckEcalCellEvents() {
  //
  TFile* f = new TFile(Datafname);
  if (!f) return;
  
  t = (TTree*)f->Get("spdsim");  
  if (!t) return;
  
  cout << "FILE: Ok! " << endl;
  
  check_pars();
  
  check_headers();
  check_hits();
  check_points(); 
  check_tracks();
  
  //analyze();
}

//______________________________________________________________________________
void check_pars() {
   //       
   cout << "\n+++++++++++++++++ <check_pars> ++++++++++++++++++"; 
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl; 
   
   TFile* fp = new TFile(Parsfname);
   if (!fp) return;
   
   SpdParSet* pars = (SpdParSet*)fp->Get("EcalCellParSet");
   if (pars) pars->print(1);
   
   fp->Close();
}

//______________________________________________________________________________
void check_headers() 
{
   cout << "\n+++++++++++++++++ <check_headers> ++++++++++++++++++"; 
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl; 
   
   if (!t) return;  
   
   TBranch* branch = t->GetBranch("MCEventHeader.");
   if (!branch) return;
   
   int entries = branch->GetEntries();
   
   cout << "Total number of HEADERS in the TREE = " << entries << endl;    
   if (entries == 0) return;
   
   EcalCellEventHeader* header = 0;
   branch->SetAddress(&header);
     
   for (int j(0); j < entries; j++)   {
        branch->GetEntry(j);  
        if (j == 0) {
            cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j << endl;
            header->Print(1);
        }
        if (j == entries-1) {
            cout << ">>>>>>>>>>>>>>>>> Print LAST  entry: " << j << endl;
            header->Print(1);
        }
   }
   
   cout << "Total number of HEADERS in the TREE = " << entries << endl;    
}

//______________________________________________________________________________
void check_hits() 
{
   cout << "\n+++++++++++++++++ <check_hits> ++++++++++++++++++";
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl; 
   
   if (!t) return;  

   TBranch* branch = t->GetBranch("EcalCellHits.");
   if (!branch) return;
   
   int entries = branch->GetEntries();
   cout << "Total number of HITS EVENTS in the TREE = " << entries << endl;
   if (entries == 0) return;
 
   EcalCellHits* ev = 0;
   branch->SetAddress(&ev);

   for (int j(0); j <entries; j++)   {
        branch->GetEntry(j);      
        if (j == 0) {
            cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j << endl;
            ev->Print(1);
        }
       
        if (j == entries-1) {
            cout << ">>>>>>>>>>>>>>>>> Print LAST  entry: " << j << endl;
            ev->Print(1);
        }
   }
   cout << "Total number of DATA EVENTS in the TREE = " << entries << endl;
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
void check_points() 
{
   cout << "\n+++++++++++++++++ <check_points> ++++++++++++++++++";
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;  
   
   if (!t) return;  
    
   TBranch* branch = t->GetBranch("EcalCellPoint");
   if (!branch) return;
   
   TClonesArray *fT = 0; 
   t->SetBranchAddress("EcalCellPoint",&fT) ;
   
   int entries = branch->GetEntries();
  
   cout << "Total number of POINTS COLLECTIONS in the TREE = " << entries << endl;    
   
   if (entries == 0) return;
   
   EcalCellPoint* point;
   Int_t ne;

   for (int j(0); j <entries; j++) {
     
       branch->GetEntry(j);      
        
       ne = fT->GetEntriesFast();
        
       cout << "entry: " << j << " points = " << ne << endl;
        
        if (j == 0) {
            cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j << endl;
            
            for (int i(0); i < ne; i++) {
                 point = (EcalCellPoint*)fT->At(i);            
                 if (i < 2) point->Print("");   
            }
            
        }
        if (j == entries-1) {
            cout << ">>>>>>>>>>>>>>>>> Print LAST entry:  " << j << endl;
          
            for (int i(0); i < ne; i++) {
                point = (EcalCellPoint*)fT->At(i);            
                if (i < 2) point->Print("");   
            }
        }
   }
   
   cout << "Total number of POINTS COLLECTIONS in the TREE = " << entries << endl;    
}

//______________________________________________________________________________
void analyze() 
{
   cout << "\n+++++++++++++++++ <analyze> ++++++++++++++++++";
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl; 
   
   if (!t) return;  

   TBranch* branch_data = t->GetBranch("EcalCellHits.");
   if (!branch_data) return;
   
   TBranch* branch_header = t->GetBranch("MCEventHeader.");
   if (!branch_header) return;
 
   int entries = branch_data->GetEntries();
   cout << "Total number of DATA EVENTS in the TREE   = " << entries << endl;
   if (entries == 0) return;
   
   int entries_h = branch_header->GetEntries();
   cout << "Total number of HEADER EVENTS in the TREE = " << entries_h << endl;
   if (entries_h == 0) return;
 
   EcalCellHits* ev_data = 0;
   branch_data->SetAddress(&ev_data);
   
   EcalCellEventHeader* ev_header = 0;
   branch_header->SetAddress(&ev_header);
   
   TH1D* hlayers(0);
   TH1D* hlayersx(0);
   TH2D* hpads(0);
   
   //entries = 1;
   
   for (int j(0); j < entries; j++)   {
     
        branch_data->GetEntry(j);      
        branch_header->GetEntry(j);      
        
        ev_header->Print(1);
        
        hlayers = ev_data->GetHZ(hlayers);
        hpads = ev_data->GetHXY(hpads);
        hlayersx = ev_data->GetHZ(hlayersx,ev_header->GetPrimLastVolPath());
   }
   
   TCanvas* chlayers  = new TCanvas("chlayers","chlayers",900,650);
   if (hlayers) {
       hlayers->GetXaxis()->SetTitle("Depth, cm");
       hlayers->GetYaxis()->SetTitle("Energy deposit, GeV");
       hlayers->Draw();
   }
   
   TCanvas* chpads  = new TCanvas("chpads","chpads",800,800);
   if (hpads) {
       hpads->GetXaxis()->SetTitle("Size, cm");
       hpads->GetYaxis()->SetTitle("Size, cm");
       hpads->GetZaxis()->SetTitle("Energy deposit, GeV");
       hpads->GetXaxis()->SetTitleOffset(2.3);
       hpads->GetYaxis()->SetTitleOffset(2.3);
       hpads->GetZaxis()->SetTitleOffset(1.3);
       hpads->GetXaxis()->SetLabelSize(0.02);
       hpads->GetYaxis()->SetLabelSize(0.02);
       hpads->GetZaxis()->SetLabelSize(0.02);
       hpads->Draw("surf1");
   }
   
   TCanvas* chlayersx  = new TCanvas("chlayersx","chlayersx",900,650);
   if (hlayersx) hlayersx->Draw();
}

