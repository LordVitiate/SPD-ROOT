// $Id$
// Author: artur   2018/04/04

//_____________________________________________________________________________
//
// SpdModulesMap
//_____________________________________________________________________________

#include "SpdModulesMap.h"

#include <iostream>

using std::cout;
using std::endl;

SpdModulesMap* SpdModulesMap::fInstance = 0;

ClassImp(SpdModulesMap)

//_____________________________________________________________________________
SpdModulesMap::SpdModulesMap():fMasterGeoLevel(0)
{
   fInstance = this;
}

//_____________________________________________________________________________
SpdModulesMap::~SpdModulesMap() 
{
   if (fInstance) delete fInstance; 
   fInstance = 0;
}

//_____________________________________________________________________________
void  SpdModulesMap::Clear() 
{ 
   fMasterGeoLevel = 0;
   fMasterVolName = "";
   
   fModules.clear(); 
}

//_____________________________________________________________________________
void SpdModulesMap::Init(Int_t level, TString volname) 
{ 
   fMasterGeoLevel = level; 
   fMasterVolName = volname; 
}

//_____________________________________________________________________________
void SpdModulesMap::Add(TString prefix, Int_t id) 
{ 
   fModules.insert(std::pair<TString,Int_t>(prefix,id));
}

//_____________________________________________________________________________
TString SpdModulesMap::GetFirstNodeName(TString path)
{
   Int_t i;
   
   for (Int_t j(0); j<GetMapGeoLevel(); j++) {
        i = path.Index("/");
        if (i < 0) return "top";  
        path.Remove(0,i+1);
   }
   
   i = path.Index("/");
   if (i < 0) return path;
   
   path.Remove(path.Index("/"),path.Length());
   return path;
}

//_____________________________________________________________________________
Int_t SpdModulesMap::SearchForId(TString node_path) const
{
   //cout << "<SpdModulesMap:::SearchForId> " << node_path << endl;
  
   if (GetGeoLevel(node_path) == fMasterGeoLevel &&
       node_path.Contains(fMasterVolName)) return kSpdTop;
  
   std::map<TString,Int_t>::const_iterator it = fModules.begin();
   for (; it != fModules.end(); it++) {
        if (node_path.Contains(it->first)) return it->second;
   }
  
   return kSpdUndefined; 
}

//_____________________________________________________________________________
void SpdModulesMap::Print() const
{
   cout << "<SpdModulesMap:::Print> " << endl;
   
   std::map<TString,Int_t>::const_iterator it = fModules.begin();
   for (; it != fModules.end(); it++) {
        printf("%5d  \"%s\" \n",it->second, it->first.Data());
   }
}



