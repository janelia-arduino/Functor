// ----------------------------------------------------------------------------
// Controller.h
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "Callback.h"
#include "Array.h"
#include "Constants.h"
#include "Method.h"


class Controller
{
public:
  Controller();
  void setup();
  void update();

private:
  Array<Method,constants::METHOD_COUNT_MAX> methods_;

  void setLedOnCallback();
  void setLedOffCallback();
};

extern Controller controller;

#endif
