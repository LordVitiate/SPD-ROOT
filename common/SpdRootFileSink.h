// $Id$
// Author: artur   2021/03/09

#ifndef __SPDROOTFILESINK_H__
#define __SPDROOTFILESINK_H__

#include "FairRootFileSink.h"
#include <map>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRootFileSink                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TBranch;

class SpdRootFileSink: public FairRootFileSink {

public:
    
    SpdRootFileSink(TFile *f, const char* Title = "OutputRootFile");
    SpdRootFileSink(const TString* RootFileName, const char* Title = "OutputRootFile");
    SpdRootFileSink(const TString RootFileName, const char* Title = "OutputRootFile");

    virtual ~SpdRootFileSink();

    // add input branches to output
    void    AddBranchToOutput(TString branch_name);
    
    // alter event status (writeble or not)
    Bool_t  SetEventStatus(Bool_t status);
    Bool_t  GetEventStatus() const { return fEventStatus; }
    void    ResetEventStatus();
    
    virtual void WriteFolder() override;

protected:

    Bool_t  CheckInputBranches();

    Bool_t  SearchForInputBranch(const TString branch_name, TString& folder_name); 
    Bool_t  SearchForOutputBranch(const TString branch_name, TString& folder_name); 

    void    AddInputBranches();
    
    std::map<TString,TString> fBranchesToAdd; // [branch_name, branch_folder]
    Bool_t  fAddAllInputBranches;
    
    std::vector<TBranch*> fBranchList;
    Bool_t  fEventStatus;
    
    ClassDefOverride(SpdRootFileSink,1)
};

#endif  /* __SPDROOTFILESINK_H__ */

