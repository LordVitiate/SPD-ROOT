 
void check_geo_file(Int_t geolevel = 2) 
{
  TFile* fg = new TFile("geofile_full.root");
  
  if (!fg) return;
  
  TGeoManager *geoMan = (TGeoManager*)fg->Get("FAIRGeom");
  geoMan->SetVisLevel(geolevel);
  geoMan->GetMasterVolume()->Draw("ogl");
  gGeoManager = geoMan;

}