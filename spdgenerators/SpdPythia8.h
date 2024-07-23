// $Id$
// Author: artur   2021/10/01

#ifndef __SPDPYTHIA8_H__
#define __SPDPYTHIA8_H__

#include <TObjArray.h>
#include <TRandom3.h>
#include <TGenerator.h>
#include <Pythia8/Pythia.h>
#include <Pythia8/Settings.h>
#include <Pythia8/ParticleData.h>

#include "SpdPrimGenParSet.h"

/*------------------------------------------------------------------------------------* 
 |                                                                                    | 
 |  *------------------------------------------------------------------------------*  | 
 |  |                                                                              |  | 
 |  |                                                                              |  | 
 |  |   PPP   Y   Y  TTTTT  H   H  III    A      Welcome to the Lund Monte Carlo!  |  | 
 |  |   P  P   Y Y     T    H   H   I    A A     This is PYTHIA version 8.240      |  | 
 |  |   PPP     Y      T    HHHHH   I   AAAAA    Last date of change: 20 Dec 2018  |  | 
 |  |   P       Y      T    H   H   I   A   A                                      |  | 
 |  |   P       Y      T    H   H  III  A   A    Now is 11 Oct 2021 at 20:24:07    |  | 
 |  |                                                                              |  | 
 |  |   Christian Bierlich;  Department of Astronomy and Theoretical Physics,      |  | 
 |  |      Lund University, Solvegatan 14A, SE-223 62 Lund, Sweden;                |  | 
 |  |      e-mail: christian.bierlich@thep.lu.se                                   |  | 
 |  |   Nishita Desai;  Department of Theoretical Physics, Tata Institute,         |  | 
 |  |      Homi Bhabha Road, Mumbai 400005, India;                                 |  | 
 |  |      e-mail: desai@theory.tifr.res.in                                        |  | 
 |  |   Ilkka Helenius;  Department of Physics, University of Jyvaskyla,           |  | 
 |  |      P.O. Box 35, FI-40014 University of Jyvaskyla, Finland;                 |  | 
 |  |      e-mail: ilkka.m.helenius@jyu.fi                                         |  | 
 |  |   Philip Ilten;  School of Physics and Astronomy,                            |  | 
 |  |      University of Birmingham, Birmingham, B152 2TT, UK;                     |  | 
 |  |      e-mail: philten@cern.ch                                                 |  | 
 |  |   Leif Lonnblad;  Department of Astronomy and Theoretical Physics,           |  | 
 |  |      Lund University, Solvegatan 14A, SE-223 62 Lund, Sweden;                |  | 
 |  |      e-mail: leif.lonnblad@thep.lu.se                                        |  | 
 |  |   Stephen Mrenna;  Computing Division, Simulations Group,                    |  | 
 |  |      Fermi National Accelerator Laboratory, MS 234, Batavia, IL 60510, USA;  |  | 
 |  |      e-mail: mrenna@fnal.gov                                                 |  | 
 |  |   Stefan Prestel;  Department of Astronomy and Theoretical Physics,          |  | 
 |  |      Lund University, Solvegatan 14A, SE-223 62 Lund, Sweden;                |  | 
 |  |      e-mail: stefan.prestel@thep.lu.se                                       |  | 
 |  |   Christine O. Rasmussen;  Department of Astronomy and Theoretical Physics,  |  | 
 |  |      Lund University, Solvegatan 14A, SE-223 62 Lund, Sweden;                |  | 
 |  |      e-mail: christine.rasmussen@thep.lu.se                                  |  | 
 |  |   Torbjorn Sjostrand;  Department of Astronomy and Theoretical Physics,      |  | 
 |  |      Lund University, Solvegatan 14A, SE-223 62 Lund, Sweden;                |  | 
 |  |      e-mail: torbjorn@thep.lu.se                                             |  | 
 |  |   Peter Skands;  School of Physics,                                          |  | 
 |  |      Monash University, PO Box 27, 3800 Melbourne, Australia;                |  | 
 |  |      e-mail: peter.skands@monash.edu                                         |  | 
 |  |                                                                              |  | 
 |  |   The main program reference is 'An Introduction to PYTHIA 8.2',             |  | 
 |  |   T. Sjostrand et al, Comput. Phys. Commun. 191 (2015) 159                   |  | 
 |  |   [arXiv:1410.3012 [hep-ph]]                                                 |  | 
 |  |                                                                              |  | 
 |  |   The main physics reference is the 'PYTHIA 6.4 Physics and Manual',         |  | 
 |  |   T. Sjostrand, S. Mrenna and P. Skands, JHEP05 (2006) 026 [hep-ph/0603175]  |  | 
 |  |                                                                              |  | 
 |  |   An archive of program versions and documentation is found on the web:      |  | 
 |  |   http://www.thep.lu.se/Pythia                                               |  | 
 |  |                                                                              |  | 
 |  |   This program is released under the GNU General Public Licence version 2.   |  | 
 |  |   Please respect the MCnet Guidelines for Event Generator Authors and Users. |  | 
 |  |                                                                              |  | 
 |  |   Disclaimer: this program comes without any guarantees.                     |  | 
 |  |   Beware of errors and use common sense when interpreting results.           |  | 
 |  |                                                                              |  | 
 |  |   Copyright (C) 2018 Torbjorn Sjostrand                                      |  | 
 |  |                                                                              |  | 
 |  |                                                                              |  | 
 |  *------------------------------------------------------------------------------*  | 
 |                                                                                    | 
 *------------------------------------------------------------------------------------*/ 

//using Pythia8::RndmEngine;

class SpdPythia8Rndm : public Pythia8::RndmEngine {

public:

    SpdPythia8Rndm():fGen(new TRandom3(0)) { }
    virtual ~SpdPythia8Rndm() { if (fGen) { delete fGen; fGen = 0; } }

    void    SetSeed(UInt_t seed = 0) { fGen->SetSeed(seed); } 
    UInt_t  GetSeed() const { return fGen->GetSeed(); }
       
    Double_t flat() { return fGen->Rndm(); }
       
private:

    TRandom3* fGen; //!
};

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdPythia8                                                                 //
//                                                                            //
// TPythia is an interface class to C++ version of Pythia 8.240               //
// event generators                                                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdPythia8: public TGenerator {
    
public:

    SpdPythia8();
    SpdPythia8(const char* xmlDir, bool banner = true);
    virtual ~SpdPythia8();

    Bool_t Init();
    
    virtual void       GenerateEvent();
    virtual Int_t      ImportParticles(TClonesArray* particles, Option_t* option = "");
    virtual TObjArray* ImportParticles(Option_t* option = "");
    
    void   SetParameter(std::string par);
    
    Int_t  GetNParticles() const { return fNParticles; }
    
    void   SetSeed(UInt_t seed, Bool_t UseExternalGenerator = false);
    UInt_t GetCurrentSeed() const { return fCurrentSeed; } // only if External Generator was defined!
    Bool_t HasExternalGenerator() const { return (fExtGenerator) ? true : false; }
    
    void   SetStorageIndex(Int_t index) { fStorageIndex = index; }
    Int_t  GetStorageIndex() const { return fStorageIndex; } 
    
    /* ----- Shortcuts to Pythia printings ----- */
    
    // Print out table of settings, either all or only changed ones
    void   ListAll() const;
    void   ListChanged() const;
    
    // Print out specified particle
    void   PartList(Int_t id) const;
    
    // Print out table of particle properties, or of only part of it
    void   PartListAll() const;
    void   PartListChanged() const;
    
    // Print out final statistics on generation
    void   PrintStatistics() const;
    
    // List the particles in an event
    void   EventListing(Bool_t showScaleAndVertex = true,
                        Bool_t showMothersAndDaughters = false) const;
                       
    /* ------------- set/get parameters ------------- */
                        
    Bool_t FillParsIn(SpdPrimGenParSet* params);
    Bool_t LoadParsFrom(SpdPrimGenParSet* params);
    
    virtual void Print(Option_t* opt = "") const;
    virtual void PrintParticles(const TClonesArray* particles) const;
    
    Pythia8::Pythia* Pythia8() const { return fPythia; }
    
protected:
    
    void AddParticlesToPdgDataBase();
    
    void SetExtGenerator(UInt_t seed); 
    void SearchForCurrentEventSeed(); 
    
    Pythia8::Pythia*     fPythia;        //! The Pythia8 instance
    Int_t                fNParticles;    //! Number of particles
    
    Int_t                fStorageIndex;  //! Parameters storage index
    TObjArray*           fParams;        //! List of parameters
    
    Pythia8::RndmEngine* fExtGenerator;  //! External generator (optionally)
    UInt_t               fCurrentSeed;   //! Current external generator seed
    
    friend class SpdPythia8Decayer;
  
    ClassDef(SpdPythia8,1)
};

#endif  /* __SPDPYTHIA8_H__ */

