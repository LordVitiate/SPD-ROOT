// $Id$
// Author: andre   2021/03/04

#ifndef __SPDECALTB2CLUSTER_H__
#define __SPDECALTB2CLUSTER_H__

#include "FairHit.h"
#include <vector>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTB2Cluster                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalTB2Cluster : public TObject {

public:

    SpdEcalTB2Cluster();
    virtual ~SpdEcalTB2Cluster();

    void AddCell(Int_t hit) { fSignals.push_back(hit); };
    
    Int_t GetSize() const { return fSignals.size(); };
    
    std::vector<Int_t> GetCells() { return fSignals; };
    
    void Print(const Option_t* opt = 0) const;

private:

    Double_t fEnergy;
    
    std::vector<Int_t> fSignals;
    
    ClassDef(SpdEcalTB2Cluster,1)
};

#endif  /* __SPDECALTB2CLUSTER_H__ */

