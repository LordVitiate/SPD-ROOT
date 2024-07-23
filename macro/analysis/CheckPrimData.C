
void check_prim();

TTree* t = 0;

TString inFile, inParFile;

void print_track(Int_t i, TParticle* primpart, SpdMCTrack* track, SpdPrimGenData* gendata);

//________________________________________________________________________________
void CheckPrimData(TString path = "", TString stype = "") {
  //
  if (!path.IsWhitespace() && !path.EndsWith("/")) path += "/";
  
  // Input data & parameters files   
  if (stype.IsWhitespace()) 
  {
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
      cout << stype.IsWhitespace() << endl;
      cout << "<CheckOutputData> Unknown data type: " << stype << endl;
      return;
  }
 
  TFile* f = new TFile(inFile);
  if (!f) return;
  
  t = (TTree*)f->Get("spdsim");  
  if (!t) return;
  
  cout << "\n >>>>>>>>>>>>>>>> DATA FILE & TREE: Ok! <<<<<<<<<<<<<<<<< " << endl;
 
  check_prim();
}


//______________________________________________________________________________
void check_prim() {
   //       
   cout << "\n+++++++++++++++++ <check_prim> ++++++++++++++++++";
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
   
   if (!t) return;  
   
   TBranch* hbranch = t->GetBranch("MCEventHeader.");
   if (!hbranch) return;
   
   TBranch* tbranch = t->GetBranch("SpdMCTrack");
   if (!tbranch) return;
   
   int hentries = hbranch->GetEntries();
   
   cout << "Total number of HEADERS in the TREE = " << hentries << endl;    
   if (hentries == 0) return;
   
   int tentries = tbranch->GetEntries();
  
   cout << "Total number of MCTRACKS in the TREE = " << tentries << endl;    
   if (tentries == 0) return;
   
   SpdMCEventHeader* header = 0;
   t->SetBranchAddress("MCEventHeader.",&header) ;       
   
   TClonesArray *fT = 0; 
   t->SetBranchAddress("SpdMCTrack",&fT) ;       
   
   TParticle* part;
   SpdPrimGenData* gendata;
   SpdMCTrack* track;
  
   Int_t np(0);
   Int_t nprim, ne;
   
   for (int j(0); j < tentries; j++)   {
        
        hbranch->GetEntry(j);  
        tbranch->GetEntry(j);
        
        nprim = header->GetNPrim();
        ne = fT->GetEntriesFast();
        
        cout << "\nentry: " << j << " primaries: " << nprim << " mctracks = " << ne << endl;
        
        if (j == 0) {
               
           cout << "\n>>>>>>>>>>>>>>>>> Print FIRST entry: " << j << " Primaries: " <<  nprim << " Tracks = " << ne << endl;
           
           header->Print(1);
           
           for (int i(0); i < ne; i++) {
                
                track = (SpdMCTrack*)fT->At(i);
                
                gendata = header->FindGenData(track->GetSecondMotherId());
                part = header->FindParticle(track->GetSecondMotherId());
                
                np += track->GetTotalPoints();                   
          
                if (track->GetMotherId() < 0) print_track(i,part,track,gendata);
                else { if (i < nprim+10) track->Print(i); }
           }
           
          cout << "Total number of points inside: " << np << "\n\n"; 
        }
        
        if (j == tentries-1) {
             
           cout << "\n>>>>>>>>>>>>>>>>> Print LAST entry:  " << j << " Primaries: " << nprim << " Tracks = " << ne << endl;
           
           header->Print(1);
           
           for (int i(0); i < ne; i++) {
             
                track = (SpdMCTrack*)fT->At(i); 
                
                gendata = header->FindGenData(track->GetSecondMotherId());
                part = header->FindParticle(track->GetSecondMotherId());
                 
                np += track->GetTotalPoints();  
                    
                if (track->GetMotherId() < 0) print_track(i,part,track,gendata);
                else { if (i < nprim+10) track->Print(i); }
                
           }
           
           cout << "Total number of points inside: " << np << "\n\n"; 
        }    
   }
        
   cout << "Total Number of MCTRACK COLLECTIONS in the TREE = " << tentries << endl;   
}

//______________________________________________________________________________
void print_track(Int_t i, TParticle* primpart, SpdMCTrack* track, SpdPrimGenData* gendata)
{
    if (!gendata) return;
    if (!primpart || !track) return;

    cout << "MC-TRACK DATA / PRIMARY GENERATOR PARTICLE DATA [!CHECKING!]: " << "\n\n";
    cout << "Id       :   " << i << "/" << primpart->GetSecondMother() << endl; 
    cout << "Pdg      :   " << track->GetPdgCode() << "/" << primpart->GetPdgCode() << endl;
    cout << "Momentum :   " << "(" << track->GetPx() << ", " << track->GetPy() << ", " << track->GetPz() 
                            << ") / (" 
                            << primpart->Px() << ", " << primpart->Py() << ", " << primpart->Pz()  << ") "<< endl;
    cout << "Vertex   :   " << "(" << track->GetStartX() << ", " << track->GetStartY() << ", " << track->GetStartZ() 
                            << ", " << track->GetStartT()
                            << ") / (" 
                            << primpart->Vx() << ", " << primpart->Vy() << ", " << primpart->Vz() 
                            << ", " << primpart->T() << ") "<< endl;
    cout << endl;
     
    cout << "MC-TRACK DATA / PRIMARY GENERATOR PARTICLE DATA [!INFO!]: " << "\n\n";
    
    cout << "Generator type :  " << "-" << " / " << gendata->GetGenType() << endl;      
     
    cout << "Mother(1)      :  " << track->GetFirstMotherId() << " / " << primpart->GetFirstMother()
                                               << " (vertex mother id = -1 / mother particle number in the GENERATOR'S LIST OF PRIMARIES) " 
                                               << endl;
    cout << "Mother(2)      :  " << track->GetSecondMotherId() << " / " << primpart->GetSecondMother() 
                                               << " (primary data track number / mc-track number) !>>> CORRESPONDENCE TABLE <<<! " << endl;
                                     
    cout << "Daughter(1)    :  " << "-" << " / " << primpart->GetFirstDaughter() 
                                               << " ( - / primary generator number)" << endl;
    cout << "Daughter(2)    :  " << "-" << " / " << primpart->GetLastDaughter()  
                                               << " ( - / primary data track number = mc-track second mother) " << endl;                              
    cout << "-------------------------------------------------------------------------- " << endl;
    
    //gendata->Print("");
    //cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ " << endl;
}

