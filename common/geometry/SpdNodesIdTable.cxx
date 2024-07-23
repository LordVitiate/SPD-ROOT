// $Id$
// Author: artur   2018/11/29


//_____________________________________________________________________________
//
// SpdNodesIdTable
//_____________________________________________________________________________

#include "SpdNodesIdTable.h"

#include <iostream>
using std::cout;
using std::endl;

using std::map;
using std::string;
using std::pair;

ClassImp(SpdNodesIdTable)

//_____________________________________________________________________________
SpdNodesIdTable::SpdNodesIdTable() 
{
 
}

//_____________________________________________________________________________
SpdNodesIdTable::~SpdNodesIdTable() 
{
 
}

//_____________________________________________________________________________
void SpdNodesIdTable::Clear()
{
    fNodesTable.clear();
}

//_____________________________________________________________________________
Int_t SpdNodesIdTable::GetTableNodeId(const Char_t* node)
{
   map <string,Int_t>::const_iterator it = fNodesTable.find(node);
   
   if (it != fNodesTable.end()) return it->second;
   
   Int_t n = fNodesTable.size();
   fNodesTable.insert(pair<string,Int_t>(node,n));
   
   return n;
}

//_____________________________________________________________________________
TString SpdNodesIdTable::GetTableIdNode(Int_t id)
{
   return "unknown"; 
}

//_____________________________________________________________________________
Bool_t SpdNodesIdTable::FillNodesTableIn(TObjArray* nodes)
{
   if (!nodes) return kFALSE;
  
   Int_t size = fNodesTable.size();
   if (!fNodesTable.size()) return kTRUE;
   
   nodes->Expand(size);
  
   map <string,Int_t>::const_iterator it = fNodesTable.begin();
   for (; it != fNodesTable.end(); it++) {
        nodes->AddAt(new TObjString(it->first.c_str()),it->second);
   }
  
   return kTRUE;
}
 
//_____________________________________________________________________________ 
Bool_t SpdNodesIdTable::LoadNodesTableFrom(TObjArray* nodes)
{
   if (!nodes) return kFALSE;
   
   fNodesTable.clear();
   
   if (nodes->IsEmpty()) return kTRUE;
   
   Int_t size = nodes->GetEntriesFast();
   TObjString* node;
   
   for (Int_t i(0); i<size; i++) {
        node = (TObjString*)nodes->At(i);
        if (node) fNodesTable.insert(pair<string,Int_t>(node->GetName(),i));
   }
  
   return kTRUE;
}
