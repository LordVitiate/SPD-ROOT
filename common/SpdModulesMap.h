// $Id$
// Author: artur   2018/04/04

#ifndef __SPDMODULESMAP_H__
#define __SPDMODULESMAP_H__

#include <TObject.h>
#include <TString.h>  
#include <Rtypes.h>  
#include <map>

#include "SpdDetectorList.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdModulesMap                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdModulesMap  {

public:

    SpdModulesMap();
    virtual ~SpdModulesMap();
    
    void Clear();
    
    void Init(Int_t level, TString volname);
    void Add(TString prefix, Int_t id);     
          
    static Int_t GetGeoLevel(const TString& path) { return path.CountChar('/'); } 
    
    Bool_t Empty() const { return fModules.empty(); }
  
    TString GetFirstNodeName(TString path);
          
    static SpdModulesMap* Instance() 
          { return (fInstance)? fInstance : fInstance = new SpdModulesMap(); }

    Int_t SearchForId(TString node_path) const;
    
    Int_t GetMasterGeoLevel() const { return fMasterGeoLevel;   }
    Int_t GetMapGeoLevel()    const { return fMasterGeoLevel+1; }
    
    void Print() const;

private:

    Int_t   fMasterGeoLevel;
    TString fMasterVolName;
    
    std::map<TString,Int_t> fModules;
    
    static SpdModulesMap* fInstance;
   
    ClassDef(SpdModulesMap,1)
};

#endif  /* __SPDMODULESMAP_H__ */

