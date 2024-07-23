// $Id$
// Author: artur   2015/10/27


//_____________________________________________________________________________
//
// EcalCellEventHeader
//_____________________________________________________________________________

#include "EcalCellEventHeader.h"
#include "FairRootManager.h"
#include <TMCProcess.h>
#include <TMath.h>
#include <iostream>

using std::cout;
using std::endl;

#define ELOSS_MIN  1e-9

ClassImp(EcalCellEventHeader)

//_____________________________________________________________________________
EcalCellEventHeader::EcalCellEventHeader(): 
 
   PdgNum_(0), Mass_(0),
   PrimLastProcessID_(0),PrimLastNDaughters_(0),PrimLastVolPath_(""),
   LPX_(0),LPY_(0),LPZ_(0),
   PrimLastIntLen_(0),PrimLastRadLen_(0),PrimLastDepth_(0),PrimLastDist_(0),
   TotalEloss_(0)
{
   fTitle = "Unknown";
   fNPrim = 1;
}

//_____________________________________________________________________________
EcalCellEventHeader::~EcalCellEventHeader() 
{
 
}

//_____________________________________________________________________________
void EcalCellEventHeader::Register()
{
  FairRootManager::Instance()->Register("MCEventHeader.", "Event", this, kTRUE);
}

//_____________________________________________________________________________
void EcalCellEventHeader::Clear() 
{ 
  fEventId = -1;
  
  PdgNum_ = 0;
  Mass_ = 0;
  fNPrim = 1;
  fX = fY = fZ = fT = fB = 0.;
  fIsSet = kFALSE;
  fRotX = fRotY = fRotZ = 0.;
  
  PrimLastProcessID_ = 0;
  PrimLastNDaughters_ = 0;
  PrimLastVolPath_ = "";
  LPX_ = LPY_ = LPZ_ = 0;
  
  PrimLastIntLen_ = 0;
  PrimLastRadLen_ = 0;
  
  TotalEloss_ = 0;
}

//_____________________________________________________________________________
Double_t EcalCellEventHeader::Theta()  const
{
  TVector3 v(fRotX,fRotY,fRotZ);
  return v.Theta();
 
  //return (fRotZ == 0) ? TMath::PiOver2() : TMath::ACos(fRotZ/GetP()); 
}
 
//_____________________________________________________________________________
Double_t EcalCellEventHeader::Phi()  const
{
  TVector3 v(fRotX,fRotY,fRotZ);
  return v.Phi();
  
  //return TMath::Pi() + TMath::ATan2(-fRotY,-fRotX); 
}

//_____________________________________________________________________________
Double_t EcalCellEventHeader::EcalCellEventHeader::GetP() const 
{ 
  return TMath::Sqrt(fRotX*fRotX + fRotY*fRotY + fRotZ*fRotZ); 
}

//_____________________________________________________________________________
Double_t EcalCellEventHeader::EcalCellEventHeader::GetEnergy() const
{ 
  return TMath::Sqrt(Mass_*Mass_ + fRotX*fRotX + fRotY*fRotY + fRotZ*fRotZ); 
}

//_____________________________________________________________________________
Double_t EcalCellEventHeader::GetEkin() const
{
  return GetEnergy()-Mass_;
}

//_____________________________________________________________________________
Double_t EcalCellEventHeader::GetP(Double_t ekin) const
{
  return TMath::Sqrt(ekin*ekin + 2*ekin*Mass_);
}
 
//_____________________________________________________________________________
void EcalCellEventHeader::Print(int opt) const
{
  if (opt < 0) return;
  
  if (opt < 1) {
      cout << "<EcalCellEventHeader::Print>" 
           << "  RUN: " << fRunId 
           << "  EVENT_ID: " << fEventId  
           << "\n";
      return;
  }

  cout << "\n";
  cout << "<EcalCellEventHeader::Print>" << " RUN: " << fRunId << "\n";
  
  cout << "Event ID:            " << fEventId << endl;
  cout << "Total eloss:         " << TotalEloss_ << endl;
  
  cout << "\n";
  cout << "Particle:            " << PdgNum_ << " (" << fTitle << ")" << endl;
  cout << "Start vertex:        " << fX << ", " << fY << ", " << fZ << endl;
  cout << "Start Momentum:      " << fRotX << ", " << fRotY << ", " << fRotZ << endl;
  cout << "Theta, Phi (deg):    " << Theta()*TMath::RadToDeg() << ", " << Phi()*TMath::RadToDeg() << endl;
  cout << "Mass:                " << Mass_ << endl;  
  cout << "Momentum:            " << GetP() << " GeV/c" << endl; 
  cout << "Energy(kin/total):   " << GetEkin() << "/" << GetEnergy() << " GeV " << endl;
  
  cout << "\n";
  cout << "Prim. l-process:     " << PrimLastProcessID_ 
                                  << " (" << TMCProcessName[PrimLastProcessID_] << ") " << endl;
  cout << "Prim. l-volume:      " << PrimLastVolPath_ << endl;
  cout << "Prim. l-ndaughters:  " << PrimLastNDaughters_ << endl;
  cout << "Prim. l-vertex:      " << LPX_ << " " << LPY_ << " " << LPZ_ << " [cm] " << endl;
  cout << "Prim. l-distance:    " << PrimLastDist_ << " [cm] " << endl;
  cout << "Prim. l-depth:       " << PrimLastDepth_ << " [g/cm2] " << endl;
  cout << "Prim. l-intlen:      " << PrimLastIntLen_ << endl;
  cout << "Prim. l-radlen:      " << PrimLastRadLen_ << endl;

  cout << "\n";
}

