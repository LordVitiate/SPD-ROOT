// $Id$
// Author: andre   2021/03/04

#ifndef __SPDECALTEC2CLUSTER_H__
#define __SPDECALTEC2CLUSTER_H__

#include <TObject.h>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTEC2Cluster                                                         //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalTEC2Cluster : public TObject {

public:
    
    SpdEcalTEC2Cluster();
    virtual ~SpdEcalTEC2Cluster();

    void AddCell(Int_t hit) { fSignals.push_back(hit); }
    
    Int_t GetSize() const { return fSignals.size(); }
    
    std::vector<Int_t> GetCells() { return fSignals; }
    
    void Print(const Option_t* opt = 0) const;
            
private:

    Double_t fEnergy;
    
    std::vector<Int_t> fSignals;
    
    ClassDef(SpdEcalTEC2Cluster,1)
};

#endif  /* __SPDECALTEC2CLUSTER_H__ */

