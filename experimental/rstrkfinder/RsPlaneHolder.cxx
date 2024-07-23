#include "RsPlaneHolder.h"

#include "TGeoManager.h"
#include "TGeoNavigator.h"
#include "TFile.h"

//#include "FairGeoParSet.h"
#include "SpdRunAna.h"

#include <set>
#include <iostream>

// ******************** Active RS plane ********************

void RsPlane::Print() {
    std::cout << geopath << " ";
    std::cout << subs << " " << layer << " ";
    std::cout << pos.X() << " " << pos.Y() << " " << pos.Z() << " ";
    std::cout << norm.X() << " " << norm.Y() << " " << norm.Z() << "\n";
}

std::istream& operator>>(std::istream& is, RsPlane& plane) {
    std::string geopath;
    int subs;
    int layer;
    double x, y, z;
    double nx, ny, nz;

    is >> geopath;
    is >> subs >> layer;
    is >> x >> y >> z;
    is >> nx >> ny >> nz;

    plane.geopath = geopath;
    plane.subs = subs;
    plane.layer = layer;
    TVector3 pos(x, y, z);
    plane.pos = pos;
    TVector3 norm(nx, ny, nz);
    plane.norm = norm;

    return is;
}

std::ostream& operator<<(std::ostream& os, const RsPlane& plane) {
    os << plane.geopath << " ";
    os << plane.subs << " " << plane.layer << " ";
    os << plane.pos.X() << " " << plane.pos.Y() << " " << plane.pos.Z() << " ";
    os << plane.norm.X() << " " << plane.norm.Y() << " " << plane.norm.Z() << "\n";

    return os;
}

// ******************** RsPlaneHolder and auxiliaries ********************

namespace RsPlaneHolderAux {

    // Function sets nav to the returned node
    TGeoNode*  CheckDaughterNode(TGeoNode* node, int inode, TGeoNavigator* nav, bool debug = false) {
        static const std::set<TString> subs_list = {"Envelope", "LayerSideSVol", "LayerECVol", "MDTSVol", "CellVol", "GasCell"};
        int     init_length = TString(nav->GetPath()).Length();
        TGeoNode* cdnode = node->GetDaughter(inode);
        nav->CdDown(cdnode);
        TString path( nav->GetPath() );
        TString path_suffix = TString( path.Data() + init_length );

        if (debug) {
            std::cout << "Checking path: " << path.Data() << std::endl;
            std::cout << "Path suffix: " << path_suffix.Data() << std::endl;
        }

        for ( auto& subs : subs_list ) {
            if (path_suffix.Contains( subs )) {
                if (debug) std::cout << "  path accepted" << std::endl;
                return cdnode;
            }
        }
        nav->CdUp();
        if (debug) std::cout << "  path descarded" << std::endl;
        return nullptr;
    }

    TGeoNode* GetBottomNode(TGeoNode* node, TGeoNavigator* nav) {
        int nnodes = node->GetNdaughters();

        TGeoNode* bootom_node;
        for (int i=0; i<nnodes; i++) {
            bootom_node = CheckDaughterNode(node, i, nav);
            if (bootom_node)
            return bootom_node;
        }
        return nullptr;
    }

    void ExploreLayers(TString base_geopath, TGeoNavigator* nav, RsPlaneHolder& plane_holder) {

        int nbase_modules = base_geopath.Contains("RsBSide") ? 8 : 2;
        for (int ioctant=1; ioctant<=nbase_modules; ioctant++){  // octant id or endcap id
            for (int ilayer=1; ilayer<=20; ilayer++){ // Change 20

                TString startpath( Form( base_geopath, ioctant, ilayer) );
                std::cout << startpath.Data() << std::endl;
                nav->cd( startpath );
                TGeoNode* gn = nav->GetCurrentNode();

                for (int i=0; i<4; i++) {
                    if (gn) {
                        gn = GetBottomNode( gn, nav );
                    }
                }

                std::cout << "In node: " << nav->GetPath() << std::endl;
                double loc_center[3] = {0, 0, 0};
                double det_pos[3];
                nav->LocalToMaster(loc_center, det_pos);
                std::cout << det_pos[0] << " " << det_pos[1] << " " << det_pos[2] << std::endl;

                double ay[3];
                double ayloc[3] = {0, 1, 0};

                nav->LocalToMasterVect(ayloc, ay);

                //cout << "HIT MODULE: " << nav->GetPath() << "\n\n";
                //cout << "\t position:   " << det_pos[0] << " " << det_pos[1] << " " << det_pos[2] << " " << endl;
                //cout << "\t   y-axis:   " << ay[0] << " " << ay[1] << " " << ay[2] << " " << endl;
                //cout << "\n";
                //cout << det_pos[0]*ay[0] + det_pos[1]*ay[1] + det_pos[2]*ay[2] << endl;

                RsPlane plane;
                if (base_geopath.Contains("RsBSide")) { //barel
                    plane.subs = ioctant + 1;
                } else { // endcap
                    plane.subs = ioctant - 1; // endcap, (1,2) -> (0,1)
                }
                plane.geopath = nav->GetPath();
                plane.layer = ilayer;
                plane.pos  = TVector3( det_pos );
                plane.norm = TVector3( ay );
                plane_holder.planes[plane.subs][plane.layer] = plane;

            }
        }
    }


    void GetRsPlanes(RsPlaneHolder& plane_holder) {

        //TString inParFile  = "params.root";
        //TFile* fp = new TFile(inParFile);
        //// import some global variables...
        //FairGeoParSet* geoset = (FairGeoParSet*)fp->Get("FairGeoParSet"); //Includes master volume, etc...?
        //delete fp;

        SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance();
        run->GetParameters("FairGeoParSet");

        TGeoNavigator* nav = gGeoManager->GetCurrentNavigator();
        ExploreLayers("/cave_1/RsBSideSVol_%d/LayerSideSVol_%d", nav, plane_holder);
        ExploreLayers("/cave_1/RsECVol_%d/LayerECVol_%d", nav, plane_holder);
    }
}

// ******************** RsPlaneHolder itself ********************

void RsPlaneHolder::InitPlanes() {
    RsPlaneHolderAux::GetRsPlanes(*this);
}

void RsPlaneHolder::LoadPlanes() {  // change this function
    std::ifstream is("planes.dat");
    RsPlane tplane;
    while (is >> tplane) {
        tplane.Print();
        planes[tplane.subs][tplane.layer] = tplane;
    }
}

void RsPlaneHolder::SavePlanes() {
    std::cout << "--------- In SavePlanes ----------" << std::endl;
    std::ofstream of("planes.dat");
    for (int subs=0; subs<=9; subs++) {
        for (int ilayer=1; ilayer<=20; ilayer++) {
            std::cout << subs << " " << ilayer << std::endl;
            auto& plane = planes.at(subs).at(ilayer);
            of << plane;
        }
    }
}

ClassImp(RsPlane)
ClassImp(RsPlaneHolder)
