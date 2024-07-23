// $Id$
// Author: artur   2018/11/29

#ifndef __SPDNODESIDTABLE_H__
#define __SPDNODESIDTABLE_H__

#include <TObject.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <map> 
#include <string>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdNodesIdTable                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdNodesIdTable: public TObject {

public:

    SpdNodesIdTable();
    virtual ~SpdNodesIdTable();
    
    virtual void Clear();
    
    virtual Int_t   GetTableNodeId(const Char_t* node);
    virtual TString GetTableIdNode(Int_t id);
    
    virtual Bool_t  FillNodesTableIn(TObjArray* nodes);
    virtual Bool_t  LoadNodesTableFrom(TObjArray* nodes);

protected:
  
    std::map <std::string,Int_t> fNodesTable; //! table [node ,id]

    ClassDef(SpdNodesIdTable,1)
};

#endif  /* __SPDNODESIDTABLE_H__ */

