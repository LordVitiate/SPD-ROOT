// $Id$
// Author: artur   2018/02/12


//_____________________________________________________________________________
//
// SpdGeoScanData
//_____________________________________________________________________________

#include "SpdGeoScanner.h"

#include <TGeoManager.h>

ClassImp(SpdGeoScanner)


//_____________________________________________________________________________
SpdGeoScanData::SpdGeoScanData()
{
  for (Int_t i(0); i<2; i++) 
  {
      fHLen[i] = 0;
      fHDepth[i] = 0;
      fHRadLen[i] = 0;
      fHHadrLen[i] = 0;
  }
}

//_____________________________________________________________________________
SpdGeoScanData::~SpdGeoScanData() 
{
   Clear();
}

//_____________________________________________________________________________
void SpdGeoScanData::Clear()
{
   for (Int_t i(0); i<2; i++) 
   {
        if (fHLen[i])     { delete fHLen[i];     fHLen[i] = 0;     }
        if (fHDepth[i])   { delete fHDepth[i];   fHDepth[i] = 0;   }
        if (fHRadLen[i])  { delete fHRadLen[i];  fHRadLen[i] = 0;  }
        if (fHHadrLen[i]) { delete fHHadrLen[i]; fHHadrLen[i] = 0; }
   }
}

//_____________________________________________________________________________
void SpdGeoScanData::Init(Int_t n, TString name1, TString name2)
{
   if (n < 1) {
       Clear();
       return;
   }
   
   if (name1.IsWhitespace()) return;
 
   TString ss;
   
   ss = Form("%s (spacing,0)",name1.Data());
   fHLen[0] = new TH1D(ss.Data(),ss.Data(),180,0,180);
   
   ss = Form("%s (depth,0)",name1.Data());
   fHDepth[0] = new TH1D(ss.Data(),ss.Data(),180,0,180);
   
   ss = Form("%s (radlen,0)",name1.Data());
   fHRadLen[0] = new TH1D(ss.Data(),ss.Data(),180,0,180);
   
   ss = Form("%s (hadrlen,0)",name1.Data());
   fHHadrLen[0] = new TH1D(ss.Data(),ss.Data(),180,0,180);
   
   if (n < 2) return;
   
   if (name2.IsWhitespace()) return;
   
   ss = Form("%s (spacing)",name2.Data());
   fHLen[0] = new TH1D(ss.Data(),ss.Data(),180,0,180);
   
   ss = Form("%s (depth)",name2.Data());
   fHDepth[0] = new TH1D(ss.Data(),ss.Data(),180,0,180);
   
   ss = Form("%s (radlen)",name2.Data());
   fHRadLen[0] = new TH1D(ss.Data(),ss.Data(),180,0,180);
   
   ss = Form("%s (hadrlen)",name1.Data());
   fHHadrLen[0] = new TH1D(ss.Data(),ss.Data(),180,0,180);
}

//_____________________________________________________________________________
//
// SpdGeoScanner
//_____________________________________________________________________________


//_____________________________________________________________________________
SpdGeoScanner::SpdGeoScanner():fGeometry(0),fFullGeoScan(0)  
{
  fGenerator = new TRandom3();
  fFullGeoScan = new SpdGeoScanData();
  
  fFullGeoScan->Init(1,"Full","");
}

//_____________________________________________________________________________
SpdGeoScanner::~SpdGeoScanner() 
{
  if (fGenerator)   delete fGenerator;
  if (fFullGeoScan) delete fFullGeoScan;
}

//_____________________________________________________________________________
void SpdGeoScanner::ClearScan() 
{
  if (!Data_.empty()) {
      for (Int_t i(0); i<Data_.size(); i++) {
           if (Data_[i]) delete Data_[i];
      }
      Data_.clear();
  }
}


