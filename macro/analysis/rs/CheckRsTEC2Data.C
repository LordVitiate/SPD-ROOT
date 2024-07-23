// #define _COMPILE_MACRO_
// 
// #if defined(_COMPILE_MACRO_) 

#include <TStopwatch.h>
#include <TTree.h>
#include <TClonesArray.h>
#include <TGeoManager.h>
#include <TGeoNavigator.h>

#include "FairGeoParSet.h"
#include "FairBaseParSet.h"
#include "SpdPrimGenParSet.h"

#include "SpdCommonGeoMapper.h"
#include "SpdCave.h"
#include "SpdDetector.h"

#include "SpdMCEventHeader.h"
#include "SpdMCTrack.h"

#include "SpdRsTEC2.h"
#include "SpdRsTEC2Point.h"

//#endif

TTree* Tree_ = 0;

SpdParSet*         RsTECPars_;
SpdPrimGenParSet*  GenPars_;

TGeoNavigator* Ntor_ = 0; 

SpdMCEventHeader*  Header_    = 0;
TClonesArray*      MCTracks_  = 0;
TClonesArray*      RsTECPoints_ = 0;

Int_t  GeoLevel_ = 0;

Bool_t  LoadGeometryAndPars(TString inParFile);
Bool_t  RegisterRunData(TString inFile);

void    AnalyseEvent(Int_t nevent);
Bool_t  AnalysePoint(SpdRsTEC2Point* point);

Bool_t  DetectorPosition(Double_t* p);
Bool_t  DetectorOrientation(Double_t* v, Char_t dir);

TTree* t = 0;

TString inFile, inParFile;

//________________________________________________________________________________
void CheckRsTEC2Data() {
  //  
  TString inParFile  = "params_tor.root";
  TString inDataFile = "run_tor.root";   
  
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
   Int_t ntracks = MCTracks_->GetEntriesFast();
       
//    cout << "\n<Analyse> mctracks = " << ntracks << endl;
// 
//    SpdMCTrack* track;
//    for (int i(0); i < ntracks; i++) {
//         track = (SpdMCTrack*)MCTracks_->At(i);              
//         //if (i < 30) track->Print(i);   
//         if (track->GetMotherId() < 0) track->Print(i);
//    }
   
   //--------------------------- RsTEC POINTS -------------------------------
  
   Int_t npoints = RsTECPoints_->GetEntriesFast();
  
   cout << "\n<Analyse> rstec-points = " << npoints << " event: " << nevent << endl;
   
   SpdRsTEC2Point* point;
   
   for (Int_t i(0); i < npoints; i++) {
        
        point = (SpdRsTEC2Point*)RsTECPoints_->At(i); 
        
        AnalysePoint(point);

   }
}

//________________________________________________________________________________
Bool_t AnalysePoint(SpdRsTEC2Point* point)
{
   if (!point) return false;
   
   cout << ">>>>>>>>>>>>>>>>> ANALYSE POINT >>>>>>>>>>>>>>>>>>>>>" << "\n\n";
   
   point->Print("");   
    
   TString node_path = point->GetHitGeoPath();
   
   if (!Ntor_->cd(node_path)) {
       cout << "No such node: " << node_path << endl;
       return false;  
   }
   else cout << "Current node path: " << Ntor_->GetPath() << "\n\n";
   
   Double_t detpos[3];           // hit node position in global CS
   Double_t ax[3], ay[3], az[3]; // hit node axis (global CS)
   
   SpdGeopathParser ppath;
   ppath.ParsePath(node_path);
   
   cout << "RS module number: " << ppath.Num(2) << "\n\n";
   
   // print node geometry levels:
   for (Int_t i(1); i<= ppath.GetCurrentPathLevel(); i++) {
        cout << i << " " <<  ppath.Num(i) << " "<< ppath.Name(i) << "    :: " << ppath.Path(i) << endl; 
   }
   cout << "\n";

   // Hit detector
   DetectorPosition(detpos);
   DetectorOrientation(ax,'x');
   DetectorOrientation(ay,'y');
   DetectorOrientation(az,'z'); 
   
   cout << "HIT NODE: " << Ntor_->GetPath() << "\n\n";
   cout << "\t position:   " << detpos[0] << " " << detpos[1] << " " << detpos[2] << " " << endl;
   cout << "\t   x-axis:   " << ax[0] << " " << ax[1] << " " << ax[2] << " " << endl;
   cout << "\t   y-axis:   " << ay[0] << " " << ay[1] << " " << ay[2] << " " << endl;
   cout << "\t   z-axis:   " << az[0] << " " << az[1] << " " << az[2] << " " << endl;
   cout << "\n";
   
   // Module
   Ntor_->cd(ppath.Path(2));
   
   DetectorPosition(detpos);
   DetectorOrientation(ax,'x');
   DetectorOrientation(ay,'y');
   DetectorOrientation(az,'z'); 
   
   cout << "HIT MODULE: " << Ntor_->GetPath() << "\n\n";
   cout << "\t position:   " << detpos[0] << " " << detpos[1] << " " << detpos[2] << " " << endl; // always (0,0,0) caused by using TGeoArb8! 
   cout << "\t   x-axis:   " << ax[0] << " " << ax[1] << " " << ax[2] << " " << endl;
   cout << "\t   y-axis:   " << ay[0] << " " << ay[1] << " " << ay[2] << " " << endl;
   cout << "\t   z-axis:   " << az[0] << " " << az[1] << " " << az[2] << " " << endl;
   cout << "\n";
   
   return true;  
}

//_____________________________________________________________________________
Bool_t DetectorPosition(Double_t* p)
{
  if (!Ntor_) return false;
  Double_t ploc[3] = {0, 0, 0};
  Ntor_->LocalToMaster(ploc,p);
  return true;
}

//_____________________________________________________________________________ 
Bool_t DetectorOrientation(Double_t* v, Char_t dir)
{
  if (!Ntor_) return false;
  
  Double_t vloc[3];
  
       if (dir == 'x' || dir == 'X') { vloc[0] = 1.; vloc[1] = 0.; vloc[2] = 0.; }
  else if (dir == 'y' || dir == 'Y') { vloc[0] = 0.; vloc[1] = 1.; vloc[2] = 0.; }
  else if (dir == 'z' || dir == 'Z') { vloc[0] = 0.; vloc[1] = 0.; vloc[2] = 1.; }
  else return kFALSE;
  
  Ntor_->LocalToMasterVect(vloc,v);
  
  if (TMath::Abs(v[0]) < 1e-15) v[0] = 0.;
  if (TMath::Abs(v[1]) < 1e-15) v[1] = 0.;
  if (TMath::Abs(v[2]) < 1e-15) v[2] = 0.;
  
  return true;
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
  
  branch = t->GetBranch("SpdMCTrack");
  if (!branch) return kFALSE;
  t->SetBranchAddress("SpdMCTrack",&MCTracks_);       
  
  branch = t->GetBranch("SpdRsTEC2Point");
  if (!branch) return kFALSE;
  t->SetBranchAddress("SpdRsTEC2Point",&RsTECPoints_) ;
 
  cout << "\n>>>>>>>>>>>>>>>> FILE & TREE: Ok! <<<<<<<<<<<<<<<<< " << endl;
  
  Tree_ = t;
  
  return kTRUE;
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

  /* RANGE SYSTEM BARREL (RsTEC) PARAMETERS */
  
  RsTECPars_ = (SpdParSet*)fp->Get("RsTECParSet");
  if (!RsTECPars_) {
      cout << "No RsTEC parameters " << endl;
      return kFALSE;
  }
  
  //RsTECPars_->print(1);
  
  /* +++++++++++++++ LOAD GEOMETRY  +++++++++++++++ */

  FairGeoParSet* geoset = (FairGeoParSet*)fp->Get("FairGeoParSet");
  
  Ntor_ = gGeoManager->GetCurrentNavigator();
  
  /* DRAW GEOMETRY */
  //gGeoManager->SetVisLevel(2);
  //gGeoManager->GetMasterVolume()->Draw("ogl");
   
  return kTRUE;
}




