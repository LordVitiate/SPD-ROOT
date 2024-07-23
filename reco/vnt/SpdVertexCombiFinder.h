// $Id$
// Author: artur   2021/04/16

#ifndef __SPDVERTEXCOMBIFINDER_H__
#define __SPDVERTEXCOMBIFINDER_H__

#include <TObject.h>
#include <map>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdVertexCombiFinder                                                       //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdVertexCombiFinder: public TObject {

public:

    SpdVertexCombiFinder();
    virtual ~SpdVertexCombiFinder();
    
    void    ClearAll();
   
    // initialization
    Bool_t  InitParticles(Int_t n, const Int_t* pdg);
    Bool_t  InitParticles(const std::vector<Int_t>& pdg);
     
    Bool_t  InitVertex(Int_t pdg1, Int_t pdg2);
    Bool_t  InitVertex(Int_t pdg1, Int_t pdg2, Int_t pdg3);
    Bool_t  InitVertex(Int_t pdg1, Int_t pdg2, Int_t pdg3, Int_t pdg4);
    Bool_t  InitVertex(Int_t n, const Int_t* pdg);
    Bool_t  InitVertex(const std::vector<Int_t>& pdg);
    
    void    SetVerboseLevel(Int_t level) { fVerboseLevel = level; } // 0, 1
     
    // getters 
    Int_t   GetNParticles()       const { return fNParticles;       }
    Int_t   GetNVertexParticles() const { return fNVertexParticles; }
    
    Int_t   GetFinderStatus() const { return fInitStatus; }
    Bool_t  GetIterStatus()   const { return fStartIter;  }

    Int_t   GetNVC() const;  // return total number of vertex candidates
      
    // iterator (returns vertex candidate)
    void    ResetIterator() { fStartIter = true; }
    Bool_t  Next(std::vector<Int_t>& vc);
    
    // to print 
    void    PrintSettings() const;
    void    PrintVertexCandidates();

private:
    
    // settings
    Int_t  fInitStatus;
    
    std::map< Int_t, std::vector<Int_t> > fParticles;
    std::map< Int_t, std::vector<Int_t> > fVtx;
    
    Int_t  fNParticles;
    Int_t  fNVertexParticles;
    
    // iterator
    Bool_t fStartIter;
        
       /* auxiliary iterator data members */
    Int_t  fNVC;
    Int_t  fNP; 
    std::vector<Int_t> fVtxN;
    std::vector<Int_t> fIdx;
    
    // verbosity
    Int_t  fVerboseLevel;
    
    // useful private methods
    void   PrepareVtx(const std::map<Int_t, Int_t>& vertex);
   
    Bool_t NextX(std::vector<Int_t>& x, const Int_t& l, const Int_t& m);
    Bool_t NextY();
   
    void   ClearIterator();
     
    inline void Assign(Int_t pos, Int_t n, const std::vector<Int_t>& idx, 
                  const std::vector<Int_t>& from, std::vector<Int_t>& to)
    { for (Int_t i(0); i<n; i++) to[pos+i] = from[idx[i]]; }
    
    inline void Assign2(Int_t pos, Int_t n, Int_t pos2, 
                        const std::vector<Int_t>& from, std::vector<Int_t>& to)
    { for (Int_t i(0); i<n; i++) to[pos+i] = from[pos2+i]; }
    
    ClassDef(SpdVertexCombiFinder,1)
};


#endif  /* __SPDVERTEXCOMBIFINDER_H__ */

