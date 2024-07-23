// $Id$
// Author: artur   2018/05/07

#ifndef __SPDGEOPATHPARSER_H__
#define __SPDGEOPATHPARSER_H__

#include <TObject.h>
#include <TString.h>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// SpdGeopathParser                                                          //
//                                                                           //
// <brief class description>                                                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

typedef std::vector< std::pair< TString,Int_t> > SPDGEOPATH;

class SpdGeopathParser: public TObject {

public:
    
    SpdGeopathParser(Int_t size = 20);
    virtual ~SpdGeopathParser();
    
    virtual void Clear();
 
    void Resize(Int_t size);
    
    virtual Int_t ParsePath(const TString& path); 
    
    const SPDGEOPATH& GetCurrentPath()       const { return fPath; }
    Int_t             GetCurrentPathLevel()  const { return fPathLevel; }
    TString           GetCurrentPathString() const;
    
    Int_t   Num(Int_t i, Bool_t begin = true)  const; 
    Int_t   VNum(Int_t i, Bool_t begin = true)  const; 
    TString Name(Int_t i, Bool_t begin = true) const;
    TString Path(Int_t i) const;
    
    static  TString GetVolumeName(TString path);
     
    virtual void Print(Option_t*) const;
    
protected:    
   
    SPDGEOPATH  fPath;
    Int_t       fPathLevel;
    
    ClassDef(SpdGeopathParser,1)
};

#endif  /* __SPDGEOPATHPARSER_H__ */

