// $Id$
// Author: artur   2018/01/05

#ifndef __SPDFIELDCREATOR_H__
#define __SPDFIELDCREATOR_H__

#include "FairFieldFactory.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdFieldCreator                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdFieldPar;
class FairField;

class SpdFieldCreator : public FairFieldFactory {

public:

  SpdFieldCreator();
  virtual ~SpdFieldCreator();

  virtual FairField* createFairField();
  virtual FairField* createFairField(SpdFieldPar* Pars);
  
  virtual void SetParameters();

protected:

  SpdFieldPar* fFieldPar;
  
  virtual FairField* CreateSingleField(TString name, Int_t type);
  virtual FairField* CreateMultiField();
  
private:

  SpdFieldCreator(const SpdFieldCreator&);
  SpdFieldCreator& operator=(const SpdFieldCreator&);

  ClassDef(SpdFieldCreator,1);
  
};

#endif /* __SPDFIELDCREATOR_H__ */



