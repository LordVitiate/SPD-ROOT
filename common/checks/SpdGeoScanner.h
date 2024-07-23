// $Id$
// Author: artur   2018/02/12

#ifndef __SPDGEOSCANNER_H__
#define __SPDGEOSCANNER_H__

#include <TNamed.h>
#include <TRandom3.h>
#include <TH1D.h>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdGeoScanner                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TGeoManager;

class SpdGeoScanData: public TNamed {
  
public:
  
   SpdGeoScanData();
   virtual ~SpdGeoScanData();
   
   void Clear();
  
   void Init(Int_t n, TString name1, TString name2);
  
   inline Bool_t CheckName(TString& s) { return (s == fName); }
   
   TH1D*  GetHLen(Int_t i)     { return (i > -1 && i < 2) ? fHLen[i]     : 0; }
   TH1D*  GetHDepth(Int_t i)   { return (i > -1 && i < 2) ? fHDepth[i]   : 0; } 
   TH1D*  GetHRadLen(Int_t i)  { return (i > -1 && i < 2) ? fHRadLen[i]  : 0; }
   TH1D*  GetHHadrLen(Int_t i) { return (i > -1 && i < 2) ? fHHadrLen[i] : 0; }
    
private:   
  
   TH1D* fHLen[2];      //!
   TH1D* fHDepth[2];    //!
   TH1D* fHRadLen[2];   //!
   TH1D* fHHadrLen[2];  //!
   
   friend class SpdGeoScanner;
    
   ClassDef(SpdGeoScanData,1)
};


class SpdGeoScanner: public TObject {

public:

    SpdGeoScanner();
    virtual ~SpdGeoScanner();
    
    void ClearScan();

private:
  
    TRandom*        fGenerator;
    TGeoManager*    fGeometry;
    SpdGeoScanData* fFullGeoScan; 
    
    std::vector<SpdGeoScanData*> Data_; //!
     
    ClassDef(SpdGeoScanner,1)
};

#endif  /* __SPDGEOSCANNER_H__ */

