// $Id$
// Author: artur   2021/03/09


//_____________________________________________________________________________
//
// SpdRootFileSink
//_____________________________________________________________________________

#include "SpdRootFileSink.h"

#include "FairRootManager.h"   
#include "FairFileSource.h"

#include <TBranch.h>
#include <TCollection.h>
#include <TClonesArray.h>
#include <TObjArray.h>

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdRootFileSink)

//_____________________________________________________________________________
SpdRootFileSink::SpdRootFileSink(TFile *f, const char* Title):
FairRootFileSink(f,Title),fAddAllInputBranches(false),fEventStatus(true)
{
   
}

//_____________________________________________________________________________
SpdRootFileSink::SpdRootFileSink(const TString* RootFileName, const char* Title):
FairRootFileSink(RootFileName,Title),fAddAllInputBranches(false),fEventStatus(true)
{
    
}

//_____________________________________________________________________________
SpdRootFileSink::SpdRootFileSink(const TString RootFileName, const char* Title):
FairRootFileSink(RootFileName,Title),fAddAllInputBranches(false),fEventStatus(true)
{

}
    
//_____________________________________________________________________________
SpdRootFileSink::~SpdRootFileSink() 
{
 
}

//_____________________________________________________________________________
Bool_t SpdRootFileSink::SetEventStatus(Bool_t status)
{
    if (fEventStatus == status) return fEventStatus;

    if (fBranchList.empty()) {
        TTree* tree = GetOutTree();
        if (!tree) return fEventStatus;
        TObjArray* branches = tree->GetListOfBranches(); 
        Int_t nb = branches->GetEntriesFast();
        for (Int_t i(0); i < nb; i++) {
             TBranch* branch = (TBranch*)branches->UncheckedAt(i);
             if (!branch) continue;
             fBranchList.push_back(branch); 
             //cout << "branch: " << fBranchList[i]->GetName() << endl;
        }
    }
    
    for (TBranch* b : fBranchList) b->SetStatus(status);  
    
    //cout << "\n-I- <SpdRootFileSink::SetEventStatus> Event status is changed: " 
    //     << fEventStatus << " -> " << status;
    
    fEventStatus = status;
    
    return fEventStatus;
}

//_____________________________________________________________________________
void SpdRootFileSink::ResetEventStatus() 
{ 
    SetEventStatus(true);
    fBranchList.clear(); 
}
 
//_____________________________________________________________________________
void SpdRootFileSink::AddBranchToOutput(TString branch_name)
{
    if (branch_name == "all" || 
        branch_name == "All" || 
        branch_name == "ALL") 
    {
       fAddAllInputBranches = true;  
       return;
    }
    
    fBranchesToAdd[branch_name] = "";
    fAddAllInputBranches = false;
}

//_____________________________________________________________________________
void SpdRootFileSink::WriteFolder()
{
    if (!fOutFolder) return;
    
    //cout << "-I- <SpdRootFileSink::WriteFolder> Main folder name: \"" 
    //     << fOutFolder->GetName() << "\"" << endl;
         
    if (fAddAllInputBranches || !fBranchesToAdd.empty()) AddInputBranches();
    
    FairRootFileSink::WriteFolder();
}

//_____________________________________________________________________________
Bool_t SpdRootFileSink::SearchForInputBranch(const TString branch_name, TString& folder_name)
{
    FairRootManager* rm = FairRootManager::Instance();
    FairFileSource* source = dynamic_cast<FairFileSource*>(rm->GetSource());
    
    if (!source) { folder_name = ""; return false; }
  
    TFile*   file = source->GetInFile();
    TFolder* folder = (TFolder*)file->Get(fOutFolder->GetName());  //!!!  
    TCollection* flist = folder->GetListOfFolders();
    
    TFolder* ff;
    TIter it(flist);
    while ((ff = (TFolder*)it.Next())) {
        if (ff->FindObjectAny(branch_name)) {
            folder_name = ff->GetName();
            return true;
        }
    }
    
    folder_name = "";
    return false;
}

//_____________________________________________________________________________
Bool_t SpdRootFileSink::SearchForOutputBranch(const TString branch_name, TString& folder_name)
{
    TCollection* flist = fOutFolder->GetListOfFolders();
    
    TFolder* ff;
    TIter it(flist);
    while ((ff = (TFolder*)it.Next())) {
        if (ff->FindObjectAny(branch_name)) {
            folder_name = ff->GetName();
            return true;
        }
    }
    
    folder_name = "";
    return false;
}

//_____________________________________________________________________________
Bool_t SpdRootFileSink::CheckInputBranches()
{
    FairRootManager* rm = FairRootManager::Instance();
    FairFileSource* source = dynamic_cast<FairFileSource*>(rm->GetSource());
    
    if (fAddAllInputBranches) 
    {
        fBranchesToAdd.clear();
        TObjArray* branches = source->GetInTree()->GetListOfBranches(); 
        Int_t nb = branches->GetEntriesFast();
        for (Int_t i(0); i < nb; i++) {
             TBranch* branch = (TBranch*)branches->UncheckedAt(i);
             if (!strcmp(branch->GetName(),"EventHeader.")) continue;
             fBranchesToAdd[branch->GetName()] = ""; 
        }
    }
        
    if (fBranchesToAdd.empty()) return false;
        
    if (!source) return false;
        
    TFile* file = source->GetInFile();
    TFolder* folder = (TFolder*)file->Get(fOutFolder->GetName());  //!!!  
    TCollection* flist = folder->GetListOfFolders();
    
    TFolder* ff;
    TIter it(flist);
    
    Int_t n(0);
    for (std::pair<const TString,TString>& branch : fBranchesToAdd) 
    {
         while ((ff = (TFolder*)it.Next())) {
             if (ff->FindObjectAny(branch.first)) {
                 branch.second = ff->GetName();
                 n++;
                 break;
             }
         }
         it.Reset();
    }
    
//     cout << "---------------------------------" << endl;
//     for (auto branch : fBranchesToAdd) 
//     {
//         cout << "["<< branch.first << "] [" << branch.second << "]" << endl;
//     }
//     cout << "---------------------------------" << endl;
 
    return (n > 0);
}

//_____________________________________________________________________________
void SpdRootFileSink::AddInputBranches()
{
    if (!CheckInputBranches()) return;
    
    FairRootManager* rm = FairRootManager::Instance();
    
    TString folder;
    for (auto branch : fBranchesToAdd) 
    {
        if (branch.second.IsWhitespace()) {
            cout << "-W- <SpdRootFileSink::AddInputBranches> No branch \"" << branch.first
                 << "\" not found in the input, nothing is done." << endl;
            continue;
        }
       
        if (SearchForOutputBranch(branch.first,folder)) {
            cout << "-W- <SpdRootFileSink::AddInputBranches> Branch \"" << branch.first
                 << "\" is already in the output, nothing is done." << endl;
            if (folder != branch.second) {     
                cout << "-W- <SpdRootFileSink::AddInputBranches> Branch \"" << branch.first
                     << "\" folders are different (input,output) : "  
                     << branch.first << "," << folder << endl;
            }
            continue;
        }

        //cout << "-I- <SpdRootFileSink::AddInputBranches> Add branch: \"" 
        //     << branch.first << "\" in the folder: " << branch.second << endl;
        
        TCollection* coll = dynamic_cast<TCollection*>(rm->GetObject(branch.first));
        if (coll) {
            cout << "-I- <SpdRootFileSink::AddInputBranches> Add collection (" 
                 << coll->ClassName() << ") at the branch: \"" 
                 << branch.first << "\"" << endl;  
            rm->Register(branch.first,branch.second,coll,true); 
            continue; 
        }
        
        TNamed* obj = dynamic_cast<TNamed*>(rm->GetObject(branch.first));
        if (obj) {
            cout << "-I- <SpdRootFileSink::AddInputBranches> Add object (" 
                 << obj->ClassName() << ") at the branch: \"" 
                 << branch.first << "\"" << endl;  
            rm->Register(branch.first,branch.second,obj,true);  
            continue; 
        }
    }
}

