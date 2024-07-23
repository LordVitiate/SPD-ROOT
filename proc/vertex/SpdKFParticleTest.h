
#ifndef __SPDKFPARTICLETEST_H__
#define __SPDKFPARTICLETEST_H__

#include <TObject.h>
 
class KFParticle;
class KFParticleSIMD;

//---------------------------------------------------------------------------------
// The KFParticle class
// .
// @author  Maksym Zyzak
// @version 1.0
// @since   13.05.07
// 
// Class to reconstruct and store the decayed particle parameters.
// The method is described in CBM-SOFT note 2007-003, 
// ``Reconstruction of decayed particles based on the Kalman filter'', 
// http://www.gsi.de/documents/DOC-2007-May-14-1.pdf
//
// This class is ALICE interface to general mathematics in KFParticleBase
// 
//  -= Copyright &copy ALICE HLT and CBM L1 Groups =-
//_________________________________________________________________________________

class SpdKFParticleTest: public TObject {

public:

    SpdKFParticleTest();
    virtual ~SpdKFParticleTest();
 
    void PrintTutorial();
    void RunTest();
  
private:
    
    void RunTestSingle();
    void RunTestSIMD();
    void CompareSingleAndSIMDResults();
  
    KFParticle*     fMotherSingle;
    KFParticleSIMD* fMotherSIMD;
  
    ClassDef(SpdKFParticleTest,1)
};

#endif  /* __SPDKFPARTICLETEST_H__ */

