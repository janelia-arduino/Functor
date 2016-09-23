// ----------------------------------------------------------------------------
// Method.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef _METHOD_H_
#define _METHOD_H_
#include "Callback.h"


class Method
{
public:
  Method();
  void attachCallback(const CBFunctor0 & callback);
  void callback();
protected:
  CBFunctor0 callback_;
  bool callback_is_attached_;
};

#endif
