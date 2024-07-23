// $Id$
// Author: artur   2018/01/31

#ifndef __SPDDETECTOR_H__
#define __SPDDETECTOR_H__

#include "SpdGeoMapper.h"
#include "SpdGeoBuilder.h"
#include "SpdNodesIdTable.h"
#include "SpdParSet.h"

#include "FairDetector.h"

#include <map>
#include <string>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdDetector                                                                //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
//  SaveDetIdOption (default: 2)                                              //
//                                                                            //
//  0 : id = -1                                                               //
//  1 : apply table (put path as identifier if inner evaluator is used)       //
//  2 : apply mapper (if no mapper, option = 1)                               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairVolume;
class TGeoVolume;

class SpdDetector: public FairDetector {

public:

    SpdDetector();
    SpdDetector(const char* Name, Int_t Det_ID, Bool_t Is_Active = kTRUE);
    
    virtual ~SpdDetector();

    virtual void  Initialize();
    virtual void  FinishRun();
    
    virtual void  Reset() {}
    virtual void  Register() {}
          
    virtual void  SetParametersType(TString ptype) { fTitle = ptype; }
    
    virtual void  SaveEmptyHits(Bool_t savehits = kTRUE) { fSaveEmptyHits = savehits; }
    virtual void  SaveDetIdOption(Int_t option) { fSaveDetIdOption = option; }
    
    Bool_t SetNodesIdTable(SpdNodesIdTable* table = 0);
    void   RemoveNodesIdTable();
    
    /*  ----- Create geometry ----- */
    
    virtual Int_t RegisterMediums();
    virtual void  RegisterActiveVolumes();
    
    virtual void  ConstructGeometry() {}
 
    /*  ------- Simulations ------- */
    
    virtual Bool_t ProcessHits(FairVolume* v = 0) { return kTRUE; }
    
    virtual void  FinishPrimary() {}
    virtual void  BeginPrimary()  {}
    virtual void  PostTrack()     {}
    virtual void  PreTrack()      {}
    virtual void  BeginEvent()    {}
    virtual void  EndOfEvent()    {}
    
    virtual void  CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {}
    
    /* ---------- setters ---------- */
    
    virtual void  SetSpecialPhysicsCuts() {}
    
    /* ---------- getters ---------- */
    
    Int_t  GetSaveDetIdOption() const { return fSaveDetIdOption; }
    Bool_t GetSaveEmptyHits()   const { return fSaveEmptyHits;   }
    
    Bool_t IsMapperDefined()  const { return Bool_t(fGeoMapper); }
    Bool_t IsBuilderDefined() const { return Bool_t(fGeoBuilder); }
   
    Int_t  GetNDataOut() const { return fNDataOut; }
    
    virtual TString  GetDataOut(Int_t /*n*/) const { return "unknown"; }
   
    virtual TClonesArray* GetCollection(Int_t /*iColl*/) const { return 0; }
   
    virtual Long_t   GetNodeId(TString nodepath);
    
    virtual Double_t GetCapacity() const { return 0; } // cm^3
    virtual Double_t GetMass()     const { return 0; } // g
    virtual Double_t GetDensity()  const; // g/cm^3
    
    virtual TString  GetParametersType() const { return fTitle; }

    virtual SpdGeoMapper*    GetMapper()       { return fGeoMapper;    }
    virtual SpdGeoBuilder*   GetBuilder()      { return fGeoBuilder;   }
    virtual SpdNodesIdTable* GetNodesIdTable() { return fNodesIdTable; } 
    
    virtual SpdParSet* GetParameters();
    
    virtual Bool_t FillParsIn(SpdParSet* params);
    virtual Bool_t LoadParsFrom(SpdParSet* params);
   
    virtual Bool_t FillNodesTableIn(SpdParSet* params);
    virtual Bool_t LoadNodesTableFrom(SpdParSet* params);
     
    virtual void Print(Option_t*) const;
    
    //______________________________________________________________//
    struct DetVolume : public TObject {
        
        DetVolume():volume_(0),counts_(0),capacity_(0),mass_(0) {}
      
        TGeoVolume* volume_;
        Int_t       counts_;
        Double_t    capacity_;
        Double_t    mass_;
        
        ClassDef(SpdDetector::DetVolume,1) 
    };
    //______________________________________________________________//

protected:
  
    virtual TGeoMedium* FindMedium(TString& medname, TString default_med);
    
    virtual Bool_t CreateNodeIdTable(SpdParSet* params);
    
    SpdGeoMapper*    fGeoMapper;          //! geometry mapper
    SpdGeoBuilder*   fGeoBuilder;         //! geometry builder
    TGeoVolume*      fMasterVolume;       //! TOP geometry volume
    Int_t            fNDataOut;           //! number of output data types
    
    /* Parameters */
    
    Bool_t           fSaveEmptyHits;      //! mark to save empty hits 
    Int_t            fSaveDetIdOption;    //! option to save nodes id
    
    /* Nodes Table */
    
    SpdNodesIdTable* fNodesIdTable;       //! table [full node path,id]
    Bool_t           fNodesIdTableIsOwn;  //! mark if NodesIdTable is own
    
    ClassDef(SpdDetector,1)
};

#endif  /* __SPDDETECTOR_H__ */

