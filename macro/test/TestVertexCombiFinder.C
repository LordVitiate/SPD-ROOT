
#include <vector>
#include <iostream>
#include "SpdVertexCombiFinder.h"

using namespace std;

void TestVertexCombiFinder() 
{

    SpdVertexCombiFinder* vfinder = new SpdVertexCombiFinder();

    std::vector<Int_t> particles = 
    { 
        310, 310, 211,  211,  211, 
       -321, 321, 321, -211, -211, 
         13,  13,  13, -13,  -211,
        -11, -11,  11,  11,   211 
    };
    
    vfinder->InitParticles(particles);
    
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
   
    Int_t n;
    std::vector<Int_t> vc;
    
    //------------------------------ Vertex 1 
    
    std::vector<Int_t> vertex1 = 
    { 
        310, 211, 211,  -321
    };
    
    vfinder->InitVertex(vertex1);  

    vfinder->PrintSettings();
    vfinder->PrintVertexCandidates();  // print set of vertex candidates
    
    // print set of vertex 1 candidates
    n = 0;
    while (vfinder->Next(vc))
    {
        cout << n++ << ":  ";
        for (Int_t i(0); i<Int_t(vc.size()); i++) cout << vc[i] << " (" << particles[vc[i]] << ")  ";
        cout << endl;
    }
    
    
    //------------------------------ Vertex 2 
    
    std::vector<Int_t> vertex2 = 
    { 
        211, -211
    };
    
    vfinder->InitVertex(vertex2);  

    vfinder->PrintSettings();
    vfinder->PrintVertexCandidates();  // print set of vertex candidates
    
    // print set of vertex 2 candidates
    n = 0;
    while (vfinder->Next(vc))
    {
        cout << n++ << ":  ";
        for (Int_t i(0); i<Int_t(vc.size()); i++) cout << vc[i] << " (" << particles[vc[i]] << ")  ";
        cout << endl;
    }
    
    //------------------------------ Vertex 3
    
    vfinder->InitVertex(11, 11, 321);  

    vfinder->PrintSettings();
    vfinder->PrintVertexCandidates();  // print set of vertex candidates
    
    // print set of vertex 2 candidates
    n = 0;
    while (vfinder->Next(vc))
    {
        cout << n++ << ":  ";
        for (Int_t i(0); i<Int_t(vc.size()); i++) cout << vc[i] << " (" << particles[vc[i]] << ")  ";
        cout << endl;
    }

}
