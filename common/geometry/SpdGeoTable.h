// $Id$
// Author: artur   2018/05/07

#ifndef __SPDGEOTABLE_H__
#define __SPDGEOTABLE_H__

#include <TObject.h>
#include <TObjArray.h>
#include <TString.h>

#include <set>
#include <map>
#include <vector>

#include "SpdGeopathParser.h"

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// SpdGeoTableRec                                                            //
//                                                                           //
// <brief class description>                                                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

class SpdGeoTableRec: public TObject {

public:
    
    SpdGeoTableRec();
    SpdGeoTableRec(TString volname, TString mothername, Int_t nodenum, Int_t nnodes);
    virtual ~SpdGeoTableRec();
    
    virtual void Clear();
  
    inline TString GetVolName()       const { return fVolName; }
    inline TString GetMotherVolName() const { return fMotherVolName; }
    
    inline Int_t GetNNodes() const { return fNNodes; }
    
    inline Int_t GetFirstNodeNum()      const { return fFirstNodeNum; }
    inline Int_t GetFirstNodeTableNum() const { return fFirstNodeTableNum; }
    inline Int_t GetLastNodeNum()       const { return fFirstNodeNum + fNNodes -1; }
    inline Int_t GetLastNodeTableNum()  const { return fFirstNodeTableNum + fNNodes -1; }
  
    inline Int_t GetGlobalNodeNum(Int_t loc_n) const { return fFirstNodeTableNum - fFirstNodeNum + loc_n;  }
    inline Int_t GetLocalNodeNum(Int_t glob_n) const { return glob_n - fFirstNodeTableNum + fFirstNodeNum; }
    
    virtual void Prn(Int_t n, Int_t i, Int_t j, Bool_t endn = kTRUE) const;
    
private:
    
    TString   fVolName;
    TString   fMotherVolName;
    Int_t     fFirstNodeNum;
    Int_t     fNNodes;
    Int_t     fFirstNodeTableNum;
    
    friend class SpdGeoTable;
    
    ClassDef(SpdGeoTableRec,1)
};

typedef std::map < TString, std::map<Int_t, SpdGeoTableRec*> > SPDGEOTABLE_TVOLUMES;

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// SpdGeoTable                                                               //
//                                                                           //
// <brief class description>                                                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

class SpdGeoTable: public SpdGeopathParser {

public:
    
    SpdGeoTable();
    virtual ~SpdGeoTable();
    
    virtual void Clear();
    
    virtual void CloseTable();
    
    virtual void SetMaxGeoLevel(int n);
    
    virtual Int_t AddTableRec(Int_t geolevel, 
			      TString volname, TString mothername, 
                              Int_t nodenum, Int_t nnodes);
  
     
    virtual Long_t  GetNodeTableNum(const TString& geopath);
    virtual TString GetNodeGeoPath(Long_t detTID);
    virtual TString GetNodeVolumeName(Long_t detTID);
      
    virtual const SpdGeoTableRec* GetRec(Int_t num) const;
    virtual const SpdGeoTableRec* GetRec(Int_t level, Int_t id) const;
    
    const TObjArray* GetTable() const { return fTable; }
    Int_t GetTableSize()        const { return (fTable) ? fTable->GetEntries() : 0; }
    
    Int_t GetMaxGeoLevel()      const { return fIdTable.size(); }
    Int_t GetNRec(Int_t level)  const { return (level < fIdTable.size()) ? fIdTable[level].size(): 0; }
    
    const SPDGEOTABLE_TVOLUMES GetTableVolumes() const { return fTableVolumes; }
    
    virtual void PrintTable(Int_t level = 0, Bool_t prnvols = kFALSE) const;
    
protected:    
    
    TObjArray* fTable;   //! set of geometry descriptors (SpdGeoTableRec)[uid]
    std::vector< std::vector<Int_t> > fIdTable; //! uid [level][id] 
    
    Long_t* fTableDims;   //! table dimensions
    
    SPDGEOTABLE_TVOLUMES fTableVolumes;  //! table list of volumes (with nodes)
    
    std::vector< std::vector<Int_t> > fTableGlobalNums;  //!

    ClassDef(SpdGeoTable,1)
};

#endif  /* __SPDGEOTABLE_H__ */

