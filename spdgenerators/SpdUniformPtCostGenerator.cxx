
#include "SpdUniformPtCostGenerator.h"

#include "FairPrimaryGenerator.h"
//#include "SpdPrimaryGenerator.h"
#include "SpdPrimGenParSet.h"

#include <TMath.h>
#include <TMCParticle.h> 
#include <TDatabasePDG.h>
#include <TParticle.h>
#include <TVector3.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;


ClassImp(SpdUniformPtCostGenerator)

//_____________________________________________________________________________________
SpdUniformPtCostGenerator::SpdUniformPtCostGenerator():SpdGenerator("SPD uniform Generator")
{
    Reset();
}

//_____________________________________________________________________________________
SpdUniformPtCostGenerator::SpdUniformPtCostGenerator(const Char_t* Name):SpdGenerator(Name)
{
    Reset();
}


//_____________________________________________________________________________
void SpdUniformPtCostGenerator::Reset()
{
    fPdg = 13;            // particle pdg-number
    fNPL = 1;             // particles per event
        
    fCosTmin = -1;
    fCosTmax = 1; 
    fpTmin = 0.1;
    fpTmax = 5.;
   
    SetSeed();
       
    fInit = kFALSE;      // Init mark // Why would one need that???

    debug_output = false;
   
    ResetEvent();
}

//_____________________________________________________________________________
void SpdUniformPtCostGenerator::ResetEvent()
{
    fNParticles = 0;
}

//_____________________________________________________________________________
void SpdUniformPtCostGenerator::SetSeed(UInt_t seed)
{
    fGen->SetSeed(seed);  // Set seed to inner rnd. gen.
    fSeed = fGen->GetSeed();
}

//_____________________________________________________________________________
void SpdUniformPtCostGenerator::SetSeeds(const std::vector<UInt_t>& seeds)
{
    if (seeds.size() < 1) return;
    SetSeed(seeds[0]);
} 

//_____________________________________________________________________________
void SpdUniformPtCostGenerator::GetSeeds(std::vector<UInt_t>& seeds) 
{ 
    seeds.clear(); 
    seeds.push_back(fSeed);
} 

//_____________________________________________________________________________
void SpdUniformPtCostGenerator::SetGenPars(Int_t pdg,
        Double_t costmin, Double_t costmax, Double_t ptmin, Double_t ptmax,
        Int_t np)
{
    fPdg = pdg;
    fNPL = np;

    if (costmin < -1) costmin = -1;
    if (costmax >  1) costmax = 1;
    
    if (!(costmax > costmin)) {
        costmin = -1;
        costmax = 1;
        cout << "-W- <SpdIsotropicGenerator::SetSpherical> Bad settings. Set default: "
             << "Theta [min,max] = " << costmin << " " <<costmax << endl; 
    }
    
    fCosTmin = costmin;
    fCosTmax = costmax; 

    if (ptmin < 0)   ptmin = 0;
    
    if (!(ptmax > ptmin)) {
        ptmin = 0;
        ptmax = 10;
        cout << "-W- <SpdIsotropicGenerator::SetSpherical> Bad settings. Set default: "
             << "Phi [min,max] = " << ptmin << " " <<ptmax << endl; 
    }
    fpTmin = ptmin; 
    fpTmax = ptmax;     
}

//_____________________________________________________________________________
Bool_t SpdUniformPtCostGenerator::Init()
{
   if (fInit) return kTRUE;
     
   fInit = kFALSE;
   
   TParticlePDG *part = TDatabasePDG::Instance()->GetParticle(fPdg);
   if (!part) {
        cout << "-E- <SpdIsotropicGenerator::Init> Unknown particle: " << fPdg << endl;
        return kFALSE;
   }
   
   if (fNPL < 1) fNPL = 1;
   
   fInit = kTRUE; 
   
   return kTRUE;
}

//_____________________________________________________________________________________
Bool_t SpdUniformPtCostGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
    if (!fInit) {
        cout << "-E- <SpdIsotropicGenerator::ReadEvent> "
            << "Parameters were not initialized properly " << endl;
        return kFALSE;
    }

    ResetEvent();

    for (int i(0); i<fNPL; i++) {

        double cost = fCosTmax - fGen->Rndm()*(fCosTmax - fCosTmin);   
        double sint = Sqrt(Abs(1-cost*cost)); 
        if (sint == 0.) sint = 1.e-8;
        double pt = fpTmax - fGen->Rndm()*(fpTmax - fpTmin);
        double p = pt/sint;  // check zero
        double phi = TwoPi() - fGen->Rndm()*(TwoPi() - 0);

        double nx = sint*Cos(phi);
        double ny = sint*Sin(phi);
        double nz = cost;   

        //AddParticle(primGen,p*nx,p*ny,p*nz);

        double px = p * nx;
        double py = p * ny;
        double pz = p * nz;

        Bool_t add = AddTrack(primGen, 1, fPdg , fNParticles, -1, px, py, pz);
        fNParticles++;

        // --------------- DEBUG INFO ---------------
        if (debug_output) {
            TParticlePDG *part;
            const Char_t* ptype = "Undefined";
            Double_t te = -1;

            TDatabasePDG *db = TDatabasePDG::Instance(); 
            if (db) {
                part = db->GetParticle(fPdg);
                if (part) {
                    ptype = part->ParticleClass();  
                }
            }

            Char_t c;
            (add) ? c = '+' : c= '-';

            printf("%-5d [%c] %8d %12d %10.6f %10.6f %10.6f %12.6f %12.6f %12.6f %12.6f %8d   %s\n",
                    fNParticles, c, 1/*status*/, fPdg, px, py, pz, 
                    0., 0., 0., 0., -1 , ptype); 
        }

    }  

    return kTRUE;
}


////_____________________________________________________________________________________
//void SpdUniformPtCostGenerator::AddParticle(FairPrimaryGenerator* primGen, 
//                            Double_t px, Double_t py, Double_t pz)
//{ 
//    Bool_t add = AddTrack(primGen, 1, fPdg , fNParticles, -1, px, py, pz);
//
//    //       Bool_t add = AddTrack(primGen, 1, fPdg , 1, -1, px, py, pz); 
//    TParticlePDG *part;
//    const Char_t* ptype = "Undefined";
//    Double_t te = -1;
//
//    TDatabasePDG *db = TDatabasePDG::Instance(); 
//    if (db) {
//        part = db->GetParticle(fPdg);
//        if (part) {
//            ptype = part->ParticleClass();  
//        }
//    }
//
//    Char_t c;
//    (add) ? c = '+' : c= '-';
//
//    printf("%-5d [%c] %8d %12d %10.6f %10.6f %10.6f %12.6f %12.6f %12.6f %12.6f %8d   %s\n",
//            fNParticles, c, 1/*status*/, fPdg, px, py, pz, 
//            0., 0., 0., 0., -1 , ptype); 
//
//    fNParticles++;
//}


//_____________________________________________________________________________________
void SpdUniformPtCostGenerator::Print(Option_t* opt) const
{
   cout << "\t-I- <SpdIsotropicGenerator::Print>\n" << endl;
   cout << "\tIs init:          " << fInit << endl;
   cout << "\tSeed:             " << fSeed << endl;
   cout << endl;
   cout << "\tPDG:              " << fPdg << endl;
   cout << endl;
}



