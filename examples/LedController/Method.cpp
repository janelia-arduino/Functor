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
  callback_is_attached_ = false;
}

void Method::attachCallback(const Functor0 & callback)
{
  callback_ = callback;
  callback_is_attached_ = true;
}

void Method::callback()
{
  if (callback_is_attached_)
  {
    callback_();
  }
}
