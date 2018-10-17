#include "LedController.h"


LedController led_controller;

void setup()
{
  led_controller.setup();
}

void loop()
{
  led_controller.update();
}
