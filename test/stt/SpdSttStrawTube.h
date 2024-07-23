// $Id$
// Author: artur   2015/09/26

#ifndef __SPDSTTSTRAWTUBE_H__
#define __SPDSTTSTRAWTUBE_H__

#include <TObject.h>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdSttStrawTube                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdSttStrawTube: public TObject {

public:

    SpdSttStrawTube();  
    virtual ~SpdSttStrawTube();
    
    /* Set-methods */
    
    virtual void SetParameters(Double_t* par) { /* not implemented here */ }
    virtual void SetNSections(Int_t n, Double_t* v = 0 /* array of size n-1 */);
    virtual void SetNTube(Int_t ntube = -1) { NTube_ = ntube; }
       
    inline void  SetTubleHalfLength(Double_t v) { TubeHalfLength_ = v; }
    inline void  SetTubeInRad(Double_t v)       { TubeRadIn_ = v;      }
    inline void  SetTubeOutRad(Double_t v)      { TubeRadOut_ = v;     }
    
    virtual void SetSource(Double_t /*x*/, Double_t /*y*/, Double_t /*z*/) { /* not implemented here */ }
    
    virtual void SetXSource(Double_t /*x*/) { /* not implemented here */ }
    virtual void SetYSource(Double_t /*y*/) { /* not implemented here */ }
    virtual void SetZSource(Double_t /*z*/) { /* not implemented here */ }
    
    virtual void SetRotation(Double_t* /*matrix*/) { /* not implemented here */ }
    
    /* Get-methods */  
    
    inline Int_t GetNTube()     { return NTube_;     }
    inline Int_t GetNSections() { return NSections_; }
  
    inline Double_t  GetTubeHalfLength()  { return TubeHalfLength_; }
    inline Double_t  GetTubeInRad()       { return TubeRadIn_;      }
    inline Double_t  GetTubeOutRad()      { return TubeRadOut_;     }
    
    virtual void GetSource(Double_t& x, Double_t& y, Double_t& z);
    
    virtual Double_t GetXSource() { return 0; }
    virtual Double_t GetYSource() { return 0; }
    virtual Double_t GetZSource() { return 0; }
    
    virtual void GetSectionSource(int n /* n = 0,... */, Double_t& x, Double_t& y, Double_t& z);
    
    virtual Double_t GetSectionSource(int n = 0/* n = 0,... */);
    virtual Double_t GetSectionHalfLength(int n = 0/* n = 0,... */);
    
    virtual Double_t* GetRotation() { return 0;  }
    virtual Double_t  GetRotation(Int_t& i, Int_t& j) { return (i == j) ? 1 : 0; }
    
    virtual std::vector<Int_t>* GetNeighboring(Int_t i = 0);
    
    virtual void ClearNeighborings();
    virtual void ClearNeighboring(Int_t i = 0);
    
    virtual void print(Int_t opt = 0);
    virtual void print_sections();
     
protected:
  
    Int_t NTube_;
    
    Double_t TubeHalfLength_;
    Double_t TubeRadIn_;
    Double_t TubeRadOut_;
    
    Int_t NSections_;
    
    std::vector<Double_t> LSections_; 
    std::vector<std::vector<Int_t>*> Neighborings_;
         
    virtual Bool_t MakeNeighboring(Int_t i) { return kFALSE; }
    
    ClassDef(SpdSttStrawTube,1)
};

#endif  /* __SPDSTTSTRAWTUBE_H__ */

