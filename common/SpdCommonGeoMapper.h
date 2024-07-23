// $Id$
// Author: artur   2018/01/30

#ifndef __SPDCOMMONGEOMAPPER_H__
#define __SPDCOMMONGEOMAPPER_H__

#include <TObject.h>
#include <TString.h>
#include <map>

#include "SpdDetectorList.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdCommonGeoMapper                                                         //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TGeoVolume;

class FairModule;
class SpdPassiveModule;
class SpdDetector;

class SpdCommonGeoMapper: public TObject {

public:

    SpdCommonGeoMapper();
    virtual ~SpdCommonGeoMapper();
    
    static SpdCommonGeoMapper* Instance();
    
    static Int_t       GetMasterGeoLevel();
    static TGeoVolume* GetMasterVolume();
    static TString     GetMasterVolumeName();
    
    static Int_t    GetNGeoSectors()            { return theNGeoSectors;      }
    static Double_t GetSectorClearance()        { return theSectorClearance;  }
    
    /*============================= PIPE ===============================*/
    
    static Int_t    GetPipeDefGeoType()         { return thePipeDefGeoType;  } 
    static TString  GetPipeMaterial1()          { return thePipeMaterial1;   } 
    static TString  GetPipeMaterial2()          { return thePipeMaterial2;   } 
    
    static Double_t GetPipeOuterRadius()        { return thePipeOuterRadius; } 
    static Double_t GetPipeThickness()          { return thePipeThickness;   }
    
    static Double_t GetPipeLength1()            { return thePipeLength1;     }  
    static Double_t GetPipeLength2()            { return thePipeLength2;     }
    static Double_t GetPipeLength3()            { return thePipeLength3;     } 
    
    /*===================== INNER TRACKING SYSTEM ======================*/

    static Int_t    GetItsDefGeoType()          { return theItsDefGeoType;     }
    static TString  GetItsBaseMaterial()        { return theItsBaseMaterial;   }
    static TString  GetItsLadderMaterial()      { return theItsLadderMaterial; }
    static TString  GetItsSensorMaterial()      { return theItsSensorMaterial; } 
    
    static Int_t    GetItsNLayers()             { return theItsNLayers;        }           
    static Double_t GetItsMinRadius()           { return theItsMinRadius;      }      
    static Double_t GetItsMaxRadius()           { return theItsMaxRadius;      }
    static Double_t GetItsMaxLength()           { return theItsMaxLength;      }
    
    /*===================== BEAM-BEAM COUNTER ========================*/
    
    static Int_t    GetBbcDefGeoType()          { return theBbcDefGeoType;     }    
    static TString  GetBbcBaseMaterial()        { return theBbcBaseMaterial;   }    
    
    static Double_t GetBbcThickness()           { return theBbcThickness;      }         
    static Double_t GetBbcSize()                { return theBbcSize;           }         
    static Double_t GetBbcWidth()               { return theBbcWidth;          }          
    
    static Double_t GetBbcMinDist()             { return theBbcMinDist;        }       
    
    /*===================== TIME-OF-FLIGHT SYSTEM (BARREL) ======================*/
    
    static Int_t    GetTofBDefGeoType()         { return theTofBDefGeoType;    }      
    static TString  GetTofBBaseMaterial()       { return theTofBBaseMaterial;  }      
    
    static Double_t GetTofBLength()             { return theTofBLength;        }           
    static Double_t GetTofBSize()               { return theTofBSize;          }           
    static Double_t GetTofBWidth()              { return theTofBWidth;         }              
    
    /*===================== TIME-OF-FLIGHT SYSTEM (ENDCAPS) =====================*/
    
    static Int_t    GetTofECDefGeoType()        { return theTofECDefGeoType;   }    
    static TString  GetTofECBaseMaterial()      { return theTofECBaseMaterial; }    
    
    static Double_t GetTofECThickness()         { return theTofECThickness;    }         
    static Double_t GetTofECSize()              { return theTofECSize;         }         
    static Double_t GetTofECWidth()             { return theTofECWidth;        }          
    
    static Double_t GetTofECMinDist()           { return theTofECMinDist;      }   
    
    /*===================== AEROGEL ========================*/
    
    static Int_t    GetAegDefGeoType()          { return theAegDefGeoType;     }    
    static TString  GetAegBaseMaterial()        { return theAegBaseMaterial;   }    
    
    static Double_t GetAegThickness()           { return theAegThickness;      }         
    static Double_t GetAegSize()                { return theAegSize;           }         
    static Double_t GetAegWidth()               { return theAegWidth;          }          
    
    static Double_t GetAegMinDist()             { return theAegMinDist;        }       
    
/*==============================================================================*/
/*= ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ =*/
/*= ++++++++++++++++++++++++ TOROIDAL GEOMETRY +++++++++++++++++++++++++++++++ =*/
/*= ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ =*/
/*==============================================================================*/
    
    /*==================== BASKET (TOR.GEOM.) ==========================*/

    static Int_t    GetTorBasketDefGeoType()     { return theTorBasketDefGeoType; }
    static Double_t GetTorBasketTotalLen()       { return theTorBasketTotalLen;   }
    
    static TString  GetTorBasketBaseMaterial()   { return theTorBasketBaseMaterial;   } 
    static TString  GetTorBasketLeafMaterial()   { return theTorBasketLeafMaterial;   }
    static TString  GetTorBasketCasingMaterial() { return theTorBasketCasingMaterial; }

    /*==================== TOROIDAL MAGNET =============================*/

    static Int_t    GetTorMagnetDefGeoType()    { return theTorMagnetDefGeoType;   }
    static TString  GetTorMagnetCoilMaterial()  { return theTorMagnetCoilMaterial; }
    
    static Double_t GetTorMagnetDistToAxis1()   { return theTorMagnetDistToAxis1; }
    static Double_t GetTorMagnetDistToAxis2()   { return theTorMagnetDistToAxis2; }
    
    static Double_t GetTorMagnetLen11()         { return theTorMagnetLen11;  }
    static Double_t GetTorMagnetSize11()        { return theTorMagnetSize11; }
    
    static Double_t GetTorMagnetLen12()         { return theTorMagnetLen12;  }  
    static Double_t GetTorMagnetSize12()        { return theTorMagnetSize12; }
    
    static Double_t GetTorMagnetLen2()          { return theTorMagnetLen2;   }
    static Double_t GetTorMagnetSize2()         { return theTorMagnetSize2;  }
    
    static Double_t GetTorMagnetCoilLen11()     { return theTorMagnetCoilLen11; }
    static Double_t GetTorMagnetCoilLen12()     { return theTorMagnetCoilLen12; }
    
    static Double_t GetTorMagnetCoilLen2()      { return theTorMagnetCoilLen2; }
    
    static Double_t GetTorMagnetCoilThickness() { return theTorMagnetCoilThickness; }
    static Double_t GetTorMagnetCoilWidth()     { return theTorMagnetCoilWidth;     }
    static Double_t GetTorMagnetCoilRR()        { return theTorMagnetCoilRR;        }
    
    /*=========== TSTB (TOROIDAL TRACKING SYSTEM BARREL) ===============*/

    static Int_t    GetTsTBDefGeoType()         { return theTsTBDefGeoType; }
    
    static TString  GetTsTBBaseMaterial()       { return theTsTBBaseMaterial; }
    
    static Double_t GetTsTBLen11()              { return theTsTBLen11;   }     
    static Double_t GetTsTBSize11()             { return theTsTBSize11;  }    
    static Double_t GetTsTBWidth11()            { return theTsTBWidth11; }    
    
    static Double_t GetTsTBLen12()              { return theTsTBLen12;   }    
    static Double_t GetTsTBSize12()             { return theTsTBSize12;  }  
    static Double_t GetTsTBWidth12()            { return theTsTBWidth12; } 
    
    static Double_t GetTsTBLen2()               { return theTsTBLen2;   }
    static Double_t GetTsTBSize2()              { return theTsTBSize2;  }
    static Double_t GetTsTBWidth2()             { return theTsTBWidth2; }
    
    static Double_t GetTsTBLen()                { return theTsTBLen;   }
    static Double_t GetTsTBSize()               { return theTsTBSize;  }
    static Double_t GetTsTBWidth()              { return theTsTBWidth; }
    
    static Bool_t   GetTsTBMakeOffset11()       { return theTsTBMakeOffset11; }
    static Bool_t   GetTsTBMakeOffset12()       { return theTsTBMakeOffset12; }
    static Bool_t   GetTsTBMakeOffset2()        { return theTsTBMakeOffset2;  }
    static Bool_t   GetTsTBMakeOffset()         { return theTsTBMakeOffset;   }
    
    /*========== TSTEC (TOROIDAL TRACKING SYSTEM ENDCAPS) ==============*/
    
    static Int_t    GetTsTECDefGeoType()        { return theTsTECDefGeoType; } 

    static TString  GetTsTECBaseMaterial()      { return theTsTECBaseMaterial;  }
    static TString  GetTsTECLayerMaterial()     { return theTsTECLayerMaterial; }

    static Double_t GetTsTECMinDist1()          { return theTsTECMinDist1; }   
    static Double_t GetTsTECMinDist2()          { return theTsTECMinDist2; } 
    static Double_t GetTsTECMinDist3()          { return theTsTECMinDist3; }

    static Double_t GetTsTECSize()              { return theTsTECSize;    }  
    static Double_t GetTsTECWidth()             { return theTsTECWidth;   }  
    
    static Double_t GetTsTECLength1()           { return theTsTECLength1; }  
    static Double_t GetTsTECLength2()           { return theTsTECLength2; }
    static Double_t GetTsTECLength3()           { return theTsTECLength3; }
       
    /*===== ECALTB (TOROIDAL ELECTROMAGNETIC CALORIMETER BARREL) ======*/
  
    static Int_t    GetEcalTBDefGeoType()       { return theEcalTBDefGeoType;   }  
    static TString  GetEcalTBBaseMaterial()     { return theEcalTBBaseMaterial; }
    
    static Double_t GetEcalTBLen11()            { return theEcalTBLen11;   }
    static Double_t GetEcalTBSize11()           { return theEcalTBSize11;  }
    static Double_t GetEcalTBWidth11()          { return theEcalTBWidth11; }

    static Double_t GetEcalTBLen12()            { return theEcalTBLen12;   }
    static Double_t GetEcalTBSize12()           { return theEcalTBSize12;  }
    static Double_t GetEcalTBWidth12()          { return theEcalTBWidth12; }
    
    static Double_t GetEcalTBLen2()             { return theEcalTBLen2;    }
    static Double_t GetEcalTBSize2()            { return theEcalTBSize2;   }
    static Double_t GetEcalTBWidth2()           { return theEcalTBWidth2;  }

    /*===== ECALTEC (TOROIDAL ELECTROMAGNETIC CALORIMETER ENDCAPS) ====*/
    
    static Int_t    GetEcalTECDefGeoType()      { return theEcalTECDefGeoType;   } 
    static TString  GetEcalTECBaseMaterial()    { return theEcalTECBaseMaterial; }

    static Double_t GetEcalTECMinDist1()        { return theEcalTECMinDist1;   }
    static Double_t GetEcalTECMinDist2()        { return theEcalTECMinDist2;   }
    
    static Double_t GetEcalTECSize()            { return theEcalTECSize;       }
    static Double_t GetEcalTECWidth()           { return theEcalTECWidth;      }
    static Double_t GetEcalTECThickness()       { return theEcalTECThickness;  }
    
    /*============ RSTB (TOROIDAL RANGE SYSTEM BARREL) =================*/
    
    static Int_t    GetRsTBDefGeoType()         { return theRsTBDefGeoType; }
    
    static TString  GetRsTBBaseMaterial()       { return theRsTBBaseMaterial; }
    
    static Double_t GetRsTBLen11()              { return theRsTBLen11;   }     
    static Double_t GetRsTBSize11()             { return theRsTBSize11;  }    
    static Double_t GetRsTBWidth11()            { return theRsTBWidth11; }    
    
    static Double_t GetRsTBLen12()              { return theRsTBLen12;   }    
    static Double_t GetRsTBSize12()             { return theRsTBSize12;  }  
    static Double_t GetRsTBWidth12()            { return theRsTBWidth12; } 
    
    static Double_t GetRsTBLen2()               { return theRsTBLen2;   }     
    static Double_t GetRsTBSize2()              { return theRsTBSize2;  }     
    static Double_t GetRsTBWidth2()             { return theRsTBWidth2; } 
    
    /*============ RSTEC (TOROIDAL RANGE SYSTEM ENDCAPS) ===============*/
    
    static Int_t    GetRsTECDefGeoType()        { return theRsTECDefGeoType; } 

    static TString  GetRsTECBaseMaterial()      { return theRsTECBaseMaterial; }

    static Double_t GetRsTECMinDist1()          { return theRsTECMinDist1; }   
    static Double_t GetRsTECMinDist2()          { return theRsTECMinDist2; }   

    static Double_t GetRsTECSize()              { return theRsTECSize;      }  
    static Double_t GetRsTECWidth()             { return theRsTECWidth;     }  
    static Double_t GetRsTECThickness()         { return theRsTECThickness; } 
    
/*==============================================================================*/
/*= ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ =*/
/*= +++++++++++++++++++++++++ SOLENOIDAL GEOMETRY ++++++++++++++++++++++++++++ =*/
/*= ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ =*/
/*==============================================================================*/

    /*==================== SOLENOIDAL MAGNET ===========================*/
    
    static Int_t    GetSolMagnetDefGeoType()    { return theSolMagnetDefGeoType;   }
    static TString  GetSolMagnetBaseMaterial()  { return theSolMagnetBaseMaterial; }
    static TString  GetSolMagnetCryoMaterial()  { return theSolMagnetCryoMaterial; } 
    
    static Int_t    GetSolMagnetNSections1()    { return theSolMagnetNSections1; }
    static Int_t    GetSolMagnetNSections2()    { return theSolMagnetNSections2; }   
    
    static Double_t GetSolMagnetTotalLen1()     { return theSolMagnetTotalLen1;   }
    static Double_t GetSolMagnetTotalSize1()    { return theSolMagnetTotalSize1;  }
    static Double_t GetSolMagnetTotalWidth1()   { return theSolMagnetTotalWidth1; }
    
    static Double_t GetSolMagnetLen1()          { return theSolMagnetLen1;   }   
    static Double_t GetSolMagnetSize1()         { return theSolMagnetSize1;  }   
    static Double_t GetSolMagnetWidth1()        { return theSolMagnetWidth1; }    

    static Double_t GetSolMagnetInnerCryoW1()   { return theSolMagnetInnerCryoW1; } 
    static Double_t GetSolMagnetOuterCryoW1()   { return theSolMagnetOuterCryoW1; }   
    static Double_t GetSolMagnetEndfCryoW1()    { return theSolMagnetEndfCryoW1;  }
    
    static Double_t GetSolMagnetLen2()          { return theSolMagnetLen2;   }
    static Double_t GetSolMagnetSize2()         { return theSolMagnetSize2;  }
    static Double_t GetSolMagnetWidth2()        { return theSolMagnetWidth2; }
    
	 /*==================== SOLENOIDAL MAGNET2 ===========================*/
	static Int_t    GetSolMagnetTwoDefGeoType()    { return theSolMagnetTwoDefGeoType;   }
	static TString  GetSolMagnetTwoBaseMaterial()  { return theSolMagnetTwoBaseMaterial; }
	static TString  GetSolMagnetTwoCryoMaterial()  { return theSolMagnetTwoCryoMaterial; }
	
	static Int_t    GetSolMagnetTwoNSections()    { return theSolMagnetTwoNSections; }
	 
    static Double_t GetSolMagnetTwoTotalLen()     { return theSolMagnetTwoTotalLen;   }
	static Double_t GetSolMagnetTwoTotalSize()    { return theSolMagnetTwoTotalSize;  }
	static Double_t GetSolMagnetTwoTotalWidth()   { return theSolMagnetTwoTotalWidth; }
	
	static Double_t GetSolMagnetTwoLen()          { return theSolMagnetTwoLen;   }
	static Double_t GetSolMagnetTwoSize()         { return theSolMagnetTwoSize;  }
	static Double_t GetSolMagnetTwoWidth()        { return theSolMagnetTwoWidth; }
	 
	static Double_t GetSolMagnetTwoInnerCryoW()   { return theSolMagnetTwoInnerCryoW; }
	static Double_t GetSolMagnetTwoOuterCryoW()   { return theSolMagnetTwoOuterCryoW; }
	static Double_t GetSolMagnetTwoEndfCryoW()    { return theSolMagnetTwoEndfCryoW;  }
	
    /*=========== TSSB (SOLENOIDAL TRACKING SYSTEM BARREL) ==============*/
    
    static Int_t    GetTsSBDefGeoType()         { return theTsSBDefGeoType; }
    static TString  GetTsSBBaseMaterial()       { return theTsSBBaseMaterial; }
   
    static Int_t    GetTsSBNSections1()         { return theTsSBNSections1; }
  
    static Double_t GetTsSBLen()                { return theTsSBLen;        }
    static Double_t GetTsSBSize()               { return theTsSBSize;       }
    static Double_t GetTsSBWidth()              { return theTsSBWidth;      }    
   
    /*========= TSSEC (SOLENOIDAL TRACKING SYSTEM ENDCAPS) ==============*/
    
    static Int_t    GetTsSECDefGeoType()        { return theTsSECDefGeoType; } 
    static TString  GetTsSECBaseMaterial()      { return theTsSECBaseMaterial; }

    static Double_t GetTsSECMinDist()           { return theTsSECMinDist;    }
    static Double_t GetTsSECSize()              { return theTsSECSize;       }
    static Double_t GetTsSECWidth()             { return theTsSECWidth;      }
    static Double_t GetTsSECThickness()         { return theTsSECThickness;  }   
    
    /*===== ECALSB (SOLENOIDAL ELECTROMAGNETIC CALORIMETER BARREL) ======*/
    
    static Int_t    GetEcalSBDefGeoType()       { return theEcalSBDefGeoType; }
    static TString  GetEcalSBBaseMaterial()     { return theEcalSBBaseMaterial; }
   
    static Int_t    GetEcalSBNSections1()       { return theEcalSBNSections1; }
    static Int_t    GetEcalSBNSections2()       { return theEcalSBNSections2; }
   
    static Double_t GetEcalSBLen()              { return theEcalSBLen;        }
    static Double_t GetEcalSBSize()             { return theEcalSBSize;       }
    static Double_t GetEcalSBWidth()            { return theEcalSBWidth;      }

    /*===== ECALSEC (SOLENOIDAL ELECTROMAGNETIC CALORIMETER ENDCAPS) ====*/

    static Int_t    GetEcalSECDefGeoType()      { return theEcalSECDefGeoType; } 
    static TString  GetEcalSECBaseMaterial()    { return theEcalSECBaseMaterial; }

    static Double_t GetEcalSECMinDist()         { return theEcalSECMinDist;    }
    static Double_t GetEcalSECSize()            { return theEcalSECSize;       }
    static Double_t GetEcalSECWidth()           { return theEcalSECWidth;      }
    static Double_t GetEcalSECThickness()       { return theEcalSECThickness;  }   
    
    /*============= RSSB (SOLENOIDAL RANGE SYSTEM BARREL) ==============*/
    
    static Int_t    GetRsSBDefGeoType()         { return theRsSBDefGeoType; }
    static TString  GetRsSBBaseMaterial()       { return theRsSBBaseMaterial; }
   
    static Int_t    GetRsSBNSections1()         { return theRsSBNSections1; }
    static Int_t    GetRsSBNSections2()         { return theRsSBNSections2; }
   
    static Double_t GetRsSBLen()                { return theRsSBLen;        }
    static Double_t GetRsSBSize()               { return theRsSBSize;       }
    static Double_t GetRsSBWidth()              { return theRsSBWidth;      }  
    
    /*============ RSSEC (SOLENOIDAL RANGE SYSTEM ENDCAPS) =============*/
    
    static Int_t    GetRsSECDefGeoType()        { return theRsSECDefGeoType;   } 
    static TString  GetRsSECBaseMaterial()      { return theRsSECBaseMaterial; }

    static Double_t GetRsSECMinDist()           { return theRsSECMinDist;    }
    static Double_t GetRsSECSize()              { return theRsSECSize;       }
    static Double_t GetRsSECWidth()             { return theRsSECWidth;      }
    static Double_t GetRsSECThickness()         { return theRsSECThickness;  } 
    
/*==============================================================================*/
/*= ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ =*/
/*= +++++++++++++++++++++++++++ HYBRID GEOMETRY ++++++++++++++++++++++++++++++ =*/
/*= ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ =*/
/*==============================================================================*/

    static Int_t    GetHybMagnetDefGeoType()    { return theHybMagnetDefGeoType;   }
    static TString  GetHybMagnetCoilMaterial()  { return theHybMagnetCoilMaterial; }
    
    static Double_t GetHybMagnetDistToAxis()    { return theHybMagnetDistToAxis;  } 
    static Double_t GetHybMagnetDistToAxis1()   { return theHybMagnetDistToAxis1; } 
    static Double_t GetHybMagnetDistToAxis2()   { return theHybMagnetDistToAxis2; } 
    static Double_t GetHybMagnetDistToAxisC()   { return theHybMagnetDistToAxisC; }
    
    static Double_t GetHybMagnetLen()           { return theHybMagnetLen;     }     
    static Double_t GetHybMagnetSize()          { return theHybMagnetSize;    }     
    static Double_t GetHybMagnetCoilLen()       { return theHybMagnetCoilLen; }     
    static Double_t GetHybMagnetRingSize()      { return theHybMagnetRingSize; }  
    
    static Double_t GetHybMagnetCoilThickness() { return theHybMagnetCoilThickness; }
    static Double_t GetHybMagnetCoilWidth()     { return theHybMagnetCoilWidth;     }
    static Double_t GetHybMagnetCoilRR()        { return theHybMagnetCoilRR;        }

    /*==================================================================*/
    
    void   OpenGeometry(TString media = "media.geo");
    
    void   DefineTorGeometrySet(Int_t type);
    void   DefineSolGeometrySet(Int_t type);
    void   DefineHybGeometrySet(Int_t type = 2);
    void   DefineQslGeometrySet();
   
    void   ConstructGeometry();
    
    void   AddPassive(SpdPassiveModule* module);
    void   AddDetector(SpdDetector* detector);
    
    void   UnsetMaterials(Bool_t precise, TString option = "base", 
                          Bool_t active = true, Bool_t passive = true);
    void   SaveEmptyHits(Bool_t savehits = true);
   
    void   FillModulesMap();
   
    void   PrintGeometryList(); 
    void   PrintGeometry();
    
    FairModule*       SearchForModule(Int_t id) const;
    SpdPassiveModule* SearchForPassive(Int_t id) const;
    SpdDetector*      SearchForActive(Int_t id) const; 
    
    static Bool_t IsPassive(Int_t id) { return  (id < 0); }
    static Bool_t IsActive(Int_t id)  { return !(id < 0); }
    
    Int_t  GetNDetectors() const { return theDetectors.size(); }
    Int_t  GetNPassives()  const { return thePassives.size();  }
    
    SpdDetector**      GetListOfDetectors(Int_t& nd);
    SpdPassiveModule** GetListOfPassives(Int_t& np);
    
    TString GetActualMediaFileName() const;  
    
    struct module_info {
      
        module_info(): id_(kSpdUndefined){}
      
        TString name_; 
        TString prefix_;
        Int_t   id_;
    };
    
    static void SetNGeoSectors(Int_t n);
    
private:
    
    void   SetTorHybridGeoPars();
    void   SetSolHybridGeoPars();
  
    static SpdCommonGeoMapper* fInstance;

    static TGeoVolume*  theMasterVolume;
    
    static Int_t    theNGeoSectors;            // number of sectors
    static Double_t theSectorClearance;        // cm, luft beetween the sectors
    
    /*============================= PIPE ===============================*/
     
    static Int_t    thePipeDefGeoType;         // geometry type
    static TString  thePipeMaterial1;          // the pipe material (geotype = 1,2,3,4)
    static TString  thePipeMaterial2;          // the pipe material (geotype = 4) 
    
    static Double_t thePipeOuterRadius;        // cm, pipe outer radius (size) 
    static Double_t thePipeThickness;          // cm, pipe thickness 
    
    static Double_t thePipeLength1;            // cm, the length of pipe (geotype = 1)
    static Double_t thePipeLength2;            // cm, the length of pipe (geotype = 2)
    static Double_t thePipeLength3;            // cm, the length of pipe (geotype = 3)
    
    /*===================== INNER TRACKING SYSTEM ======================*/

    static Int_t    theItsDefGeoType;          // geometry type 
    static TString  theItsBaseMaterial;        // the inner tracking system base material 
    static TString  theItsLadderMaterial;      // the inner tracker ladder material
    static TString  theItsSensorMaterial;      // the inner tracker sensor material
    static Int_t    theItsNLayers;             // the number of layers inside inner tracker
    static Double_t theItsMinRadius;           // cm, the inner tracker tube min. radius 
    static Double_t theItsMaxRadius;           // cm, the inner tracker tube max. radius 
    static Double_t theItsMaxLength;           // cm, the inner tracker tube max. length
    
    /*===================== BEAM-BEAM COUNTER ========================*/

    static Int_t    theBbcDefGeoType;          // geometry type 
    
    static TString  theBbcBaseMaterial;        // the BBC base material 
    
    static Double_t theBbcThickness;           // cm, thickness of BBC endcap 
    static Double_t theBbcSize;                // cm, size of BBC endcap
    static Double_t theBbcWidth;               // cm, width of BBC endcap 
    
    static Double_t theBbcMinDist;             // cm, minimal distance to (0,0,0)
    
    /*===================== TIME-OF-FLIGHT SYSTEM (BARREL) ======================*/
    
    static Int_t    theTofBDefGeoType;         // barrel geometry type
    
    static TString  theTofBBaseMaterial;       // barrel base material
    
    static Double_t theTofBLength;             // cm, section length of barrel 
    static Double_t theTofBSize;               // cm, section size of barrel 
    static Double_t theTofBWidth;              // cm, section width of barrel 
    
    /*===================== TIME-OF-FLIGHT SYSTEM (ENDCAPS) =====================*/
    
    static Int_t    theTofECDefGeoType;        // endcap geometry type
    
    static TString  theTofECBaseMaterial;      // endcap base material
    
    static Double_t theTofECThickness;         // cm, section thickness of endcap 
    static Double_t theTofECSize;              // cm, section size of endcap
    static Double_t theTofECWidth;             // cm, section width of endcap 
    
    static Double_t theTofECMinDist;           // cm, minimal distance to (0,0,0)
 
    /*===================== AEROGEL (AEG) ======================*/
    
    static Int_t    theAegDefGeoType;          // geometry type 
    
    static TString  theAegBaseMaterial;        // the Aerogel base material 
    
    static Double_t theAegThickness;           // cm, thickness of Aerogel endcap 
    static Double_t theAegSize;                // cm, size of Aerogel endcap
    static Double_t theAegWidth;               // cm, width of Aerogel endcap 
    
    static Double_t theAegMinDist;             // cm, minimal distance to (0,0,0)
 
    /*==================== BASKET (TOR.GEOM.) ==========================*/

    static Int_t    theTorBasketDefGeoType;    // basket geometry type 
    static Double_t theTorBasketTotalLen;      // basket total length (summing all sections)
    
    static TString  theTorBasketBaseMaterial;  // the basket base material  
    static TString  theTorBasketLeafMaterial;  // the basket leaf material  
    static TString  theTorBasketCasingMaterial;// the basket casing material   
    
    /*==================== TOROIDAL MAGNET =============================*/
    
    static Int_t    theTorMagnetDefGeoType;    // geometry type
    static TString  theTorMagnetCoilMaterial;  // the magnet coil material 
       
    static Double_t theTorMagnetDistToAxis1;   // cm, minimal distance to central axis (geotype = 1)
    static Double_t theTorMagnetDistToAxis2;   // cm, minimal distance to central axis (geotype = 2)
    
    static Double_t theTorMagnetLen11;         // cm, central section length (geotype = 1)
    static Double_t theTorMagnetSize11;        // cm, central section/coil size (geotype = 1)
    
    static Double_t theTorMagnetLen12;         // cm, lateral section length (geotype = 1) 
    static Double_t theTorMagnetSize12;        // cm, central section/coil size (geotype = 1)
    
    static Double_t theTorMagnetLen2;          // cm, section length (geotype = 2)
    static Double_t theTorMagnetSize2;         // cm, section size (type = 2)
     
    static Double_t theTorMagnetCoilLen11;     // cm, central coil length (geotype = 1)
    static Double_t theTorMagnetCoilLen12;     // cm, lateral coil length (geotype = 1)
    
    static Double_t theTorMagnetCoilLen2;      // cm, coil length (geotype = 2)
    
    static Double_t theTorMagnetCoilThickness; // cm, coil thickness (geotype = 1,2)
    static Double_t theTorMagnetCoilWidth;     // cm, coil width (geotype = 1,2)
    static Double_t theTorMagnetCoilRR;        // cm, coil inner rouning radius (geotype = 1,2)
    
    /*=========== TSTB (TOROIDAL TRACKING SYSTEM BARREL) ===============*/

    static Int_t    theTsTBDefGeoType;         // geometry type
    
    static TString  theTsTBBaseMaterial;       // default base material
    
    static Double_t theTsTBLen11;              // cm, central section length of barrel (geotype = 1)
    static Double_t theTsTBSize11;             // cm, central section size of barrel  (geotype = 1)
    static Double_t theTsTBWidth11;            // cm, central section width of barrel (geotype = 1)
    
    static Double_t theTsTBLen12;              // cm, lateral section length of barrel (geotype = 1)
    static Double_t theTsTBSize12;             // cm, lateral section size  of barrel (geotype = 1)
    static Double_t theTsTBWidth12;            // cm, lateral section width of barrel (geotype = 1)

    static Double_t theTsTBLen2;               // cm, length of barrel (geotype = 2)
    static Double_t theTsTBSize2;              // cm, size of barrel (geotype = 2)
    static Double_t theTsTBWidth2;             // cm, width of barrel (geotype = 2)
    
    static Double_t theTsTBLen;                // cm, length of barrel (geotype = 3)
    static Double_t theTsTBSize;               // cm, size of barrel (geotype = 3)
    static Double_t theTsTBWidth;              // cm, width of barrel (geotype = 3)
    
    static Bool_t   theTsTBMakeOffset11;       // make clearance for central section
    static Bool_t   theTsTBMakeOffset12;       // make clearance for lateral section
    static Bool_t   theTsTBMakeOffset2;        // make clearance for the section
    static Bool_t   theTsTBMakeOffset;         // make clearance for the section
    
    /*========== TSTEC (TOROIDAL TRACKING SYSTEM ENDCAPS) ==============*/
    
    static Int_t    theTsTECDefGeoType;        // geometry type

    static TString  theTsTECBaseMaterial;      // default module base material
    static TString  theTsTECLayerMaterial;     // default layer base material

    static Double_t theTsTECMinDist1;          // cm, minimal distance to (0,0,0) (geotype = 1)
    static Double_t theTsTECMinDist2;          // cm, minimal distance to (0,0,0) (geotype = 2)
    static Double_t theTsTECMinDist3;          // cm, minimal distance to (0,0,0) (geotype = 3)

    static Double_t theTsTECSize;              // cm, full size
    static Double_t theTsTECWidth;             // cm, full width
    
    static Double_t theTsTECLength1;           // cm, full length (geotype = 1)
    static Double_t theTsTECLength2;           // cm, full length (geotype = 2)
    static Double_t theTsTECLength3;           // cm, full length (geotype = 3)
   
    /*===== ECALTB (TOROIDAL ELECTROMAGNETIC CALORIMETER BARREL) =======*/
    
    static Int_t    theEcalTBDefGeoType;       // geometry type
    
    static TString  theEcalTBBaseMaterial;     // default base material
     
    static Double_t theEcalTBLen11;            // cm, central section length of barrel (geotype = 1)
    static Double_t theEcalTBSize11;           // cm, central section size of barrel  (geotype = 1)
    static Double_t theEcalTBWidth11;          // cm, central section width of barrel (geotype = 1)

    static Double_t theEcalTBLen12;            // cm, lateral section length of barrel (geotype = 1)
    static Double_t theEcalTBSize12;           // cm, lateral section size  of barrel (geotype = 1)
    static Double_t theEcalTBWidth12;          // cm, lateral section width of barrel (geotype = 1)
    
    static Double_t theEcalTBLen2;             // cm, length of barrel (geotype = 2)
    static Double_t theEcalTBSize2;            // cm, size of barrel (geotype = 2)
    static Double_t theEcalTBWidth2;           // cm, width of barrel (geotype = 2)

    /*===== ECALTEC (TOROIDAL ELECTROMAGNETIC CALORIMETER ENDCAPS) ====*/
    
    static Int_t    theEcalTECDefGeoType;      // geometry type
                                               
    static TString  theEcalTECBaseMaterial;    // default base material
                                               
    static Double_t theEcalTECMinDist1;        // cm, minimal distance to (0,0,0) (geotype = 1)
    static Double_t theEcalTECMinDist2;        // cm, minimal distance to (0,0,0) (geotype = 2)
                                               
    static Double_t theEcalTECSize;            // cm, cap size
    static Double_t theEcalTECWidth;           // cm, cap width
    static Double_t theEcalTECThickness;       // cm, cap thickness
    
    /*============ RSTB (TOROIDAL RANGE SYSTEM BARREL) =================*/
    
    static Int_t    theRsTBDefGeoType;         // geometry type
    
    static TString  theRsTBBaseMaterial;       // default base material
    
    static Double_t theRsTBLen11;              // cm, central section length of barrel (geotype = 1)
    static Double_t theRsTBSize11;             // cm, central section size of barrel  (geotype = 1)
    static Double_t theRsTBWidth11;            // cm, central section width of barrel (geotype = 1)
    
    static Double_t theRsTBLen12;              // cm, lateral section length of barrel (geotype = 1)
    static Double_t theRsTBSize12;             // cm, lateral section size  of barrel (geotype = 1)
    static Double_t theRsTBWidth12;            // cm, lateral section width of barrel (geotype = 1)

    static Double_t theRsTBLen2;               // cm, length of barrel (geotype = 2)
    static Double_t theRsTBSize2;              // cm, size of barrel (geotype = 2)
    static Double_t theRsTBWidth2;             // cm, width of barrel (geotype = 2)
    
    /*============ RSTEC (TOROIDAL RANGE SYSTEM ENDCAPS) ===============*/
    
    static Int_t    theRsTECDefGeoType;        // geometry type

    static TString  theRsTECBaseMaterial;      // default base material

    static Double_t theRsTECMinDist1;          // cm, minimal distance to (0,0,0) (geotype = 1)
    static Double_t theRsTECMinDist2;          // cm, minimal distance to (0,0,0) (geotype = 2)

    static Double_t theRsTECSize;              // cm, cap size
    static Double_t theRsTECWidth;             // cm, cap width
    static Double_t theRsTECThickness;         // cm, cap thickness
                                               
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    
    /*==================== SOLENOIDAL MAGNET ===========================*/
    
    static Int_t    theSolMagnetDefGeoType;    // geometry type
    static TString  theSolMagnetBaseMaterial;  // the magnet material
    static TString  theSolMagnetCryoMaterial;  // the cryostat material
    
    static Int_t    theSolMagnetNSections1;    // number of sections (geotype = 1) 
    static Int_t    theSolMagnetNSections2;    // number of sections (geotype = 2) 
   
    static Double_t theSolMagnetTotalLen1;     // cm, total length (geotype = 1)
    static Double_t theSolMagnetTotalSize1;    // cm, size (geotype = 1)
    static Double_t theSolMagnetTotalWidth1;   // cm, width (geotype = 1)
    
    static Double_t theSolMagnetLen1;          // cm, core length, (geotype = 1)
    static Double_t theSolMagnetSize1;         // cm, core size, (geotype = 1)
    static Double_t theSolMagnetWidth1;        // cm, core width (geotype = 1)

    static Double_t theSolMagnetInnerCryoW1;   // cm, inner part cryostat width, (geotype = 1)
    static Double_t theSolMagnetOuterCryoW1;   // cm, outer part (z-) cryostat width, (geotype = 1)
    static Double_t theSolMagnetEndfCryoW1;    // cm, end face part (z+) cryostat width, (geotype = 1)

    static Double_t theSolMagnetLen2;          // cm, total length (geotype = 2)
    static Double_t theSolMagnetSize2;         // cm, size (geotype = 2)
    static Double_t theSolMagnetWidth2;        // cm, width (geotype = 2)
    
	/*==================== SOLENOIDAL MAGNET2 ===========================*/
	 
	static Int_t    theSolMagnetTwoDefGeoType;    // geometry type
	static TString  theSolMagnetTwoBaseMaterial;  // the magnet material
	static TString  theSolMagnetTwoCryoMaterial;  // the cryostat material
	 
	static Int_t    theSolMagnetTwoNSections;    // number of sections (geotype = 1) 
	 
	static Double_t theSolMagnetTwoTotalLen;     // cm, total length (geotype = 1)
	static Double_t theSolMagnetTwoTotalSize;    // cm, size (geotype = 1)
	static Double_t theSolMagnetTwoTotalWidth;   // cm, width (geotype = 1)
	 
	static Double_t theSolMagnetTwoLen;          // cm, core length, (geotype = 1)
	static Double_t theSolMagnetTwoSize;         // cm, core size, (geotype = 1)
	static Double_t theSolMagnetTwoWidth;        // cm, core width (geotype = 1)
	 
	static Double_t theSolMagnetTwoInnerCryoW;   // cm, inner part cryostat width, (geotype = 1)
	static Double_t theSolMagnetTwoOuterCryoW;   // cm, outer part (z-) cryostat width, (geotype = 1)
	static Double_t theSolMagnetTwoEndfCryoW;    // cm, end face part (z+) cryostat width, (geotype = 1)

    /*========== TSSB (SOLENOIDAL TRACKING SYSTEM BARREL) ==============*/
    
    static Int_t    theTsSBDefGeoType;         // geometry type
    
    static TString  theTsSBBaseMaterial;       // default base material
    
    static Int_t    theTsSBNSections1;         // number of sections (geotype = 1)
  
    static Double_t theTsSBLen;                // cm, length of barrel    
    static Double_t theTsSBSize;               // cm, size of barrel
    static Double_t theTsSBWidth;              // cm, width of barrel 
    
    /*========= TSSEC (SOLENOIDAL TRACKING SYSTEM ENDCAPS) =============*/
    
    static Int_t    theTsSECDefGeoType;        // geometry type
    
    static TString  theTsSECBaseMaterial;      // default base material
     
    static Double_t theTsSECMinDist;           // cm, minimal distance to (0,0,0)
    static Double_t theTsSECSize;              // cm, cap size
    static Double_t theTsSECWidth;             // cm, cap width
    static Double_t theTsSECThickness;         // cm, cap thickness
    
    /*===== ECALSB (SOLENOIDAL ELECTROMAGNETIC CALORIMETER BARREL) ======*/
    
    static Int_t    theEcalSBDefGeoType;       // geometry type
    
    static TString  theEcalSBBaseMaterial;     // default base material
    
    static Int_t    theEcalSBNSections1;       // number of sections (geotype = 1)
    static Int_t    theEcalSBNSections2;       // number of sections (geotype = 2) 
    
    static Double_t theEcalSBLen;              // cm, length of barrel    
    static Double_t theEcalSBSize;             // cm, size of barrel
    static Double_t theEcalSBWidth;            // cm, width of barrel 
    
    /*===== ECALSEC (SOLENOIDAL ELECTROMAGNETIC CALORIMETER ENDCAPS) ====*/

    static Int_t    theEcalSECDefGeoType;      // geometry type
    
    static TString  theEcalSECBaseMaterial;    // default base material
     
    static Double_t theEcalSECMinDist;         // cm, minimal distance to (0,0,0)
    static Double_t theEcalSECSize;            // cm, cap size
    static Double_t theEcalSECWidth;           // cm, cap width
    static Double_t theEcalSECThickness;       // cm, cap thickness
    
    /*============= RSSB (SOLENOIDAL RANGE SYSTEM BARREL) ==============*/
    
    static Int_t    theRsSBDefGeoType;         // geometry type
    
    static TString  theRsSBBaseMaterial;       // default base material
    
    static Int_t    theRsSBNSections1;         // number of sections (geotype = 1)
    static Int_t    theRsSBNSections2;         // number of sections (geotype = 2) 
    
    static Double_t theRsSBLen;                // cm, length of barrel    
    static Double_t theRsSBSize;               // cm, size of barrel
    static Double_t theRsSBWidth;              // cm, width of barrel     
    
    /*============ RSSEC (SOLENOIDAL RANGE SYSTEM ENDCAPS) =============*/

    static Int_t    theRsSECDefGeoType;        // geometry type
    
    static TString  theRsSECBaseMaterial;      // default base material
     
    static Double_t theRsSECMinDist;           // cm, minimal distance to (0,0,0)
    static Double_t theRsSECSize;              // cm, cap size
    static Double_t theRsSECWidth;             // cm, cap width
    static Double_t theRsSECThickness;         // cm, cap thickness    
    
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    
    /*======================= HYBRID MAGNET =============================*/

    static Int_t    theHybMagnetDefGeoType;     // geometry type
    static TString  theHybMagnetCoilMaterial;   // the magnet coil material
    
    static Double_t theHybMagnetDistToAxis;     // cm, minimal distance to central axis for the toroidal coil
    static Double_t theHybMagnetDistToAxis1;    // cm, minimal distance to central axis for the first ring coil
    static Double_t theHybMagnetDistToAxis2;    // cm, minimal distance to central axis for the second ring coil
    static Double_t theHybMagnetDistToAxisC;    // cm, minimal distance to central axis for the central ring coil
    
    static Double_t theHybMagnetLen;            // cm, central section length 
    static Double_t theHybMagnetSize;           // cm, central section/coil size  
    static Double_t theHybMagnetCoilLen;        // cm, central coil length  
    static Double_t theHybMagnetRingSize;       // cm, ring coil size (max. radius)
    
    static Double_t theHybMagnetCoilThickness;  // cm, coil thickness
    static Double_t theHybMagnetCoilWidth;      // cm, coil width
    static Double_t theHybMagnetCoilRR;         // cm, central coil inner rouning radius 
    
    /*===================================================================*/
     
    std::map <SpdPassiveModule*, module_info*> thePassives;
    std::map <SpdDetector*, module_info*>      theDetectors;

    ClassDef(SpdCommonGeoMapper,1)
};

#endif  /* __SPDCOMMONGEOMAPPER_H__ */

