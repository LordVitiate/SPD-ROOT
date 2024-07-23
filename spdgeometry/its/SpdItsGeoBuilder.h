// $Id$
// Author: artur   2019/03/07

#ifndef __SPDITSGEOBUILDER_H__
#define __SPDITSGEOBUILDER_H__

#include "SpdGeoBuilder.h"
#include "SpdGeoMapper.h"

#include <TGeoMatrix.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdItsGeoBuilder                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdGeoVVolume;

class SpdItsGeoBuilder: public SpdGeoBuilder {

public:

    SpdItsGeoBuilder();
    virtual ~SpdItsGeoBuilder();
    
    static SpdItsGeoBuilder* Instance();
    static SpdItsGeoBuilder* GetBuilder();
    
    virtual Bool_t ConstructGeometry();
    
    virtual void SetMapper(SpdGeoMapper* mapper);
    
    virtual SpdGeoMapper* GetMapper() const { return fGeoMapper; }
    virtual SpdGeoTable*  GetTable()  const;
    
    /* ++++++++++++++++++++++++++++++++++++ GEOMETRY NAVIGATION ++++++++++++++++++++++++++++++++++++ */
    
    virtual TGeoNavigator* SetNodePath(const TString& path); // -> set CURRENT NODE
    virtual TGeoNavigator* SetNodePath(Long_t detTID);       // -> set CURRENT NODE
   
    virtual TGeoNavigator* SetNodePath(const TString& path, Int_t vid); // -> set CURRENT NODE & CHANNEL
    virtual TGeoNavigator* SetNodePath(Long_t detTID, Int_t vid);       // -> set CURRENT NODE & CHANNEL
    
    virtual void SetVid(Int_t vid) { fVid = vid; } 
    
    // get current node volume
    virtual SpdGeoVVolume* GetNodeVVolume(); 
    
    // detector position in  global c.s.
    virtual Bool_t GetDetectorPosition(Double_t* p);  

    // transform POINT from global to local c.s. of a given node
    virtual Bool_t GlobalToLocalP(const Double_t* gpnt /*global*/, Double_t* lpnt /*local*/); 

    // transform POINT to global from local c.s. of a given node   
    virtual Bool_t LocalToGlobalP(const Double_t* lpnt /*local*/, Double_t* gpnt /*global*/);
           
protected:
 
    /* data members */
    static SpdItsGeoBuilder* fInstance;
    
    SpdGeoMapper* fGeoMapper; //!
    
    Int_t fVid; //!
    
    virtual TGeoVolume* BuildVolume(const SpdGeoVolPars* vpars);
    
private:

    ClassDef(SpdItsGeoBuilder,1)
};

#endif  /* __SPDITSGEOBUILDER_H__ */

