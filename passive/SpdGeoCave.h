
#ifndef __SPDGEOCAVE_H__
#define __SPDGEOCAVE_H__

#include "FairGeoSet.h"    
#include <iosfwd>       
#include <Rtypes.h>        
#include <TString.h>       

#include <fstream>         

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdCave                                                                    //
//                                                                            //
// Class for the geometry of the detector part CAVE                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairGeoMedia;

class SpdGeoCave : public FairGeoSet {
    
public:

    SpdGeoCave();
   ~SpdGeoCave() {}
 
    const char* getModuleName(Int_t) { return Name_.Data(); }
    
    void  addRefNodes();
    
    Bool_t read(std::fstream&, FairGeoMedia*);
    void   write(std::fstream&);

    void print();

protected:

    TString Name_;
    
    ClassDef(SpdGeoCave,0) 
};


#endif 


