// $Id$
// Author: artur   2021/01/20

//_____________________________________________________________________________
//
// SpdParticlesAndProcesses
//_____________________________________________________________________________

#include "SpdParticlesAndProcesses.h"

#include <TDatabasePDG.h>
#include <iostream>

using std::cout;
using std::endl;

Bool_t SpdParticlesAndProcesses::IsParticlesAddedToDB = false;

Bool_t SpdParticlesAndProcesses::IsCharmoniumInit    = false;
Bool_t SpdParticlesAndProcesses::IsCharmedMesonInit  = false;
Bool_t SpdParticlesAndProcesses::IsCharmedBaryonInit = false;

std::set<Int_t> SpdParticlesAndProcesses::Charmonium_;
std::set<Int_t> SpdParticlesAndProcesses::CharmedMeson_;
std::set<Int_t> SpdParticlesAndProcesses::CharmedBaryons_;

ClassImp(SpdParticlesAndProcesses)

//_____________________________________________________________________________________
void SpdParticlesAndProcesses::AddParticlesToDatabasePDG()
{
   if (IsParticlesAddedToDB) return;

   TDatabasePDG *db = TDatabasePDG::Instance();

   //cout << "-I- <SpdParticlesAndProcesses::AddParticlesToDatabasePDG> " << endl;
   
   //---------------------------------------------------------------------------
   // name, title, mass, stable, decayWidth, charge, ParticleClass, PdgCode, ...

   if (!db->GetParticle(9941003)) db->AddParticle("J/psi[1S0(8)]"    ,  "J/psi[1S0(8)]"     ,  3.29692,  true, 0, 0, "Meson", 9941003);
   if (!db->GetParticle(9942003)) db->AddParticle("J/psi[3S0(8)]"    ,  "J/psi[3S0(8)]"     ,  3.29692,  true, 0, 0, "Meson", 9942003);
   if (!db->GetParticle(9940003)) db->AddParticle("J/psi[3S1(8)]"    ,  "J/psi[3S1(8)]"     ,  3.29692,  true, 0, 0, "Meson", 9940003);
   if (!db->GetParticle(9940005)) db->AddParticle("chi_2c[3S1(8)]"   ,  "chi_2c[3S1(8)]"    ,  3.75620,  true, 0, 0, "Meson", 9940005);
   if (!db->GetParticle(9940011)) db->AddParticle("chi_0c[3S1(8)]"   ,  "chi_0c[3S1(8)]"    ,  3.61475,  true, 0, 0, "Meson", 9940011);
   if (!db->GetParticle(9940023)) db->AddParticle("chi_1c[3S1(8)]"   ,  "chi_1c[3S1(8)]"    ,  3.71066,  true, 0, 0, "Meson", 9940023);
   if (!db->GetParticle(9940103)) db->AddParticle("psi(2S)[3S1(8)]"  ,  "psi(2S)[3S1(8)]"   ,  3.88611,  true, 0, 0, "Meson", 9940103);
   if (!db->GetParticle(9941103)) db->AddParticle("psi(2S)[1S0(8)]"  ,  "psi(2S)[1S0(8)]"   ,  3.88611,  true, 0, 0, "Meson", 9941103);
   if (!db->GetParticle(9942033)) db->AddParticle("psi(3770)[3PJ(8)]",  "psi(3770)[3PJ(8)]" ,  3.97315,  true, 0, 0, "Meson", 9942033);
   if (!db->GetParticle(9942103)) db->AddParticle("psi(2S)[3PJ(8)]"  ,  "psi(2S)[3PJ(8)]"   ,  3.88611,  true, 0, 0, "Meson", 9942103);
   if (!db->GetParticle(9950005)) db->AddParticle("chi_2b[3S1(8)]"   ,  "chi_2b[3S1(8)]"    , 10.11220,  true, 0, 0, "Meson", 9950005);
   if (!db->GetParticle(9950011)) db->AddParticle("chi_0b[3S1(8)]"   ,  "chi_0b[3S1(8)]"    , 10.05940,  true, 0, 0, "Meson", 9950011);
   if (!db->GetParticle(9950023)) db->AddParticle("chi_1b[3S1(8)]"   ,  "chi_1b[3S1(8)]"    , 10.09280,  true, 0, 0, "Meson", 9950023);
   
   if (!db->GetParticle(990)) db->AddParticle("Pomeron", "Pomeron", 0, true, 0, 0, "Reggeon", 990);
   
   if (!db->GetParticle(90)) db->AddParticle("string", "string", 0, true, 0, 0, "QCD string", 90);
   
   // QCD  diffractive state
   if (!db->GetParticle(9900110)) db->AddParticle("rho_diff0", "rho_diff0", 0, true, 0, 0, "QCD diffr.", 9900110);
   if (!db->GetParticle(9900210)) db->AddParticle("pi_diffr+", "pi_diffr+", 0, true, 0, 1, "QCD diffr.", 9900210);
   if (!db->GetParticle(9900220)) db->AddParticle("omega_di",  "omega_di",  0, true, 0, 0, "QCD diffr.", 9900220);
   if (!db->GetParticle(9900330)) db->AddParticle("phi_diff",  "phi_diff",  0, true, 0, 0, "QCD diffr.", 9900330);
   if (!db->GetParticle(9900440)) db->AddParticle("J/psi_di",  "J/psi_di",  0, true, 0, 0, "QCD diffr.", 9900440);
   if (!db->GetParticle(9902110)) db->AddParticle("n_diffr0",  "n_diffr0",  0, true, 0, 0, "QCD diffr.", 9902110);
   if (!db->GetParticle(9902210)) db->AddParticle("p_diffr+",  "p_diffr+",  0, true, 0, 1, "QCD diffr.", 9902210);
   

    if (!db->GetParticle(1000010020)) db->AddParticle("d+", "d+", 1.876124, kTRUE, 0., 3, "Ion", 1000010020);
    if (!db->GetParticle(-1000010020)) db->AddParticle("d-", "d-", 1.876124, kTRUE, 0., 3, "Ion", -1000010020);


   IsParticlesAddedToDB = true; 
}

//_____________________________________________________________________________________
Int_t SpdParticlesAndProcesses::VerifyPhysProbe(Int_t pdg)
{
    if (VerifyCharmonium(pdg)) return kSpdCharmonium;
    if (VerifyCharmedMeson(pdg)) return kSpdCharmedMeson;
    if (VerifyCharmedBaryon(pdg)) return kSpdCharmedBaryon;
    
    return kSpdUnknown;
}

//_____________________________________________________________________________________
Bool_t SpdParticlesAndProcesses::VerifyCharmonium(Int_t pdg)
{
    if (!IsCharmoniumInit) 
    {
        Charmonium_.insert(441);       // ηc(1S)  
        Charmonium_.insert(10441);     // χc0(1P) 
        Charmonium_.insert(100441);    // ηc(2S)  
        Charmonium_.insert(443);       // J/ψ(1S) 
        Charmonium_.insert(10443);     // hc(1P)  
        Charmonium_.insert(20443);     // χc1(1P) 
        Charmonium_.insert(100443);    // ψ(2S)   
        Charmonium_.insert(30443);     // ψ(3770) 
        Charmonium_.insert(9000443);   // ψ(4040) 
        Charmonium_.insert(9010443);   // ψ(4160) 
        Charmonium_.insert(9020443);   // ψ(4415) 
        Charmonium_.insert(445);       // χc2(1P) 
        Charmonium_.insert(100445);    // χc2(2P) 
        
        IsCharmoniumInit = true;
    }
    
    return (Charmonium_.find(pdg) != Charmonium_.end());
}

//_____________________________________________________________________________________    
Bool_t SpdParticlesAndProcesses::VerifyCharmedMeson(Int_t pdg)
{
    if (!IsCharmedMesonInit) 
    {
        CharmedMeson_.insert(411);      // D+         
        CharmedMeson_.insert(421);      // D0         
        CharmedMeson_.insert(10411);    // D∗0(2400)+ 
        CharmedMeson_.insert(10421);    // D∗0(2400)0 
        CharmedMeson_.insert(413);      // D∗(2010)+  
        CharmedMeson_.insert(423);      // D∗(2007)0  
        CharmedMeson_.insert(10413);    // D1(2420)+  
        CharmedMeson_.insert(10423);    // D1(2420)0  
        CharmedMeson_.insert(20413);    // D1(H)+     
        CharmedMeson_.insert(20423);    // D1(2430)0  
        CharmedMeson_.insert(415);      // D∗2(2460)+ 
        CharmedMeson_.insert(425);      // D∗2(2460)0 
        CharmedMeson_.insert(431 );     // D+s        
        CharmedMeson_.insert(10431);    // D∗s0(2317)+
        CharmedMeson_.insert(433);      // D∗+s       
        CharmedMeson_.insert(10433);    // Ds1(2536)+ 
        CharmedMeson_.insert(20433);    // Ds1(2460)+ 
        CharmedMeson_.insert(435);      // D∗s2(2573)+
        
        IsCharmedMesonInit = true;
    }
    
    return (CharmedMeson_.find(abs(pdg)) != CharmedMeson_.end());
}
//_____________________________________________________________________________________    
Bool_t SpdParticlesAndProcesses::VerifyCharmedBaryon(Int_t pdg)
{
    if (!IsCharmedBaryonInit) 
    {
        CharmedBaryons_.insert(4122);    // Λ+c   
        CharmedBaryons_.insert(4222);    // Σ++c  
        CharmedBaryons_.insert(4212);    // Σ+c   
        CharmedBaryons_.insert(4112);    // Σ0c   
        CharmedBaryons_.insert(4224);    // Σ∗++c 
        CharmedBaryons_.insert(4214);    // Σ∗+c  
        CharmedBaryons_.insert(4114);    // Σ∗0c  
        CharmedBaryons_.insert(4232);    // Ξ+c   
        CharmedBaryons_.insert(4132);    // Ξ0c   
        CharmedBaryons_.insert(4322);    // Ξ′+c  
        CharmedBaryons_.insert(4312);    // Ξ′0c  
        CharmedBaryons_.insert(4324);    // Ξ∗+c  
        CharmedBaryons_.insert(4314);    // Ξ∗0c  
        CharmedBaryons_.insert(4332);    // Ω0c   
        CharmedBaryons_.insert(4334);    // Ω∗0c  
        CharmedBaryons_.insert(4412);    // Ξ+cc  
        CharmedBaryons_.insert(4422);    // Ξ++cc 
        CharmedBaryons_.insert(4414);    // Ξ∗+cc 
        CharmedBaryons_.insert(4424);    // Ξ∗++cc
        CharmedBaryons_.insert(4432);    // Ω+cc  
        CharmedBaryons_.insert(4434);    // Ω∗+cc 
        CharmedBaryons_.insert(4444);    // Ω++ccc
        
        IsCharmedBaryonInit = true;
    }
    
    return (CharmedBaryons_.find(abs(pdg)) != CharmedBaryons_.end());
}

//_____________________________________________________________________________________
Bool_t SpdParticlesAndProcesses::IsQuarkOrGluon(Int_t pdg)
{
    if (abs(pdg) > 0 && abs(pdg) < 9) return true; // quark
    if (pdg == 21) return true; // gluon
    return false;
}

