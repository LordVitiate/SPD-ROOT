// $Id$
// Author: artur   2018/05/07

//_____________________________________________________________________________
//
// SpdGeopathParser
//_____________________________________________________________________________

#include "SpdGeopathParser.h"

#include <iostream>

using std::cout;
using std::endl;
using std::vector;

ClassImp(SpdGeopathParser)

//_____________________________________________________________________________
SpdGeopathParser::SpdGeopathParser(Int_t size):fPathLevel(0)
{
   fPath.resize(size);
}
   
//_____________________________________________________________________________   
SpdGeopathParser::~SpdGeopathParser()
{
}

//_____________________________________________________________________________ 
void SpdGeopathParser::Clear()
{ 
   fPath.clear();
   fPathLevel = 0; 
}

//_____________________________________________________________________________ 
void SpdGeopathParser::Resize(Int_t size)
{
   fPath.resize(size);
   fPathLevel = 0;
}

//_____________________________________________________________________________
Int_t SpdGeopathParser::ParsePath(const TString& path) 
{
   fPathLevel = 0;
     
   TString spath(path), name;
   
   Int_t length = spath.Length();
  
   Int_t i1, i2(0), i(0), num(-1);
   
   i1 = spath.Index("/");
   if (i1 == length-1) i1 = -1;
 
   Bool_t end = kFALSE;
  
   while (!end) {
     
      i2 = spath.Index("/",i1+1);
      
      if (i2 < 0 || i2 == length-1) 
      {
          if (i2 < 0) i2 = length;
          end = kTRUE;
      }
      
      name = spath(i1+1,i2-i1-1);
      i1 = i2;
           
      i = name.Index("_");    
      fPath[fPathLevel].first = name(0,i);  
      
      name = name(i+1,name.Length());  
      fPath[fPathLevel].second = name.Atoi();
      
      //cout << "<SpdGeopathParser::ParsePath> " << path << " :: " << fPathLevel 
      //     << " | " << fPath[fPathLevel].first 
      //     << " | " << fPath[fPathLevel].second<< endl;
      
      fPathLevel++; 
   }
   
   //cout << "-I- <SpdGeopathParser::ParsePath> " << GetCurrentPathString() << " Geometry level: " << fPathLevel << endl;
   
   return fPathLevel;
}

//_____________________________________________________________________________
TString SpdGeopathParser::GetCurrentPathString() const
{
   if (fPathLevel == 0) return "";
   
   TString path;
   SPDGEOPATH::const_iterator it = fPath.begin();
   Int_t n(1);
   for (; it != fPath.end(); it++) {
        path += Form("/%s_%d",it->first.Data(),it->second);
        if (n == fPathLevel) break;
        n++;
   }
 
   return path;
}

//_____________________________________________________________________________
Int_t SpdGeopathParser::Num(Int_t i, Bool_t begin) const
{
   if (i < 1 || i > fPathLevel) return 0;
   return (begin) ? fPath[i-1].second : fPath[fPathLevel-i].second;
}

//_____________________________________________________________________________
Int_t SpdGeopathParser::VNum(Int_t i, Bool_t begin) const
{
   if (i < 1 || i > fPathLevel) return 0;
   TString vname = Name(i,begin);
     
   Int_t j = vname.Length()-1; 
   
   if (j <= 0) return 0; 
   if (!isdigit(vname[j])) return 0;
   while(!(--j < 0)) { if (!isdigit(vname[j])) break;}
   if (!(j < 0)) vname.Remove(0,j+1);
   
   Int_t l = vname.Atoi();
 
   return l;  
}
    
//_____________________________________________________________________________
TString SpdGeopathParser::Name(Int_t i, Bool_t begin) const
{
   if (i < 1 || i > fPathLevel) return "";
   return (begin) ? fPath[i-1].first : fPath[fPathLevel-i].first;
}

//_____________________________________________________________________________
TString SpdGeopathParser::Path(Int_t i) const
{
   if (i < 1 || i > fPathLevel) return "";
   
   TString path("");
   for (Int_t j(0); j<i; j++) {
        path += Form("/%s_%d",fPath[j].first.Data(),fPath[j].second);
   }
 
   return path;
}

//_____________________________________________________________________________   
void SpdGeopathParser::Print(Option_t*) const
{
    cout << "<SpdGeopathParser::Print>" << endl;
    
    cout << endl;
    cout << "path:       " << GetCurrentPathString() << endl;
    cout << "path level: " << fPathLevel << endl;
    cout << endl;
    
    for (Int_t i(1); i<=fPathLevel; i++) {
         cout << "i = " << i << "|  Path: "<< Path(i) << "  Name: " << Name(i) 
              << "  VNum: " << VNum(i) << "  Num: " << Num(i) << endl;
    }
    cout << endl;
}

//_____________________________________________________________________________   
TString SpdGeopathParser::GetVolumeName(TString path) // static
{
   if (path.IsWhitespace()) return path;
   Int_t i;
   i = path.Last('_');
   path.Remove(i);
   i = path.Last('/');
   path.Remove(0,i+1);
   return path;
}

