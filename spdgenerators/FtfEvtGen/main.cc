//               FTF test: Pbar+P interaction channels 
//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
//      edition  29.08.2014  A.Galoyan 
// -------------------------------------------------------------------
//      GEANT 4 class file --- Copyright CERN 1998
//      CERN Geneva Switzerland
//
//
//      File name:     Test30
//
//      Author:        V.Ivanchenko
//
//      Creation date: 12 March 2002
//
//      Modifications:
//      14.11.03 Renamed to cascade
//      09.05.06 Return back to test30
// -------------------------------------------------------------------
#include "globals.hh"
#include "G4Version.hh"
#include "G4ios.hh"
#include <fstream>
#include <iomanip>
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "FTFtest1.icc"
#include "G4ChipsComponentXS.hh"                  // Uzhi 29.01.13
#include "UZHI_diffraction.hh"
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TStopwatch.h"
#include "TParticle.h"


int main(int argc, char** argv)
{
	  CLHEP::RanluxEngine defaultEngine( 1234567, 4 );
		long seed = 0;
	if (argc > 2){
		seed = atol(argv[2]);
	} else {
		seed = 1234567;
	}
	defaultEngine.setSeed(seed, 4);
  CLHEP::HepRandom::setTheEngine( &defaultEngine );
  G4cout << "========================================================" << G4endl;
  G4cout << "======              FTF Test Start              ========" << G4endl;
  G4cout << "========================================================" << G4endl;
  // -------------------------------------------------------------------
  // Control on input
  
  if(argc < 2) {
    G4cout << "Input file is not specified! Exit" << G4endl;
    exit(1);
  }
  
  G4cout << "Initial generator seed: " << seed << G4endl; 

  // ATTENTION  SPD adjustment (number of events) ATTENTION
  int in_nevents = -1;
  if (argc > 3) in_nevents = atoi(argv[3]);
  G4cout << "Initial number of events: " << in_nevents << G4endl;
  
  // ATTENTION  SPD adjustment (particle momentum in Lab. SC, GeV /OR/ sqrt(s) per 2 hadrons, GeV) ATTENTION
  double epar = -1.;
  if (argc > 4)  epar = atof(argv[4]);
  G4cout << "Energy parameter: " << epar << G4endl;
  
  // ATTENTION  SPD adjustment (number of events to skip) ATTENTION
  int nskipev = 0;
  if (argc > 5) nskipev = atof(argv[5]);
  G4cout << "Initial events to skip: " << nskipev << G4endl;
  
  // ATTENTION  SPD adjustment (output file name) ATTENTION
  TString outfile("");
  if (argc > 6) outfile = argv[6];
  G4cout << "Initial output file: " << outfile << G4endl;
  G4cout << G4endl;

  std::ifstream* fin = new std::ifstream();
  G4String fname = argv[1];
  fin->open(fname.c_str());
  if( !fin->is_open()) {
    G4cout << "Input file <" << fname << "> does not exist! Exit" << G4endl;
    exit(1);
  }

//-----------------------------------------------------------------------
  #include "FTFtest2.icc"   // Initialization
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
//G4double sigTot = 0;  //[R.K. 01/2017] unused variables
//G4double sigEl  = 0; //[R.K. 01/2017] unused variables
//G4double sigIn  = 0; //[R.K. 01/2017] unused variables

//int npart; //[R.K. 01/2017] unused variables
 //   Root initialization
 
 // ATTENTION  SPD adjustment ATTENTION

 if (outfile.IsWhitespace()) {
     outfile = gSystem->Getenv("VMCWORKDIR");
     if (outfile.IsWhitespace()) outfile = "FTF.root";
     else outfile += "/input/FTF.root";
 }
 
//outfile="FTF.root";
 G4cout << "Output file <" << outfile << "> " << G4endl;
 
 TFile f1(outfile,"RECREATE","ROOT_Tree");

 Int_t activeCnt=0;
 TTree* fTree = new TTree("data","FTF Background");
 TClonesArray* fEvt;

 fEvt=new TClonesArray("TParticle",100);
 fTree->Branch("Npart",&activeCnt,"Npart/I");
 fTree->Branch("Particles",&fEvt, 32000,99);

 TLorentzVector Mom;
 TLorentzVector V(0,0,0,0);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // -------- Loop over run

  G4String line, line1;
  G4bool end = true;

  for(G4int run=0; run<100; run++) {
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//-------------------------- Current histograms -------------------------
    
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  do {
  
    #include "FTFtest3.icc"  // -------- Read input file
    
    // ATTENTION  SPD adjustment (particle momentum redefinition) ATTENTION
     
    if (sqrtSnn > 0.) 
    {
        G4cout << "\n++++++++++++++++++++++++++++++++++++++++++++++++\n";
        G4cout << "+++++++++++++++++ SQRT(S) MODE +++++++++++++++++\n";
        G4cout << "++++++++++++++++++++++++++++++++++++++++++++++++\n";
    
        if (epar > 0) {
            sqrtSnn = epar;
            G4double Enn=(sqr(sqrtSnn)-1.763)/2.0/0.939;
            Plab = std::sqrt(sqr(Enn)-sqr(0.939));
            Plab *= GeV;
        } 
    }
    else 
    {  
        G4cout << "\n++++++++++++++++++++++++++++++++++++++++++++++++\n";
        G4cout << "+++++++++++++++++ PLAB MODE ++++++++++++++++++++\n";
        G4cout << "++++++++++++++++++++++++++++++++++++++++++++++++\n";
        
        if (epar > 0) Plab = epar * GeV;
    }
      
    #include "FTFtest4.icc"  // -------- Start run processing  
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

    G4cout << "cross(mb)in= " << cross_sec*1000./barn << G4endl
           << "cross(mb)el= " << cross_secel*1000./barn<<G4endl<<G4endl;

    cross_inel=cross_sec-cross_secel; // +++++++++++++++++++++++++

    cross_sec/=millibarn;   // Inel Cross section in mb
    cross_secel/=millibarn; // Elas Cross section in mb
    cross_inel/=millibarn;  // Inel Cross section in mb

    G4cout<<"Element A Z N: "<<A<<" "<<Z<<" "<<A-Z<<G4endl;
    G4cout<<"Proposed Xs (mb): Tot El In: "
          <<cross_sec<<" "<<cross_secel<<" "<<cross_inel<<G4endl;

//---------------------------------------------------------------------------
// Kossov cross sections      ---------------------------
    G4double chipsTot, chipsEl, chipsIn;

    static G4ChipsComponentXS* _instance = new G4ChipsComponentXS();
    G4ChipsComponentXS* CHIPSxsManager = _instance;

    G4bool CHIPapplic=true;                          //false;   Uzhi 29.01.13
    if(ionParticle) CHIPapplic = false;              // Uzhi 2019
    if(CHIPapplic)
    {
     chipsTot=CHIPSxsManager->GetTotalElementCrossSection(part,energy,Z,A-Z);
     chipsEl =CHIPSxsManager->GetElasticElementCrossSection(part,energy,Z,A-Z);
     chipsIn =CHIPSxsManager->GetInelasticElementCrossSection(part,energy,Z,A-Z);
     chipsTot/=millibarn; chipsEl/=millibarn; chipsIn/=millibarn;

     G4cout<<"CHIPS cross sections are used:----------------------"<<G4endl<<
             "Plab          Total        Elastic      Inelastic"   <<G4endl;
     G4cout<<" "<<Plab/GeV<<" "<< chipsTot<<" "<<chipsEl<<" "<<chipsIn <<G4endl<<G4endl;

     //sigTot=chipsTot; sigEl=chipsEl; sigIn=chipsIn; //[R.K. 01/2017] unused variables
    } else
    {
     //sigTot = cross_sec;  //[R.K. 01/2017] unused variables
     //sigEl  = cross_secel; //[R.K. 01/2017] unused variables
     //sigIn  = cross_inel; //[R.K. 01/2017] unused variables

     G4cout<<"Proposed Xs (mb) are used: Tot El In: "
           <<cross_sec<<" "<<cross_secel<<" "<<cross_inel<<G4endl;
    }

//+++++++++++++++++++++++++++++++++ For each energy +++++++++++++++++++++
    // ATTENTION  SPD adjustment (number of events redefinition) ATTENTION
    if (in_nevents > 0) nevt = in_nevents + nskipev; 
    
    G4int Ntotal=nevt;  
    G4int Nfault=0;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++       
//-------------------------------------------------------

    const G4DynamicParticle* sec = 0;
    G4ParticleDefinition* pd;
    G4ThreeVector  mom;
    G4LorentzVector labv, fm;
    G4double e, px, py, pz, theta; //pt,  //[R.K. 01/2017] unused variables
    G4VParticleChange* aChange = 0;

//  G4double E=energy+part->GetPDGMass();                                  // Elab Proj
//  G4double SqrtS=std::sqrt(sqr(part->GetPDGMass())+sqr(938.)+2.*938.*E); // per  Proj+N
//  G4double Ycms=0.5*std::log((E+Plab)/(E-Plab));                         //      Proj+N

    // -------- Event loop
    G4cout<<"Events start "<<nevt<< " (skip events: " << nskipev << ") "<<G4endl;
//  G4int Ninelast=nevt;                              // Uzhi
//=================================================================
    for (G4int iter=0; iter<nevt; ++iter) {
//=================================================================
      if(verbose > 0) G4cout<<"Start events loop***********************"<<G4endl;

      if(verbose>=1 || iter == modu*(iter/modu)) { 
        G4cout << "### " << iter << "-th event start " <<Plab/GeV<<G4endl;
      }

      if(saverand) {defaultEngine.saveStatus("initial.conf");}

      G4double e0 = energy;
      do {
        if(sigmae > 0.0) e0 = G4RandGauss::shoot(energy,sigmae);
      } while (e0 < 0.0);

      dParticle.SetKineticEnergy(e0);

      gTrack->SetStep(step);
      gTrack->SetKineticEnergy(e0);
      G4double amass = phys->GetNucleusMass();
      // note: check of 4-momentum balance for CHIPS is not guranteed due to
      // unknown isotope      
      aChange = proc->PostStepDoIt(*gTrack,*step); 
      
      if (iter < nskipev) continue;

      G4double mass = part->GetPDGMass();

      if ( ionParticle ) 
      {
       e0/=ionA; G4double mass_N=939.*MeV;                              // Init 4-mom
       labv = G4LorentzVector(0.0, 0.0, std::sqrt(e0*(e0 + 2.*mass_N)), //   NN
	 		      e0 + mass_N +  mass_N); 
      } else
      {                                             
       labv = G4LorentzVector(0.0, 0.0, std::sqrt(e0*(e0 + 2.*mass)),   //   hA
  		              e0 + mass + amass); 
      }

      G4ThreeVector bst = labv.boostVector();          // To CMS NN in AA or hA
//------------
      G4LorentzVector labNN(0.0, 0.0, std::sqrt(e0*(e0 + 2.*mass)),e0 + mass + amass);
      G4ThreeVector boostNN = labNN.boostVector();
//------------

      // take into account local energy deposit
      G4double de = aChange->GetLocalEnergyDeposit();
      G4LorentzVector dee = G4LorentzVector(0.0, 0.0, 0.0, de); 
      labv -= dee;

      G4int n = aChange->GetNumberOfSecondaries();     // Multiplicity of prod. part.
      //npart = n; //[R.K. 01/2017] unused variables
   fEvt->Clear();
   Int_t cnt = 0;
 
      if(verbose>=1) G4cout<<" Uzhi ------------ N prod. part "<<n<<G4endl;
//++++++++++++++++ Variables for each event +++++++++++++++++++++++++++++
      if((verbose > 0) && (n < 2)) {G4cout<<"Multiplicity of produced < 2!!!"<<G4endl;}
      if(n < 2) {Nfault++; Ntotal--;}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
G4double SumPx,SumPy,SumPz,SumE;
SumPx=0.; SumPy=0.; SumPz=0.; SumE=0.;
      for(G4int i=0; i<n; ++i)              // Loop over produced particles
      {
        sec = aChange->GetSecondary(i)->GetDynamicParticle();
        pd  = sec->GetDefinition();
        G4String pname=pd->GetParticleName();

        if(verbose>=1) G4cout<<" Part  "<<i<<" "<<pname
                             <<" "<<sec->Get4Momentum()/GeV
                             <<sec->Get4Momentum().mag()/GeV<<G4endl;

        fm  = sec->Get4Momentum();
        labv -= fm;   // For checking energy-momentum conservation

        if(sqrtSnn) fm.boost(-bst);         // Uzhi 2019

        mom = fm.vect();                    // Uzhi 2019   sec->GetMomentum();
//      G4double mas = pd->GetPDGMass();
//	G4double p = mom.mag();

//Uzhi        labv -= fm;   // For checking energy-momentum conservation

	// electron can come only from internal conversion
	// its mass should be added to initial state
        if(pd == electron) { 

	  labv += G4LorentzVector(0.0,0.0,0.0,electron_mass_c2); 
	}

        px = mom.x()/GeV;
        py = mom.y()/GeV;
        pz = mom.z()/GeV;    
        //pt = std::sqrt(px*px +py*py)/GeV;  //[R.K. 01/2017] unused variables
        e  = fm.e()/GeV; // - m;
SumPx +=px; SumPy +=py; SumPz +=pz; SumE +=e;

        theta = mom.theta();
if(std::abs(pd->GetBaryonNumber()) < 2)
{ 
     int id = pd->GetPDGEncoding();
// choice of inelastic    if( (n>2) || ((abs(id)!=2212)&&(n==2)) ) {
     Mom.SetPxPyPzE(px,py,pz,e);
      
     // ATTENTION  SPD adjustment: set fisrt and second mother/daughter id = -1  ATTENTION
     //TParticle  fparticle(id,1,0,0,0,0,Mom,V);
     TParticle  fparticle(id,1,-1,-1,-1,-1,Mom,V);
     
     new((*fEvt)[cnt++]) TParticle(fparticle); 
// }
}
        theta=theta*180./pi;
// Uzhi 2019        fm.boost(-bst);

//        G4double costcm = std::cos(fm.theta());
	de += e;

	//	delete sec;       	 
        delete aChange->GetSecondary(i);

      } //     end of the loop on particles
   activeCnt = cnt;

   fTree->Fill();

      if(verbose > 0) {
        if(sqrtSnn == 0.) G4cout << "Energy/Momentum balance= " << labv/GeV << G4endl;
        if(sqrtSnn) G4cout << "energy/Momentum balance= " <<SumPx<<" "<<SumPy<<" "<<SumPz<<" "<<SumE<<G4endl;
                      }
      aChange->Clear();

      if(verbose > 0) 
      { 
       G4cout << "End event =====================================" <<Plab<< G4endl; // Uzhi
       G4int Uzhi_i;                                                        // Uzhi
       G4cin >> Uzhi_i;                                                     // Uzhi
      }
//

    }   // End of the event loop ------------------------------------
    timer->Stop();
    G4cout << "  "  << *timer << G4endl;
    delete timer;

    //if(verbose > 0) {
      G4cout << "###### End of run # " << run << "     ######" << G4endl;
    //}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ Uzhi 

  } while(end);
  }  // End of job ----------------------------------------------
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//  delete pFrame;
//  delete lFrame;
//  delete sFrame;
  delete mate;
  delete fin;
  delete phys;
  partTable->DeleteAllParticles();
  f1.Write();

  G4cout << "###### End of test #####" << G4endl;
}
