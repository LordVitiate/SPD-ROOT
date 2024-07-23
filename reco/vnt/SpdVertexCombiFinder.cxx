// $Id$
// Author: artur   2021/04/16


//_____________________________________________________________________________
//
// SpdVertexCombiFinder
//_____________________________________________________________________________

#include <TMath.h>
#include <TString.h>
#include "SpdVertexCombiFinder.h"
#include <iostream>

typedef std::pair< Int_t, Int_t> pi_;
typedef std::pair< Int_t, std::vector<Int_t> > pdgv_;
typedef  std::map< Int_t, Int_t>::iterator itp_;
typedef  std::map< Int_t, std::vector<Int_t> >::iterator itm_;

#define SPDVCF_MAXNCOMBI 1000000

using namespace std;

ClassImp(SpdVertexCombiFinder)

//_____________________________________________________________________________
SpdVertexCombiFinder::SpdVertexCombiFinder():fInitStatus(-1),
fNParticles(0),fNVertexParticles(0),
fStartIter(true),fNVC(1),fNP(-1),
fVerboseLevel(0)
{
 
}

//_____________________________________________________________________________
SpdVertexCombiFinder::~SpdVertexCombiFinder() 
{
 
}

//_____________________________________________________________________________
void SpdVertexCombiFinder::ClearAll()
{
    fParticles.clear();
    fVtx.clear();
    
    fNParticles = 0;
    fNVertexParticles = 0;
    
    fInitStatus = -1;
    fStartIter = true; 
}

//_____________________________________________________________________________
Bool_t SpdVertexCombiFinder::InitParticles(Int_t n, const Int_t* pdg)
{
    ClearAll();
    
    if (n < 2) return false; 

    itm_ it;
    for (Int_t i(0); i<n; i++) {
         it = fParticles.find(pdg[i]);
         if (it != fParticles.end()) it->second.push_back(i);
         else {
             vector<Int_t> v = {i};
             fParticles.insert(pdgv_(pdg[i],v)); 
         }
    }
     
    fNParticles = n;
     
    fInitStatus = 0;
    
    return true;
}

//_____________________________________________________________________________
Bool_t SpdVertexCombiFinder::InitParticles(const std::vector<Int_t>& pdg)
{
    ClearAll();
     
    if (pdg.size() < 2) return false;
    
    fNParticles = pdg.size();
    
    itm_ it;
    for (Int_t i(0); i<fNParticles; i++) {
         it = fParticles.find(pdg[i]);
         if (it != fParticles.end()) it->second.push_back(i);
         else {
             vector<Int_t> v = {i};
             fParticles.insert(pdgv_(pdg[i],v)); 
         }
    }
     
    fInitStatus = 0;
    
    return true;
}

//_____________________________________________________________________________
Bool_t SpdVertexCombiFinder::InitVertex(Int_t pdg1, Int_t pdg2)
{
    fNVertexParticles = 0;
    
    if (fInitStatus < 0) return false;
    
    fInitStatus = 0;
    fStartIter = true;
    
    if (!fVtx.empty()) fVtx.clear();
    
    // first check 
    if (fParticles.find(pdg1) == fParticles.end()) return false;
    if (fParticles.find(pdg2) == fParticles.end()) return false;
    
    // set vertex
    map<Int_t,Int_t> Vertex;
    
    if (pdg1 == pdg2) Vertex.insert(pi_(pdg1,2));
    else {
       Vertex.insert(pi_(pdg1,1));
       Vertex.insert(pi_(pdg2,1));
    }
    
    // second check 
    itm_ it;
    for (pi_ p : Vertex) {
         it = fParticles.find(p.first);
         if (p.second > it->second.size()) return false;
    }
    
    PrepareVtx(Vertex);

    fInitStatus = 1;
    
    fNVertexParticles = 2;
    
    if (fVerboseLevel > 0) {
        cout << "-I- <SpdVertexCombiFinder::InitVertex> Finder status: " 
             << fInitStatus << "; vertex candidates: " << GetNVC() << endl;
    }
    
    return true;
}

//_____________________________________________________________________________
Bool_t SpdVertexCombiFinder::InitVertex(Int_t pdg1, Int_t pdg2, Int_t pdg3)
{
    fNVertexParticles = 0;
    
    if (fInitStatus < 0) return false;
    
    fInitStatus = 0;
    fStartIter = true; 
    
    if (!fVtx.empty()) fVtx.clear();
    
    // first check 
    if (fParticles.find(pdg1) == fParticles.end()) return false;
    if (fParticles.find(pdg2) == fParticles.end()) return false;
    if (fParticles.find(pdg3) == fParticles.end()) return false;
    
    // set vertex
    map<Int_t,Int_t> Vertex;
    
    Vertex.insert(pi_(pdg1,1));

    if (pdg2 == pdg1) Vertex[pdg1]++;
    else Vertex.insert(pi_(pdg2,1));
        
         if (pdg3 == pdg1) Vertex[pdg1]++;
    else if (pdg3 == pdg2) Vertex[pdg2]++;
    else Vertex.insert(pi_(pdg3,1));
 
    // second check 
    itm_ it;
    for (pi_ p : Vertex) {
         it = fParticles.find(p.first);
         if (p.second > it->second.size()) return false;
    }
    
    PrepareVtx(Vertex);

    fInitStatus = 1;
    
    fNVertexParticles = 3;
    
    if (fVerboseLevel > 0) {
        cout << "-I- <SpdVertexCombiFinder::InitVertex> Finder status: " 
             << fInitStatus << "; vertex candidates: " << GetNVC() << endl;
    }  
    
    return true;
}

//_____________________________________________________________________________
Bool_t SpdVertexCombiFinder::InitVertex(Int_t pdg1, Int_t pdg2, Int_t pdg3, Int_t pdg4)
{
    fNVertexParticles = 0;
    
    if (fInitStatus < 0) return false;
    fStartIter = true; 
    
    fInitStatus = 0;
    fStartIter = true; 
 
    if (!fVtx.empty()) fVtx.clear();
    
    // first check 
    if (fParticles.find(pdg1) == fParticles.end()) return false;
    if (fParticles.find(pdg2) == fParticles.end()) return false;
    if (fParticles.find(pdg3) == fParticles.end()) return false;
    if (fParticles.find(pdg4) == fParticles.end()) return false;
    
    // set vertex
    map<Int_t,Int_t> Vertex;
    
    Vertex.insert(pi_(pdg1,1));

    if (pdg2 == pdg1) Vertex[pdg1]++;
    else Vertex.insert(pi_(pdg2,1));
        
         if (pdg3 == pdg1) Vertex[pdg1]++;
    else if (pdg3 == pdg2) Vertex[pdg2]++;
    else Vertex.insert(pi_(pdg3,1));
    
         if (pdg4 == pdg1) Vertex[pdg1]++;
    else if (pdg4 == pdg2) Vertex[pdg2]++;
    else if (pdg4 == pdg3) Vertex[pdg3]++;
    else Vertex.insert(pi_(pdg4,1));
    
    // second check 
    itm_ it;
    for (pi_ p : Vertex) {
         it = fParticles.find(p.first);
         if (p.second > it->second.size()) return false;
    }
    
    PrepareVtx(Vertex);

    fInitStatus = 1;
    
    fNVertexParticles = 4;
    
    if (fVerboseLevel > 0) {
        cout << "-I- <SpdVertexCombiFinder::InitVertex> Finder status: " 
             << fInitStatus << "; vertex candidates: " << GetNVC() << endl;
    }
    
    return true;
}

//_____________________________________________________________________________
Bool_t SpdVertexCombiFinder::InitVertex(Int_t n, const Int_t* pdg)
{
    fNVertexParticles = 0;
    
    if (fInitStatus < 0) return false;
    
    fInitStatus = 0;
    fStartIter = true; 
    
    if (!fVtx.empty()) fVtx.clear();
    
    // first check
    for (Int_t i(0); i<n; i++) {
        if (fParticles.find(pdg[i]) == fParticles.end()) return false;
    }

    // set vertex
    map<Int_t,Int_t> Vertex;

    itp_ ix;
    for (Int_t i(0); i<n; i++) {
         ix = Vertex.find(pdg[i]);
         if (ix != Vertex.end()) (ix->second)++;
         else Vertex.insert(pi_(pdg[i],1));
    }

    // second check 
    itm_ it;
    for (pi_ p : Vertex) {
         it = fParticles.find(p.first);
         if (p.second > it->second.size()) return false;
    }
    
    PrepareVtx(Vertex);

    fInitStatus = 1;
    
    fNVertexParticles = n;
    
    if (fVerboseLevel > 0) {
        cout << "-I- <SpdVertexCombiFinder::InitVertex> Finder status: " 
             << fInitStatus << "; vertex candidates: " << GetNVC() << endl;
    }
   
    return true;
}

//_____________________________________________________________________________
Bool_t SpdVertexCombiFinder::InitVertex(const std::vector<Int_t>& pdg)
{
    fNVertexParticles = 0;
    
    if (fInitStatus < 0) return false;
    
    fInitStatus = 0;
    fStartIter = true; 
    
    if (!fVtx.empty()) fVtx.clear();
    
    // first check
    Int_t n = pdg.size();
    for (Int_t i(0); i<n; i++) {
        if (fParticles.find(pdg[i]) == fParticles.end()) return false;
    }
    
    // set vertex
    map<Int_t,Int_t> Vertex;
    itp_ ix;
    for (Int_t i(0); i<n; i++) {
         ix = Vertex.find(pdg[i]);
         if (ix != Vertex.end()) (ix->second)++;
         else Vertex.insert(pi_(pdg[i],1));
    }

    // second check 
    itm_ it;
    for (pi_ p : Vertex) {
         it = fParticles.find(p.first);
         if (p.second > it->second.size()) return false;
    }
    
    PrepareVtx(Vertex);

    fInitStatus = 1;
    
    fNVertexParticles = n;

    if (fVerboseLevel > 0) {
        cout << "-I- <SpdVertexCombiFinder::InitVertex> Finder status: " 
             << fInitStatus << "; vertex candidates: " << GetNVC() << endl;
    }
    
    return true;
}

//_____________________________________________________________________________
void SpdVertexCombiFinder::PrepareVtx(const std::map<Int_t, Int_t>& vertex)
{
    itm_ it;
    Int_t n, m, l, s;
    vector<Int_t> v, idx;
    
    for (pi_ p : vertex) {
        
         it = fParticles.find(p.first);
         
         m = p.second;          //-> vertex particles
         n = it->second.size(); //-> particles
      
         if (m == 1 && n == 1) {
             v.resize(2);
             v[0] = m;
             v[1] = it->second.front();
         } 
         else {
             v.resize(m*Int_t(TMath::Binomial(n,m))+1);
             v[0] = m;
             
             idx.resize(n);
             for (Int_t i(0); i<n; i++) idx[i] = i;
            
             Assign(1,m,idx,it->second,v);
             
             if (m < n) 
             {
                 l = n-m;
                 s = 1;
                 while (NextX(idx,l,m)) {
                    s += m;
                    Assign(s,m,idx,it->second,v);
                 }
             }
         }
         
         fVtx.insert(pdgv_(it->first,v));
    }
}

//_____________________________________________________________________________
Int_t SpdVertexCombiFinder::GetNVC() const 
{
    if (fInitStatus != 1) return 0;
    
    Double_t nvc = 1;
    Int_t n;
    for (auto p : fVtx) {
         n = (p.second.size()-1)/p.second[0] ;
         nvc *= n;
    }
    return Int_t(nvc);
}

//_________________________________________________________
Bool_t SpdVertexCombiFinder::NextX(std::vector<Int_t>& x, 
                                   const Int_t& l, const Int_t& m)
{  
    for (Int_t i = m-1; i >= 0; i--) {
         if (x[i] < l+i) {
             x[i]++;
             for (Int_t j = i+1; j < m; j++) x[j] = x[j-1] + 1;
             return true;
         }
    }
    return false;
}

//_________________________________________________________
Bool_t SpdVertexCombiFinder::NextY()
{  
    for (int i = fNP; i>=0; i--) {
         if (fIdx[i] < fVtxN[i]-1) {
             fIdx[i]++;
             return true;
         }
         else fIdx[i] = 0;
    }
    return false;
}

//_____________________________________________________________________________
Bool_t SpdVertexCombiFinder::Next(std::vector<Int_t>& vc)
{
    if (fInitStatus != 1) return false;
    
    if (fStartIter) 
    {
        fNVC = GetNVC();
        
        if (fNVC > SPDVCF_MAXNCOMBI) {
            cout << "-W- <SpdVertexCombiFinder::Next> Very large number of vertex candidates: "
                 << fNVC  << " (max = " << SPDVCF_MAXNCOMBI << ")" << endl;
            ClearIterator();
            return false;     
        }
    
        vc.resize(fNVertexParticles,0);
        
        fNP = fVtx.size();
        
        fIdx.resize(fNP,0);
        fVtxN.resize(fNP);
        
        fNP--;
        
        Int_t pos(0), i(0), m; 
        for (auto v: fVtx) {
             m = v.second[0];
             fVtxN[i++] = (v.second.size()-1)/m;
             Assign2(pos,m,1,v.second,vc);
             pos += m;
        }
        
        fStartIter = false;
        
        return true;
    }
    
    if (fNP < 0) return false;
    
    if (NextY()) {
        Int_t pos(0), i(0), m; 
        for (auto v: fVtx) {
             m = v.second[0];
             Assign2(pos,m,m*fIdx[i]+1,v.second,vc);
             pos += m;
             i++;
        }
        return true;
    }
    
    ClearIterator();
    return false;
}

//_____________________________________________________________________________
void SpdVertexCombiFinder::ClearIterator()
{
    fNVC =  1;  
    fNP  = -1;
    fIdx.clear();
    fVtxN.clear(); 
}

//_____________________________________________________________________________
void SpdVertexCombiFinder::PrintSettings() const
{
    cout << "\n\t<SpdVertexCombiFinder::PrintSettings> " << "\n\n";

    Int_t n, m;    
    cout << "particles: \n\n";
    
    n = 0;
    for (auto p : fParticles) {
         std::vector<Int_t> v = p.second;
         printf("[%3d] %10d : ",n++,p.first);   
         for (Int_t i(0); i < p.second.size(); i++) {
              printf(" (%3d) ",(p.second)[i]);   
         }
         printf("\n");
    }
    
    cout << "\nvertex: \n\n";
  
    n = 0;
    for (auto p : fVtx) {
         std::vector<Int_t> v = p.second;
         printf("[%3d] %10d : %d \n",n++,p.first,v[0]);   
    }
    printf("\n");
     
    cout << "vtx: \n\n";
    
    n = 0;
    for (auto p : fVtx) {
         std::vector<Int_t> v = p.second;
         printf("[%3d] %10d, %5d  :  ",n++,p.first,v[0]);   
         m = (v.size()-1)/v[0];
         for (Int_t i(0); i < m; i++) {
              printf("(");  
              for (Int_t j = i*v[0]; j < (i+1)*v[0]; j++) printf("%3d ",(p.second)[j+1]);  
              printf(") ");  
         }
         printf("\n");
    }
    printf("\n");
}

//_____________________________________________________________________________
void SpdVertexCombiFinder::PrintVertexCandidates() 
{
    cout << "\n\t<SpdVertexCombiFinder::PrintVertexCandidates> " << "\n";
    
    printf("\n-----------------------------------------------------\n");  
    printf("%5s : ","pdg");
    for (auto p : fVtx) {
         for (Int_t i(0); i<p.second[0]; i++) printf(" %8d",p.first);  
    }
    printf("\n-----------------------------------------------------\n");
    
    ResetIterator();
    
    printf("%5s   ","");
    for (Int_t i(0); i<fNVertexParticles; i++) printf(" %8s",Form(" [%d]",i));
    printf("\n");

    std::vector<Int_t> vc;
    Int_t n(0);
    
    printf("%5s : \n","id");
    while (Next(vc))
    {
        printf("%5d : ",n++);
        for (auto idx : vc) printf(" %8d",idx);  
        printf("\n");
    }
    printf("\n");
    
    ResetIterator();
}

