// ----------------------------------------------------------------------------
// Controller.cpp
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------
#include "LedController.h"


void writeLedOn()
{
  Serial.println("led on");
}

void writeLedOff()
{
  Serial.println("led off");
}

void LedController::setup()
{
  // Pin Setup
  pinMode(constants::led_pin, OUTPUT);

  // Methods
  Method led_on_method;
  led_on_method.attachCallback(makeFunctor((Functor0 *)0,*this,&LedController::setLedOnCallback));
  methods_[0] = led_on_method;

  Method write_led_on_method;
  write_led_on_method.attachCallback(makeFunctor((Functor0 *)0,&writeLedOn));
  methods_[1] = write_led_on_method;

  Method led_off_method;
  led_off_method.attachCallback(makeFunctor((Functor0 *)0,*this,&LedController::setLedOffCallback));
  methods_[2] = led_off_method;

  Method write_led_off_method;
  write_led_off_method.attachCallback(makeFunctor((Functor0 *)0,&writeLedOff));
  methods_[3] = write_led_off_method;

  // Setup Streams
  Serial.begin(constants::baud);
}

void LedController::update()
{
  for (size_t i=0; i<constants::METHOD_COUNT_MAX; ++i)
  {
    methods_[i].callback();
    delay(constants::loop_delay);
  }
}

void LedController::setLedOnCallback()
{
  digitalWrite(constants::led_pin,HIGH);
}

void LedController::setLedOffCallback()
{
  digitalWrite(constants::led_pin,LOW);
}
