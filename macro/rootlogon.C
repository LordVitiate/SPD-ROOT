
#define LOAD_MACROPATH
#define LOAD_SPDLIBRARIES
//#define LOAD_SPDINCLUDES

void SetMacroPath();
void LoadLibraries();
void LoadIncludes();
 
void rootlogon() 
{
  //cout << "::rootlogon::" << endl;
 
  if (!gROOT) return;
  
#ifdef LOAD_MACROPATH
  SetMacroPath();
#endif
  
#ifdef LOAD_SPDLIBRARIES
  LoadLibraries();
#endif  
  
#ifdef LOAD_SPDINCLUDES
  LoadIncludes();
#endif    
  
}

//______________________________________________________________
void SetMacroPath()
{
  //cout << ":SetMacroPath:" << endl;
 
  TString PROJECT = gSystem->Getenv("VMCWORKDIR");
  if (PROJECT == "") PROJECT = ".";

  //cout << "Project install directory: " << PROJECT <<endl;

  TString macropath;   
 
  //macropath = gROOT->GetMacroPath();   
  //cout << "macropath: " << macropath << endl;
 
  TString mpath = gSystem->ConcatFileName(PROJECT,"macro");
  macropath = ".:" + mpath + ":";
  
  gROOT->SetMacroPath(macropath);
  
  // add macro subdirectories
  macropath += mpath + "/geom" + ":";
  //macropath += mpath + "/analysis" + ":";
  macropath += mpath + "/my" + ":";
  //macropath += mpath + "/fullchain/testmisc/tracking" + ":";
  //macropath += mpath + "/fullchain/testmisc/vertexrec" + ":";
  //macropath += mpath + "/analysis/its" + ":";
  //macropath += mpath + "/analysis/tst" + ":";
  
  //cout << "macropath: " << macropath << endl;
  
  gROOT->SetMacroPath(macropath);
 
  //return;
  
  // print macro path
  int id(0),idx(0);  
  cout << "path to ROOT macros: " << endl;
  while ((idx = macropath.Index(":",id)) != -1) {
         cout << macropath(id,idx-id) << endl;
         id = idx+1;
  }  
}

//______________________________________________________________
void LoadLibraries()
{
  //cout << ":LoadLibraries:" << endl;
  
  cout << "load libraries ... ";
  
  gSystem->Load("libSpdData");
  gSystem->Load("libSpdCommon");
  gSystem->Load("libSpdField");
  gSystem->Load("libSpdGen");
  gSystem->Load("libSpdGeometry");
  gSystem->Load("libSpdDisplay");
  gSystem->Load("libClustering");
  gSystem->Load("libSpdPassive");
  gSystem->Load("libSpdIts");
  gSystem->Load("libSpdTst");
  gSystem->Load("libSpdEcalt");
  gSystem->Load("libSpdRst");
  gSystem->Load("libSpdTest");
 
//   gSystem->Load("libSpdTss");
//   gSystem->Load("libSpdEcals");
//   gSystem->Load("libSpdRss");  
//   gSystem->Load("libMpdEmc");
//   gSystem->Load("libSpdEcal");
  
  // tracking
  gSystem->Load("libGenFit2");
  gSystem->Load("libTrackingGF");
  gSystem->Load("libSpdReco");
  
  gSystem->Load("libKFParticle");
  gSystem->Load("libVertexKF");
  
  //gSystem->ListLibraries();
  
  cout << "Ok " << endl;     
}

//______________________________________________________________
void LoadIncludes()
{
  //cout << ":LoadIncludes:" << endl;

  cout << "load includes  ... ";

  gROOT->ProcessLine(".include $SIMPATH/include/");
  gROOT->ProcessLine(".include $FAIRROOTPATH/include/");
  
  gROOT->ProcessLine(".include ../spddata/"); 
  gROOT->ProcessLine(".include ../spddata/params"); 
  gROOT->ProcessLine(".include ../spddata/mcdata"); 
  gROOT->ProcessLine(".include ../spddata/IdealTrackData"); 
  gROOT->ProcessLine(".include ../common/");
  gROOT->ProcessLine(".include ../common/geometry");
  gROOT->ProcessLine(".include ../common/region");
  gROOT->ProcessLine(".include ../common/checks");
  gROOT->ProcessLine(".include ../field/");
  gROOT->ProcessLine(".include ../spdgenerators/");
  gROOT->ProcessLine(".include ../spdgeometry/");
  gROOT->ProcessLine(".include ../spdgeometry/sol/");
  gROOT->ProcessLine(".include ../spdgeometry/its/");
  gROOT->ProcessLine(".include ../spdgeometry/tst/");
  gROOT->ProcessLine(".include ../spdgeometry/ecalt/");
  gROOT->ProcessLine(".include ../spdgeometry/rst/");
  gROOT->ProcessLine(".include ../spddisplay/");
  gROOT->ProcessLine(".include ../proc/");
  gROOT->ProcessLine(".include ../procmc/");
  gROOT->ProcessLine(".include ../reco/");
  gROOT->ProcessLine(".include ../passive");
  gROOT->ProcessLine(".include ../its");
  gROOT->ProcessLine(".include ../tst/");
  gROOT->ProcessLine(".include ../tst/barrel");
  gROOT->ProcessLine(".include ../tst/ecps");
  gROOT->ProcessLine(".include ../ecalt/");
  gROOT->ProcessLine(".include ../ecalt/barrel");
  gROOT->ProcessLine(".include ../ecalt/ecps");
  gROOT->ProcessLine(".include ../rst/");
  gROOT->ProcessLine(".include ../rst/barrel");
  gROOT->ProcessLine(".include ../rst/ecps");
  gROOT->ProcessLine(".include ../tss/");
  gROOT->ProcessLine(".include ../tss/barrel");
  gROOT->ProcessLine(".include ../tss/ecps");
  gROOT->ProcessLine(".include ../ecals/");
  gROOT->ProcessLine(".include ../ecals/barrel");
  gROOT->ProcessLine(".include ../ecals/ecps");
  gROOT->ProcessLine(".include ../rss/");
  gROOT->ProcessLine(".include ../rss/barrel");
  gROOT->ProcessLine(".include ../rss/ecps");
  gROOT->ProcessLine(".include ../test/tracker");
  gROOT->ProcessLine(".include ../test/ecalcell");
  gROOT->ProcessLine(".include ../test/sts/");
  gROOT->ProcessLine(".include ../mpdgenerators/");
  gROOT->ProcessLine(".include ../emc/");
  gROOT->ProcessLine(".include ../ecal/");
  
  gROOT->ProcessLine(".include ../proc/clustering");
  gROOT->ProcessLine(".include ../proc/tracking");
  gROOT->ProcessLine(".include ../proc/vertex");
  
  
  gROOT->ProcessLine(".include ../external/KFParticle/KFParticle");
  gROOT->ProcessLine(".include ../external/KFParticle/KFParticleTest");
  
  cout << " Ok " << endl;
}  
  
  
  




