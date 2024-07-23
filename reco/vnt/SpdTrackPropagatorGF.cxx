// $Id$
// Author: artur   2021/02/15

//_____________________________________________________________________________
//
// SpdTrackPropagatorGF
//_____________________________________________________________________________

#include "SpdTrackPropagatorGF.h"

#include "SpdRunAna.h"

#include "SpdFieldCreator.h"
#include "SpdField.h"
#include "SpdFieldPar.h"
#include "SpdGFMagneticField.h"

#include "SpdRKTrackRep.h"

#include "GFCore.hh"
#include "GFTrackReps.hh"

#include <TGeoManager.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace genfit;

ClassImp(SpdTrackPropagatorGF)

//_____________________________________________________________________________
SpdTrackPropagatorGF::SpdTrackPropagatorGF():fIsInit(false),
fTrackRep(0),fExtField(0),fState5Plane(0)
{
 
}

//_____________________________________________________________________________
SpdTrackPropagatorGF::~SpdTrackPropagatorGF() 
{
   if (fTrackRep) delete fTrackRep;
}

//_____________________________________________________________________________
Bool_t SpdTrackPropagatorGF::LoadGeometry() // private
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance(); 
   if (!gGeoManager && run) run->GetParameters("FairGeoParSet");
   return (gGeoManager) ? true : false; 
}

//_____________________________________________________________________________
Bool_t SpdTrackPropagatorGF::LoadField() // private
{
   SpdField* field(0);
   SpdGFMagneticField* Field(0);

   // Check if magnetis SPD field already initialized
   
   if (FieldManager::getInstance()->isInitialized()) 
   {
       Field = dynamic_cast<SpdGFMagneticField*>(FieldManager::getInstance()->getField());
       if (Field) {
            cout << "\t-I- <SpdTrackPropagatorGF::LoadField> "
                 << "Field has been already initialized ... ok" << endl;
       }
       return (Field) ? true : false;
   }

   // Searh for external field
   
   if (fExtField) 
   {
       field = fExtField; 
       
       // Initialize GenFit field
       Field = new SpdGFMagneticField();
       Field->SetField(field,false); // ATTENTION  false = not field owner! 
    
       // Initialize GenFit field manager
       FieldManager::getInstance()->init(Field);   
       return true;
   }
   
   // Try to create field using its parameters
   
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance(); 
   if (!run) return false;
   
   SpdFieldPar* field_pars = dynamic_cast<SpdFieldPar*>(run->GetParameters("SpdFieldPar"));
   
   if (!field_pars) {
       SpdSetParSet* pars = (SpdSetParSet*)run->GetParameters("SpdSetParSet");
       if (pars) {
           field_pars = pars->GetFieldPars();
           //((FairParSet*)field_pars)->init();
       }
   }
   
   if (!field_pars) {
       cout << "\t-E- <SpdTrackPropagatorGF::LoadField> No field parameters (\"SpdFieldPar\")" << endl;
       return false; 
   }
   
   // Create SPD field using parameters
   SpdFieldCreator* fieldCreator = new SpdFieldCreator();  
   field = (SpdField*)fieldCreator->createFairField(field_pars);
   if (field) field->Print(""); // FIXME
   delete fieldCreator;
    
   // Initialize GenFit field
   Field = new SpdGFMagneticField();
   Field->SetField(field,true);   // ATTENTION  field owner! 
    
   // Initialize GenFit field manager
   FieldManager::getInstance()->init(Field); 
   
   //FieldManager::getInstance()->useCache(true);
   
   return true;
}

//_____________________________________________________________________________
SpdField* SpdTrackPropagatorGF::GetField()
{
   FieldManager* fm = FieldManager::getInstance();
   if (!fm->isInitialized()) return 0;
   SpdGFMagneticField* field = dynamic_cast<SpdGFMagneticField*>(fm->getField());
   return (field) ? field->GetField() : 0;
}

//_____________________________________________________________________________
Bool_t SpdTrackPropagatorGF::Init()
{
   cout << "\t-I- <SpdTrackPropagatorGF::Init> ... " << endl; 
   
   if (fIsInit) return fIsInit;
   
   // Load Geometry
       
   if (!LoadGeometry()) {
       cout << "\t-E- <SpdTrackPropagatorGF::Init> Geometry not defined " << endl;
       return false;  
   }
   
   // Load field
   
   if (!LoadField()) {
       cout << "\t-E- <SpdTrackPropagatorGF::Init> Bad field initialization " << endl;
       return false; 
   }
   
   // Init GF materials 
   
   if (!MaterialEffects::getInstance()->isInitialized()) {
       MaterialEffects::getInstance()->init(new TGeoMaterialInterface());
       MaterialEffects::getInstance()->setNoEffects(false);
       cout << "\t-I- <SpdTrackPropagatorGF::Init> "
            << "Geometry initialization ... ok " << endl;
   }
   else {
       cout << "\t-I- <SpdTrackPropagatorGF::Init> "
            << "Geometry has been already initialized ... ok " << endl;
   }   
   
   fIsInit = true;
    
   cout << "\t-I- <SpdTrackPropagatorGF::Init> Intialization successfull " << endl;
     
   return fIsInit;   
}

//_____________________________________________________________________________
Bool_t SpdTrackPropagatorGF::InitTrack(Int_t pdg, Char_t propagateDir)
{ 
   if (!fIsInit) return false;
   if (fTrackRep) delete fTrackRep;
   fTrackRep = new SpdRKTrackRep(pdg,propagateDir);
   return true;
}

//_____________________________________________________________________________
Bool_t SpdTrackPropagatorGF::SetMaterialEffects(Bool_t on)
{
   if (!fIsInit) return false;
   if (MaterialEffects::getInstance()) {
       MaterialEffects::getInstance()->setNoEffects(!on);
       return true;
   }
}

//_____________________________________________________________________________
Double_t SpdTrackPropagatorGF::ExtrapolateToPlane(TVector3 plane_point, TVector3 plane_norm, 
                                                  SpdTrackState& state)
{
   if (!fIsInit || !fTrackRep) {
       cout << "-W- <SpdTrackPropagatorGF::ExtrapolateToPlane> Propagator not initialized properly" << endl;
       return 0;
   } 
   
   SharedPlanePtr plane_ptr = SharedPlanePtrCreator::getPlanePtr(new DetPlane(plane_point,plane_norm));

   TVector3 pos = state.GetPosition();
   TVector3 mom = state.GetMomentum();

   Double_t dist = 0;
   Bool_t extrap = false;
   
   if (!state.IsCovDefined()) 
   {
       StateOnPlane* sop = new StateOnPlane(fTrackRep);
       fTrackRep->setPosMom(*sop,pos,mom);
       extrap = true;
       try { dist = sop->extrapolateToPlane(plane_ptr); }
       catch (Exception& e) { extrap = false; }
       if (extrap) {
           sop->getPosMom(pos,mom);
           state.SetTime(fTrackRep->getTime(*sop));
           state.SetMomSigma(0);
           fState5.ResizeTo(sop->getState()); 
           fState5 = sop->getState();
           fState5Plane = sop->getPlane().get();
       }
       delete sop;
   }
   else 
   {
       MeasuredStateOnPlane* sop = new MeasuredStateOnPlane(fTrackRep);
       TMatrixDSym cov; 
       state.GetCovariance(cov);
       fTrackRep->setPosMomCov(*sop,pos,mom,cov);
       extrap = true;
       try { dist = sop->extrapolateToPlane(plane_ptr); }
       catch (Exception& e) { extrap = false; }
       if (extrap) {
           sop->getPosMomCov(pos,mom,cov);
           state.SetCovariance(cov);
           state.SetTime(fTrackRep->getTime(*sop));
           state.SetMomSigma(TMath::Sqrt(sop->getMomVar()));
           fState5.ResizeTo(sop->getState()); 
           fState5 = sop->getState();
           fStateCov5x5.ResizeTo(sop->getCov());
           fStateCov5x5 = sop->getCov();
           fState5Plane = sop->getPlane().get();
       }
       delete sop;
   }

   if (extrap) {
       state.SetPosition(pos);
       state.SetMomentum(mom);
       return dist;
   }
   
   return 0;
}

//_____________________________________________________________________________
Double_t SpdTrackPropagatorGF::ExtrapolateToLine(TVector3 line_point, TVector3 line_dir, 
                                                 SpdTrackState& state)
{ 
   if (!fIsInit || !fTrackRep) {
       cout << "-W- <SpdTrackPropagatorGF::ExtrapolateToLine> Propagator not initialized properly" << endl;
       return 0;
   } 
   
   TVector3 pos = state.GetPosition();
   TVector3 mom = state.GetMomentum();
   
   Double_t dist = 0;
   Bool_t extrap = false;
   
   if (!state.IsCovDefined()) 
   {
       StateOnPlane* sop = new StateOnPlane(fTrackRep);
       fTrackRep->setPosMom(*sop,pos,mom);
       extrap = true;
       try { dist = sop->extrapolateToLine(line_point,line_dir); }
       catch (Exception& e) { extrap = false; }
       if (extrap) {
           sop->getPosMom(pos,mom);
           state.SetTime(fTrackRep->getTime(*sop));
           state.SetMomSigma(0);
           fState5.ResizeTo(sop->getState()); 
           fState5 = sop->getState();
           fState5Plane = sop->getPlane().get();
       }
       delete sop;
   }
   else 
   {
       MeasuredStateOnPlane* sop = new MeasuredStateOnPlane(fTrackRep);
       TMatrixDSym cov; 
       state.GetCovariance(cov);
       fTrackRep->setPosMomCov(*sop,pos,mom,cov);
       extrap = true;
       try { dist = sop->extrapolateToLine(line_point,line_dir); }
       catch (Exception& e) { extrap = false; }
       if (extrap) {
           sop->getPosMomCov(pos,mom,cov);
           state.SetCovariance(cov);
           state.SetTime(fTrackRep->getTime(*sop));
           state.SetMomSigma(TMath::Sqrt(sop->getMomVar()));
           fState5.ResizeTo(sop->getState()); 
           fState5 = sop->getState();
           fStateCov5x5.ResizeTo(sop->getCov());
           fStateCov5x5 = sop->getCov();
           fState5Plane = sop->getPlane().get();
       }
       delete sop;
   }

   if (extrap) {
       state.SetPosition(pos);
       state.SetMomentum(mom);
       return dist;
   }
   
   return 0;  
}

//_____________________________________________________________________________
Double_t SpdTrackPropagatorGF::ExtrapolateToPoint(TVector3 point, 
                                                  SpdTrackState& state)
{
   if (!fIsInit || !fTrackRep) {
       cout << "-W- <SpdTrackPropagatorGF::ExtrapolateToPoint> Propagator not initialized properly" << endl;
       return 0;
   } 
   
   TVector3 pos = state.GetPosition();
   TVector3 mom = state.GetMomentum();
   
   Double_t dist = 0;
   Bool_t extrap = false;
   
   if (!state.IsCovDefined()) 
   {
       StateOnPlane* sop = new StateOnPlane(fTrackRep);
       fTrackRep->setPosMom(*sop,pos,mom);
       extrap = true;
       try { dist = sop->extrapolateToPoint(point); }
       catch (Exception& e) { extrap = false; }
       if (extrap) {
           sop->getPosMom(pos,mom);
           state.SetTime(fTrackRep->getTime(*sop));
           state.SetMomSigma(0);
           fState5.ResizeTo(sop->getState()); 
           fState5 = sop->getState();
           fState5Plane = sop->getPlane().get();
       }
       delete sop;
   }
   else 
   {
       MeasuredStateOnPlane* sop = new MeasuredStateOnPlane(fTrackRep);
       TMatrixDSym cov; 
       state.GetCovariance(cov);
       fTrackRep->setPosMomCov(*sop,pos,mom,cov);
       extrap = true;
       try { dist = sop->extrapolateToPoint(point); }
       catch (Exception& e) { extrap = false; }
       if (extrap) {
           sop->getPosMomCov(pos,mom,cov);
           state.SetCovariance(cov);
           state.SetTime(fTrackRep->getTime(*sop));
           state.SetMomSigma(TMath::Sqrt(sop->getMomVar()));
           fState5.ResizeTo(sop->getState()); 
           fState5 = sop->getState();
           fStateCov5x5.ResizeTo(sop->getCov());
           fStateCov5x5 = sop->getCov();
           fState5Plane = sop->getPlane().get();
       }
       delete sop;
   }

   if (extrap) {
       state.SetPosition(pos);
       state.SetMomentum(mom);
       return dist;
   }

   return 0;   
}

//_____________________________________________________________________________
Double_t SpdTrackPropagatorGF::ExtrapolateBy(Double_t step, 
                                             SpdTrackState& state)
{
   if (!fIsInit || !fTrackRep) {
       cout << "-W- <SpdTrackPropagatorGF::ExtrapolateBy> Propagator not initialized properly" << endl;
       return 0;
   } 
   
   TVector3 pos = state.GetPosition();
   TVector3 mom = state.GetMomentum();
   
   Double_t dist = 0;
   Bool_t extrap = false;
   
   if (!state.IsCovDefined()) 
   {
       StateOnPlane* sop = new StateOnPlane(fTrackRep);
       fTrackRep->setPosMom(*sop,pos,mom);
       extrap = true;
       try { dist = sop->extrapolateBy(step); }
       catch (Exception& e) { extrap = false; }
       if (extrap) {
           sop->getPosMom(pos,mom);
           state.SetTime(fTrackRep->getTime(*sop));
           state.SetMomSigma(0);
           fState5.ResizeTo(sop->getState()); 
           fState5 = sop->getState();
           fState5Plane = sop->getPlane().get();
       }
       delete sop;
   }
   else 
   {
       MeasuredStateOnPlane* sop = new MeasuredStateOnPlane(fTrackRep);
       TMatrixDSym cov; 
       state.GetCovariance(cov);
       fTrackRep->setPosMomCov(*sop,pos,mom,cov);
       extrap = true;
       try { dist = sop->extrapolateBy(step); }
       catch (Exception& e) { extrap = false; }
       if (extrap) {
           sop->getPosMomCov(pos,mom,cov);
           state.SetCovariance(cov);
           state.SetTime(fTrackRep->getTime(*sop));
           state.SetMomSigma(TMath::Sqrt(sop->getMomVar()));
           fState5.ResizeTo(sop->getState()); 
           fState5 = sop->getState();
           fStateCov5x5.ResizeTo(sop->getCov());
           fStateCov5x5 = sop->getCov();
           fState5Plane = sop->getPlane().get();
       }
       delete sop;
   }

   if (extrap) {
       state.SetPosition(pos);
       state.SetMomentum(mom);
       return dist;
   }

   return 0;   
}

//_____________________________________________________________________________
Double_t SpdTrackPropagatorGF::ExtrapolateToCylinder(Double_t radius /*cm*/, SpdTrackState& state,
                                                     TVector3 line_point, TVector3 line_dir)
{
   if (!fIsInit || !fTrackRep) {
       cout << "-W- <SpdTrackPropagatorGF::ExtrapolateToCylinder> Propagator not initialized properly" << endl;
       return 0;
   } 
   
   TVector3 pos = state.GetPosition();
   TVector3 mom = state.GetMomentum();
   
   Double_t dist = 0;
   Bool_t extrap = false;
   
   line_dir.SetMag(1);
   
   if (!state.IsCovDefined()) 
   {
       StateOnPlane* sop = new StateOnPlane(fTrackRep);
       fTrackRep->setPosMom(*sop,pos,mom);
       extrap = true;
       try { dist = sop->extrapolateToCylinder(radius,line_point,line_dir); }
       catch (Exception& e) { extrap = false; }
       if (extrap) {
           sop->getPosMom(pos,mom);
           state.SetTime(fTrackRep->getTime(*sop));
           state.SetMomSigma(0);
           fState5.ResizeTo(sop->getState()); 
           fState5 = sop->getState();
           fState5Plane = sop->getPlane().get();
       }
       delete sop;
   }
   else 
   {
       MeasuredStateOnPlane* sop = new MeasuredStateOnPlane(fTrackRep);
       TMatrixDSym cov; 
       state.GetCovariance(cov);
       fTrackRep->setPosMomCov(*sop,pos,mom,cov);
       extrap = true;
       try { dist = sop->extrapolateToCylinder(radius,line_point,line_dir); }
       catch (Exception& e) { extrap = false; }
       if (extrap) {
           sop->getPosMomCov(pos,mom,cov);
           state.SetCovariance(cov);
           state.SetTime(fTrackRep->getTime(*sop));
           state.SetMomSigma(TMath::Sqrt(sop->getMomVar()));
           fState5.ResizeTo(sop->getState()); 
           fState5 = sop->getState();
           fStateCov5x5.ResizeTo(sop->getCov());
           fStateCov5x5 = sop->getCov();
           fState5Plane = sop->getPlane().get();
       }
       delete sop;
   }

   if (extrap) {
       state.SetPosition(pos);
       state.SetMomentum(mom);
       return dist;
   }
        
   return 0;    
}

//_____________________________________________________________________________
Double_t SpdTrackPropagatorGF::ExtrapolateToSphere(Double_t radius /*cm*/, SpdTrackState& state, TVector3 point)
{
   if (!fIsInit || !fTrackRep) {
       cout << "-W- <SpdTrackPropagatorGF::ExtrapolateToSphere> Propagator not initialized properly" << endl;
       return 0;
   } 
   
   TVector3 pos = state.GetPosition();
   TVector3 mom = state.GetMomentum();
   
   Double_t dist = 0;
   Bool_t extrap = false;
   
   if (!state.IsCovDefined()) 
   {
       StateOnPlane* sop = new StateOnPlane(fTrackRep);
       fTrackRep->setPosMom(*sop,pos,mom);
       extrap = true;
       try { dist = sop->extrapolateToSphere(radius,point); }
       catch (Exception& e) { extrap = false; }
       if (extrap) {
           sop->getPosMom(pos,mom);
           state.SetTime(fTrackRep->getTime(*sop));
           state.SetMomSigma(0);
           fState5.ResizeTo(sop->getState()); 
           fState5 = sop->getState();
           fState5Plane = sop->getPlane().get();
       }
       delete sop;
   }
   else 
   {
       MeasuredStateOnPlane* sop = new MeasuredStateOnPlane(fTrackRep);
       TMatrixDSym cov; 
       state.GetCovariance(cov);
       fTrackRep->setPosMomCov(*sop,pos,mom,cov);
       extrap = true;
       try { dist = sop->extrapolateToSphere(radius,point); }
       catch (Exception& e) { extrap = false; }
       if (extrap) {
           sop->getPosMomCov(pos,mom,cov);
           state.SetCovariance(cov);
           state.SetTime(fTrackRep->getTime(*sop));
           state.SetMomSigma(TMath::Sqrt(sop->getMomVar()));
           fState5.ResizeTo(sop->getState()); 
           fState5 = sop->getState();
           fStateCov5x5.ResizeTo(sop->getCov());
           fStateCov5x5 = sop->getCov();
           fState5Plane = sop->getPlane().get();
       }
       delete sop;
   }

   if (extrap) {
       state.SetPosition(pos);
       state.SetMomentum(mom);
       return dist;
   }

   return 0;   
}
    
//_____________________________________________________________________________
Double_t SpdTrackPropagatorGF::ExtrapolateToPlane(TVector3 plane_point, TVector3 plane_norm, 
                               const SpdTrackState& start_state, SpdTrackState& final_state)
{
    final_state = start_state;
    return ExtrapolateToPlane(plane_point,plane_norm,final_state);
}

//_____________________________________________________________________________    
Double_t SpdTrackPropagatorGF::ExtrapolateToLine(TVector3 line_point, TVector3 line_dir, 
                               const SpdTrackState& start_state, SpdTrackState& final_state)
{
    final_state = start_state;
    return ExtrapolateToLine(line_point,line_dir,final_state);
}

//_____________________________________________________________________________    
Double_t SpdTrackPropagatorGF::ExtrapolateToPoint(TVector3 point, 
                               const SpdTrackState& start_state, SpdTrackState& final_state)
{
    final_state = start_state;
    return ExtrapolateToPoint(point,final_state);
}

//_____________________________________________________________________________    
Double_t SpdTrackPropagatorGF::ExtrapolateToCylinder(Double_t radius /*cm*/, 
                               const SpdTrackState& start_state, SpdTrackState& final_state,
                               TVector3 line_point, TVector3 line_dir)
{
    final_state = start_state;
    return ExtrapolateToCylinder(radius,final_state,line_point,line_dir);
}

//_____________________________________________________________________________       
Double_t SpdTrackPropagatorGF::ExtrapolateToSphere(Double_t radius /*cm*/, 
                               const SpdTrackState& start_state, SpdTrackState& final_state,
                               TVector3 point)
{
    final_state = start_state;
    return ExtrapolateToSphere(radius,final_state,point);
}

//_____________________________________________________________________________    
Double_t SpdTrackPropagatorGF::ExtrapolateBy(Double_t step, 
                               const SpdTrackState& start_state, SpdTrackState& final_state)
{
    final_state = start_state;
    return ExtrapolateBy(step,final_state);
}

