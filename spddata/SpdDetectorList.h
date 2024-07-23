
#ifndef __SPDDETECTORLIST_H__
#define __SPDDETECTORLIST_H__

#include <TString.h>

#define NSPDDETECTORS 23

// kSTOPHERE is needed for iteration over the enum. 
// All detectors or passives have to be put before.

enum DetectorId {
                 kSpdEcalTB, kSpdEcalTEC,    //  0, 1
                 kSpdTsTB,   kSpdTsTEC,      //  2, 3
                 kSpdRsTB,   kSpdRsTEC,      //  4, 5 
          
                 kSpdEcalSB, kSpdEcalSEC,    //  6, 7
                 kSpdTsSB,   kSpdTsSEC,      //  8, 9
                 kSpdRsSB,   kSpdRsSEC,      // 10, 11          
    
                 kSpdIts,                    // 12
                 kSpdTofB,   kSpdTofEC,      // 13, 14
                 
                 kSpdZdc,   // 15
                 kSpdBbc,   // 16
                 kSpdAeg,   // 17
                
                 kSpdBarrel, kSpdStt, kSpdSts, kSpdHmrs,  // 18,19,20,21 
                 
                 kTestDetector, // 22
         
                 kSTOPHERE      // 23
};

enum PassiveID {
                 kSpdUndefined = -1000,
                 
                 kSpdHybMagnet = -9,
                 
                 kSpdPipe = -8,
                 
                 kSpdSolFrame  = -7,
                 kSpdSolBasket = -6,
                 kSpdSolMagnet = -5,
                 
                 kSpdTorFrame  = -4, 
                 kSpdTorBasket = -3, 
                 kSpdTorMagnet = -2, 
                                
                 kSpdTop = -1                
};

enum SpdDataBranchTypeId 
{   
   kMCEvent           = 0,
   kMCParticles       = 1,  
    
   kMCItsHits          = 10,
   
   kMCTsHits           = 20,
//   kMCTsBHits         = 21,
//   kMCTsECHits        = 22,
   kTsParticles      = 28,
    
   kMCTofHits         = 30,
//   kMCTofHitsB        = 31,
//   kMCTofHitsEC       = 32,
   kTofParticles     = 38,
   
   kMCEcalHits         = 40,
//   kMCEcalBHits       = 41,
//   kMCEcalECHits      = 42,   
//   kMCEcalClusters    = 43,
//   kMCEcalClustersB   = 44,
//   kMCEcalClustersEC  = 45,
   kMCEcalClustersInfo = 46,   
   kMCEcalParticles    = 47,   
  
   kMCRsHits           = 50,
//   kMCRsBHits         = 51,
//   kMCRsECHits        = 52,
   kMCRsClusters       = 53,
//   kMCRsClustersB      = 54,
//   kMCRsClustersEC     = 55,
//   kMCRsClustersInfo = 56,   
   kMCRsParticles      = 57,
   
   kMCZdcHits         = 60,
   kMCBbcHits         = 70,
  
   kMCAegHits         = 80,
   kAegParticles     = 81,

   kMCTracks          = 110,
   kMCVertices        = 120,
   
//   kRCEvent           = 200, 
//   kRCParticles       = 201,
   
   kRCTracks          = 210,
   kRCTrackSeeds      = 211,
   kRCVertices        = 220,
   
   kRCEcalClusters    = 230,
   kRCEcalParticles   = 231,

//   kRCRsClusters      = 240
   
};

static const Char_t* const SpdDataBranchName(Int_t ibranch) 
{
    switch (ibranch) 
    {    
        case  0 : return "MCEvent.";
        case  1 : return "MCParticles";
            
        case 10 : return "ItsMCHits";
    
        case 20 : return "TsMCHits";
//        case 21 : return "TsMCHitsB";
//        case 22 : return "TsMCHitsEC";
        case 28 : return "TsParticles";        
 
        case 30 : return "TofMCHits";
//        case 31 : return "TofMCHitsB";
//        case 32 : return "TofMCHitsEC";
        case 38 : return "TofParticles";        
        
        case 40 : return "EcalMCHits";
//        case 41 : return "EcalMCHitsB";
//        case 42 : return "EcalMCHitsEC";
//        case 43 : return "MCEcalClusters";
//        case 44 : return "MCEcalClustersB";
//        case 45 : return "MCEcalClustersEC";
        case 46 : return "MCEcalClustersInfo";        
        case 47 : return "MCEcalParticles";        
       
        case 50 : return "RsMCHits";
//        case 51 : return "RsMCHitsB";
//        case 52 : return "RsMCHitsEC";
        case 53 : return "MCRsClusters";
//        case 54 : return "MCRsClustersB";
//        case 55 : return "MCRsClustersEC";
//      case 56 : return "MCRsClustersInfo";               
        case 57 : return "MCRsParticles";

        case 60 : return "ZdcMCHits";
        case 70 : return "BbcMCHits";
        case 80 : return "AegMCHits";
        case 81 : return "AegParticles";

        case 110: return "MCTracks";
        case 120: return "MCVertices";
        
//        case 200: return "RCEvent.";
//        case 201: return "RCParticles";
        
        case 210: return "RCTracks";
        case 211: return "RCTrackSeeds";
        case 220: return "RCVertices";
        
        case 230: return "RCEcalClusters";
        case 231: return "RCEcalParticles";

//        case 240: return "RCRsClusters";
        
        default: break;    
    }
    return "";
}    

#endif
