// $Id$
// Author: artur   2018/05/07


//_____________________________________________________________________________
//
// SpdGeoTable
//_____________________________________________________________________________

#include "SpdGeoTable.h"

#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;
using std::vector;

ClassImp(SpdGeoTableRec)
ClassImp(SpdGeoTable)

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// SpdGeoTableRec                                                            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdGeoTableRec::SpdGeoTableRec():fFirstNodeNum(0),fNNodes(0),fFirstNodeTableNum(0)
{
}

//_____________________________________________________________________________
SpdGeoTableRec::SpdGeoTableRec(TString volname, TString mothername, 
                               Int_t nodenum, Int_t nnodes):
fVolName(volname),fMotherVolName(mothername),                               
fFirstNodeNum(nodenum),fNNodes(nnodes),fFirstNodeTableNum(0)
{
}
   
//_____________________________________________________________________________   
SpdGeoTableRec::~SpdGeoTableRec()
{
}

//_____________________________________________________________________________   
void SpdGeoTableRec::Clear()
{
   fVolName = "";
   fMotherVolName = "";
   fFirstNodeNum = 0;
   fNNodes = 0;
   fFirstNodeTableNum = 0;
}

//_____________________________________________________________________________  
void SpdGeoTableRec::Prn(Int_t n, Int_t i, Int_t j, Bool_t endn) const
{
   printf("%3d %3d %3d %5d [%5d %5d]   %-20s %-20s [%6d %6d]", 
          n, i, j, fNNodes, GetFirstNodeNum() , GetLastNodeNum(), 
          fVolName.Data(), fMotherVolName.Data(), 
          GetFirstNodeTableNum(), GetLastNodeTableNum()
          );
   if (endn) printf("\n");
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// SpdGeoTable                                                               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdGeoTable::SpdGeoTable():/*fPathLevel(0),*/fTableDims(0)
{
    fTable = new TObjArray();
}
   
//_____________________________________________________________________________   
SpdGeoTable::~SpdGeoTable()
{
    if (fTable) { 
        fTable->Delete();
        delete fTable; 
    }
    
    if (fTableDims) delete [] fTableDims;
}

//_____________________________________________________________________________ 
void SpdGeoTable::Clear()
{  
    SetMaxGeoLevel(0);
    
    if (fTable) { 
        fTable->Delete();
        delete fTable; 
        fTable = 0;
    }
   
    SpdGeopathParser::Clear();
    
    if (fTableDims) { delete [] fTableDims; fTableDims = 0; }
    
    fTableVolumes.clear();
    fTableGlobalNums.clear();
}

//_____________________________________________________________________________ 
void SpdGeoTable::CloseTable()
{ 
    Int_t maxlevel = GetMaxGeoLevel();
    
    if (maxlevel == 0) {
        Clear();
        return;
    }
    
    Resize(maxlevel+1);
    
    if (fTableDims) {
        delete [] fTableDims; 
	fTableDims = 0;
    }
    
    SPDGEOTABLE_TVOLUMES::iterator itm;
    
    SpdGeoTableRec* rec;
    TString volname;
    
    for (Int_t i(0); i<fTable->GetEntriesFast(); i++) {
         
         rec = (SpdGeoTableRec*)fTable->At(i); 
         if (!rec) continue;

         volname = rec->GetVolName();
         itm = fTableVolumes.find(volname);

         if (itm == fTableVolumes.end()) {
            std::map<Int_t, SpdGeoTableRec*> nrec;
            nrec.insert(std::pair<Int_t, SpdGeoTableRec*>(rec->GetLastNodeNum(),rec));
            fTableVolumes.insert(std::pair<TString, std::map<Int_t, SpdGeoTableRec*> >(volname,nrec));
         }
         else { 
            itm->second.insert(std::pair<Int_t, SpdGeoTableRec*>(rec->GetLastNodeNum(),rec));
         }
    }
    
    if (fTableGlobalNums.size() > 0) fTableGlobalNums.clear();
    
    Int_t nl = GetMaxGeoLevel();
    fTableGlobalNums.resize(nl);
   
    Int_t m;
    for (Int_t i(0); i<nl; i++) {
         m = fIdTable[i].size();
	 fTableGlobalNums[i].resize(m);
         for (Int_t j(0); j<m; j++) {
              rec = (SpdGeoTableRec*)fTable->At(fIdTable[i][j]);
	      fTableGlobalNums[i][j] = rec->GetLastNodeTableNum();
         }
    }
    
    fTableDims = new Long_t[maxlevel];

    Int_t k = maxlevel-1;
    fTableDims[k] = 1;
    
    for (Int_t i(1); k>0; i++, k--) {
         m = fTableGlobalNums[maxlevel-i].size();
         fTableDims[k-1] = fTableDims[k]*(fTableGlobalNums[maxlevel-i][m-1] + 1);
         
         if (fTableDims[k-1] < 0) {
             cout << "-F- <SpdGeoTable::CloseTable> Error. Very large size of the table" << endl;
             exit(1);
         }         
    }
    
    m =  fTableGlobalNums[0].size();  
    Long_t max_table_num = fTableDims[0]*(fTableGlobalNums[0][m-1]+1);
    
    if (max_table_num < 0) {
        cout << "-F- <SpdGeoTable::CloseTable> Error. Very large size of the table" << endl;
        exit(1);
    }
    
    cout << "-I- <SpdGeoTable::CloseTable> Max. table node uid: " << max_table_num-1 << endl;
   
//     k = maxlevel-1;          
//     for (Int_t i(1); k>0; i++, k--) {
//          m = fTableGlobalNums[maxlevel-i].size();
//          printf("Dimensions [%3d],[%3d]:  [%20li, %20li]   mult = %d\n",
//                 k, k-1, fTableDims[k], fTableDims[k-1], fTableGlobalNums[maxlevel-i][m-1]+1);
//     }
//     m =  fTableGlobalNums[0].size();
//     printf("Dimensions [%3d],[%3s]:  [%20li, %20li]   mult = %d\n",
//             0, "x", fTableDims[0], max_table_num, 
//             fTableGlobalNums[0][m-1]+1);
    
    cout << "-I- <SpdGeoTable::CloseTable> Register volumes: " << fTableVolumes.size() << endl;
}

//_____________________________________________________________________________  
void SpdGeoTable::SetMaxGeoLevel(int n)
{
    if (n < 1 && fTable) {
        fTable->Delete();
        delete fTable;  
        fTable = new TObjArray();
        fIdTable.clear();
        return;
    }
    
    if (fTable && !fTable->IsEmpty()) {
        delete fTable; 
        fTable = new TObjArray();
    }
    
    fIdTable.clear();
    fIdTable.resize(n);
}

//_____________________________________________________________________________  
Int_t SpdGeoTable::AddTableRec(Int_t geolevel, TString volname, TString mothername, 
                               Int_t nodenum, Int_t nnodes)
{
   if (!fTable) fTable = new TObjArray();
   
   if (geolevel+1 > fIdTable.size()) {
       fIdTable.resize(geolevel+1);
   }
    
   SpdGeoTableRec* rec = new SpdGeoTableRec(volname,mothername,nodenum,nnodes);
   fTable->Add(rec);
  
   vector<int>& level = fIdTable[geolevel];
   Int_t n = level.size();
   if (n == 0) {
       rec->fFirstNodeTableNum = 1;
       level.push_back(fTable->GetLast());
   }
   else {
       SpdGeoTableRec* rec1 = (SpdGeoTableRec*)fTable->At(level[n-1]);
       rec->fFirstNodeTableNum = rec1->GetLastNodeTableNum()+1;
       level.push_back(fTable->GetLast());
   }
  return 0;
}

//_____________________________________________________________________________   
const SpdGeoTableRec* SpdGeoTable::GetRec(Int_t num) const
{
   if (fTable->GetSize() >= num || num < 0) return 0;
   return (SpdGeoTableRec*)fTable->At(num);
}

//_____________________________________________________________________________   
const SpdGeoTableRec* SpdGeoTable::GetRec(Int_t level, Int_t id) const
{
   if (level >= fIdTable.size()) return 0;
   if (id >= fIdTable[level].size()) return 0;
   return (SpdGeoTableRec*)fTable->At(fIdTable[level][id]);
}

//_____________________________________________________________________________   
Long_t SpdGeoTable::GetNodeTableNum(const TString& geopath) 
{
   if (!geopath.BeginsWith("/")) return 0;
  
   if (!fTableDims) CloseTable();
   
   if (!fTableDims) return 0;
   
   if (ParsePath(geopath) < 1) return 0;
   
   SPDGEOTABLE_TVOLUMES::const_iterator it;
   
   const SpdGeoTableRec* xrec = 0;
   const SpdGeoTableRec* rec;
   
   std::map<Int_t, SpdGeoTableRec*>::const_iterator itx;
   
   Long_t tid = 0;
   Long_t nnum, glob_num;
   
   for (Int_t i(fPathLevel-1); i > 0; i--) {
     
        it = fTableVolumes.find(fPath[i].first);
        if (it == fTableVolumes.end()) {
            fPathLevel = 0;
            return 0;
        }

        const std::map<Int_t, SpdGeoTableRec*>& mrec = it->second;

        rec = 0;
        nnum = fPath[i].second;
        itx = mrec.begin();

        do {
           if (nnum <= itx->first) {
              rec = itx->second;
              break;
           }
        } 
        while (++itx != mrec.end());

        if (!rec) return 0;

        if (xrec) {
            if (xrec->GetMotherVolName() != rec->GetVolName()) {
               fPathLevel = 0;
               return 0;
            }
        }

        glob_num = rec->GetGlobalNodeNum(nnum);
        tid += fTableDims[i-1]*glob_num;
        xrec = rec;
        
        //rec->Prn(nnum,glob_num,rec->GetLocalNodeNum(glob_num));
   }
  
   //cout << "<SpdGeoTable::GetNodeTableNum> TID: " << tid << " PATH: " << geopath << endl;
      
   return tid;
}

//_____________________________________________________________________________
TString SpdGeoTable::GetNodeGeoPath(Long_t detTID) 
{  
   if (detTID < 1) return "MASTER";
     
   if (!fTableDims) CloseTable();
   
   if (!fTableDims) return "MASTER";
   
   fPathLevel = 0;
   
   Int_t nl = GetMaxGeoLevel();
  
   std::vector<Int_t>::const_iterator it;
   
   const SpdGeoTableRec* xrec = 0;
   const SpdGeoTableRec* rec;
    
   Long_t sumn(0);
   Long_t n, loc_num, glob_num;
    
   for (Int_t i(0); i<nl; i++) {
    
        if (detTID <= sumn) break;
        
        n = Long_t((detTID - sumn)/fTableDims[i]);
        
        if (n < 1) {
            fPathLevel = 0;
            return "MASTER";
        }
        
        sumn += n*fTableDims[i];
        glob_num = n;

        it = std::lower_bound(fTableGlobalNums[i].begin(),fTableGlobalNums[i].end(),glob_num);
        n = it - fTableGlobalNums[i].begin();

        rec = GetRec(i,n);
        
        if (!rec) {
            fPathLevel = 0;
            return "MASTER";
        }
        
        if (xrec) {
            if (xrec->GetVolName() != rec->GetMotherVolName()) {
                fPathLevel = 0;
                return "MASTER";
            }
        }

        loc_num = rec->GetLocalNodeNum(glob_num);

        if (i == 0) {
            fPath[i].first = rec->GetMotherVolName();
            fPath[i].second = 1;
        }

        fPath[i+1].first = rec->GetVolName();
        fPath[i+1].second = loc_num;

        fPathLevel++;
        xrec = rec;
              
        //rec->Prn(rec->GetLocalNodeNum(glob_num),glob_num,loc_num);
   }
 
   fPathLevel++;
 
   TString geopath = GetCurrentPathString();
  
   //cout << "-I- <SpdGeoTable::GetNodeGeoPath> TID: " << detTID << " PATH: " << geopath << " Level: " << fPathLevel << endl;
   //cout << "-I- <SpdGeoTable::GetNodeGeoPath> TID: " << GetNodeTableNum(geopath) << endl;
 
   return geopath;
}

//_____________________________________________________________________________
TString SpdGeoTable::GetNodeVolumeName(Long_t detTID) 
{  
   return (GetNodeGeoPath(detTID) != "MASTER") ? fPath[GetMaxGeoLevel()].first : "";
}

//_____________________________________________________________________________   
void SpdGeoTable::PrintTable(Int_t level, Bool_t prnvols) const
{
    Int_t ntable = (fTable) ? fTable->GetEntries() : 0;
    
    cout << "<SpdGeoTable::PrintTable> Size: " << ntable
         << "; Max. GeoLevel: " << GetMaxGeoLevel() << endl;
         
    if (ntable < 1) return;
    
    TString divider('-',91);
    
    Int_t nr(fIdTable.size()), m;
    SpdGeoTableRec* rec;
    
    cout << divider << endl;
    
    if (fTable && !fTable->IsEmpty()) {
        for (Int_t i(0); i<nr; i++) {
              m = fIdTable[i].size();
              printf("level : %3d,  records:  %-12d\n",i+1,m);
        }
    }
    
    cout << divider << endl;
    printf("%3s %3s %3s %5s [%5s %5s]   %-20s %-20s [%6s %6s]\n", 
          "I", "L", "LI", "NN", "NF(I)", "NL(I)", "Name", "Mother", "NF(L)", "NL(L)");
     
    cout << divider << endl;
    
    Int_t n = 0;
    
    if (fTable && !fTable->IsEmpty()) {
        for (Int_t i(0); i<nr; i++) {
             if (level > 0 && i >= level) break;
             m = fIdTable[i].size();
             for (Int_t j(0); j<m; j++) {
                  rec = (SpdGeoTableRec*)fTable->At(fIdTable[i][j]);
                  rec->Prn(n,i,j);
	          n++;
             }
             if (m > 0) cout << divider << endl;
        }
    }
    
    if (prnvols && !fTableVolumes.empty()) {
      
        cout << divider << endl; 
      
        cout << "<SpdGeoTable::Print> Volumes/Nodes: " << endl;
        cout << divider << endl;
    
        printf(" %-5s %-20s : %-20s\n\n","I","Volume","REC_MAX_NODE_NUMS");
          
        SPDGEOTABLE_TVOLUMES::const_iterator itm = fTableVolumes.begin();
        std::map<Int_t, SpdGeoTableRec*>::const_iterator it;
        
        n = 1;
        for (; itm != fTableVolumes.end(); ++itm) {
             it = itm->second.begin();
             printf(" %-5d %-20s : ", n++, itm->first.Data());
             for (; it != itm->second.end(); ++it) {
                 printf("%3d, ",it->first);
             }
             printf("\n");
        }
        
        cout << divider << endl;
    }
    
//    printf("\n%6s %6s %6s\n\n","Level","NRec","NNodes");
//    for (Int_t i(0); i<GetMaxGeoLevel(); i++) {
//         m = fTableGlobalNums[i].size();
// 	   n = fTableGlobalNums[i][m-1];
// 	   printf("%6d %6d %6d\n",i,m,n);
//    }
//    cout << divider << endl;  
    
}


