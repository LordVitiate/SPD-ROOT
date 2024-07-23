// $Id$
// Author: artur   2013/08/15

#ifndef __RWDATA_H__
#define __RWDATA_H__

#include <Rtypes.h>
#include <Riostream.h>
#include <TString.h>
#include <TObject.h>

#define SOFD sizeof(double)
#define SOFF sizeof(float)
#define SOFS sizeof(short)
#define SOFI sizeof(int)
#define SOFL sizeof(long)
#define SOFB sizeof(bool)
#define SOFC sizeof(char)

//using namespace std;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// RWdata                                                                     //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //  
////////////////////////////////////////////////////////////////////////////////

class RWdata  {
        
public:
        
    RWdata() {} 
    virtual ~RWdata() {}
    
    virtual void Clear() {}
    
    FILE* WriteFile(FILE* fl);
    FILE* ReadFile(FILE* fl);
    
    FILE* WOpenFile(TString path, TString fname, TString opt = "wb", int p_opt = 0);
    FILE* ROpenFile(TString path, TString fname, TString opt = "rb", int p_opt = 0);
    
    long Save(TString path, TString fname, int p_opt = 0);
    long Load(TString path, TString fname, int p_opt = 0);
 
protected:
        
    virtual bool Write(FILE* /*fl*/) = 0;
    virtual bool Read(FILE* /*fl*/) = 0;
      
    //ClassDef(RWdata,1)
};

#endif  /* __RWDATA_H__ */

