// -------------------------------------------------------------------------
// -----                SpdUrqmdGenerator header file                  -----
// -----          Created 11/06/04  by V. Friese / D.Bertini           -----
// -------------------------------------------------------------------------


/** SpdUrqmdGenerator.h
 *@ author V.Friese <v.friese@gsi.de>
 *@author D.Bertini <d.bertini@gsi.de>
 *
 The SpdUrqmdGenerator reads the output file 14 (ftn14) from UrQMD. The UrQMD
 calculation has to be performed in the CM system of the collision; Lorentz
 transformation into the lab is performed by this class.
 Derived from FairGenerator.
**/
#define GZIP_SUPPORT // version with gz support

#ifndef SPDURQMDGENERATOR_H
#define SPDURQMDGENERATOR_H

#include "SpdGenerator.h"

#include <fstream>
#include <map>

#include "TClonesArray.h"

#ifdef GZIP_SUPPORT
#ifndef __CINT__
#include <zlib.h>
#endif
#endif

class TVirtualMCStack;
class FairPrimaryGenerator;

class SpdUrqmdGenerator : public SpdGenerator
{
  public:

    /** Default constructor without arguments should not be used. **/
    SpdUrqmdGenerator();


    /** Standard constructor.
     * @param fileName The input file name
     **/
    SpdUrqmdGenerator(const char* fileName);


    /** Destructor. **/
    ~SpdUrqmdGenerator();

    virtual Bool_t Init() { return kTRUE; }  //FIXME!!!

    /** Reads on event from the input file and pushes the tracks onto
     ** the stack. Abstract method in base class.
     ** @param pStack    pointer to the stack
     ** @param ver       not used
     **/
    Bool_t ReadEvent(FairPrimaryGenerator* primGen);

    //Skip some events in file
    Bool_t SkipEvents(Int_t count);

    void SetEventPlane(Double_t phiMin, Double_t phiMax); //FIXME!!!

   inline const TClonesArray* GetEvent() const { return fParticles; }
   void SetKeepEvent(Bool_t v);
   void ResetEvent();
  
   void SetVerboseLevel(Int_t vl) {fVerbose = vl;} //FIXME!!!

  private:

#ifdef GZIP_SUPPORT
    #ifndef __CINT__
    gzFile fInputFile;                    //!  Input file
    #endif
#else
    FILE* fInputFile;                     //!  Input file
#endif

    std::map<Int_t,Int_t> fParticleTable;      //!  Map from UrQMD PID to PDGPID

    Double32_t fPhiMin, fPhiMax; // Limits of event plane angle          //FIXME!!!
    Bool_t fEventPlaneSet; // Flag whether event plane angle is used      //FIXME!!!

    const Char_t* fFileName;              //!  Input file name

    /** Private method ReadConversionTable. Reads the conversion table
        from UrQMD particle code to PDG particle code and fills the
        conversion map. Is called from the constructor. **/
    void ReadConversionTable();

    TClonesArray* fParticles; // local storage
    Bool_t fKeepEvent;
    Int_t fVerbose; //FIXME!!!

    SpdUrqmdGenerator(const SpdUrqmdGenerator&);
    SpdUrqmdGenerator& operator=(const SpdUrqmdGenerator&);

    ClassDef(SpdUrqmdGenerator,1);

};

#endif


