
SpdMCDataIterator* IT = 0;

// parameters
const SpdSetParSet* Pars_ = 0;

//----------------------------------
// pointers to current data objects

// main MC
const SpdMCEvent*     Event_     = 0;           //! Main mc-data object
const TClonesArray*   Particles_ = 0;           //! List of mc-particles 
const TClonesArray*   Vertices_  = 0;           //! List of mc-vertices

// hits
const TClonesArray*   ItsHits_   = 0;           //! List of ITS mc-hits
const TClonesArray*   TsHits_    = 0;           //! List of TS mc-hits
const TClonesArray*   TofHits_   = 0;           //! List of TOF mc-hits
const TClonesArray*   EcalHits_  = 0;           //! List of ECAL mc-hits
const TClonesArray*   RsHits_    = 0;           //! List of RS mc-hits
const TClonesArray*   BbcHits_   = 0;           //! List of BBC mc-hits
const TClonesArray*   AegHits_   = 0;           //! List of AEG mc-hits
const TClonesArray*   ZdcHits_   = 0;           //! List of ZDC mc-hits

// objects
const TClonesArray*   Tracks_          = 0;     //! List of mc-tracks 
const TClonesArray*   VerticesRC_      = 0;     //! List of rc-vertices 

const TClonesArray*   EcalClustersRC_     = 0;  //! List of ECAL rc-clusters 
const TClonesArray*   EcalParticlesRC_    = 0;  //! List of ECAL rs-particles 
const TClonesArray*   EcalClustersMCInfo_ = 0;  //! List of ECAL rc-clusters mc-info
const TClonesArray*   EcalParticlesMC_    = 0;  //! List of ECAL mc-particles 

const TClonesArray*   RsClustersMC_  = 0;       //! List of Rs mc-clusters 
const TClonesArray*   RsParticlesMC_ = 0;       //! List of Rs mc-particles 

const TClonesArray*   TsParticles_   = 0;       //! List of TS particles 
const TClonesArray*   TofParticles_  = 0;       //! List of TOF particles 

//____________________________________________________________________
Bool_t DoSomething(Int_t nevent)
{
    Bool_t save_current_event;
   
    if (nevent%2 == 0) save_current_event = true;
    //if ((nevent+1)%2 == 0) save_current_event = true;
    else save_current_event = false;
    
    //if (nevent == 6) return false;
        
    return  save_current_event;  
}

//___________________________________________________________________________________________
void ReadRecoData2() 
{
    IT = new SpdMCDataIterator();
    
    //----------------------------------------------------------
    // Add input data files
    //----------------------------------------------------------
    // (1) Add single file which is located in the /macro directory  
    
    IT->AddSourceFile("reco_full.root");
    
    // (2) Add data files from specified input directory  
    
//     TString dataDir; 
//     dataDir = gSystem->WorkingDirectory();
//     dataDir += "/indata";
//     
//     IT->SetCurrentDataDir(dataDir);
//     
//     IT->AddSourceFile("reco_full1.root");
//     IT->AddSourceFile("reco_full2.root");
//     IT->AddSourceFile("reco_full3.root");
//     IT->AddSourceFile("reco_full4.root");
//     IT->AddSourceFile("reco_full5.root");
    
    //----------------------------------------------------------
    // Set output file
    
    IT->SetOutputFile("reco_outputx.root");
    
    IT->AddBranchToOutput("all");
     
    //----------------------------------------------------------
    // Activate input data branches for reading
    
    //IT->ActivateBranch("MCEvent.");
    //IT->ActivateBranch("MCParticles");
    //IT->ActivateBranch("MCVertices");
    //IT->ActivateBranch("ItsMCHits")      
    //IT->ActivateBranch("TsMCHits");   
    //IT->ActivateBranch("TofMCHits");
    //IT->ActivateBranch("EcalMCHits");      
    //IT->ActivateBranch("RsMCHits");   
    //IT->ActivateBranch("BbcMCHits");
    //IT->ActivateBranch("AegMCHits");
    //IT->ActivateBranch("ZdcMCHits");
    //IT->ActivateBranch("MCTracks");        
    //IT->ActivateBranch("RCVertices");      
    //IT->ActivateBranch("RCEcalClusters");  
    //IT->ActivateBranch("RCEcalParticles"); 
    //IT->ActivateBranch("MCEcalClustersInfo");
    //IT->ActivateBranch("MCEcalParticles");
    //IT->ActivateBranch("MCRsClusters"); 
    //IT->ActivateBranch("MCRsParticles");
    //IT->ActivateBranch("TsParticles");
    //IT->ActivateBranch("TofParticles"); 
    
    // Alternatively, just enable all branches
    IT->ActivateBranch("all");
    
    IT->InactivateBranch("TsParticles");
    IT->InactivateBranch("TofParticles"); 
    
    IT->InactivateBranch("BbcMCHits");
    IT->InactivateBranch("AegMCHits");
    IT->InactivateBranch("ZdcMCHits");
    
    //----------------------------------------------------------
    // Iterator initialization
    IT->Init();
    
    Pars_ = IT->GetParameters(); 
    
    Event_     = IT->GetEvent();
    Particles_ = IT->GetParticles();    
    Vertices_  = IT->GetVertices(); 

    ItsHits_   = IT->GetItsHits(); 
    TsHits_    = IT->GetTsHits(); 
    TofHits_   = IT->GetTofHits();   
    EcalHits_  = IT->GetEcalHits(); 
    RsHits_    = IT->GetRsHits(); 
    BbcHits_   = IT->GetBbcHits(); 
    AegHits_   = IT->GetAegHits(); 
    ZdcHits_   = IT->GetZdcHits();

    Tracks_          = IT->GetTracks();         
    VerticesRC_      = IT->GetVerticesRC();    
    
    EcalClustersRC_  = IT->GetEcalClustersRC(); 
    EcalParticlesRC_ = IT->GetEcalParticlesRC();
    EcalClustersMCInfo_ = IT->GetEcalClustersMCInfo();
    EcalParticlesMC_    = IT->GetEcalParticlesMC();
    
    RsClustersMC_  = IT->GetRsClustersMC();   
    RsParticlesMC_ = IT->GetRsParticlesMC(); 
    
    TsParticles_   = IT->GetTsParticles();    
    TofParticles_  = IT->GetTofParticles();   
    
    //------------------------------------------------------------
    Int_t ne_total(0), ne_accepted(0);
    Bool_t save_event;
    
    while (IT->NextEvent()) 
    {
        //if (Event_) Event_->PrintEvent();
        
        save_event = DoSomething(ne_total);
        
        if (!save_event) IT->SkipEventToSave();
        else ne_accepted++;
     
        ne_total++;
    }
    
    // ATTENTION save output tree ATTENTION
    IT->Finish(); 

    cout << ">>>>>> Events (accepted/total): " << ne_accepted << "/" << ne_total << endl;

    //if (Pars_) Pars_->PrintParameters(0); //option = 0,1 
}

