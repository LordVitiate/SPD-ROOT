// $Id$
// Author: artur   2021/02/20

#ifndef __SPDVERTEXRC_H__
#define __SPDVERTEXRC_H__

#include <TObject.h>
#include <TVector3.h>
#include <set>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdVertexRC                                                                //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdVtxFitPar;

class SpdVertexRC: public TObject {

public:
    
    SpdVertexRC();
    SpdVertexRC(Int_t vtype);
    virtual ~SpdVertexRC();
    
    virtual void DeleteFitPars();
    virtual void Clear(Option_t* option = "");
    virtual void RemoveTracks() { fTracks.clear(); }
   
    void AddTrack(Int_t track_id) { fTracks.insert(track_id); }

    //------------ Setters ------------

    void SetId(Int_t id)  { fId = id; }
    void SetAsPrimary()   { fVertexType = 0; }
    void SetVertexType(Int_t vtype); // vtype = 0,1,2
    
    // get (or create) vertex fit paramaters
    SpdVtxFitPar* FitPars();
    
    //------------ Getters ------------
    
    Int_t  GetId()             const { return fId; }
    Int_t  GetVertexType()     const { return fVertexType; }
    Bool_t IsPrimary()         const { return (fVertexType == 0); }
    
    Bool_t HasTracks()         const { return !fTracks.empty(); }
    Int_t  GetNTracks()        const { return fTracks.size();   }
    Bool_t FindTrack(Int_t id) const { return (fTracks.find(id) != fTracks.end()); }
    
    // WARNING Slow method (complicity ~O(i))
    Int_t  GetTrack(Int_t i) const;
    
    const std::set<int>& GetTracks() const { return fTracks; }
    
    SpdVtxFitPar* GetFitPars() const { return fFitPars; }
 
    virtual void Print(Option_t* option = "") const;
 
private:
    
    Int_t            fId;         // vertex unique id (index in the corresponding array); id < 0 = undefined
    Int_t            fVertexType; // vertex (parameters) type (0=primary, >0: secondary)
    std::set<Int_t>  fTracks;     // set of vertex track ids
    SpdVtxFitPar*    fFitPars;    // fit parameters

    ClassDef(SpdVertexRC,1)
};

//--------------------------------------------------------------------------------
inline void SpdVertexRC::SetVertexType(Int_t vtype)
{ 
         if (vtype < 1) fVertexType = 0;
    else if (vtype > 2) fVertexType = 2;
    else fVertexType = vtype;
}

#endif  /* __SPDVERTEXRC_H__ */

