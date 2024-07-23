
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// SpdGeoVolPars for Its module (ItsVolPars)
#pragma link C++ class SpdItsChip+;
#pragma link C++ class SpdItsLadder+;
#pragma link C++ class SpdItsLayer+;
#pragma link C++ class SpdItsEClayer+;

// SpdGeoVolPars for TsTB module (TsBVolPars)
#pragma link C++ class SpdTsBStrawInternal+;
#pragma link C++ class SpdTsBStraw+;
#pragma link C++ class SpdTsBStrawDLayer+;
#pragma link C++ class SpdTsBStrawModuleH+;
// obsolete
#pragma link C++ class SpdTsBStrawLayer+;
#pragma link C++ class SpdTsBStrawModule+;

// SpdGeoVolPars for TsTEC module (TsECVolPars)
#pragma link C++ class SpdTsECStrawInternal+;
#pragma link C++ class SpdTsECStraw+;
#pragma link C++ class SpdTsECStrawLayer+;
#pragma link C++ class SpdTsECStrawModule+;
#pragma link C++ class SpdTsECStrawModuleArb+;
#pragma link C++ class SpdTsECStrawModulePgon+;

// mappers 
#pragma link C++ class SpdItsGeoMapperX+;

#pragma link C++ class SpdMvdGeoMapper+;

#pragma link C++ class SpdEcalTBGeoMapper+;
#pragma link C++ class SpdEcalBGeoMapper+;
#pragma link C++ class SpdEcalTB2GeoMapper+;
#pragma link C++ class SpdEcalTECGeoMapper+;
#pragma link C++ class SpdEcalECGeoMapper+;
#pragma link C++ class SpdEcalTEC2GeoMapper+;
#pragma link C++ class SpdEcalSBGeoMapper+;
#pragma link C++ class SpdEcalSECGeoMapper+;

#pragma link C++ class SpdTofBGeoMapper+;
#pragma link C++ class SpdTofECGeoMapper+;

#pragma link C++ class SpdRsTBGeoMapper+;
#pragma link C++ class SpdRsTB2GeoMapper+;
#pragma link C++ class SpdRsTECGeoMapper+;
#pragma link C++ class SpdRsTEC2GeoMapper+;
#pragma link C++ class SpdRsSBGeoMapper+;
#pragma link C++ class SpdRsSECGeoMapper+;

#pragma link C++ class SpdTsTECGeoMapper+;
#pragma link C++ class SpdTsTECGeoBuilder+;

#pragma link C++ class SpdTsTBGeoMapper+;
#pragma link C++ class SpdTsTECGeoMapper+;
#pragma link C++ class SpdTsSBGeoMapper+;
#pragma link C++ class SpdTsSECGeoMapper+;

#pragma link C++ class SpdZdcGeoMapper+;
#pragma link C++ class SpdBbcGeoMapper+;
#pragma link C++ class SpdAegGeoMapper+;

// builders
#pragma link C++ class SpdItsGeoBuilder+;
#pragma link C++ class SpdTsTBGeoBuilder+;
#pragma link C++ class SpdTsSBGeoBuilder+;

#pragma link C++ class SpdGeoFactory+;
#pragma link C++ class SpdGeoLoader+;

#endif
