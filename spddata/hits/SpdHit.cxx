// $Id$
// Author: artur   2020/09/20


//_____________________________________________________________________________
//
// SpdHit
//_____________________________________________________________________________

#include "SpdHit.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdHit)

//_____________________________________________________________________________
void SpdHit::Reset()
{
    fId     = -1;
    fModId  = -1;
    fWeight =  1;
    
    fTimeStamp = -1.;
    fTimeStampError = -1.;
}

//_____________________________________________________________________________
void SpdHit::Print(Option_t* option) const
{
    cout << "<SpdHit::Print>" << "\n\n";
    cout << "Hit id:                " << fId     << endl;
    cout << "Module (detector) id:  " << fModId  << endl;
    cout << "Hit weight:            " << fWeight << endl;
    cout << "Hit time (error) [s]:  " << fTimeStamp << "( " << fTimeStampError << ") " << endl;
}


