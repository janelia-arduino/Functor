#include "Arduino.h"
#include "Streaming.h"
#include "Array.h"
#include "Callback.h"
#include "Constants.h"
#include "Method.h"
#include "Controller.h"


void setup()
{
  controller.setup();
}

void loop()
{
  controller.update();
}
