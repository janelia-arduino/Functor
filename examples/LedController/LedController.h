// ----------------------------------------------------------------------------
// Controller.h
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------
#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <Arduino.h>
#include <Functor.h>

#include "Constants.h"
#include "Method.h"


class LedController
{
public:
  void setup();
  void update();

private:
  Method methods_[constants::METHOD_COUNT_MAX];

  void setLedOnCallback();
  void setLedOffCallback();
};

#endif
