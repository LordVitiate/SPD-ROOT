// -------------------------------------------------------------------------
// -----                SpdUrqmdGenerator source file                  -----
// -----                Created 24/06/04  by V. Friese                 -----
// -------------------------------------------------------------------------
#include "SpdUrqmdGenerator.h"

#include "FairPrimaryGenerator.h"
#include "FairMCEventHeader.h"
//#include "constants.h"

#include "TMCProcess.h"
#include "TObjArray.h"
#include "TPDGCode.h"
#include "TParticle.h"
#include "TRandom.h"
#include "TString.h"
#include "TVirtualMCStack.h"
#include "TLorentzVector.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include <iostream>
#include <cstring>

#include <stdio.h>

using namespace std;
using namespace TMath;

//const Double_t kProtonMass = 0.938271998;


// -----   Default constructor   ------------------------------------------

SpdUrqmdGenerator::SpdUrqmdGenerator()
: SpdGenerator("SPD URQMD Generator"),
fInputFile(NULL),
fParticleTable(),
fFileName(NULL),
fPhiMin(0.),
fPhiMax(0.),
fEventPlaneSet(kFALSE) {
}
// ------------------------------------------------------------------------



// -----   Standard constructor   -----------------------------------------

SpdUrqmdGenerator::SpdUrqmdGenerator(const char* fileName)
: SpdGenerator("SPD URQMD Generator"),
  fInputFile(NULL),
  fParticleTable(),
  fFileName(fileName),
  fPhiMin(0.),
  fPhiMax(0.),
  fEventPlaneSet(kFALSE),
  fParticles(0),
  fVerbose(0),
  fKeepEvent(kFALSE) {
    //  fFileName = fileName;
    cout << "-I SpdUrqmdGenerator: Opening input file " << fFileName << endl;
    fInputFile = gzopen(fFileName, "rb");
    if (!fInputFile) {
        Fatal("SpdUrqmdGenerator", "Cannot open input file.");
        exit(1);
    }
    cout << "done!" << endl;
    ReadConversionTable();
}
// ------------------------------------------------------------------------



// -----   Destructor   ---------------------------------------------------

SpdUrqmdGenerator::~SpdUrqmdGenerator() {
    //  cout<<"Enter Destructor of SpdUrqmdGenerator"<<endl;
    if (fInputFile) {
#ifdef GZIP_SUPPORT
        gzclose(fInputFile);
#else
        fclose(fInputFile);
#endif
        fInputFile = NULL;
    }
    fParticleTable.clear();
    //  cout<<"Leave Destructor of SpdUrqmdGenerator"<<endl;
    if (fParticles) {
        fParticles->Delete();
        delete fParticles;
    }
}
// ------------------------------------------------------------------------

void SpdUrqmdGenerator::ResetEvent()
{
   if (fParticles) 
      fParticles->Clear();
}

void SpdUrqmdGenerator::SetKeepEvent(Bool_t v)
{
    if (v) {
        if (!fParticles) fParticles = new TClonesArray("TParticle");
        else fParticles->Delete();
    }
    else {
        if (fParticles) {
            fParticles->Delete();
            delete fParticles;
            fParticles = 0;
        }
    }

   fKeepEvent = v;
}
// -----   Public method ReadEvent   --------------------------------------

Bool_t SpdUrqmdGenerator::ReadEvent(FairPrimaryGenerator* primGen) {

    // ---> Check for input file
    if (!fInputFile) {
        cout << "-E SpdUrqmdGenerator: Input file not open! " << endl;
        return kFALSE;
    }

    // ---> Check for primary generator
    //if (!primGen) {
    //    cout << "-E- SpdUrqmdGenerator::ReadEvent: "
    //            << "No PrimaryGenerator!" << endl;
    //    return kFALSE;
    //}

    // ---> Define event variables to be read from file
    int evnr = 0, ntracks = 0, aProj = 0, zProj = 0, aTarg = 0, zTarg = 0;
    float b = 0., ekin = 0.;

    int ityp = 0, i3 = 0, ichg = 0, pid = 0;
    float ppx = 0., ppy = 0., ppz = 0., m = 0.;

    // ---> Read and check first event header line from input file
    char read[200];
#ifdef GZIP_SUPPORT
    gzgets(fInputFile, read, 200); // line 1
#else
    fgets(read, 200, fInputFile);
#endif
    Int_t urqmdVersion = 0;
    sscanf(read, "UQMD   version:       %d   1000  %d  output_file  14", &urqmdVersion, &urqmdVersion);
    cout << "URQMD VERSION USED = " << urqmdVersion << endl;
#ifdef GZIP_SUPPORT
    if (gzeof(fInputFile)) {
        cout << "-I SpdUrqmdGenerator : End of input file reached." << endl;
        gzclose(fInputFile);
#else
    if ( feof(fInputFile) ) {
        cout << "-I SpdUrqmdGenerator : End of input file reached." << endl;
        fclose(fInputFile);
#endif
        fInputFile = NULL;
        return kFALSE;
    }
    if (read[0] != 'U') {
        cout << "-E SpdUrqmdGenerator: Wrong event header" << endl;
        return kFALSE;
    }

    // ---> Read rest of event header
#ifdef GZIP_SUPPORT
    gzgets(fInputFile, read, 200); // line 2
    sscanf(read, "projectile:  (mass, char) %d %d target:  (mass, char) %d %d",
            &aProj, &zProj, &aTarg, &zTarg); // line 2
    gzgets(fInputFile, read, 200); // line 3
    gzgets(fInputFile, read, 36); // line 4
    gzgets(fInputFile, read, 200); // line 4
    sscanf(read, "%f", &b); // line 4
    gzgets(fInputFile, read, 39); // line 5
    gzgets(fInputFile, read, 200); // line 5
    sscanf(read, "%e", &ekin); // line 5
    gzgets(fInputFile, read, 7); // line 6
    gzgets(fInputFile, read, 200); // line 6
    sscanf(read, "%d", &evnr); // line 6

    for (Int_t iline = 0; iline < ((urqmdVersion == 30400) ? 11 : 8); iline++)
        gzgets(fInputFile, read, 200);

    gzgets(fInputFile, read, 200); // line 18
    sscanf(read, "%d", &ntracks); // line 18
    gzgets(fInputFile, read, 200); // line 19
#else
    fgets(read, 26, fInputFile);
    fscanf(fInputFile, "%d", &aProj);
    fscanf(fInputFile, "%d", &zProj);
    fgets(read, 25, fInputFile);
    fscanf(fInputFile, "%d", &aTarg);
    fscanf(fInputFile, "%d", &zTarg);
    fgets(read, 200, fInputFile);
    fgets(read, 200, fInputFile);
    fgets(read, 36, fInputFile);
    fscanf(fInputFile, "%f", &b);
    fgets(read, 200, fInputFile);
    fgets(read, 39, fInputFile);
    fscanf(fInputFile, "%e", &ekin);
    fgets(read, 200, fInputFile);
    fgets(read, 7, fInputFile);
    fscanf(fInputFile, "%d", &evnr);
    fgets(read, 200, fInputFile);
    for (int iline=0; iline<8; iline++)  { fgets(read, 200,fInputFile); }
    fscanf(fInputFile, "%d", &ntracks);
    fgets(read, 200, fInputFile);
    fgets(read, 200, fInputFile);
#endif

    // ---> Calculate beta and gamma for Lorentztransformation
    TDatabasePDG* pdgDB = TDatabasePDG::Instance();
    TParticlePDG* kProton = pdgDB->GetParticle(2212);
    Double_t kProtonMass = kProton->Mass();

    Double_t eBeam = ekin + kProtonMass;
    Double_t pBeam = TMath::Sqrt(eBeam * eBeam - kProtonMass * kProtonMass);
    Double_t betaCM = pBeam / (eBeam + kProtonMass);
    Double_t gammaCM = TMath::Sqrt(1. / (1. - betaCM * betaCM));

    cout << "-I SpdUrqmdGenerator: Event " << evnr << ",  b = " << b
            << " fm,  multiplicity " << ntracks << ", ekin: " << ekin << endl;


    Double_t phi = 0.;
    // ---> Generate rotation angle
    if (fEventPlaneSet) {
        phi = gRandom->Uniform(fPhiMin, fPhiMax);
    }

    // Set event id and impact parameter in MCEvent if not yet done

    if(primGen){
      FairMCEventHeader* event = primGen->GetEvent();
      if (event && (!event->IsSet())) {
        event->SetEventID(evnr);
        event->SetB(b);
        event->MarkSet(kTRUE);
        event->SetRotZ(phi);
       }
    }

    Int_t itr = 0;
    // ---> Loop over tracks in the current event
    for (int itrack = 0; itrack < ntracks; itrack++) {
        if(fVerbose > 1) cout << "Read track " << itrack << endl;
        // Read momentum and PID from file
#ifdef GZIP_SUPPORT
        gzgets(fInputFile, read, 81);
        gzgets(fInputFile, read, 200);
        sscanf(read, "%e %e %e %e %d %d %d", &ppx, &ppy, &ppz, &m, &ityp, &i3, &ichg);
#else
        fgets(read, 81, fInputFile);
        fscanf(fInputFile, "%e", &ppx);
        fscanf(fInputFile, "%e", &ppy);
        fscanf(fInputFile, "%e", &ppz);
        fscanf(fInputFile, "%e", &m);
        fscanf(fInputFile, "%d", &ityp);
        fscanf(fInputFile, "%d", &i3);
        fscanf(fInputFile, "%d", &ichg);
        fgets(read, 200, fInputFile);
#endif

        // Convert UrQMD type and charge to unique pid identifier
        if (ityp >= 0) {
            pid = 1000 * (ichg + 2) + ityp;
        } else {
            pid = -1000 * (ichg + 2) + ityp;
        }

        // Convert Unique PID into PDG particle code
        if (fParticleTable.find(pid) == fParticleTable.end()) {
            cout << "-W SpdUrqmdGenerator: PID " << ityp << " charge "
                    << ichg << " not found in table (" << pid << ")" << endl;
            continue;
        }
        Int_t pdgID = fParticleTable[pid];

        // CM system
        Double_t mass = Double_t(m);
        Double_t px = Double_t(ppx);
        Double_t py = Double_t(ppy);
        Double_t pz = Double_t(ppz);
        Double_t e = sqrt(mass * mass + px * px + py * py + pz * pz);
    //  transform to laboratory
    //    if (gCoordinateSystem == sysLaboratory) 
    //        pz = gammaCM * (pz + betaCM * e);
        Double_t ee = sqrt(mass * mass + px * px + py * py + pz * pz);

        if (fEventPlaneSet) {
            Double_t pt = Sqrt(px * px + py * py);
            Double_t azim = ATan2(py, px);
            azim += phi;
            px = pt * Cos(azim);
            py = pt * Sin(azim);
        }

        TLorentzVector pp;
        pp.SetPx(px);
        pp.SetPy(py);
        pp.SetPz(pz);
        pp.SetE(ee);

        if (fKeepEvent && fParticles) {
            Int_t np = fParticles->GetEntriesFast();
            TParticle* particle = new ((*fParticles)[np]) TParticle();
            particle->SetPdgCode(pdgID);
            particle->SetProductionVertex(0, 0, 0, 0); 

            if(fVerbose)
                cout << "   PDG " << pdgID << " " << px << " " << py << " " << pz << endl;
                        
            TParticlePDG* pdgpart = TDatabasePDG::Instance()->GetParticle(pdgID); 
            if (pdgpart) {
                 Double_t pdgmass = pdgpart->Mass();
                 Double_t te = TMath::Sqrt(px*px + py*py + pz*pz + pdgmass*pdgmass);   
                 particle->SetMomentum(px, py, pz, te);
            }
            else {
                 particle->SetMomentum(px, py, pz, -1);    
            }
        }




      // Give track to PrimaryGenerator  (for FairGenerator only!)
      // if(primGen) 
	  // {
      //        //primGen->AddTrack(pdgID, px, py, pz, 0., 0., 0.);
      // }  
      
      AddTrack(primGen, 1, pdgID , itr++, -1, px, py, pz); 
           
    }

    return kTRUE;
}
// ------------------------------------------------------------------------


// -----   Public method ReadEvent   --------------------------------------

Bool_t SpdUrqmdGenerator::SkipEvents(Int_t count) {
    if (count <= 0) {
        return kTRUE;
    }

    for (Int_t ii = 0; ii < count; ii++) {
        // ---> Check for input file
        if (!fInputFile) {
            cout << "-E SpdUrqmdGenerator: Input file not open! " << endl;
            return kFALSE;
        }

        // ---> Define event variables to be read from file
        int evnr = 0, ntracks = 0, aProj = 0, zProj = 0, aTarg = 0, zTarg = 0;
        float b = 0., ekin = 0.;

        // ---> Read and check first event header line from input file
        char read[200];
#ifdef GZIP_SUPPORT
        gzgets(fInputFile, read, 200); // line 1
#else
        fgets(read, 200, fInputFile);
#endif
        Int_t urqmdVersion = 0;
        sscanf(read, "UQMD   version:       %d   1000  %d  output_file  14", &urqmdVersion, &urqmdVersion);
        cout << "URQMD VERSION USED = " << urqmdVersion << endl;

#ifdef GZIP_SUPPORT
        if (gzeof(fInputFile)) {
            cout << "-I SpdUrqmdGenerator : End of input file reached." << endl;
            gzclose(fInputFile);
#else
        if ( feof(fInputFile) ) {
            cout << "-I SpdUrqmdGenerator : End of input file reached." << endl;
            fclose(fInputFile);
#endif
            fInputFile = NULL;
            return kFALSE;
        }
        if (read[0] != 'U') {
            cout << "-E SpdUrqmdGenerator: Wrong event header" << endl;
            return kFALSE;
        }

        // ---> Read rest of event header
#ifdef GZIP_SUPPORT
        gzgets(fInputFile, read, 200); // line 2
        sscanf(read, "projectile:  (mass, char) %d %d target:  (mass, char) %d %d",
                &aProj, &zProj, &aTarg, &zTarg); // line 2
        gzgets(fInputFile, read, 200); // line 3
        gzgets(fInputFile, read, 36); // line 4
        gzgets(fInputFile, read, 200); // line 4
        sscanf(read, "%f", &b); // line 4
        gzgets(fInputFile, read, 39); // line 5
        gzgets(fInputFile, read, 200); // line 5
        sscanf(read, "%e", &ekin); // line 5
        gzgets(fInputFile, read, 7); // line 6
        gzgets(fInputFile, read, 200); // line 6
        sscanf(read, "%d", &evnr); // line 6

        for (Int_t iline = 0; iline < ((urqmdVersion == 30400) ? 11 : 8); iline++)
            gzgets(fInputFile, read, 200);

        gzgets(fInputFile, read, 200); // line 15
        sscanf(read, "%d", &ntracks); // line 15
        gzgets(fInputFile, read, 200); // line 16
#else
        fgets(read, 26, fInputFile);
        fscanf(fInputFile, "%d", &aProj);
        fscanf(fInputFile, "%d", &zProj);
        fgets(read, 25, fInputFile);
        fscanf(fInputFile, "%d", &aTarg);
        fscanf(fInputFile, "%d", &zTarg);
        fgets(read, 200, fInputFile);
        fgets(read, 200, fInputFile);
        fgets(read, 36, fInputFile);
        fscanf(fInputFile, "%f", &b);
        fgets(read, 200, fInputFile);
        fgets(read, 39, fInputFile);
        fscanf(fInputFile, "%e", &ekin);
        fgets(read, 200, fInputFile);
        fgets(read, 7, fInputFile);
        fscanf(fInputFile, "%d", &evnr);
        fgets(read, 200, fInputFile);
        for (int iline=0; iline<8; iline++)  { fgets(read, 200,fInputFile); }
        fscanf(fInputFile, "%d", &ntracks);
        fgets(read, 200, fInputFile);
        fgets(read, 200, fInputFile);
#endif

        cout << "-I SpdUrqmdGenerator: Event " << evnr << " skipped!" << endl;

        // ---> Loop over tracks in the current event
        for (int itrack = 0; itrack < ntracks; itrack++) {

            // Read momentum and PID from file
#ifdef GZIP_SUPPORT
            gzgets(fInputFile, read, 200);
#else
            fgets(read, 81, fInputFile);
            fgets(read, 200, fInputFile);
#endif
        }
    }
    return kTRUE;
}
// ------------------------------------------------------------------------

// -----   Private method ReadConverisonTable   ---------------------------

void SpdUrqmdGenerator::ReadConversionTable() {

    TString work = getenv("VMCWORKDIR");
    TString fileName = work + "/input/urqmd_pdg.dat";
    ifstream* pdgconv = new ifstream(fileName.Data());

    Int_t index = 0;
    Int_t pdgId = 0;

    while (!pdgconv->eof()) {
        index = pdgId = 0;
        *pdgconv >> index >> pdgId;
        fParticleTable[index] = pdgId;
    }

    pdgconv->close();
    delete pdgconv;

    cout << "-I SpdUrqmdGenerator: Particle table for conversion from "
            << "UrQMD loaded" << endl;

}
// ------------------------------------------------------------------------

void SpdUrqmdGenerator::SetEventPlane(Double_t phiMin, Double_t phiMax) {
    fPhiMin = phiMin;
    fPhiMax = phiMax;
    fEventPlaneSet = kTRUE;
}



ClassImp(SpdUrqmdGenerator);
