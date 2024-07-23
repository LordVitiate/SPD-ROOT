// $Id$
// Author: artur   2016/01/10

#ifndef __SPDMCEVENTHEADER_H__
#define __SPDMCEVENTHEADER_H__

#include <FairMCEventHeader.h>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMCEventHeader                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdPrimGenData;
class TObjArray;
class TParticle;

class SpdMCEventHeader: public FairMCEventHeader {

public:

    SpdMCEventHeader();
    virtual ~SpdMCEventHeader();
  
    virtual void Register();
    virtual void Reset();
    
    virtual SpdPrimGenData* AddGenData(const Char_t* generator = "undefined");
    
    Int_t      GetNPrimGenData() const;
    TObjArray* GetPrimGenDataList() const { return fPrimGenDataList; }
    
    virtual SpdPrimGenData* GetGenData(Int_t index  /* >0 */ ) const;
    virtual SpdPrimGenData* GetGenData(TString generator) const;
    
    virtual SpdPrimGenData* GetPrimData(TString type = "SpdPrimaryGenerator");
    virtual SpdPrimGenData* GetDecayerData();
    
    virtual TParticle*      FindParticle(Int_t ipart) const;
    virtual SpdPrimGenData* FindGenData(Int_t ipart) const;
    
    virtual void Print(int opt = 0);

protected:
  
    SpdPrimGenData* fDecayerData;
    TObjArray*      fPrimGenDataList;

    ClassDef(SpdMCEventHeader,1)
};

#endif  /* __SPDMCEVENTHEADER_H__ */

