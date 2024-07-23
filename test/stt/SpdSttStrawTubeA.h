// $Id$
// Author: artur   2016/01/25

#ifndef __SPDSTTSTRAWTUBEA_H__
#define __SPDSTTSTRAWTUBEA_H__

#include <TObject.h>
#include "SpdSttStrawTube.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdSttStrawTubeA                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdSttStrawTubeA: public SpdSttStrawTube {

public:

    SpdSttStrawTubeA();
    virtual ~SpdSttStrawTubeA() {}
   
    virtual void SetNTube(Int_t ntube);
   
    void Scale(Double_t d); // d = distance between tubes
    
    virtual void SetZSource(Double_t z) { Z_ = z; }
    
    inline Int_t  Ring()    { return nr_; } // 1,...
    inline Int_t  Sector()  { return ns_; } // 0,...,5
    inline Int_t  Tube()    { return nt_; } // 0,...,Ring()-1 
    
    inline Bool_t IsVertex() { return !nt_; } 
    
    Double_t RadialDist();  
    Double_t RadialDistPlus();  
    Double_t RadialDistMinus();
    
    inline Double_t GetScaleFactor() { return V_[0][0]; }  
    
    virtual void GetSource(Double_t& x, Double_t& y, Double_t& z);
    
    virtual Double_t GetXSource();
    virtual Double_t GetYSource();
    virtual Double_t GetZSource() { return Z_; }
    
    virtual void print(Int_t opt = 0);

protected:
  
    virtual Bool_t MakeNeighboring(Int_t i);
 
private:
  
    Double_t V_[6][2]; // hexagon vertices, (x,y)
    Double_t K_[6][2]; // hexagon edges, (x,y)
    
    Int_t nr_;  // number of ring in the section (= total number of tubes in sector)
    Int_t ns_;  // number of sector in the ring 
    Int_t nt_;  // number of tube in the sector
    
    Double_t Z_;
    
    ClassDef(SpdSttStrawTubeA,1)
};


#endif  /* __SPDSTTSTRAWTUBEA_H__ */

