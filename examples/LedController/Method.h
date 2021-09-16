// ----------------------------------------------------------------------------
// Method.h
//
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------
#ifndef _METHOD_H_
#define _METHOD_H_
#include <Functor.h>


class Method
{
public:
  Method();
  void attachCallback(const Functor0 & callback);
  void callback();
protected:
  Functor0 callback_;
};

#endif
