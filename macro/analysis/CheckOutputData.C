
void check_headers();
void check_mctracks();

void check_points_tstb();
void check_points_tstec();

void check_points_tofb();
void check_points_tofec();

void check_points_its();

void check_points_ecaltb2();
void check_points_ecaltec2();

void check_points_rstb2();
void check_points_rstec2();

void check_points_bbc();
void check_points_aeg();
void check_points_zdc();

void check_geotracks();

void readparams(TString stype); 

TTree* t = 0;

TString inFile, inParFile;

//________________________________________________________________________________
void CheckOutputData(TString path = "", TString stype = "") {
  //
  if (!path.IsWhitespace() && !path.EndsWith("/")) path += "/";
   
  // Input data & parameters files   
  if (stype.IsWhitespace()) {
      inFile    = path+"run.root";   
      inParFile = path+"params.root";
  }
  else if (stype == "tor" || stype == "hyb") 
  { 
      inFile    = path+"run_tor.root";   
      inParFile = path+"params_tor.root";
  }
  else if (stype == "sol") 
  {  
      inFile    = path+"run_sol.root";   
      inParFile = path+"params_sol.root";
  }
  else {
      cout << "<CheckOutputData> Unknown data type: " << stype << endl;
      return;
  }
    
  SpdParticlesAndProcesses::AddParticlesToDatabasePDG();
   
  readparams(stype);
  
  TFile* f = new TFile(inFile);
  if (!f) return;
  
  t = (TTree*)f->Get("spdsim");  
  if (!t) return;
  
  cout << "\n >>>>>>>>>>>>>>>> DATA FILE & TREE: Ok! <<<<<<<<<<<<<<<<< " << endl;
  
  check_headers(); 
  check_mctracks();
  
  //return;
  
//  check_points_tstb();
//  check_points_tstec();
  
//  check_points_tofb();
//  check_points_tofec();
  
//   check_points_its(); 
//   
//   check_points_rstb2();
//   check_points_rstec2();
//   
//   check_points_ecaltb2();
//   check_points_ecaltec2();
//   

  //check_points_bbc();
  //check_points_aeg();
  //check_points_zdc();
  
  //check_geotracks();
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
void check_mctracks() {
   //       
   cout << "\n+++++++++++++++++ <check_mctracks> ++++++++++++++++++";
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
   
   if (!t) return;  
   
   TBranch* branch = t->GetBranch("SpdMCTrack");
   if (!branch) return;
   
   int entries = branch->GetEntries();
   
   cout << "Total number of MCTRACKS in the TREE = " << entries << endl;    
   if (entries == 0) return;
   
   TClonesArray *fT = 0; 
   t->SetBranchAddress("SpdMCTrack",&fT) ;   
   
   SpdMCTrack* track;
   Int_t np(0);
   Int_t ne;
   
   for (int j(0); j < entries; j++)   {
        
        branch->GetEntry(j);
        
        ne = fT->GetEntriesFast();
        
        cout << "entry: " << j << " mctracks = " << ne << endl;
        
        if (j == 0) {
             
           cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j << " Tracks = " << ne << endl;
           
           for (int i(0); i < ne; i++) {
                track = (SpdMCTrack*)fT->At(i);            
                np += track->GetTotalPoints();    
                //if (i < 200) 
                if (track->GetMotherId() < 0)  
                    track->Print(i);   
           }
           
           cout << "Total number of points inside: " << np << "\n\n"; 
        }
        
        if (j == entries-1) {
             
           cout << ">>>>>>>>>>>>>>>>> Print LAST entry:  " << j << " Tracks = " << ne << endl;
           
           for (int i(0); i < ne; i++) {
                track = (SpdMCTrack*)fT->At(i);            
                np += track->GetTotalPoints();    
                //if (i < 200)
                if (track->GetMotherId() < 0)   
                    track->Print(i);   
           }
           
           cout << "Total number of points inside: " << np << "\n\n"; 
        }
        
   }
        
   cout << "Total Number of MCTRACK COLLECTIONS in the TREE = " << entries << endl;   
}

//______________________________________________________________________________
void check_points_tstb() 
{
   // 
   if (!t) return;  
    
   TBranch* branch = t->GetBranch("SpdTsTBPoint");
   if (!branch) return;
   
   cout << "\n+++++++++++++++++ <check_points_tstb> ++++++++++++++++++";
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;  
  
   TClonesArray *fT = 0; 
   t->SetBranchAddress("SpdTsTBPoint",&fT) ;
   
   int entries = branch->GetEntries();
  
   cout << "Total number of TSTB-point COLLECTIONS in the TREE = " << entries << endl;    
   
   if (entries == 0) return;
   
   SpdTsTBPoint* point;
   Int_t ne;

   for (int j(0); j <entries; j++) {
     
       branch->GetEntry(j);      
        
       ne = fT->GetEntriesFast();
        
       cout << "entry: " << j << " points = " << ne << endl;
        
        if (j == 0) {
            cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j << endl;
            
            for (int i(0); i < ne; i++) {
                 point = (SpdTsTBPoint*)fT->At(i);            
                 if (i < 2) point->Print("");   
            }
            
        }
        if (j == entries-1) {
            cout << ">>>>>>>>>>>>>>>>> Print LAST entry:  " << j << endl;
          
            for (int i(0); i < ne; i++) {
                point = (SpdTsTBPoint*)fT->At(i);            
                if (i < 2) point->Print("");   
            }
        }
   }
   
   cout << "Total number of TSTB-point COLLECTIONS in the TREE = " << entries << endl;    
}

//______________________________________________________________________________
void check_points_tstec() 
{
   // 
   if (!t) return;  
    
   TBranch* branch = t->GetBranch("SpdTsTECPoint");
   if (!branch) return;
   
   cout << "\n+++++++++++++++++ <check_points_tstec> ++++++++++++++++++";
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;  
  
   TClonesArray *fT = 0; 
   t->SetBranchAddress("SpdTsTECPoint",&fT) ;
   
   int entries = branch->GetEntries();
  
   cout << "Total number of TSTEC-point COLLECTIONS in the TREE = " << entries << endl;    
   
   if (entries == 0) return;
   
   SpdTsTECPoint* point;
   Int_t ne;

   for (int j(0); j <entries; j++) {
     
       branch->GetEntry(j);      
        
       ne = fT->GetEntriesFast();
        
       cout << "entry: " << j << " points = " << ne << endl;
        
        if (j == 0) {
            cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j << endl;
            
            for (int i(0); i < ne; i++) {
                 point = (SpdTsTECPoint*)fT->At(i);            
                 if (i < 2) point->Print("");   
            }
            
        }
        if (j == entries-1) {
            cout << ">>>>>>>>>>>>>>>>> Print LAST entry:  " << j << endl;
          
            for (int i(0); i < ne; i++) {
                point = (SpdTsTECPoint*)fT->At(i);            
                if (i < 2) point->Print("");   
            }
        }
   }
   
   cout << "Total number of TSTEC-point COLLECTIONS in the TREE = " << entries << endl;    
}

//______________________________________________________________________________
void check_points_tofb() 
{
   // 
   if (!t) return;  
    
   TBranch* branch = t->GetBranch("SpdTofBPoint");
   if (!branch) return;
   
   cout << "\n+++++++++++++++++ <check_points_tofb> ++++++++++++++++++";
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;  
  
   TClonesArray *fT = 0; 
   t->SetBranchAddress("SpdTofBPoint",&fT) ;
   
   int entries = branch->GetEntries();
  
   cout << "Total number of TOFB-point COLLECTIONS in the TREE = " << entries << endl;    
   
   if (entries == 0) return;
   
   SpdTofBPoint* point;
   Int_t ne;

   for (int j(0); j <entries; j++) {
     
       branch->GetEntry(j);      
        
       ne = fT->GetEntriesFast();
        
       cout << "entry: " << j << " points = " << ne << endl;
        
        if (j == 0) {
            cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j << endl;
            
            for (int i(0); i < ne; i++) {
                 point = (SpdTofBPoint*)fT->At(i);            
                 if (i < 2) point->Print("");   
            }
        }
        if (j == entries-1) {
            cout << ">>>>>>>>>>>>>>>>> Print LAST entry:  " << j << endl;
          
            for (int i(0); i < ne; i++) {
                point = (SpdTofBPoint*)fT->At(i);            
                if (i < 2) point->Print("");   
            }
        }
   }
   
   cout << "Total number of TOFB-point COLLECTIONS in the TREE = " << entries << endl;    
}

//______________________________________________________________________________
void check_points_tofec() 
{
   // 
   if (!t) return;  
    
   TBranch* branch = t->GetBranch("SpdTofECPoint");
   if (!branch) return;
   
   cout << "\n+++++++++++++++++ <check_points_tofec> ++++++++++++++++++";
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;  
  
   TClonesArray *fT = 0; 
   t->SetBranchAddress("SpdTofECPoint",&fT) ;
   
   int entries = branch->GetEntries();
  
   cout << "Total number of TOFEC-point COLLECTIONS in the TREE = " << entries << endl;    
   
   if (entries == 0) return;
   
   SpdTofECPoint* point;
   Int_t ne;

   for (int j(0); j <entries; j++) {
     
       branch->GetEntry(j);      
        
       ne = fT->GetEntriesFast();
        
       cout << "entry: " << j << " points = " << ne << endl;
        
       if (j == 0) {
           cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j << endl;
            
           for (int i(0); i < ne; i++) {
                point = (SpdTofECPoint*)fT->At(i);            
                if (i < 2) point->Print("");   
           }
       }
       if (j == entries-1) {
           cout << ">>>>>>>>>>>>>>>>> Print LAST entry:  " << j << endl;
          
           for (int i(0); i < ne; i++) {
                point = (SpdTofECPoint*)fT->At(i);            
                if (i < 2) point->Print("");   
           }
       }
   }
   
   cout << "Total number of TOFEC-point COLLECTIONS in the TREE = " << entries << endl;    
}

//______________________________________________________________________________
void check_points_its() 
{
   // 
   if (!t) return;  
    
   TBranch* branch = t->GetBranch("SpdItsPoint");
   if (!branch) return;
   
   cout << "\n+++++++++++++++++ <check_points_its> ++++++++++++++++++";
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;  
  
   TClonesArray *fT = 0; 
   t->SetBranchAddress("SpdItsPoint",&fT) ;
   
   int entries = branch->GetEntries();
  
   cout << "Total number of ITS-point COLLECTIONS in the TREE = " << entries << endl;    
   
   if (entries == 0) return;
   
   SpdItsPoint* point;
   Int_t ne;

   for (int j(0); j <entries; j++) {
     
       branch->GetEntry(j);      
        
       ne = fT->GetEntriesFast();
        
       cout << "entry: " << j << " points = " << ne << endl;
        
        if (j == 0) {
            cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j << endl;
            
            for (int i(0); i < ne; i++) {
                 point = (SpdItsPoint*)fT->At(i);            
                 if (i < 2) point->Print("");   
            }
            
        }
        if (j == entries-1) {
            cout << ">>>>>>>>>>>>>>>>> Print LAST entry:  " << j << endl;
          
            for (int i(0); i < ne; i++) {
                point = (SpdItsPoint*)fT->At(i);            
                if (i < 2) point->Print("");   
            }
        }
   }
   
   cout << "Total number of ITS-point COLLECTIONS in the TREE = " << entries << endl;    
}

//______________________________________________________________________________
void check_points_aeg() 
{
   // 
   if (!t) return;  
    
   TBranch* branch = t->GetBranch("SpdAegPoint");
   if (!branch) return;
   
   cout << "\n+++++++++++++++++ <check_points_aeg> ++++++++++++++++++";
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;  
  
   TClonesArray *fT = 0; 
   t->SetBranchAddress("SpdAegPoint",&fT) ;
   
   int entries = branch->GetEntries();
  
   cout << "Total number of AEG-point COLLECTIONS in the TREE = " << entries << endl;    
   
   if (entries == 0) return;
   
   SpdAegPoint* point;
   Int_t ne;

   for (int j(0); j <entries; j++) {
     
       branch->GetEntry(j);      
        
       ne = fT->GetEntriesFast();
        
       cout << "entry: " << j << " points = " << ne << endl;
        
        if (j == 0) {
            cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j << endl;
            
            for (int i(0); i < ne; i++) {
                 point = (SpdAegPoint*)fT->At(i);            
                 if (i < 2) point->Print("");   
            }
            
        }
        if (j == entries-1) {
            cout << ">>>>>>>>>>>>>>>>> Print LAST entry:  " << j << endl;
          
            for (int i(0); i < ne; i++) {
                point = (SpdAegPoint*)fT->At(i);            
                if (i < 2) point->Print("");   
            }
        }
   }
   
   cout << "Total number of AEG-point COLLECTIONS in the TREE = " << entries << endl;    
}

//______________________________________________________________________________
void check_points_bbc() 
{
   // 
   if (!t) return;  
    
   TBranch* branch = t->GetBranch("SpdBbcPoint");
   if (!branch) return;
   
   cout << "\n+++++++++++++++++ <check_points_bbc> ++++++++++++++++++";
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;  
  
   TClonesArray *fT = 0; 
   t->SetBranchAddress("SpdBbcPoint",&fT) ;
   
   int entries = branch->GetEntries();
  
   cout << "Total number of BBC-point COLLECTIONS in the TREE = " << entries << endl;    
   
   if (entries == 0) return;
   
   SpdBbcPoint* point;
   Int_t ne;

   for (int j(0); j <entries; j++) {
     
       branch->GetEntry(j);      
        
       ne = fT->GetEntriesFast();
        
       cout << "entry: " << j << " points = " << ne << endl;
        
        if (j == 0) {
            cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j << endl;
            
            for (int i(0); i < ne; i++) {
                 point = (SpdBbcPoint*)fT->At(i);            
                 if (i < 2) point->Print("");   
            }
            
        }
        if (j == entries-1) {
            cout << ">>>>>>>>>>>>>>>>> Print LAST entry:  " << j << endl;
          
            for (int i(0); i < ne; i++) {
                point = (SpdBbcPoint*)fT->At(i);            
                if (i < 2) point->Print("");   
            }
        }
   }
   
   cout << "Total number of BBC-point COLLECTIONS in the TREE = " << entries << endl;    
}

//______________________________________________________________________________
void check_points_zdc() 
{
   // 
   if (!t) return;  
    
   TBranch* branch = t->GetBranch("SpdZdcPoint");
   if (!branch) return;
   
   cout << "\n+++++++++++++++++ <check_points_zdc> ++++++++++++++++++";
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;  
  
   TClonesArray *fT = 0; 
   t->SetBranchAddress("SpdZdcPoint",&fT) ;
   
   int entries = branch->GetEntries();
  
   cout << "Total number of ZDC-point COLLECTIONS in the TREE = " << entries << endl;    
   
   if (entries == 0) return;
   
   SpdZdcPoint* point;
   Int_t ne;

   for (int j(0); j <entries; j++) {
     
       branch->GetEntry(j);      
        
       ne = fT->GetEntriesFast();
        
       cout << "entry: " << j << " points = " << ne << endl;
        
        if (j == 0) {
            cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j << endl;
            
            for (int i(0); i < ne; i++) {
                 point = (SpdZdcPoint*)fT->At(i);            
                 if (i < 2) point->Print("");   
            }
            
        }
        if (j == entries-1) {
            cout << ">>>>>>>>>>>>>>>>> Print LAST entry:  " << j << endl;
          
            for (int i(0); i < ne; i++) {
                point = (SpdZdcPoint*)fT->At(i);            
                if (i < 2) point->Print("");   
            }
        }
   }
   
   cout << "Total number of ZDC-point COLLECTIONS in the TREE = " << entries << endl;    
}

//______________________________________________________________________________
void check_points_rstb2() 
{
   // 
   if (!t) return;  
    
   TBranch* branch = t->GetBranch("SpdRsTB2Point");
   if (!branch) return;
   
   cout << "\n+++++++++++++++++ <check_points_rstb> ++++++++++++++++++";
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;  
  
   TClonesArray *fT = 0; 
   t->SetBranchAddress("SpdRsTB2Point",&fT) ;
   
   int entries = branch->GetEntries();
  
   cout << "Total number of RSTB2-point COLLECTIONS in the TREE = " << entries << endl;    
   
   if (entries == 0) return;
   
   SpdRsTB2Point* point;
   Int_t ne;

   for (int j(0); j <entries; j++) {
     
        branch->GetEntry(j);      
        
        ne = fT->GetEntriesFast();
        
        cout << "entry: " << j << " points = " << ne << endl;
        
        if (j == 0) {
            cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j << endl;
            
            for (int i(0); i < ne; i++) {
                 point = (SpdRsTB2Point*)fT->At(i);            
                 if (i < 2) point->Print("");   
            }
            
        }
        
        if (j == entries-1) {
            cout << ">>>>>>>>>>>>>>>>> Print LAST entry:  " << j << endl;
          
            for (int i(0); i < ne; i++) {
                point = (SpdRsTB2Point*)fT->At(i);            
                if (i < 2) point->Print("");   
            }
        }
   }
   
   cout << "Total number of RSTB2-point COLLECTIONS in the TREE = " << entries << endl;    
}

//______________________________________________________________________________
void check_points_rstec2() 
{
   // 
   if (!t) return;  
    
   TBranch* branch = t->GetBranch("SpdRsTEC2Point");
   if (!branch) return;
   
   cout << "\n+++++++++++++++++ <check_points_rstec> +++++++++++++++++";
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;  
  
   TClonesArray *fT = 0; 
   t->SetBranchAddress("SpdRsTEC2Point",&fT) ;
   
   int entries = branch->GetEntries();
  
   cout << "Total number of RSTEC2-point COLLECTIONS in the TREE = " << entries << endl;    
   
   if (entries == 0) return;
   
   SpdRsTEC2Point* point;
   Int_t ne;

   for (int j(0); j <entries; j++) {
     
        branch->GetEntry(j);      
        
        ne = fT->GetEntriesFast();
        
        cout << "entry: " << j << " points = " << ne << endl;
        
        if (j == 0) {
            cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j << endl;
            
            for (int i(0); i < ne; i++) {
                 point = (SpdRsTEC2Point*)fT->At(i);            
                 if (i < 2) point->Print("");   
            }
            
        }
        
        if (j == entries-1) {
            cout << ">>>>>>>>>>>>>>>>> Print LAST entry:  " << j << endl;
          
            for (int i(0); i < ne; i++) {
                point = (SpdRsTEC2Point*)fT->At(i);            
                if (i < 2) point->Print("");   
            }
        }
   }
   
   cout << "Total number of RSTEC2-point COLLECTIONS in the TREE = " << entries << endl;    
}

//______________________________________________________________________________
void check_points_ecaltb2() 
{
   // 
   if (!t) return;  
    
   TBranch* branch = t->GetBranch("SpdEcalTB2Point");
   if (!branch) return;
   
   cout << "\n+++++++++++++++ <check_points_ecaltb> +++++++++++++++++";
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;  
  
   TClonesArray *fT = 0; 
   t->SetBranchAddress("SpdEcalTB2Point",&fT) ;
   
   int entries = branch->GetEntries();
  
   cout << "Total number of ECALTB2-point COLLECTIONS in the TREE = " << entries << endl;    
   
   if (entries == 0) return;
   
   SpdEcalTB2Point* point;
   Int_t ne;

   for (int j(0); j <entries; j++) {
     
        branch->GetEntry(j);      
        
        ne = fT->GetEntriesFast();
        
        cout << "entry: " << j << " points = " << ne << endl;
        
        if (j == 0) {
            cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j << endl;
            
            for (int i(0); i < ne; i++) {
                 point = (SpdEcalTB2Point*)fT->At(i);            
                 if (i < 2) point->Print("");   
            }
            
        }
        
        if (j == entries-1) {
            cout << ">>>>>>>>>>>>>>>>> Print LAST entry:  " << j << endl;
          
            for (int i(0); i < ne; i++) {
                point = (SpdEcalTB2Point*)fT->At(i);            
                if (i < 2) point->Print("");   
            }
        }
   }
   
   cout << "Total number of ECALTB2-point COLLECTIONS in the TREE = " << entries << endl;    
}

//______________________________________________________________________________
void check_points_ecaltec2() 
{
   // 
   if (!t) return;  
    
   TBranch* branch = t->GetBranch("SpdEcalTEC2Point");
   if (!branch) return;
   
   cout << "\n+++++++++++++++ <check_points_ecaltec> +++++++++++++++++";
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;  
  
   TClonesArray *fT = 0; 
   t->SetBranchAddress("SpdEcalTEC2Point",&fT) ;
   
   int entries = branch->GetEntries();
  
   cout << "Total number of ECALTEC2-point COLLECTIONS in the TREE = " << entries << endl;    
   
   if (entries == 0) return;
   
   SpdEcalTEC2Point* point;
   Int_t ne;

   for (int j(0); j <entries; j++) {
     
        branch->GetEntry(j);      
        
        ne = fT->GetEntriesFast();
        
        cout << "entry: " << j << " points = " << ne << endl;
        
        if (j == 0) {
            cout << ">>>>>>>>>>>>>>>>> Print FIRST entry: " << j << endl;
            
            for (int i(0); i < ne; i++) {
                 point = (SpdEcalTEC2Point*)fT->At(i);            
                 if (i < 2) point->Print("");   
            }
            
        }
        
        if (j == entries-1) {
            cout << ">>>>>>>>>>>>>>>>> Print LAST entry:  " << j << endl;
          
            for (int i(0); i < ne; i++) {
                point = (SpdEcalTEC2Point*)fT->At(i);            
                if (i < 2) point->Print("");   
            }
        }
   }
   
   cout << "Total number of ECALTEC2-point COLLECTIONS in the TREE = " << entries << endl;    
}

//______________________________________________________________________________
void check_geotracks() {
   //   
   cout << "\n+++++++++++++++++ <check_geotracks> ++++++++++++++++++"; 
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
            cout << ">>>>>>>>>>>>>>>>> Print LAST entry:  " << j 
                 << ",  tracks = " << tracks->GetEntriesFast() << endl;
        }
   }
  
   cout << "Total number of TRACK COLLECTIONS in the TREE = " << entries << endl; 
}

//________________________________________________________________________
void readparams(TString stype)
{
  cout << "\n+++++++++++++++++ <readparams> ++++++++++++++++++"; 
  cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl; 
   
  TFile* fp = new TFile(inParFile);
  if (!fp) return;
  
  if (stype.IsWhitespace() || stype == "tor" || stype == "hyb") 
  { 
     /* TS BARREL */
     SpdParSet* tstb_pars = (SpdParSet*)fp->Get("TsTBParSet");
     if (tstb_pars) tstb_pars->print(1);
     
     /* TS ENDCAPS */     
     SpdParSet* tstec_pars = (SpdParSet*)fp->Get("TsTECParSet");
     if (tstec_pars) tstec_pars->print(1);
     
     /* TOF BARREL */
     SpdParSet* tofb_pars = (SpdParSet*)fp->Get("TofBParSet");
     if (tofb_pars) tofb_pars->print(1);
     
     /* TOF ENDCAPS */
     SpdParSet* tofec_pars = (SpdParSet*)fp->Get("TofECParSet");
     if (tofb_pars) tofec_pars->print(1);
     
     /* ECAL BARREL */
     SpdParSet* ecaltb_pars = (SpdParSet*)fp->Get("EcalTBParSet");
     if (ecaltb_pars) ecaltb_pars->print(1);
     
     /* ECAL ENDCAPS */
     SpdParSet* ecaltec_pars = (SpdParSet*)fp->Get("EcalTECParSet");
     if (ecaltec_pars) ecaltec_pars->print(1);
     
     /* RS BARREL */
     SpdParSet* rstb_pars = (SpdParSet*)fp->Get("RsTBParSet");
     if (rstb_pars) rstb_pars->print(1);
            
     /* RS ENDCAPS */
     SpdParSet* rstec_pars = (SpdParSet*)fp->Get("RsTECParSet");
     if (rstec_pars) rstec_pars->print(1);
     
     /* INNER TRACKER SYSTEM (ITS) */
     SpdParSet *its_pars = (SpdParSet*)fp->Get("ItsParSet");
     if (its_pars) its_pars->print(1);
     
     /* ZERO-DEGREE CALORIMETER (ZDC) */
     SpdParSet *zdc_pars = (SpdParSet*)fp->Get("ZdcParSet");
     if (zdc_pars) zdc_pars->print(1);
     
     /* BEAM-BEAM COUNTER (BBC) */
     SpdParSet *bbc_pars = (SpdParSet*)fp->Get("BbcParSet");
     if (bbc_pars) bbc_pars->print(1);
     
     /* AEROGEL (AEG) */
     SpdParSet *aeg_pars = (SpdParSet*)fp->Get("AegParSet");
     if (aeg_pars) aeg_pars->print(1);
  }
  else if (stype == "sol") {
    
     /* TS BARREL */
     SpdParSet* tssb_pars = (SpdParSet*)fp->Get("TsSBParSet");
     if (tssb_pars) tssb_pars->print(1);
     
     /* ECAL BARREL */
     SpdParSet* ecalsb_pars = (SpdParSet*)fp->Get("EcalSBParSet");
     if (ecalsb_pars) ecalsb_pars->print(1);
     
     /* RS BARREL */
     SpdParSet* rssb_pars = (SpdParSet*)fp->Get("RsSBParSet");
     if (rssb_pars) rssb_pars->print(1);
     
     /* TS ENDCAPS */
     SpdParSet* tssec_pars = (SpdParSet*)fp->Get("TsSECParSet");
     if (tssec_pars) tssec_pars->print(1);
     
     /* ECAL ENDCAPS */
     SpdParSet *ecalsec_pars = (SpdParSet*)fp->Get("EcalSECParSet");
     if (ecalsec_pars) ecalsec_pars->print(1);
     
     /* RS ENDCAPS */
     SpdParSet *rssec_pars = (SpdParSet*)fp->Get("RsSECParSet");
     if (rssec_pars) rssec_pars->print(1);
  }
  
  /* PRIMARY GENERATOR */
  SpdPrimGenParSet *gen_pars = (SpdPrimGenParSet*)fp->Get("PrimGenParSet");
  if (gen_pars) gen_pars->print(1);
  
  fp->Close();
  
  cout << "\n+++++++++++++++++ <readparams> ++++++++++++++++++"; 
  cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl; 
}
