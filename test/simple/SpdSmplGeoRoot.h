#ifndef __SPDSMPLGEOROOT_H__
#define __SPDSMPLGEOROOT_H__

#include <Riostream.h>
#include <TGeoManager.h>
#include <TGeoVolume.h>
#include <TGeoMatrix.h>
#include <TString.h>

#define DEFAULT_MED_PARS 20

class SpdSmplGeoRoot: public TObject {

public:
        
    SpdSmplGeoRoot(TString modname = "SPDS01", TString name = "SpdSmplGeoRoot", TString title = "SpdSmplGeoRoot");
    virtual ~SpdSmplGeoRoot();
    
    inline void    SetModuleName(TString name) { theMODULEname = name; }
    inline TString FName(TString name)         { return theMODULEname + "_" + name; }
    
    bool SetMediumOutFile(TString path, TString name = "spds_media.geo") { return OpenFile(path,name,"Medium");   }
    bool SetGeometryOutFile(TString path, TString name = "spds_det.geo") { return OpenFile(path,name,"Geometry"); }
    
    void MakeMediaFile();
    
    /// Build detectors
    bool Build(); 
    
    void BuildBarrel();
    void BuildVertexDetector();
    void BuildStrawDetectors();
    void BuildEcalTOFDetectors();
    void BuildEnvDetectors();
    
private:
        
    /// methods
    void        SetDefaultMedia();
    bool        OpenFile(TString& path, TString& name, TString type);
    bool        AddNode(TGeoVolume* mother, TGeoVolume* vol, int node_id, TGeoMatrix* matrix);
    TGeoMedium* FindMedium(TString name);
    TString     GetShape(TGeoVolume* vol);
    bool        AddShapeData(TGeoVolume* vol, FILE* f);
    
    /// data memebers   
    TGeoVolume* Top_;
    bool        Closed_;
    TString     theTOPname;
    TString     theMODULEname;
    
    FILE*   FMedia_;
    FILE*   FGeom_;
    
    /// auxiliary
    TString  Delim_;
    
    double   DefMPars_[DEFAULT_MED_PARS]; 

    ClassDef(SpdSmplGeoRoot,1)
};

#endif  /* __SPDSMPLGEOROOT_H__ */

