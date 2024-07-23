
//#define _COMPILE_MACRO_

#if defined(_COMPILE_MACRO_) 

#include <TStopwatch.h>
#include <TTree.h>
#include <TClonesArray.h>
#include <TGeoManager.h>

#include "FairBaseParSet.h"
#include "SpdPrimGenParSet.h"

#include "SpdCommonGeoMapper.h"
#include "SpdCave.h"
#include "SpdDetector.h"

#include "SpdMCEventHeader.h"
#include "SpdMCTrack.h"

#include "SpdTsTBGeoMapper.h"
#include "SpdTsTBGeoBuilder.h"
#include "SpdTsTBPoint.h"

#endif

TTree* Tree_ = 0;

SpdParSet*          TsTBPars_;
SpdPrimGenParSet*   GenPars_;

SpdTsTBGeoMapper*   TsTBGeoMapper_;
SpdTsTBGeoBuilder*  TsTBGeoBuilder_;

SpdMCEventHeader*   Header_     = 0;
TClonesArray*       MCTracks_   = 0;
TClonesArray*       TsTBPoints_ = 0;

Int_t  GeoLevel_ = 0;

Bool_t  LoadParameters(TString inParFile);
Bool_t  RegisterRunData(TString inFile);

void    AnalyseEvent();
void    ScanNodeGeo(Long_t tid);
void    PrintCurrentNode(const Char_t* title);

//________________________________________________________________________________
void CheckTsTBData() 
{
  // Input data & parameters files   

  TString inParFile  = "params.root";
  TString inDataFile = "run.root";   
  
  if (!LoadParameters(inParFile)) {
      cout << "Bad parameters " << endl;
      return;
  }
   
  if (!RegisterRunData(inDataFile)) {
      cout << "No enough data in the file " << endl;
      return;
  }
  
  Int_t entries = Tree_->GetEntries();
  
  cout << "Entries(tree): " << entries << endl;
  
  if (entries == 0) return;
   
  //return;
  
  cout << "\n>>>>>>>>>>>>>>>> DATA : Ok! <<<<<<<<<<<<<<<<< " << endl;
   
  entries = 1; // FIXME
  
  for (Int_t i(0); i < entries; i++) {
       Tree_->GetEntry(i);
       AnalyseEvent();
  }
}

//________________________________________________________________________________
void AnalyseEvent()
{
   Header_->Print(1);
   
   //-------------------------- MC-TRACKS ---------------------------------
   Int_t ntracks = MCTracks_->GetEntriesFast();
       
   cout << "\n<Analyse> mctracks = " << ntracks << endl;
        
   SpdMCTrack* track;
   for (int i(0); i < ntracks; i++) {
        track = (SpdMCTrack*)MCTracks_->At(i);              
        //if (i < 30) track->Print(i);   
        if (track->GetMotherId() < 0) track->Print(i);   
   }
   
   //--------------------------- TSTB POINTS -------------------------------
   Int_t ntstb_points = TsTBPoints_->GetEntriesFast();
        
   cout << "\n<Analyse> tstb-points = " << ntstb_points << endl;
   
   if (ntstb_points < 1) return;
  
   ntstb_points = 3;  // FIXME
   
   SpdTsTBPoint* point;
       
   for (Int_t i(0); i < ntstb_points; i++) {
        
        point = (SpdTsTBPoint*)TsTBPoints_->At(i); 
      
        //--------------- SCAN SENSITIVE NODE  ----------------------------
        
        ScanNodeGeo(point->GetDetectorTID());
        
        //--------------- PRINT POINT INFO  -------------------------------
        
        point->Print("");  
                    
        cout << "----------------------------------------------";
        cout << "----------------------------------------------" << endl;
   }         
}

//________________________________________________________________________________
void ScanNodeGeo(Long_t tid)
{
   cout << ">>>>>>>>>>>>>>>>> Scan node geometry <<<<<<<<<<<<<<<<<<" << "\n\n";
        
   TsTBGeoBuilder_->SetNodePath(tid);  // set straw node
   
   TString node_path = TsTBGeoBuilder_->GetNodePath(); // get straw "path"
     
   /* parse straw info  */
   
   SpdGeopathParser parser;
   
   cout << "Node id / Volume name:   " 
        << tid << " / " << parser.GetVolumeName(node_path) << "\n\n";
   
   parser.ParsePath(node_path);
   
   Int_t module_copy_number = parser.Num(2);
   Int_t layer_copy_number  = parser.Num(3);
   Int_t straw_copy_number  = parser.Num(4);
   
   Int_t module_vol_number = parser.VNum(2);
   Int_t layer_vol_number  = parser.VNum(3);
   Int_t straw_vol_number  = parser.VNum(4);
   
   TString module_path = parser.Path(2);
   TString layer_path  = parser.Path(3);
   TString straw_path  = parser.Path(4);
   
   cout << "straw  path:         " << straw_path  << endl;
   cout << "layer  path:         " << layer_path  << endl;
   cout << "module path:         " << module_path << endl; 
   
   cout << endl;
   
   cout << "straw  name:         " << parser.Name(4) << endl;
   cout << "layer  name:         " << parser.Name(3) << endl;
   cout << "module name:         " << parser.Name(2) << endl; 
   
   cout << endl;
   
   cout << "straw  [volume/copy] number:     " << straw_vol_number  << " / " << straw_copy_number << endl;
   cout << "layer  [volume/copy] number:     " << layer_vol_number  << " / " << layer_copy_number << endl;
   cout << "module [volume/copy] number:     " << module_vol_number << " / " << module_copy_number << endl;
   
   cout << endl;
   
   cout << "MODULE NUMBER:   " << module_copy_number << endl;
   cout << "LAYER NUMBER:    " << layer_vol_number << endl;
   
   cout << endl;
   
   /* scan nodes */
 
   if (straw_copy_number != 0) {
       PrintCurrentNode("straw");
   }
      
   if (layer_copy_number != 0) {
       TsTBGeoBuilder_->SetNodePath(layer_path); // set layer node 
       PrintCurrentNode("layer");
   }
   
   if (module_copy_number != 0) {
       TsTBGeoBuilder_->SetNodePath(module_path); // set module node 
       PrintCurrentNode("module");
   }
     
   cout << "\n<<<<<<<<<<<<<<<<< Scan node geometry <<<<<<<<<<<<<<<<<<<<<" << "\n\n";
}

//________________________________________________________________________________
void PrintCurrentNode(const Char_t* title)
{
   cout << ">>>>>>>>>>>> scan " << title << " >>>>>>>>>>>> \n" << endl;
   
   const TGeoVolume* volume = TsTBGeoBuilder_->GetNodeVolume(); 
   if (volume) TsTBGeoBuilder_->PrintGeoVolume(volume);
   
   const SpdGeoVolPars* volpars = TsTBGeoMapper_->GetVolPars(volume->GetName());
   if (volpars) volpars->PrintGeoVolPars();
   
   const SpdGeoVVolume* vvolume = TsTBGeoMapper_->GetVVolume(volume->GetName());
   if (vvolume) vvolume->PrintGeoVVolume();
   
   Double_t node_pos[3];  // node position (x,y,z)
    
   TsTBGeoBuilder_->GetDetectorPosition(node_pos);  
   printf("%s position (X,Y,Z):    (%12.6f, %12.6f, %12.6f)  [cm, cm, cm]  \n",
           title,node_pos[0],node_pos[1],node_pos[2]);
   
   TsTBGeoBuilder_->GetDetectorPositionRPZ(node_pos);    
   printf("%s position (R,Phi,Z):  (%12.6f, %12.6f, %12.6f)  [cm, deg, cm] \n",
           title,node_pos[0],node_pos[1],node_pos[2]);
   
   Double_t node_dir[3];  // node orientation (x,y,z-axis)
   
   TsTBGeoBuilder_->GetDetectorOrientation(node_dir,'x');  
   printf("%s X-axis:              (%10.6f, %10.6f, %10.6f) \n",
           title,node_dir[0],node_dir[1],node_dir[2]);
   
   TsTBGeoBuilder_->GetDetectorOrientation(node_dir,'y'); 
   printf("%s Y-axis:              (%10.6f, %10.6f, %10.6f) \n",
           title,node_dir[0],node_dir[1],node_dir[2]);
   
   TsTBGeoBuilder_->GetDetectorOrientation(node_dir,'z'); 
   printf("%s Z-axis:              (%10.6f, %10.6f, %10.6f) \n",
           title,node_dir[0],node_dir[1],node_dir[2]);
   
   cout << "\n<<<<<<<<<<<< scan " << title << " <<<<<<<<<<<<" << endl;
}

//________________________________________________________________________________
Bool_t RegisterRunData(TString inFile)
{
  TFile* f = new TFile(inFile);
  if (!f) return kFALSE;
  
  TTree* t = (TTree*)f->Get("spdsim");  
  if (!t) return kFALSE;

  TBranch* branch;
  
  branch = t->GetBranch("MCEventHeader.");
  if (!branch) return kFALSE;
  t->SetBranchAddress("MCEventHeader.",&Header_) ;
  
  branch = t->GetBranch("SpdMCTrack");
  if (!branch) return kFALSE;
  t->SetBranchAddress("SpdMCTrack",&MCTracks_);       
  
  branch = t->GetBranch("SpdTsTBPoint");
  if (!branch) return kFALSE;
  t->SetBranchAddress("SpdTsTBPoint",&TsTBPoints_) ;
 
  cout << "\n>>>>>>>>>>>>>>>> FILE & TREE: Ok! <<<<<<<<<<<<<<<<< " << endl;
  
  Tree_ = t;
  
  return kTRUE;
}

//________________________________________________________________________________
Bool_t LoadParameters(TString inParFile)
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

  /* INNER TRACKER SYSTEM (TSTB) PARAMETERS */
  
  TsTBPars_ = (SpdParSet*)fp->Get("TsTBParSet");
  if (!TsTBPars_) {
      cout << "No TSTB parameters " << endl;
      return kFALSE;
  }
  
  //TsTBPars_->print(1);
 
  /* +++++++++++++++ EXTRACT TSTB GEOMETRY  +++++++++++++++ */
 
  SpdCommonGeoMapper::Instance()->OpenGeometry();
  //SpdCommonGeoMapper::Instance()->DefineHybGeometrySet(); // FIXME!!!
   
  FairModule* Cave = new SpdCave();
  Cave->SetGeometryFileName("cave.geo");
  Cave->ConstructGeometry();
 
  FairBaseParSet* geoset = (FairBaseParSet*)fp->Get("FairBaseParSet");
  
  SpdDetector* det = SpdCommonGeoMapper::Instance()->SearchForActive(kSpdTsTB); 
  if (!det) return kFALSE;
 
  det->LoadParsFrom(TsTBPars_);
  det->ConstructGeometry();
  
  TsTBGeoMapper_  = dynamic_cast<SpdTsTBGeoMapper*>(det->GetMapper());
  TsTBGeoBuilder_ = dynamic_cast<SpdTsTBGeoBuilder*>(det->GetBuilder());
  
  if (!TsTBGeoMapper_ || !TsTBGeoBuilder_) return kFALSE;
  
  GeoLevel_ = TsTBGeoBuilder_->GetActualGeoLevel();
  
  //TsTBGeoMapper_->Print(""); 
  //TsTBGeoMapper_->PrintVolPars();
  //TsTBGeoBuilder_->Print(""); 
  
  //exit(1);
  
  /* DRAW GEOMETRY */
  gGeoManager->SetVisLevel(2);
  gGeoManager->GetMasterVolume()->Draw("ogl");
   
  return kTRUE;
}


