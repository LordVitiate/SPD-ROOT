// $Id$
// Author: artur   2013/08/15
// 
//_____________________________________________________________________________
//
// RWdata
//
//_____________________________________________________________________________

#include "RWdata.h"

#include <iostream>

using std::cout;
using std::endl;

//ClassImp(RWdata)

//_____________________________________________________________________________  
FILE* RWdata::WriteFile(FILE* fl) {
   // 
   if (!fl) {
       cout << "<RWdata::WriteFile> Warning. File not open for writing" << endl;
       return fl;
   }  
   Write(fl);
   return fl;
}

//_____________________________________________________________________________
FILE* RWdata::ReadFile(FILE* fl) {
   //         
   if (!fl) {
       cout << "<RWdata::ReadFile> Warning. File not open for reading" << endl;
       return fl;
   } 
   Read(fl);
   return fl; 
}

//_____________________________________________________________________________
FILE* RWdata::WOpenFile(TString path, TString fname, TString opt, int p_opt) {
   //  
   if (!path.EndsWith("/")) path += "/";
   fname = path + fname; 
   FILE* f = fopen(fname.Data(),opt.Data());
   if (!f) cout << "<RWdata::WOpenFile> Warning. No such file : " << fname << endl;
   else { 
       if (p_opt == 1) cout << "<RWdata::WOpenFile> Open[" << opt << "]: " 
                            << fname << " ... ok " << endl; 
   }
   return f;
}

//_____________________________________________________________________________
FILE* RWdata::ROpenFile(TString path, TString fname, TString opt, int p_opt) {
   //    
   if (!path.EndsWith("/")) path += "/";
   fname = path + fname;   
   FILE* f = fopen(fname.Data(),opt.Data());
   if (!f) cout << "<RWdata::ROpenFile> Warning. No such file : " << fname << endl;
   else { 
      if (p_opt == 1) cout << "<RWdata::ROpenFile> Open[" << opt << "]: " 
                           << fname  << " ... ok " << endl; 
   } 
   return f;
}   

//_____________________________________________________________________________
long RWdata::Save(TString path, TString fname, int p_opt) {
    //
    FILE* f = WOpenFile(path,fname);
    if (!f) return 0;
    f = WriteFile(f);
    if (f) { 
        long b = ftell(f); 
        fclose(f);
        if (p_opt == 1) cout << "<RWdata::Save> File " << fname << " : "<< path << endl; 
        return b;         
    }
    if (p_opt == 1) {
        cout << "<RWdata::Save> Warning. File " 
             << fname << " : "<< path << " is not saved" << endl; 
    }
    return 0;   
}

//_____________________________________________________________________________
long RWdata::Load(TString path, TString fname, int p_opt) {
    //
    FILE* f = ROpenFile(path,fname);
    if (!f) return 0;
    f = ReadFile(f);
    if (f) { 
        long b = ftell(f); 
        fclose(f); 
        if (p_opt == 1) cout << "<RWdata::Load> File " << fname << " : "<< path << endl; 
        return b;         
    }
    if (p_opt == 1) {
        cout << "<RWdata::Load> Warning. File " 
             << fname << " : "<< path << " is not loaded" << endl; 
    }
    return 0;  
}

