#ifndef RsPlaneHolder_h
#define RsPlaneHolder_h

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "TVector3.h"

struct RsPlane {
    std::string geopath;
    int subs; //1, 2 (endcaps), ... 10 (barrel)
    int layer;
    TVector3 pos;
    TVector3 norm;

    void Print();

    friend std::istream& operator>>(std::istream& is, RsPlane& plane);
    friend std::ostream& operator<<(std::ostream& os, const RsPlane& plane);
    ClassDef(RsPlane,1)
};

struct RsPlaneHolder {

    std::map<int, std::map<int, RsPlane>> planes; // maps det_id to vector of planes

    void LoadPlanes();
    void SavePlanes();
    void InitPlanes();

    RsPlane GetPlane(int subs, int layer ) {
        return planes.at(subs).at(layer);
    }
    ClassDef(RsPlaneHolder,1)
};

#endif
