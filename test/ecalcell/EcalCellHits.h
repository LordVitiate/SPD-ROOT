// $Id$
// Author: artur   2015/10/15

#ifndef __ECALCELLHITS_H__
#define __ECALCELLHITS_H__

#include <TNamed.h>
#include <map>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// EcalCellHits                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TH1D;
class TH2D;

class EcalCellHits: public TNamed {

public:

    EcalCellHits();
    virtual ~EcalCellHits();
    
    void Clear();
    void Print(int opt = 0) const;
     
    void SetEventId(Int_t id) { EventId_ = id;   } 
    int  GetEventId()  const  { return EventId_; }
   
    void   SetEdep(TString node, double edep);
    void   AddEdep(TString node, double edep);
    double GetEdep(TString node);
    
    const std::map<TString,double>& GetEdep() { return Edep_; }
    
    void SetNZ(Int_t n)   { NZ_  = n; }
    void SetNXY(Int_t n)  { NXY_ = n; }
      
    Int_t GetNZ(Int_t n)  const { return NZ_;  }
    Int_t GetNXY(Int_t n) const { return NXY_; }
   
    Double_t SumEdep() const;

    TH1D* GetHZ(TH1D* h, Bool_t reset = false);
    TH2D* GetHXY(TH2D* h, Bool_t reset = false);
    
    TH1D* GetHZ(TH1D* h, TString vertex, Bool_t reset = false);
    
    void GetIJ(Int_t n, Int_t& i, Int_t& j)  const { i = (n-1)/NXY_ + 1; j = (n-1)%NXY_ + 1; }

protected:
  
    int EventId_;    // event number
    
    std::map<TString,double> Edep_; // [node path,edep]
    
    Int_t NZ_;   // number of layers
    Int_t NXY_;  // number of pads
    
    ClassDef(EcalCellHits,1)
};

#endif  /* __ECALCELLHITS_H__ */

