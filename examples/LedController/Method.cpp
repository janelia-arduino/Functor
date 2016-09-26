// ----------------------------------------------------------------------------
// Method.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "Method.h"


// Method
Method::Method()
{
}

void Method::attachCallback(const Functor0 & callback)
{
  callback_ = callback;
}

void Method::callback()
{
  if (callback_)
  {
    callback_();
  }
}
