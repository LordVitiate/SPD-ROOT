void Standard();
void Custom();

void TestMapper();
void TestBuilder();

//______________________________________________________________
void ConstructIts() 
{
   // 
   //TestMapper();
   //return;
  
   SpdCommonGeoMapper::Instance()->OpenGeometry();
  
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   SpdCommonGeoMapper::Instance()->DefineQslGeometrySet();
   
   SpdPipe* Pipe = new SpdPipe();
   Pipe->ConstructGeometry();
   //Pipe->Print("");
   
   SpdItsGeoMapperX::Instance()->SetGeometryPars(3); //1, 2, (3 = default)
   
   // enable(1)/disable(0) endcaps; default = enable
   //SpdItsGeoMapperX::Instance()->EnableEndcaps(0);
   
   //Custom();
   
   SpdIts* vxdet = new SpdIts();
   vxdet->ConstructGeometry();
   
   /* ~~~~~~~~~~~~~ PRINT GEOMETRY INFO ~~~~~~~~~~~~~~~~~ */
   
   //SpdItsGeoMapperX::Instance()->Print("");           // print all parameters
   SpdItsGeoMapperX::Instance()->PrintVolPars(); 
   SpdItsGeoMapperX::Instance()->PrintGeoTable(); 
   
   
   TestBuilder();
   
   /* ~~~~~~~~~~~~~ DRAW GEOMETRY  ~~~~~~~~~~~~~~~~~ */
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(2);
  
   geoMan->GetMasterVolume()->Draw("ogl");
   
   //geoMan->GetMasterVolume()->Draw();
   
   //TGeoVolume* vol = SpdItsGeoBuilder::Instance()->GetVolume("ItsLayer1");
   //if (vol) vol->Draw("ogl");
   
   //TGeoVolume* vol = SpdItsGeoBuilder::Instance()->GetVolume("ItsLadder1");
   //if (vol) vol->Draw("ogl");
    
   //TGeoVolume* vol = SpdItsGeoBuilder::Instance()->GetVolume("ItsChip1");
   //if (vol) vol->Draw("ogl");
  
}

//______________________________________________________________
void TestBuilder() 
{   
   SpdItsGeoBuilder* builder = SpdItsGeoBuilder::Instance();
   //builder->Print("");
   
   SpdGeoMapper* mapper = builder->GetMapper();
   //mapper->Print("");
   
   cout << "GEOMETRY TYPE: " << mapper->GetGeoType() << endl;
   
   if (mapper->GetGeoType() != 3) return;
   
   TString path = "/cave_1/ItsLayer3_1/ItsLadder3_3/ItsChip1_58";
    
   Double_t* p  = new Double_t[3];
   Double_t* d  = new Double_t[3];
   Double_t* gp = new Double_t[3];
   
   //-------------------------------------------------------------------------------
   
   //--------------------------------------
   // set CHIP path only!
   //--------------------------------------
   
   if (!builder->SetNodePath(path)) return;  
   
   // get chip orientation
   builder->GetDetectorPosition(p);
   
   cout << "chip path: " << path << endl;
   
   printf("chip position (global): %12.8f %12.8f %12.8f\n",p[0],p[1],p[2]);
   
   builder->GetDetectorOrientation(d,'x');  
   
   printf("chip x-axis:   %12.8f %12.8f %12.8f\n",d[0],d[1],d[2]);
   
   builder->GetDetectorOrientation(d,'y');  
   
   printf("chip y-axis:   %12.8f %12.8f %12.8f\n",d[0],d[1],d[2]);
   
   builder->GetDetectorOrientation(d,'z');  
   
   printf("chip z-axis:   %12.8f %12.8f %12.8f\n",d[0],d[1],d[2]);
   
   // get chip corner position
   
   Double_t lphimax = 3.04; // -> x
   Double_t lzmax   = 3.04; // -> z

   p[0] = lphimax; p[1] = 0; p[2] = lzmax;
   builder->LocalToGlobalP(p,gp);
   printf("chip corner [x+,z+] (global): %12.8f %12.8f %12.8f\n",gp[0],gp[1],gp[2]);
   
   p[0] = lphimax; p[1] = 0; p[2] = -lzmax;
   builder->LocalToGlobalP(p,gp);
   printf("chip corner [x+,z-] (global): %12.8f %12.8f %12.8f\n",gp[0],gp[1],gp[2]);
   
   p[0] = -lphimax; p[1] = 0; p[2] = -lzmax;
   builder->LocalToGlobalP(p,gp);
   printf("chip corner [x-,z-] (global): %12.8f %12.8f %12.8f\n",gp[0],gp[1],gp[2]);
   
   p[0] = -lphimax; p[1] = 0; p[2] = lzmax;
   builder->LocalToGlobalP(p,gp);
   printf("chip corner [x-,z+] (global): %12.8f %12.8f %12.8f\n",gp[0],gp[1],gp[2]);
   
   //-------------------------------------------------------------------------------
   
   Int_t nz    = 319;  // 0, nz_max-1   = 320 cannels (total)
   Int_t nphi  = 619;  // 0, nphi_max-1 = 640 channel (total)
   
   Int_t vid = nz*640 + nphi; // channel id (vid)
   
   //--------------------------------------
   // set CHIP path and CHANNEL id 
   // CHANNEL spatial orientation is the same as for mother CHIP!
   //--------------------------------------
   
   builder->SetNodePath(path,vid);  
   
   // get chip
   SpdGeoVVolume* vvol = builder->GetNodeVVolume(); 
 
   if (!vvol) {
       cout << "\nno such vvolume: vid = " << vid << endl; 
       return;
   }
   
   // get channel z-,phi- numbers
   Int_t z_ch, phi_ch;
   ((SpdGeoVVolumeBox2D*)vvol)->CellId(vid,z_ch,phi_ch);
   
   cout << endl;
   
   cout << "vvolume: mother = " << vvol->GetMotherVolume() 
        << "  channel: (" << z_ch << " , "<< phi_ch << ")" << endl;
   
   cout << "------------------------------------------------------------------------------------\n";
   vvol->PrintVolume();
   cout << "------------------------------------------------------------------------------------\n";
   
   // get local position of the channel in the chip coordinate system
   vvol->GetCellPos(vid,p[0],p[1],p[2]);
   
   printf("channel position (chip c.s.):   %12.8f %12.8f %12.8f\n",p[0],p[1],p[2]);
  
   cout << endl;
    
   // get channel global position (using GetDetectorPosition method)
   builder->GetDetectorPosition(p);
   
   printf("channel position (global) [1]:  %12.8f %12.8f %12.8f\n",p[0],p[1],p[2]);
   
   // get channel global position (using LocalToGlobalP method)
   p[0] = 0; p[1] = 0; p[2] = 0;  // position of channel in the channel coordinate system
   builder->LocalToGlobalP(p,gp);  
   printf("channel position (global) [2]:  %12.8f %12.8f %12.8f\n",gp[0],gp[1],gp[2]);
   
   cout << endl;
}

//______________________________________________________________
void TestMapper() 
{
   SpdItsGeoMapperX* mapper = SpdItsGeoMapperX::Instance();
  
   mapper->InitGeometry(3);
   
   SpdParSet* params = new SpdParSet();
  
   mapper->FillParametersIn(params);
   
   params->print(1);

   SpdItsGeoMapperX::Instance()->PrintVolPars(); 
   SpdItsGeoMapperX::Instance()->PrintGeoTable(); 
  
}

//______________________________________________________________
void Standard()
{
   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   //                           BUILD VERTEX DETECTOR
   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  
}

//______________________________________________________________
void Custom()
{
   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   //                           BUILD  VERTEX DETECTOR
   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   
   SpdItsGeoMapperX* mapper = SpdItsGeoMapperX::Instance();
  
   // ----- case 1 ----- 
   //mapper->InitGeometry(1); 
   
   // ----- case 2 ----- 
   //SpdParameter* par;
   //par = SpdItsGeoMapperX::Instance()->AddParameter("ItsGeoType");
   //*par = 2; 
   
   // ----- case 3 ----- 
   //mapper->SetGeometryPars(3,4);
   
   // ----- case 4 ----- 
   //mapper->SetGeometryPars(3,4);
   //mapper->SetNLayers(3);
   
   // ----- case 5 ----- 
   mapper->SetGeometryPars(5,1);
     
}

