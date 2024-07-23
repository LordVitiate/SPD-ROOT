// $Id$
// Author: artur   2019/09/06

#ifndef __SPDIDEALTRACKCOLL_H__
#define __SPDIDEALTRACKCOLL_H__

#include <TNamed.h>
#include <vector>
#include "SpdIdealTrack.h"

class SpdIdealDataInfo; 
class SpdIdealTrack; 

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdIdealTrackColl                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdIdealTrackColl: public TNamed {

public:

    SpdIdealTrackColl();
    virtual ~SpdIdealTrackColl();
    
    void  DeleteAll();
    void  ClearData();
    
    void  SetRunId(UInt_t id) { fRunId = id; }
    void  SetEventId(Int_t id) { fEventId = id; }
    void  SetInfoNTracks(Int_t n);
    
    void  SetVertex(Double_t vx, Double_t vy, Double_t vz) { fVertex.SetXYZ(vx,vy,vz); }
    void  SetVertex(TVector3 v) { fVertex = v; }
    void  SetVertexTime(Double_t t) { fVertexTime = t; }
    
    void  SetErrorFlag(Int_t error)  { fErrorFlag = error; }
          
    Int_t AddDataInfo(SpdIdealDataInfo* datainfo);
    Int_t AddTrack(SpdIdealTrack* track);
    Int_t AddTrackMark(Int_t label);
    
    TObjArray*  ReplaceDataInfo(TObjArray* x) { TObjArray* a = fDataInfo; fDataInfo = x; return a; }
    TObjArray*  ReplaceTracks(TObjArray* x) { TObjArray* a = fTracks; fTracks = x; return a; }
    
    Int_t GetNPrimTracks() const { return fPrimTrackMarks.size(); } 
    
    Int_t GetNDataInfo() const { return fDataInfo->GetEntriesFast(); }
    Int_t GetNTracks()   const { return fTracks->GetEntriesFast();   }
    
    TVector3 GetVertex() const { return fVertex; }
    Double_t GetVertexTime() const { return fVertexTime; }
    
    Bool_t IsEmptyEvent() const { return (GetNTracks() < 1); } 
    
    TObjArray*  GetDataInfo() const { return fDataInfo; }
    TObjArray*  GetTracks()   const { return fTracks;   }
    
    Int_t  GetEventId() const { return fEventId; }
    UInt_t GetRunId() const { return fRunId; }
    
    Int_t  GetErrorFlag() const { return fErrorFlag; }
    
    SpdIdealDataInfo* FindDataInfo(Int_t detId);
    
    SpdIdealDataInfo* GetDataInfo(Int_t i);
    SpdIdealTrack*    GetTrack(Int_t i);
    
    std::vector<Int_t>& GetPrimTrackMarks() { return fPrimTrackMarks; }
    
    virtual void PrintData() const;
    
private:
    
    UInt_t     fRunId;        // MC run id
    Int_t      fEventId;      // MC event id
    
    TObjArray* fDataInfo;     // mc-data hits info
    TObjArray* fTracks;       // ideal tracks collection

    TVector3   fVertex;       // event vertex
    Double_t   fVertexTime;   // start time for the event
    
    Int_t      fErrorFlag;    // error flag (=0: no errors when preparing) 
    
    std::vector<Int_t> fPrimTrackMarks; // SpdTrackSelectionMark (see SpdIdealTrack)
    
    ClassDef(SpdIdealTrackColl,1)
};

#endif  /* __SPDIDEALTRACKCOLL_H__ */

