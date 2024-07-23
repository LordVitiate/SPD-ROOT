
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

#include "SpdItsGeoMapperX.h"
#include "SpdItsGeoBuilder.h"
#include "SpdItsPoint.h"

#endif

TTree* Tree_ = 0;

SpdParSet*         ItsPars_;
SpdPrimGenParSet*  GenPars_;

SpdItsGeoMapperX*  ItsGeoMapper_;
SpdItsGeoBuilder*  ItsGeoBuilder_;

SpdMCEventHeader*  Header_    = 0;
TClonesArray*      MCTracks_  = 0;
TClonesArray*      ItsPoints_ = 0;

Int_t  GeoLevel_ = 0;

Bool_t  LoadParameters(TString inParFile);
Bool_t  RegisterRunData(TString inFile);

void    AnalyseEvent();
void    ScanNodeGeo(Long_t tid);
void    PrintCurrentNode(const Char_t* title);

//________________________________________________________________________________
void CheckItsData() 
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
   
   //--------------------------- ITS POINTS -------------------------------
   Int_t nitspoints = ItsPoints_->GetEntriesFast();
        
   cout << "\n<Analyse> its-points = " << nitspoints << endl;
         
   nitspoints = 3;  // FIXME
   
   SpdItsPoint* point;
   
   Int_t nchannels;           // number of hitted channels in a chip
   Int_t vid, iz_ch, iphi_ch; // chip channel id's 
   Double_t vedep;            // chip channel energy deposit (GeV)
   
   Double_t* pos_chip = new Double_t[3];  // chip global position (x,y,z)
   Double_t* pos_chan = new Double_t[3];  // chip channel global position (x,y,z)
   
   TString mothername;        // chip volume name
   SpdGeoVVolumeBox2D* vchip; // chip vvolume
     
   for (Int_t i(0); i < nitspoints; i++) {
        
        point = (SpdItsPoint*)ItsPoints_->At(i); 
        
        //--------------- SCAN SENSITIVE NODE  ----------------------------
        
        ScanNodeGeo(point->GetNodeId());
        
        //--------------- PRINT POINT INFO  -------------------------------
        
        point->Print("");   
        
        //--------------- EXTRACT CHANNEL DATA  ---------------------------
        
        cout << ">>>>>> Extract chip data: >>>>>>" << endl;
        
        ItsGeoBuilder_->SetNodePath(point->GetNodeId()); // set node (chip)
    
        ItsGeoBuilder_->GetDetectorPosition(pos_chip);   // get chip position
        
        cout << "chip full path:     " << ItsGeoBuilder_->GetNodePath() << endl;
        
        mothername = ItsGeoBuilder_->GetNodeVolumeName();
        vchip = dynamic_cast<SpdGeoVVolumeBox2D*>(ItsGeoMapper_->GetVVolume(mothername));
  
        if (vchip) {
            cout << "chip name:          " << vchip->GetName() << endl;
            cout << "chip mother volume: " << vchip->GetMotherVolume() << endl;
        }
        
        printf("chip position:      (%10.6f, %10.6f, %10.6f) \n\n",
               pos_chip[0],pos_chip[1],pos_chip[2]);
           
        nchannels = point->GetVDataSize();
        
        printf("%5s | %12s   %5s %5s | %12s    %s \n\n",
               "I","Node_ID","iz","iphi","Edep [keV]","Channel position (x,y,z) [cm]");
          
        for (Int_t j(0); j < nchannels; j++) 
        {
             vid   = point->GetVid(j);
             vedep = point->GetVed(j);
             
             iz_ch = -1; 
             iphi_ch = -1;   
             
             pos_chan[0] = 0;
             pos_chan[1] = 0;
             pos_chan[2] = 0;
             
             if (vchip) vchip->CellId(vid, iz_ch, iphi_ch);
             
             ItsGeoBuilder_->SetVid(vid);// set channel
             
             ItsGeoBuilder_->GetDetectorPosition(pos_chan); // get channel position
                        
             printf("%5d | %12d : %5d %5d | %12.6f   (%10.6f, %10.6f, %10.6f) \n",
                    j, vid, iz_ch, iphi_ch, vedep*1e6,
                    pos_chan[0], pos_chan[1], pos_chan[2]
                   );
        }
       
        cout << "----------------------------------------------";
        cout << "----------------------------------------------" << endl;
   }         
}

//________________________________________________________________________________
void ScanNodeGeo(Long_t tid)
{
   cout << ">>>>>>>>>>>>>>>>> SCAN NODE GEOMETRY >>>>>>>>>>>>>>>>>>>>>" << "\n\n";
   
   ItsGeoBuilder_->SetNodePath(tid);  // set chip node
   
   TString node_path = ItsGeoBuilder_->GetNodePath();  // get chip "path"
   
   /* parse chip info  */
   
   SpdGeopathParser parser;
   
   cout << "Node id / Volume name:   " 
        << tid << " / " << parser.GetVolumeName(node_path) << "\n\n";
   
   parser.ParsePath(node_path);
   
   Int_t layer_copy_number  = parser.Num(2);
   Int_t ladder_copy_number = parser.Num(3);
   Int_t chip_copy_number   = parser.Num(4);
   
   Int_t layer_vol_number  = parser.VNum(2);
   Int_t ladder_vol_number = parser.VNum(3);
   Int_t chip_vol_number   = parser.VNum(4);
   
   TString layer_path  = parser.Path(2);
   TString ladder_path = parser.Path(3);
   TString chip_path   = parser.Path(4);
   
   cout << "chip   path:       " << chip_path   << endl;
   cout << "ladder path:       " << ladder_path << endl;
   cout << "layer  path:       " << layer_path  << endl; 
   
   cout << endl;
   
   cout << "chip   name:       " << parser.Name(4) << endl;
   cout << "ladder name:       " << parser.Name(3) << endl;
   cout << "layer  name:       " << parser.Name(2) << endl; 
   
   cout << endl;
   
   cout << "chip   [volume/copy] number:   " << chip_vol_number   << " / " << chip_copy_number   << endl;
   cout << "ladder [volume/copy] number:   " << ladder_vol_number << " / " << ladder_copy_number << endl;
   cout << "layer  [volume/copy] number:   " << layer_vol_number  << " / " << layer_copy_number  << endl;
    
   cout << endl;
   
   if (layer_copy_number != 0) 
       cout << "LAYER  NUMBER:    " << layer_vol_number   << endl;
   if (ladder_copy_number != 0) 
       cout << "LADDER NUMBER:    " << ladder_copy_number << endl;
   
   cout << endl;
   
   /* scan nodes */
     
   if (chip_copy_number != 0) {
       PrintCurrentNode("chip");
   }
   if (ladder_copy_number != 0) {
       ItsGeoBuilder_->SetNodePath(ladder_path); // set ladder node 
       PrintCurrentNode("ladder");
   }    
   if (layer_copy_number != 0) {
       ItsGeoBuilder_->SetNodePath(layer_path); // set layer node 
       PrintCurrentNode("layer");
   }
     
   cout << "<<<<<<<<<<<<<<<<< SCAN NODE GEOMETRY <<<<<<<<<<<<<<<<<<<<<" << "\n\n";
}

//________________________________________________________________________________
void PrintCurrentNode(const Char_t* title)
{
   cout << ">>>>>>>>>>>> scan " << title << " >>>>>>>>>>>> \n" << endl;
   
   const TGeoVolume* volume = ItsGeoBuilder_->GetNodeVolume(); 
   if (volume) ItsGeoBuilder_->PrintGeoVolume(volume);
   
   const SpdGeoVolPars* volpars = ItsGeoMapper_->GetVolPars(volume->GetName());
   if (volpars) volpars->PrintGeoVolPars();
   
   const SpdGeoVVolume* vvolume = ItsGeoMapper_->GetVVolume(volume->GetName());
   if (vvolume) vvolume->PrintGeoVVolume();
   
   Double_t node_pos[3];  // node position (x,y,z)
    
   ItsGeoBuilder_->GetDetectorPosition(node_pos);  
   printf("%s position (X,Y,Z):    (%12.6f, %12.6f, %12.6f)  [cm, cm, cm]  \n",
           title,node_pos[0],node_pos[1],node_pos[2]);
   
   ItsGeoBuilder_->GetDetectorPositionRPZ(node_pos);    
   printf("%s position (R,Phi,Z):  (%12.6f, %12.6f, %12.6f)  [cm, deg, cm] \n",
           title,node_pos[0],node_pos[1],node_pos[2]);
   
   Double_t node_dir[3];  // node orientation (x,y,z-axis)
   
   ItsGeoBuilder_->GetDetectorOrientation(node_dir,'x');  
   printf("%s X-axis:              (%10.6f, %10.6f, %10.6f) \n",
           title,node_dir[0],node_dir[1],node_dir[2]);
   
   ItsGeoBuilder_->GetDetectorOrientation(node_dir,'y'); 
   printf("%s Y-axis:              (%10.6f, %10.6f, %10.6f) \n",
           title,node_dir[0],node_dir[1],node_dir[2]);
   
   ItsGeoBuilder_->GetDetectorOrientation(node_dir,'z'); 
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
  
  branch = t->GetBranch("SpdItsPoint");
  if (!branch) return kFALSE;
  t->SetBranchAddress("SpdItsPoint",&ItsPoints_) ;
 
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

  /* INNER TRACKER SYSTEM (ITS) PARAMETERS */
  
  ItsPars_ = (SpdParSet*)fp->Get("ItsParSet");
  if (!ItsPars_) {
      cout << "No ITS parameters " << endl;
      return kFALSE;
  }
  
  //ItsPars_->print(1);
 
  /* +++++++++++++++ EXTRACT ITS GEOMETRY  +++++++++++++++ */
 
  SpdCommonGeoMapper::Instance()->OpenGeometry();
   
  FairModule* Cave = new SpdCave();
  Cave->SetGeometryFileName("cave.geo");
  Cave->ConstructGeometry();
 
  FairBaseParSet* geoset = (FairBaseParSet*)fp->Get("FairBaseParSet");
  
  SpdDetector* det = SpdCommonGeoMapper::Instance()->SearchForActive(kSpdIts); 
  if (!det) return kFALSE;
 
  det->LoadParsFrom(ItsPars_);
  det->ConstructGeometry();
  
  ItsGeoMapper_  = dynamic_cast<SpdItsGeoMapperX*>(det->GetMapper());
  ItsGeoBuilder_ = dynamic_cast<SpdItsGeoBuilder*>(det->GetBuilder());
  
  if (!ItsGeoMapper_ || !ItsGeoBuilder_) return kFALSE;
  
  GeoLevel_ = ItsGeoBuilder_->GetActualGeoLevel();
  
  //ItsGeoMapper_->Print(""); 
  //ItsGeoMapper_->PrintVolPars();
  //ItsGeoBuilder_->Print(""); 
  
  //exit(1);
  
  /* DRAW GEOMETRY */
  gGeoManager->SetVisLevel(2);
  gGeoManager->GetMasterVolume()->Draw("ogl");
   
  return kTRUE;
}

// 8*9 + 15*17 + 20*21 + 22*25 + 26*29 = 2051
// 2051 * 640 * 320 = 420044800


