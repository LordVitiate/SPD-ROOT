TTree* Tree_ = 0;

SpdParSet*         EcalTBPars_;
SpdPrimGenParSet*  GenPars_;

TGeoNavigator* Ntor_ = 0; 

SpdMCEventHeader*  Header_    = 0;
TClonesArray*      MCTracks_  = 0;
TClonesArray*      EcalPoints_ = 0;

Int_t  GeoLevel_ = 0;

Bool_t  LoadGeometryAndPars(TString inParFile);
Bool_t  RegisterRunData(TString inFile);

void    AnalyseEvent(Int_t nevent);
Bool_t  AnalysePoint(SpdEcalTB2Point* point);

TTree* t = 0;

TString inFile, inParFile;

//________________________________________________________________________________
void CheckEcalTB2Points() {
    
  TString inParFile  = "test_params.root";
  TString inDataFile = "test_data.root";   
  
  if (!LoadGeometryAndPars(inParFile)) {
      cout << "Bad geometry or parameters " << endl;
      return;
  }

  if (!RegisterRunData(inDataFile)) {
      cout << "Bad data in the file " << endl;
      return;
  }
  
  Int_t entries = Tree_->GetEntries();
  
  cout << "Entries(tree): " << entries << endl;
  
  if (entries == 0) return;
   
  //return;
  
  cout << "\n>>>>>>>>>>>>>>>> DATA : Ok! <<<<<<<<<<<<<<<<< " << endl;
   
  //entries = 1; // FIXME
  
  for (Int_t i(0); i < entries; i++) {
       Tree_->GetEntry(i);
       AnalyseEvent(i);
  }
}

//________________________________________________________________________________
void AnalyseEvent(Int_t nevent)
{
    
//   Header_->Print(1);
   
   //-------------------------- MC-TRACKS ---------------------------------
//   Int_t ntracks = MCTracks_->GetEntriesFast();
       
//    cout << "\n<Analyse> mctracks = " << ntracks << endl;
// 
//    SpdMCTrack* track;
//    for (int i(0); i < ntracks; i++) {
//         track = (SpdMCTrack*)MCTracks_->At(i);              
//         //if (i < 30) track->Print(i);   
//         if (track->GetMotherId() < 0) track->Print(i);
//    }
   
   //--------------------------- EcalTB POINTS ---------------------------
  
   Int_t npoints = EcalPoints_->GetEntriesFast();
  
   cout << "\n<Analyse> ecalQEC-points = " << npoints << " event: " << nevent << endl;
   
   SpdEcalTB2Point* point;
   
   for (Int_t i(0); i < npoints; i++) {
        
        point = (SpdEcalTB2Point*)EcalPoints_->At(i); 
        
        AnalysePoint(point);
   }
}

//________________________________________________________________________________
Bool_t AnalysePoint(SpdEcalTB2Point* point)
{
   if (!point) return false;
   
   cout << ">>>>>>>>>>>>>>>>> ANALYSE POINT >>>>>>>>>>>>>>>>>>>>>" << "\n\n";
   
   point->Print("");   
    
   TString node_path = point->GetNodePath();
   
   if (!Ntor_->cd(node_path)) {
       cout << "No such node: " << node_path << endl;
       return false;  
   }
   else cout << "Current node path: " << Ntor_->GetPath() << "\n\n";
      
   cout << "Energy deposited: " << point->GetEloss() << "\n";
   
   SpdGeopathParser ppath;
   ppath.ParsePath(node_path);
   
   cout << "\n";

   return true;  
}

//________________________________________________________________________________
Bool_t LoadGeometryAndPars(TString inParFile)
{
  TFile* fp = new TFile(inParFile);
  if (!fp) return kFALSE;
      
  /* PRIMARY GENERATOR PARAMETERS */
  
  GenPars_ = (SpdPrimGenParSet*)fp->Get("PrimGenParSet");
  if (!GenPars_) {
      cout << "No PrimGen parameters " << endl;
      return kFALSE;
  }
      
  //GenPars_->print(1);

  /* RANGE SYSTEM BARREL (EcalTB) PARAMETERS */
  
  EcalTBPars_ = (SpdParSet*)fp->Get("EcalTBParSet");
  if (!EcalTBPars_) {
      cout << "No ECAL parameters " << endl;
      return kFALSE;
  }
  
  //EcalTBPars_->print(1);
  
  /* +++++++++++++++ LOAD GEOMETRY  +++++++++++++++ */

  FairGeoParSet* geoset = (FairGeoParSet*)fp->Get("FairGeoParSet");
  
  Ntor_ = gGeoManager->GetCurrentNavigator();
  
  /* DRAW GEOMETRY */
  //gGeoManager->SetVisLevel(2);
  //gGeoManager->GetMasterVolume()->Draw("ogl");
   
  return kTRUE;
}

//________________________________________________________________________________
Bool_t RegisterRunData(TString inFile)
{
  TFile* f = new TFile(inFile);
  if (!f) return kFALSE;
  
  TTree* t = (TTree*)f->Get("cbmsim");  
  if (!t) return kFALSE;

  TBranch* branch;
  
  branch = t->GetBranch("MCEventHeader.");
  if (!branch) return kFALSE;
  t->SetBranchAddress("MCEventHeader.",&Header_) ;
  
  //branch = t->GetBranch("SpdMCTrack");
  //if (!branch) return kFALSE;
  //t->SetBranchAddress("SpdMCTrack",&MCTracks_);       
  
  branch = t->GetBranch("SpdEcalTB2Point");
  if (!branch) return kFALSE;
  t->SetBranchAddress("SpdEcalTB2Point",&EcalPoints_) ;
 
  cout << "\n>>>>>>>>>>>>>>>> FILE & TREE: Ok! <<<<<<<<<<<<<<<<< " << endl;
  
  Tree_ = t;
  
  return kTRUE;
}
