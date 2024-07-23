

void CheckGeoLoader()
{
   SpdGeoLoader* loader = new SpdGeoLoader();
   
   /* ---- special loader settings ---- */
   
   //SpdGeoLoader::ForceTopGeoFile("cave_precise.geo"); // force CAVE geometry (material = vacuum)
   //SpdGeoLoader::ForceTopGeoFile("cave.geo");         // force CAVE geometry (material = air)
   
   //SpdGeoLoader::ForceMediaFile(TString(getenv("VMCWORKDIR"))+"/geometry/media1.geo");    
   
   //SpdGeoLoader::ResetMediaFromParams(false); // actives: false = use basic module's materials, true (default) = use actual parameters 
   //SpdGeoLoader::ResetPassivesMedia(true);    // passives: false (default) = use actual simu materials, true = use deafult materials
   
   //SpdGeoLoader::UnsetMaterials(true);        // true = set ALL module's materials as vacuum, false (default) = geometry materials 
   
   /* ---- LoadGeometry: load list of modules and construct TOP (cave) volume ---- */
   
   loader->LoadGeometry("params.root");        // load all modules using its parameters, construct TOP volume
   //loader->LoadGeometry();                   // load all modules, construct TOP volume 

    return;
    
   /* ---- LoadModule: construct modules; parameters = [module id, unset materials (default = false)] ---- */
   
   // loader->LoadModule("All",false);
   // loader->LoadModule("Actives",false);
   
   //loader->LoadModule("Passives",false);
  
   // loader->LoadModule("Pipe",false);
   
//    loader->LoadModule("Inner tracker system",false); // id = 12
//    loader->LoadModule("TS barrel (tor)",false);      // id = 2
//    loader->LoadModule("TS endcaps (tor)",false);     // id = 3
    
   loader->LoadModule(2,false); 
   loader->LoadModule(3,false); 
   loader->LoadModule(12,false); 
   
   /* ---- darwing and dumping ---- */
  
   loader->DrawGeometry();
   
   //loader->PrintGeometry();
   loader->PrintActualGeometry();
   
   return;
   
   Int_t DetId = 12; 
   
   SpdParSet* pars = 0;
   SpdGeoMapper* mapper = 0;
   SpdGeoBuilder* builder = 0;
   
   pars    = loader->GetActiveParameters(DetId);
   mapper  = loader->GetMapper(DetId);
   builder = loader->GetBuilder(DetId);
      
   if (pars) pars->print(1);
   if (mapper)  cout << "MAPPER:  " << mapper->GetName() << endl;
   if (builder) cout << "BUILDER: " << builder->GetName() << endl;
   
   delete loader;
   
}
