#include <Arduino.h>
#include <Streaming.h>
#include <Array.h>
#include <Functor.h>

#include "Constants.h"
#include "Method.h"
#include "Controller.h"


Controller controller;

void setup()
{
  controller.setup();
}

void loop()
{
  controller.update();
}
