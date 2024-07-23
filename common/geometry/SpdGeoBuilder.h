// $Id$
// Author: artur   2018/05/07

#ifndef __SPDGEOBUILDER_H__
#define __SPDGEOBUILDER_H__

#include <TGeoVolume.h>
#include <TVector3.h>
#include <map>

#include "SpdGeoMapper.h"
#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdGeoBuilder                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TGeoNavigator;

class SpdGeoBuilder: public TObject {

public:

    SpdGeoBuilder();
    virtual ~SpdGeoBuilder();
    
    virtual void ClearVolumes() { fVolumes.clear(); } 
    
    virtual Bool_t ConstructGeometry() { return kFALSE; }
    
    virtual void CheckActiveVolumes();
      
    virtual void LockGeometry()   { fLockGeometry = kTRUE;  }
    virtual void UnlockGeometry() { fLockGeometry = kFALSE; }
    
    virtual void SetMapper(SpdGeoMapper* /*mapper*/) { }
    virtual void SetMasterVolume(TGeoVolume* vol);
   
    virtual SpdGeoMapper* GetMapper() const { return 0; }
    virtual SpdGeoTable*  GetTable()  const;
    
    virtual TGeoMedium* GetMedia(TString medname);    
    virtual TGeoVolume* GetVolume(TString volname);

    TGeoVolume* GetMasterVolume() const { return fMasterVolume; } 
   
    Bool_t  IsGeometryLocked() { return fLockGeometry; }
    
    void   SetMaxBuildGeoLevel(Int_t level) { fMaxBuildGeoLevel = level; }
    Int_t  GetMaxBuildGeoLevel() const { return fMaxBuildGeoLevel; }
    
    Int_t  GetActualGeoLevel() const;
    
    virtual void FillParametersIn(SpdParSet* params);
    virtual void LoadParametersFrom(SpdParSet* params);
    
    virtual void Print(Option_t*) const;
     
    /* ++++++++++++++++++++++++++++++++++++ GEOMETRY NAVIGATION ++++++++++++++++++++++++++++++++++++ */
    
    virtual TGeoNavigator* GetNavigator();
       
    virtual TGeoNavigator* SetNodePath(const TString& path); // -> set CURRENT NODE
    virtual TGeoNavigator* SetNodePath(Long_t detTID);       // -> set CURRENT NODE
    
    // get current node path
    virtual TString  GetNodePath(); 
    
    // get current node volume
    virtual TGeoVolume* GetNodeVolume(); 
    
    // get current node size (box)
    virtual Bool_t GetNodeSize(Double_t* l);  // l = Lx/2, Ly/2, Lz/2
    
    // get current node volume
    virtual TString GetNodeVolumeName(); 
    
    // detector position in  global c.s.
    virtual Bool_t GetDetectorPosition(Double_t* p);      // x, y ,z (cartesian c.s.)
    
    virtual Bool_t GetDetectorPositionXYZ(Double_t* p);   // x, y ,z (cartesian c.s.)
    virtual Bool_t GetDetectorPositionRPZ(Double_t* p);   // r, phi, z (cylindrical c.s.)
    virtual Bool_t GetDetectorPositionRTP(Double_t* p);   // r, theta, phi (shperical c.s.)
    
    // detector orientation in global c.s.
    virtual Bool_t GetDetectorOrientation(Double_t* v, Char_t dir = 'z' /*x,y,z*/);  
 
    // transform POINT from global to local c.s. of a given node
    virtual Bool_t GlobalToLocalP(const Double_t* gpnt /*global*/, Double_t* lpnt /*local*/); 

    // transform POINT to global from local c.s. of a given node   
    virtual Bool_t LocalToGlobalP(const Double_t* lpnt /*local*/, Double_t* gpnt /*global*/);
    
    // transform VECTOR from global to local c.s. of a given node 
    virtual Bool_t GlobalToLocalV(const Double_t* gvct /*global*/, Double_t* lvct /*local*/); 
      
    // transform VECTOR to global from local c.s. of a given node
    virtual Bool_t LocalToGlobalV(const Double_t* lvct /*local*/, Double_t* gvct /*global*/); 
    
    virtual void PrintGeoVolume(const TGeoVolume* vol, Int_t option = 0);
    
protected:
 
    virtual void ConstructVolumes(const SpdGeoTable* geotable);
    virtual TGeoVolume* BuildVolume(const SpdGeoVolPars*  vpars) { return 0; }
    
    void CheckActivity(TGeoVolume* vol, std::set<TString>& actvols);

    std::map<TString,TGeoVolume*> fVolumes; //!
     
    TGeoVolume* fMasterVolume; //!
    Bool_t      fLockGeometry; //!

    /* Parameters */
    Int_t       fMaxBuildGeoLevel; //!
   
private:
  
    //std::map<TString,TGeoVolume*> fVolumes; //!
   
    ClassDef(SpdGeoBuilder,1)
};

#endif  /* __SPDGEOBUILDER_H__ */

