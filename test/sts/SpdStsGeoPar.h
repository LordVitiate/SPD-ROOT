/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

#ifndef __SPDSTSGEOPAR_H__
#define __SPDSTSGEOPAR_H__

#include "FairParGenericSet.h"

class TObjArray;
class FairParamList;

class SpdStsGeoPar       : public FairParGenericSet
{
  public:

    /** List of FairGeoNodes for sensitive  volumes */
    TObjArray*      fGeoSensNodes;

    /** List of FairGeoNodes for sensitive  volumes */
    TObjArray*      fGeoPassNodes;

    SpdStsGeoPar(const char* name="SpdStsGeoPar",
                      const char* title="SpdSts Geometry Parameters",
                      const char* context="TestDefaultContext");
    ~SpdStsGeoPar(void);
    void clear(void);
    void putParams(FairParamList*);
    Bool_t getParams(FairParamList*);
    TObjArray* GetGeoSensitiveNodes() {return fGeoSensNodes;}
    TObjArray* GetGeoPassiveNodes()   {return fGeoPassNodes;}

  private:
    SpdStsGeoPar(const SpdStsGeoPar&);
    SpdStsGeoPar& operator=(const SpdStsGeoPar&);

    ClassDef(SpdStsGeoPar,1)
};

#endif
