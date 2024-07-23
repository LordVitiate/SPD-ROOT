/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

#ifndef __SPDSTSPOINT_H__
#define __SPDSTSPOINT_H__

#include "FairMCPoint.h"

#include "TObject.h"
#include "TVector3.h"

class SpdStsPoint : public FairMCPoint
{

  public:

    /** Default constructor **/
    SpdStsPoint();


    /** Constructor with arguments
     *@param trackID  Index of MCTrack
     *@param detID    Detector ID
     *@param pos      Ccoordinates at entrance to active volume [cm]
     *@param mom      Momentum of track at entrance [GeV]
     *@param tof      Time since event start [ns]
     *@param length   Track length since creation [cm]
     *@param eLoss    Energy deposit [GeV]
     **/
    SpdStsPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
                     Double_t tof, Double_t length, Double_t eLoss);




    /** Destructor **/
    virtual ~SpdStsPoint();

    /** Output to screen **/
    virtual void Print(const Option_t* opt) const;

  private:
    /** Copy constructor **/
    SpdStsPoint(const SpdStsPoint& point);
    SpdStsPoint operator=(const SpdStsPoint& point);

    ClassDef(SpdStsPoint,1)

};

#endif
