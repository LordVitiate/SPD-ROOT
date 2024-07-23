// $Id$
// Author: artur   2021/02/15

#ifndef __SPDTRACKPROPAGATORGF_H__
#define __SPDTRACKPROPAGATORGF_H__

#include <TObject.h>
#include "SpdTrackState.h"
#include "AbsTrackRep.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTrackPropagatorGF                                                       //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdGFMagneticField;
class SpdField;

class SpdTrackPropagatorGF: public TObject {

public:

    SpdTrackPropagatorGF();
    virtual ~SpdTrackPropagatorGF();
    
    // ilnitialize geometry and magnetic field
    Bool_t    Init(); 
    
    // propagation direction (-1, 0, 1) -> (backward, auto, forward)
    Bool_t    InitTrack(Int_t pdg, Char_t propagateDir = 0);
    
    // switch (on/off) material effects
    Bool_t    SetMaterialEffects(Bool_t on = true);
    
    // methods return covered distance
    //------------------------------------------
    Double_t  ExtrapolateToPlane(TVector3 plane_point, TVector3 plane_norm, SpdTrackState& state); 
    Double_t  ExtrapolateToLine(TVector3 line_point, TVector3 line_dir, SpdTrackState& state);
    Double_t  ExtrapolateToPoint(TVector3 point, SpdTrackState& state);
    Double_t  ExtrapolateToCylinder(Double_t radius /*cm*/, SpdTrackState& state,
                                    TVector3 line_point = TVector3(0,0,0), TVector3 line_dir = TVector3(0,0,1));
    Double_t  ExtrapolateToSphere(Double_t radius /*cm*/, SpdTrackState& state, 
                                  TVector3 point = TVector3(0,0,0));
    Double_t  ExtrapolateBy(Double_t step /*cm*/, SpdTrackState& state);
    
    //-----------------------------------------
    Double_t  ExtrapolateToPlane(TVector3 plane_point, TVector3 plane_norm, 
                                 const SpdTrackState& start_state, SpdTrackState& final_state);
    
    Double_t  ExtrapolateToLine(TVector3 line_point, TVector3 line_dir, 
                                const SpdTrackState& start_state, SpdTrackState& final_state);
    
    Double_t  ExtrapolateToPoint(TVector3 point,
                                 const SpdTrackState& start_state, SpdTrackState& final_state);
    
    Double_t  ExtrapolateToCylinder(Double_t radius /*cm*/, 
                                    const SpdTrackState& start_state, SpdTrackState& final_state,
                                    TVector3 line_point = TVector3(0,0,0), TVector3 line_dir = TVector3(0,0,1)
                                   );
    
    Double_t  ExtrapolateToSphere(Double_t radius /*cm*/, 
                                  const SpdTrackState& start_state, SpdTrackState& final_state,
                                  TVector3 point = TVector3(0,0,0));
    
    Double_t  ExtrapolateBy(Double_t step,
                            const SpdTrackState& start_state, SpdTrackState& final_state);
    
    // Getters
    Bool_t                  IsInit()          const { return fIsInit;      }
    
    genfit::AbsTrackRep*    GetTrackRep()     const { return fTrackRep;    }
    
    // current state info
    const TVectorD&         GetState5()       const { return fState5;      }
    const TMatrixDSym&      GetStateCov5x5()  const { return fStateCov5x5; }
    const genfit::DetPlane* GetState5Plane()  const { return fState5Plane; }
     
    SpdField*               GetField();
    
protected:
    
    /* ----- Methods ----- */
    
    virtual Bool_t LoadGeometry();
    virtual Bool_t LoadField();
    
    /* ----- Data members ----- */
    
    Bool_t  fIsInit;  //!

    genfit::AbsTrackRep*    fTrackRep;     //! GenFit track representation    
    SpdField*               fExtField;     //! external defined magnetic field (optional)
    
    TVectorD                fState5;       //! 5-dim state
    TMatrixDSym             fStateCov5x5;  //! 5x5 covariance matrix
    const genfit::DetPlane* fState5Plane;  //! plane for 5-dim state 
    
    ClassDef(SpdTrackPropagatorGF,1)
};

#endif  /* __SPDTRACKPROPAGATORGF_H__ */

