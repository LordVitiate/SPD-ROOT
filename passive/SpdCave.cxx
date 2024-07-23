
#include "SpdCave.h"
#include "SpdGeoCave.h"       

#include "SpdParSetContFact.h"
#include "SpdPassiveGeoParSet.h"

#include "FairGeoMedia.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdCave)

//_____________________________________________________________________________________
SpdCave::SpdCave():FairModule()
{
  
}

//_____________________________________________________________________________________
SpdCave::SpdCave(const char* name, const char* title):FairModule(name,title)
{

}

//_____________________________________________________________________________________
void SpdCave::ConstructGeometry()
{
    FairGeoLoader* loader = FairGeoLoader::Instance();
    FairGeoInterface* GeoInterface = loader->getGeoInterface();
  
    SpdGeoCave* MGeo = new SpdGeoCave();
    MGeo->setGeomFile(GetGeometryFileName());
  
    GeoInterface->addGeoModule(MGeo);
  
    Bool_t rc = GeoInterface->readSet(MGeo);
    if (rc) MGeo->create(loader->getGeoBuilder());  
    
    FillParsIn(GetParameters());
    
    cout << "-I- <SpdCave::ConstructGeometry> Actual geometry: " << fgeoName << endl;
}

//_____________________________________________________________________________________
void SpdCave::SetGeometryFileName(TString fname, TString geoVer)
{
    if (fgeoName == "Not defined" || fgeoName.IsWhitespace()) {
        FairModule::SetGeometryFileName(fname);
        cout << "-I- <SpdCave::SetGeometryFileName> Geometry: " << fgeoName << endl;
        return;
    }
    
    if (fname == fgeoName) {
        cout << "-I- <SpdCave::SetGeometryFileName> Geometry has already been defined: " 
             << fgeoName << endl;
        return;
    }
        
    FairModule::SetGeometryFileName(fname);
    cout << "-I- <SpdCave::SetGeometryFileName> New geomery: " << fgeoName << endl;
}

//_____________________________________________________________________________________
TString SpdCave::GetGeometryFileName()
{
    SpdPassiveGeoParSet* pars = GetParameters();
    
    if (fgeoName != "Not defined" && !fgeoName.IsWhitespace()) {
    
        if (!pars) return fgeoName;
        
        TString s = fgeoName;
        if (s.Contains("/")) s.Remove(0,s.Last('/')+1);

        TString ss;
        if (pars->GetParameter("SpdCave/GeometryFileName",ss)) {
            if (s != ss) {
                cout << "-W- <SpdCave::GetGeometryFileName> Geometry is changed: " 
                     << ss << " -> " << s << endl;
            }
            else return fgeoName;
        }
        else {
            pars->SetParameter("SpdCave/GeometryFileName",s.Data()); // save user geometry
            pars->setChanged();
            pars->setInputVersion(FairRun::Instance()->GetRunId(),1);
        }

        cout << "-I- <SpdCave::GetGeometryFileName> Set user geometry: " << fgeoName << endl;
    }
    else {
        TString s;
        if (pars) {
            if (pars->GetParameter("SpdCave/GeometryFileName",s)) {
                cout << "-I- <SpdCave::GetGeometryFileName> Set geometry using parameters: " << s << endl;
            }
            else {
                s = "cave.geo";
                cout << "-W- <SpdCave::GetGeometryFileName> Set default geometry: " << s << endl;
                pars->SetParameter("SpdCave/GeometryFileName",s.Data()); // save default geometry
                pars->setChanged();
                pars->setInputVersion(FairRun::Instance()->GetRunId(),1);
            }
        }
        else {
            s = "cave.geo";
            cout << "-W- <SpdCave::GetGeometryFileName> Set default geometry (*): " << s << endl;
        }
        SetGeometryFileName(s);
    }
    
    return fgeoName;
}

//_____________________________________________________________________________________
Bool_t SpdCave::FillParsIn(SpdPassiveGeoParSet* params)
{
    if (!params) return kFALSE;
    
    FairGeoLoader* loader = FairGeoLoader::Instance();
    FairGeoInterface* GeoInterface = loader->getGeoInterface();
    
    TString s = GeoInterface->getMedia()->getInputFile();
    if (s.Contains("/")) s.Remove(0,s.Last('/')+1);
    
    TString ss;
    if (params->GetParameter("Global/MediaFileName",ss)) {
        if (s != ss) {
            cout << "-W- <SpdCave::FillParsIn> Media file is changed: " 
                 << ss << " -> " << s << endl;
        }
    }
    else {
        params->SetParameter("Global/MediaFileName",s.Data());
    }
    
    return kTRUE;
}

//_____________________________________________________________________________________
Bool_t SpdCave::LoadParsFrom(SpdPassiveGeoParSet* params)
{
    if (!params) return kFALSE;
    
    TString s;
    if (!params->GetParameter("SpdCave/GeometryFileName",s)) {
        cout << "-W- <SpdCave::LoadParsFrom> Geometry not defined: " << endl; 
        return kFALSE;
    }
    
    SetGeometryFileName(s);
    
    cout << "-I- <SpdCave::LoadParsFrom> Load geometry: " << fgeoName << endl;

    return kTRUE;
}

//_____________________________________________________________________________________
SpdPassiveGeoParSet* SpdCave::GetParameters()
{
    FairRun* run = FairRun::Instance();
    if (!run) return 0;
    FairRuntimeDb* rtdb = run->GetRuntimeDb(); 
    if (!rtdb) return 0;
    SpdPassiveGeoParSet* pars = (SpdPassiveGeoParSet*)(rtdb->getContainer("PassiveGeoParSet"));
    return pars;
}


