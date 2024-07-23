/**
 * @file SpdRunAna.h
 * @brief Header file for the SpdRunAna class.
 *
 * This file contains the class definition and member function declarations 
 * for the SpdRunAna class, which is responsible for running and analyzing 
 * simulations within the FAIRROOT framework.
 */

#include "SpdRunAna.h"

#include "FairIon.h"
#include "FairField.h"

#include "FairBaseParSet.h" 
#include "FairGeoParSet.h"    
#include "FairRuntimeDb.h"  
#include "FairEventHeader.h"
#include "FairTask.h"

#include "FairFileSource.h"
#include "FairRootFileSink.h"

#include "SpdParticlesAndProcesses.h"
#include "SpdCommonGeoMapper.h"

#include <iostream>
using std::cout;
using std::endl;

/**
 * @class SpdRunAna
 * @brief Implements the simulation run analysis for the SPD.
 *
 * The SpdRunAna class is derived from FairRunAna and provides functionality
 * to initialize, load, save, and manage simulation parameters. It also 
 * executes tasks related to event processing and data management.
 */
ClassImp(SpdRunAna)

//_____________________________________________________________________________
/**
 * @brief Default constructor for SpdRunAna.
 *
 * Initializes member variables and adds particles to the database.
 */
SpdRunAna::SpdRunAna():FairRunAna(),fParSet(0),
fKeepParameters(true),
fLoadGeoParSet(true),fLoadBaseParSet(true),
fGeoParSet(0),fBaseParSet(0),
fIsParsSaved(false)
{ 
    SpdParticlesAndProcesses::AddParticlesToDatabasePDG();  
   
    fgRinstance = this;
}

//_____________________________________________________________________________
/**
 * @brief Destructor for SpdRunAna.
 *
 * Deletes the parameter set and performs necessary cleanup.
 */
SpdRunAna::~SpdRunAna() 
{
    DeleteParSet();
}

//_____________________________________________________________________________
/**
 * @brief Deletes the parameter set.
 *
 * Frees the memory allocated for the parameter set.
 */
void SpdRunAna::DeleteParSet()
{
    if (fParSet) {
        delete fParSet;
        fParSet = 0;
    }  
}
 
//_____________________________________________________________________________
/**
 * @brief Retrieves the parameter set assembly.
 *
 * @return Pointer to SpdSetParSet containing simulation parameters.
 */
SpdSetParSet* SpdRunAna::GetParSetAssembly()  
{ 
    if (fParSet) return fParSet; 
    fParSet = new SpdSetParSet();
    return fParSet;
}

//_____________________________________________________________________________
/**
 * @brief Retrieves the parameters for a given name.
 *
 * @param pars_name The name of the parameters to retrieve.
 * @return Pointer to FairParGenericSet containing the requested parameters.
 */
FairParGenericSet* SpdRunAna::GetParameters(TString pars_name)
{
    FairParGenericSet* parameters = 0;
    
    FairRuntimeDb* rtdb = GetRuntimeDb();
    if (rtdb && (rtdb->getFirstInput() || rtdb->getSecondInput())) {
        if (pars_name == "FairGeoParSet" || pars_name == "FairBaseParSet") return parameters;
        parameters = dynamic_cast<FairParGenericSet*>(rtdb->getContainer(pars_name));
        if (parameters) ((FairParSet*)parameters)->init();
    }
    else {
        FairRootManager* rm = FairRootManager::Instance();
        FairFileSource* source = dynamic_cast<FairFileSource*>(rm->GetSource());
        if (!source) return 0;
        parameters = dynamic_cast<FairParGenericSet*>(((TFile*)source->GetRootFile())->Get(pars_name));
        if (pars_name == "FairGeoParSet")  fGeoParSet  = parameters;
        if (pars_name == "FairBaseParSet") fBaseParSet = parameters;
    }
    
    return parameters;
}

//_____________________________________________________________________________
/**
 * @brief Initializes the run analysis.
 *
 * Sets up particles, loads parameters, and initializes the task execution.
 */
void SpdRunAna::Initialize()
{
    cout << "-I- <SpdRunAna::Initialize>(1) >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> " << endl; 
    
    SpdParticlesAndProcesses::AddParticlesToDatabasePDG();
    
    LoadParameters();
    
    Init();
    
    if (fKeepParameters) SaveParameters();
   
    cout << "-I- <SpdRunAna::Initialize>(2) <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << "\n\n"; 
}

//_____________________________________________________________________________
/**
 * @brief Loads simulation parameters.
 *
 * Attempts to load base parameters and geometry from the runtime database or file source.
 */
void SpdRunAna::LoadParameters()
{
    // try loading base parameters & geometry from Runtime Database (from separate file)
    FairRuntimeDb* rtdb = GetRuntimeDb();
    if (!rtdb) return;
    
    if (rtdb->getFirstInput() || rtdb->getSecondInput()) 
    {
        if (!fLoadGeoParSet) {
            FairParSet* parset1 = GetRuntimeDb()->getContainer("FairGeoParSet");
            if (parset1) parset1->setStatic(true);
        }
        if (!fLoadBaseParSet) {
            FairParSet* parset2 = GetRuntimeDb()->getContainer("FairBaseParSet");
            if (parset2) parset2->setStatic(true);
        }    
    }
    else {
        fLoadGeoParSet  = false;
        fLoadBaseParSet = false;
        FairParSet* parset1 = GetRuntimeDb()->getContainer("FairGeoParSet");
        if (parset1) parset1->setStatic(true);
        FairParSet* parset2 = GetRuntimeDb()->getContainer("FairBaseParSet");
        if (parset2) parset2->setStatic(true);
    }
    
    FairRootManager* rm = FairRootManager::Instance();
    FairFileSource* source = dynamic_cast<FairFileSource*>(rm->GetSource());
    if (!source) return;
    fParSet = dynamic_cast<SpdSetParSet*>(((TFile*)source->GetRootFile())->Get("SpdSetParSet"));
}

//_____________________________________________________________________________
/**
 * @brief Saves simulation parameters.
 *
 * Writes the parameters to a root file sink if they haven't been saved yet.
 */
void SpdRunAna::SaveParameters()
{
    if (fIsParsSaved) return;
    
    cout << "-I- <SpdRunAna::SaveParameters> ... " << endl;
    
    fIsParsSaved = true;
    
    FairRootManager* ioman = FairRootManager::Instance();
    FairRootFileSink* sink = (FairRootFileSink*)ioman->GetSink(); 
    
    if (!sink) return;
    
    FairParSet *geoparset(0), *baseparset(0);
    
    FairRuntimeDb* rtdb = GetRuntimeDb();
    if (rtdb && (rtdb->getFirstInput() || rtdb->getSecondInput())) {
        geoparset  = GetRuntimeDb()->getContainer("FairGeoParSet");
        baseparset = GetRuntimeDb()->getContainer("FairBaseParSet"); 
    }
    else {
        FairRootManager* rm = FairRootManager::Instance();
        FairFileSource* source = dynamic_cast<FairFileSource*>(rm->GetSource());
        if (source) {
            if (fGeoParSet) geoparset = fGeoParSet;
            else geoparset  = (FairParSet*)((TFile*)source->GetRootFile())->Get("FairGeoParSet");
            if (fBaseParSet) baseparset = fBaseParSet;
            else baseparset = (FairParSet*)((TFile*)source->GetRootFile())->Get("FairBaseParSet");
        }
    }
    
    if (geoparset)  sink->WriteObject(geoparset,"FairGeoParSet",TObject::kOverwrite);
    if (baseparset) sink->WriteObject(baseparset,"FairBaseParSet",TObject::kOverwrite);
    
    if (fParSet) sink->WriteObject(fParSet,"SpdSetParSet",TObject::kOverwrite);
    
    //if (geoparset) geoparset->setStatic(false);
    //if (baseparset) baseparset->setStatic(false); 
   
    cout << "\t spdpars: " << fParSet 
         << "; geopars: " << geoparset 
         << "; basepars: " << baseparset << endl; 
}
  
//_____________________________________________________________________________
/**
 * @brief Finalizes the task.
 *
 * Prints a message indicating the completion of the task.
 */
void SpdRunAna::FinishTask()
{
    cout << "\n-I- <SpdRunAna::FinishTask> " << endl;    
}

//_____________________________________________________________________________
/**
 * @brief Processes a single entry in the run.
 *
 * @param entry The index of the entry to process.
 *
 * Reads the event data for the specified entry and reinitializes the task if the run ID changes.
 */
void SpdRunAna::RunEntry(Int_t entry)
{
   fRootManager->ReadEvent(entry);
   
   UInt_t tmpId = fEvtHeader->GetRunId();
   
   if (tmpId != fRunId) 
   {
       fRunId = tmpId;
       if (!fStatic) {
           Reinit( fRunId );
           fTask->ReInitTask();
       }
   }
   
   fRootManager->StoreWriteoutBufferData(fRootManager->GetEventTime());
   fTask->ExecuteTask("");
}
