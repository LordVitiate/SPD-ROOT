#ifndef __SPDRSTID_H__
#define __SPDRSTID_H__

#include "Rtypes.h"

class SpdRsTID {

public:
    
    static Int_t DetectorId(Int_t iSector, Int_t iLayer, Int_t iMDT, Int_t iCell) 
                 { return iCell + iMDT * 10 + iLayer * 1000 + iSector * 100000;}

    static Short_t Sector(Int_t detID) { return detID/100000;     }  // EC = 1..2, Barrel = 1..8
    static Short_t Layer(Int_t detID)  { return (detID/1000)%100; }  // Layers = 1..20
    static Short_t MDT(Int_t detID)    { return (detID/10)%100;   }  // MDT = 1..?
    static Short_t Cell(Int_t detID)   { return detID%10;         }  // Cell = 1..8

private:
        
     SpdRsTID();
    ~SpdRsTID();
};

#endif
