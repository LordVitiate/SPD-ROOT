
#include "SpdPipe.h"
#include "SpdCommonGeoMapper.h"
#include "SpdPassiveGeoParSet.h"

#include <TGeoPcon.h>
#include <TGeoCone.h>
#include <TGeoManager.h>
#include <TMath.h>

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdPipe)

    //_____________________________________________________________________________________
    SpdPipe::SpdPipe() : SpdPassiveModule("Pipe", "SPDPIPE", kSpdPipe), fPipe(0),
                         fHybPipeC(0), fHybPipeLZminus(0), fHybPipeLZplus(0), fHybPipeISpace(0)
{
    SetDefault(SpdCommonGeoMapper::GetPipeDefGeoType());
}

//_____________________________________________________________________________________
SpdPipe::SpdPipe(const char *Name, const char *Prefix) : SpdPassiveModule(Name, Prefix, kSpdPipe), fPipe(0),
                                                         fHybPipeC(0), fHybPipeLZminus(0), fHybPipeLZplus(0), fHybPipeISpace(0)
{
    SetDefault(SpdCommonGeoMapper::GetPipeDefGeoType());
}

//_____________________________________________________________________________________
void SpdPipe::SetPipeType(TString type)
{
    if (type == "gen" || type == "general")
        SetDefault(1);
    else if (type == "tor" || type == "toroidal")
        SetDefault(2);
    else if (type == "sol" || type == "solenoidal")
        SetDefault(3);
    else if (type == "hyb" || type == "hybrid")
        SetDefault(4);
    else if (type == "sol2")
        SetDefault(5);
    else
    {
        cout << "-W- <SpdPipe::SetPipeType> Unknown type: " << type << endl;
        SetDefault(0);
    }
}

//_____________________________________________________________________________________
void SpdPipe::ConstructGeometry()
{
    if (fGeoType < 1 || fGeoType > 5)
        return;

    if (fPipe)
    {
        cout << "-W- <SpdPipe::ConstructGeometry>  Geometry is already built " << endl;
        return;
    }

    fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();

    if (!fMasterVolume)
    {
        cout << "-E- <SpdPipe::ConstructGeometry> No MASTER volume " << endl;
        return;
    }

    if (fPipeType == "hybrid")
    {

        BuildHybridPipe();

        // fMasterVolume->AddNode(fPipe,1);

        fMasterVolume->AddNode(fHybPipeC, 1);
        fMasterVolume->AddNode(fHybPipeLZminus, 1);
        fMasterVolume->AddNode(fHybPipeLZplus, 1);
        fMasterVolume->AddNode(fHybPipeISpace, 1);
    }
    else if (fPipeType == "sol2")
    {

       
        BuildPipe();

        // fMasterVolume->AddNode(fPipe,1);

        // fMasterVolume->AddNode(fHybPipeC, 1);
        //  fMasterVolume->AddNode(fHybPipeLZminus, 1);
        //  fMasterVolume->AddNode(fHybPipeLZplus, 1);
        //   fMasterVolume->AddNode(fHybPipeISpace, 1);
    }
    else
    {
        TGeoMedium *medium = FindMedium(fMaterial[0], "beryllium");

        if (!medium)
        {
            cout << "-E- <SpdPipe::ConstructGeometry> "
                 << "Undefined PIPE material: " << fMaterial[0] << endl;
            return;
        }

        TGeoCone *shape = new TGeoCone(fHalfLength, fR - fThickness, fR, fR - fThickness, fR);
        fPipe = new TGeoVolume(fPipeName, shape, medium);

        fMasterVolume->AddNode(fPipe, 1);
    }
    FillParsIn(GetParameters());
}

//_____________________________________________________________________________________
void SpdPipe::BuildHybridPipe()
{
    cout << "-I- <SpdPipe::BuildHybridPipe> " << endl;

    TGeoMedium *medium = FindMedium(fBaseMaterial, "air");

    if (!medium)
    {
        cout << "-E- <SpdPipe::ConstructGeometry> "
             << "Undefined PIPE base material: " << fBaseMaterial << endl;
        return;
    }

    TGeoMedium *cmedium = FindMedium(fMaterial[0], "air");

    if (!cmedium)
    {
        cout << "-E- <SpdPipe::ConstructGeometry> "
             << "Undefined PIPE central part material: " << fMaterial[0] << endl;
        return;
    }

    TGeoMedium *lmedium = FindMedium(fMaterial[1], "air");

    if (!lmedium)
    {
        cout << "-E- <SpdPipe::ConstructGeometry> "
             << "Undefined PIPE lateral part material: " << fMaterial[1] << endl;
        return;
    }

    const Double_t dl = 200.; // cm

    // create main pipe tube

    // TGeoCone* shape = new TGeoCone( 920./2 + dl, 0, 13.0/2, 0, 13.0/2); // cm
    // fPipe = new TGeoVolume(fPipeName, shape, medium);

    fPipe = new TGeoVolumeAssembly(fPipeName);

    // create central pipe tube

    const Int_t cnsec = 2;

    Double_t cpars[cnsec][3] = // cnsec * (z, rmin, rmax) [cm]
        {
            -170. / 2, 6.4 / 2 - 0.1, 6.4 / 2,
            170. / 2, 6.4 / 2 - 0.1, 6.4 / 2};

    TGeoPcon *cshape = new TGeoPcon(0, 360, cnsec);

    for (Int_t i(0); i < cnsec; i++)
    {
        cshape->DefineSection(i, cpars[i][0], cpars[i][1], cpars[i][2]);
    }

    // cshape->DefineSection(0, -170./2, 6.4/2-0.1, 6.4/2 ); // cm
    // cshape->DefineSection(1,  170./2, 6.4/2-0.1, 6.4/2 ); // cm

    fHybPipeC = new TGeoVolume(FullName("central", 1), cshape, cmedium);

    // define lateral tube parameters

    const Int_t lnsec = 5;

    Double_t a = 1. / TMath::Tan(TMath::DegToRad() * 10. / 2);

    Double_t l1 = a * (8.3 - 6.4) / 2;
    Double_t l2 = a * (13.0 - 8.3) / 2;

    // cout << "l1,2 = " << l1 << " " << l2 << endl;

    Double_t npars[lnsec][3] = // lnsec * (z, rmin, rmax) [cm]
        {
            -375. - 170. / 2 - dl, 13.0 / 2 - 0.15, 13.0 / 2,
            -165. - 170. / 2 - l2, 13.0 / 2 - 0.15, 13.0 / 2,
            -165. - 170. / 2, 8.3 / 2 - 0.15, 8.3 / 2,
            -170. / 2 - l1, 8.3 / 2 - 0.15, 8.3 / 2,
            -170. / 2, 6.4 / 2 - 0.15, 6.4 / 2};

    // cout << endl;
    // for (Int_t i(0); i<lnsec; i++) {
    //      printf("%3d  %12.6f  %12.6f  %12.6f\n",i,npars[i][0],npars[i][1],npars[i][2]);
    // }
    // cout << endl;

    // create lateral (z-) pipe tube

    TGeoPcon *lshapeminus = new TGeoPcon(0, 360, lnsec);
    TGeoPcon *lshapeplus = new TGeoPcon(0, 360, lnsec);

    for (Int_t i(0); i < lnsec; i++)
    {
        lshapeminus->DefineSection(i, npars[i][0], npars[i][1], npars[i][2]);
        lshapeplus->DefineSection(i, -npars[i][0], npars[i][1], npars[i][2]);
    }

    TGeoPcon *inspacespace = new TGeoPcon(0, 360, 2 * lnsec + cnsec);

    Int_t nn = 0;
    for (Int_t i(0); i < lnsec; i++)
        inspacespace->DefineSection(nn++, npars[i][0], 0, npars[i][1]);
    for (Int_t i(0); i < cnsec; i++)
        inspacespace->DefineSection(nn++, cpars[i][0], 0, cpars[i][1]);
    for (Int_t i(lnsec - 1); i > -1; i--)
        inspacespace->DefineSection(nn++, -npars[i][0], 0, npars[i][1]);

    fHybPipeLZminus = new TGeoVolume(FullName("zminus", 1), lshapeminus, lmedium);
    fHybPipeLZplus = new TGeoVolume(FullName("zplus", 1), lshapeplus, lmedium);

    fHybPipeISpace = new TGeoVolume(FullName("ISPACE", 1), inspacespace, medium);

    // add nodes to main tube

    fPipe->AddNode(fHybPipeC, 1);
    fPipe->AddNode(fHybPipeLZminus, 1);
    fPipe->AddNode(fHybPipeLZplus, 1);
    fPipe->AddNode(fHybPipeISpace, 1);

    fHybPipeISpace->SetFillColor(kBlue + 3);
    fHybPipeISpace->SetTransparency(90.);

    fHybPipeC->SetFillColor(kRed + 3);
    fHybPipeC->SetTransparency(80.);
}

//_____________________________________________________________________________________
void SpdPipe::BuildPipe()
{
    cout << "-I- <SpdPipe::BuildPipe> " << endl;

    // *** material ***
    TGeoMedium *vacuum_medium = FindMedium(fBaseMaterial, "air");

    //    TGeoMedium *beryllium_medium = FindMedium(fMaterial[0], "air");
   TGeoMedium *center_medium = FindMedium(fMaterial[0], "air");
   TGeoMedium *aluminium_medium = FindMedium(fMaterial[1], "air");

   // ***  parameters ***
   Double_t center_r = 6.4 / 2.0;   // [cm]
   Double_t center_thickness = 0.1; // [cm]
   Double_t center_length = 179.0;  // [cm]

   const int Nsectors = 2;
   Double_t lateral_r[Nsectors] = {8.3 / 2., 12.8 / 2.}; // [cm]
   Double_t lateral_thickness[Nsectors] = {0.15, 0.2};   // [cm]
   Double_t lateral_length[Nsectors] = {164.0, 195.5};   // [cm]
   Double_t lateral_cone_length = 5.31;                  // [cm]

   Double_t lateral_knob_r = 13.0 / 2;       // [cm]
   Double_t lateral_knob_thickness = 4.7;    // [cm]
   Double_t lateral_knob_length = 4.0;       // [cm]
   Double_t lateral_knob_position_z = 246.6; // [cm]

   // *** create volume ***
   // ======= CENTER ========
   TGeoVolume *center_pipe = gGeoManager->MakeTube("center_pipe",
                                                   center_medium,
                                                   center_r - center_thickness,
                                                   center_r,
                                                   center_length / 2.);

   TGeoVolume *vacuum_center_pipe = gGeoManager->MakeTube("vacuum_center_pipe",
                                                          vacuum_medium,
                                                          0,
                                                          center_r - center_thickness,
                                                          center_length / 2.);

   // ======= LATERAL ========
   TGeoVolume *lateral_pipe[4];
   TGeoVolume *vacuum_lateral_pipe[3];

   // --- sector 1
   lateral_pipe[0] = gGeoManager->MakeTube("lateral_pipe_sector1",
                                           aluminium_medium,
                                           lateral_r[0] - lateral_thickness[0],
                                           lateral_r[0],
                                           lateral_length[0] / 2.);

   vacuum_lateral_pipe[0] = gGeoManager->MakeTube("vacuum_lateral_pipe_sector1",
                                                  vacuum_medium,
                                                  0,
                                                  lateral_r[0] - lateral_thickness[0],
                                                  lateral_length[0] / 2.);

   // --- sector 1 cone
   lateral_pipe[1] = gGeoManager->MakeCone("lateral_pipe_sector1_cone",
                                           aluminium_medium,
                                           lateral_cone_length / 2.,
                                           lateral_r[0] - lateral_thickness[0], lateral_r[0],
                                           lateral_r[1] - lateral_thickness[1], lateral_r[1]);

   vacuum_lateral_pipe[1] = gGeoManager->MakeCone("vacuum_lateral_pipe_sector1_cone",
                                                  vacuum_medium,
                                                  lateral_cone_length / 2.,
                                                  0., lateral_r[0] - lateral_thickness[0],
                                                  0., lateral_r[1] - lateral_thickness[1]);

   // --- sector 2
   lateral_pipe[2] = gGeoManager->MakeTube("lateral_pipe_sector2",
                                           aluminium_medium,
                                           lateral_r[1] - lateral_thickness[1],
                                           lateral_r[1],
                                           lateral_length[1] / 2.);

   vacuum_lateral_pipe[2] = gGeoManager->MakeTube("vacuum_lateral_pipe_sector2",
                                                  vacuum_medium,
                                                  0.,
                                                  lateral_r[1] - lateral_thickness[1],
                                                  lateral_length[1] / 2.);

   // --- sector 1 knob
   lateral_pipe[3] = gGeoManager->MakeTube("lateral_pipe_knob_sector1",
                                           aluminium_medium,
                                           lateral_r[0],
                                           lateral_knob_r,
                                           lateral_knob_length / 2.);

   // *** place in space ***

   // --------------------------------------------------------------------
   // --- CENTER
   Double_t zcenter = 0;
   fMasterVolume->AddNode(center_pipe, 1, new TGeoTranslation(0, 0, zcenter));
   fMasterVolume->AddNode(vacuum_center_pipe, 2, new TGeoTranslation(0, 0, zcenter));

   // --- LATERAL
   // == sector 1 ==
   zcenter = (center_length / 2. + lateral_length[0] / 2.);
   // plus
   fMasterVolume->AddNode(lateral_pipe[0], 3, new TGeoTranslation(0, 0, zcenter));
   fMasterVolume->AddNode(vacuum_lateral_pipe[0], 4, new TGeoTranslation(0, 0, zcenter));
   // minus
   fMasterVolume->AddNode(lateral_pipe[0], 5, new TGeoTranslation(0, 0, -zcenter));
   fMasterVolume->AddNode(vacuum_lateral_pipe[0], 6, new TGeoTranslation(0, 0, -zcenter));

   // =cone=
   TGeoRotation rot;
   TGeoTranslation trans;

   zcenter = center_length / 2. + lateral_length[0] + lateral_cone_length / 2.;
   trans.SetTranslation(0, 0, zcenter);
   // plus
   fMasterVolume->AddNode(lateral_pipe[1], 7, new TGeoCombiTrans(trans, rot));
   fMasterVolume->AddNode(vacuum_lateral_pipe[1], 8, new TGeoCombiTrans(trans, rot));

   trans.SetTranslation(0, 0, -zcenter);
   rot.RotateX(180);
   // minus
   fMasterVolume->AddNode(lateral_pipe[1], 9, new TGeoCombiTrans(trans, rot));
   fMasterVolume->AddNode(vacuum_lateral_pipe[1], 10, new TGeoCombiTrans(trans, rot));

   // knob
   zcenter = lateral_knob_position_z + lateral_knob_length / 2.;
   fMasterVolume->AddNode(lateral_pipe[3], 11, new TGeoTranslation(0, 0, zcenter));
   fMasterVolume->AddNode(lateral_pipe[3], 12, new TGeoTranslation(0, 0, -zcenter));

   // == sector 2 ==
   zcenter = center_length / 2. + lateral_length[0] + lateral_cone_length + lateral_length[1] / 2.;
   // plus
   fMasterVolume->AddNode(lateral_pipe[2], 13, new TGeoTranslation(0, 0, zcenter));
   fMasterVolume->AddNode(vacuum_lateral_pipe[2], 14, new TGeoTranslation(0, 0, zcenter));
   // minus
   fMasterVolume->AddNode(lateral_pipe[2], 15, new TGeoTranslation(0, 0, -zcenter));
   fMasterVolume->AddNode(vacuum_lateral_pipe[2], 16, new TGeoTranslation(0, 0, -zcenter));

   // -------------------------------
   center_pipe->SetFillColor(kYellow);
   center_pipe->SetLineColor(kYellow);
   center_pipe->SetTransparency(80);

   for (auto icolor = 1; icolor < 4; ++icolor)
   {
        lateral_pipe[icolor - 1]->SetFillColor(kYellow + 2 * icolor);
        lateral_pipe[icolor - 1]->SetLineColor(kYellow + 2 * icolor);
        lateral_pipe[icolor - 1]->SetTransparency(80);
    }

    for (auto icolor = 1; icolor < 4; ++icolor)
    {
        vacuum_lateral_pipe[icolor - 1]->SetFillColor(kRed);
        vacuum_lateral_pipe[icolor - 1]->SetLineColor(kRed);
        vacuum_lateral_pipe[icolor - 1]->SetTransparency(90);
    }
}
//_____________________________________________________________________________________
void SpdPipe::SetR(Double_t r)
{
    if (fPipe || fPipeType == "hybrid")
        return;
    (r > 1.) ? fR = r : fR = 5.;
}

//_____________________________________________________________________________________
void SpdPipe::SetThickness(Double_t dr)
{
    if (fPipe || fPipeType == "hybrid")
        return;
    (dr > 0.1 && dr < fR) ? fThickness = dr : fThickness = 1.;
}

//_____________________________________________________________________________________
void SpdPipe::SetHalfLength(Double_t dl)
{
    if (fPipe || fPipeType == "hybrid")
        return;
    (dl > 5.) ? fHalfLength = dl : fHalfLength = 300;
}

//_____________________________________________________________________________________
void SpdPipe::SetPipeMaterial(TString material, Int_t n)
{
    if (fPipe)
        return;

    TGeoMedium *medium = FindMedium(material, "");
  /*  if (!medium)
    {
        cout << "-E- <SpdPipe::SetPipeMaterial> "
             << " The material: \"" << material << "\" is not defined " << endl;
        return;
    }
*/
    if (n < 0 || n > 1)
    {
        cout << "-E- <SpdPipe::SetPipeMaterial> Bad material index (0 or 1) " << endl;
        return;
    }

    fMaterial[n] = material;
}

//_____________________________________________________________________________
void SpdPipe::UnsetMaterials(TString media)
{
    if (fPipe)
        return;

    // cout << "-I- <SpdPipe::UnsetMaterials> " << media << endl;

    fBaseMaterial = media;
    fMaterial[0] = media;
    fMaterial[1] = media;

    fUnsetMedia = media;
}

//_____________________________________________________________________________
void SpdPipe::ResetMaterials()
{
    if (fPipe)
        return;
    SetDefaultMaterials(fGeoType);
}

//_____________________________________________________________________________________
Double_t SpdPipe::GetCapacity() const
{
    if (!fPipe)
        return 0;
    if (fPipeType != "hybrid")
        return (fPipe) ? fPipe->Capacity() : 0;

    Double_t V = 0;

    if (fHybPipeC)
        V += fHybPipeC->Capacity();
    if (fHybPipeLZminus)
        V += fHybPipeLZminus->Capacity();
    if (fHybPipeLZplus)
        V += fHybPipeLZplus->Capacity();

    return V;
}

//_____________________________________________________________________________________
Double_t SpdPipe::GetMass() const
{
    if (!fPipe)
        return 0;
    if (fPipeType != "hybrid")
        return fPipe->Capacity() * fPipe->GetMaterial()->GetDensity();

    Double_t M = 0;

    if (fHybPipeC)
        M += fHybPipeC->Capacity() * fHybPipeC->GetMaterial()->GetDensity();
    if (fHybPipeLZminus)
        M += fHybPipeLZminus->Capacity() * fHybPipeLZminus->GetMaterial()->GetDensity();
    if (fHybPipeLZplus)
        M += fHybPipeLZplus->Capacity() * fHybPipeLZplus->GetMaterial()->GetDensity();

    return M;
}

//_____________________________________________________________________________________
Double_t SpdPipe::GetDensity() const
{
    if (!fPipe)
        return 0;
    return GetMass() / GetCapacity();
}

//_____________________________________________________________________________________
void SpdPipe::SetDefault(Int_t geotype)
{
    cout << "<SpdPipe::SetDefault> " << geotype << endl;

    fBaseMaterial = "vacuum";

    if (geotype < 1 || geotype > 5)
    {

        fGeoType = 0;

        fMaterial[0] = "vacuum2";
        fMaterial[1] = "vacuum2";

        fR = 0;
        fThickness = 0;

        fPipeType = "unknown";
        fPipeName = FullName("", kFALSE);
        fHalfLength = 0;

        return;
    }

    fGeoType = geotype;

    fMaterial[0] = SpdCommonGeoMapper::GetPipeMaterial1();
    fMaterial[1] = SpdCommonGeoMapper::GetPipeMaterial2();

    if (fGeoType == 1)
    {

        fPipeType = "general";
        fPipeName = FullName("", kFALSE);
        fHalfLength = SpdCommonGeoMapper::GetPipeLength1() / 2.;
        fR = SpdCommonGeoMapper::GetPipeOuterRadius();
        fThickness = SpdCommonGeoMapper::GetPipeThickness();

        return;
    }

    if (fGeoType == 2)
    {

        fPipeType = "toroidal";
        fPipeName = FullName("TOR", kFALSE);
        fHalfLength = SpdCommonGeoMapper::GetPipeLength2() / 2.;
        fR = SpdCommonGeoMapper::GetPipeOuterRadius();
        fThickness = SpdCommonGeoMapper::GetPipeThickness();

        return;
    }

    if (fGeoType == 3)
    {

        fPipeType = "solenoidal";
        fPipeName = FullName("SOL", kFALSE);
        fHalfLength = SpdCommonGeoMapper::GetPipeLength3() / 2.;
        fR = SpdCommonGeoMapper::GetPipeOuterRadius();
        fThickness = SpdCommonGeoMapper::GetPipeThickness();

        return;
    }

    if (fGeoType == 4)
    {

        fPipeType = "hybrid";
        fPipeName = FullName("HYB", kFALSE);

        return;
    }
    if (fGeoType == 5)
    {

        fPipeType = "sol2";
        fPipeName = FullName("SOL2", kFALSE);
    }
}

//_____________________________________________________________________________________
void SpdPipe::SetDefaultMaterials(Int_t geotype)
{
    // cout <<"<SpdPipe::SetDefaultMaterials> " << geotype << endl;

    fBaseMaterial = "vacuum";

    if (geotype < 1 || geotype > 5)
    {

        fMaterial[0] = "vacuum2";
        fMaterial[1] = "vacuum2";

        return;
    }

    fMaterial[0] = SpdCommonGeoMapper::GetPipeMaterial1();
    fMaterial[1] = SpdCommonGeoMapper::GetPipeMaterial2();
}

//_____________________________________________________________________________________
void SpdPipe::SetCenterMaterial(TString MaterialCenter = "aluminium")
{
    if (MaterialCenter == "aluminium" || MaterialCenter == "beryllium")
    {
        // beryllium or aluminium
        fMaterial[0] = MaterialCenter;
        cout << "Set center pipe materials is " <<MaterialCenter<<  endl;
    }
    else
    {
        cout << "Center materials is unknown" << endl;
        cout << "Set default materials is aluminium" << endl;
        fMaterial[0] = "aluminium";
    }
}

//_____________________________________________________________________________________
void SpdPipe::Print(Option_t *) const
{
    cout << "<SpdPipe::Print> Name/Prefix: " << fName << "/" << fTitle << endl;

    cout << "\n";
    cout << "\tGeometry type:    " << fPipeType << endl;
    cout << "\tUnique module Id: " << fModuleId << endl;
    cout << "\tUnset media:      " << fUnsetMedia << endl;

    if (fGeoType < 1)
    {
        cout << endl;
        return;
    }

    if (fPipeType != "hybrid")
    {
        cout << "\tPipe material:    " << fMaterial[0] << endl;
        cout << "\n";

        cout << "\tLength:           " << 2. * fHalfLength << " [cm] " << endl;
        cout << "\tThickness:        " << fThickness << " [cm] " << endl;
        cout << "\tInner radius:     " << fR - fThickness << " [cm] " << endl;
        cout << "\tOuter radius:     " << fR << " [cm] " << endl;
        cout << "\n";
    }

    if (fPipeType == "hybrid")
    {
        cout << "\tPipe material (base):    " << fBaseMaterial << endl;
        cout << "\tPipe material (central): " << fMaterial[0] << endl;
        cout << "\tPipe material (lateral): " << fMaterial[1] << endl;
        cout << "\n";

        if (fPipe)
        {
            printf("\t%3s  %12s  %12s  %12s\n\n", "", "Z [mm]", "Rmin [mm]", "Rmax [mm]");
            TGeoPcon *shape;
            Int_t nn(0);
            if (fHybPipeLZminus)
            {
                shape = (TGeoPcon *)fHybPipeLZminus->GetShape();
                for (int i(0); i < shape->GetNz(); i++)
                {
                    printf("\t%-3d  %12.6f  %12.6f  %12.6f\n", ++nn,
                           shape->GetZ(i) * 10, shape->GetRmin(i) * 10, shape->GetRmax(i) * 10);
                }
                printf("\n");
            }
            if (fHybPipeC)
            {
                shape = (TGeoPcon *)fHybPipeC->GetShape();
                for (int i(0); i < shape->GetNz(); i++)
                {
                    printf("\t%-3d  %12.6f  %12.6f  %12.6f\n", ++nn,
                           shape->GetZ(i) * 10, shape->GetRmin(i) * 10, shape->GetRmax(i) * 10);
                }
                printf("\n");
            }
            if (fHybPipeLZplus)
            {
                shape = (TGeoPcon *)fHybPipeLZplus->GetShape();
                for (int i(0); i < shape->GetNz(); i++)
                {
                    printf("\t%-3d  %12.6f  %12.6f  %12.6f\n", ++nn,
                           shape->GetZ(i) * 10, shape->GetRmin(i) * 10, shape->GetRmax(i) * 10);
                }
                printf("\n");
            }
        }
    }

    if (!fPipe)
        return;

    cout << "\tCapacity:         " << GetCapacity() * 1e-6 << " [m^3] " << endl;
    cout << "\tMass:             " << GetMass() * 1e-3 << " [kg] " << endl;
    cout << "\tDensity:          " << GetDensity() << " [g/(cm^3)] " << endl;

    cout << "\n";
}

//_____________________________________________________________________________
Bool_t SpdPipe::FillParsIn(SpdPassiveGeoParSet *params)
{
    if (!SpdPassiveModule::FillParsIn(params))
        return kFALSE;
    return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdPipe::LoadParsFrom(SpdPassiveGeoParSet *params)
{
    if (!SpdPassiveModule::LoadParsFrom(params))
        return kFALSE;
    return kTRUE;
}
