// ----------------------------------------------------------------------------
// Controller.cpp
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "Controller.h"


Controller::Controller()
{
}

void Controller::setup()
{
  // Pin Setup
  pinMode(constants::led_pin, OUTPUT);

  // Methods
  Method led_on_method;
  led_on_method.attachCallback(makeFunctor((CBFunctor0 *)0,*this,&Controller::setLedOnCallback));
  methods_.push_back(led_on_method);

  Method led_off_method;
  led_off_method.attachCallback(makeFunctor((CBFunctor0 *)0,*this,&Controller::setLedOffCallback));
  methods_.push_back(led_off_method);

  // Setup Streams
  Serial.begin(constants::baudrate);
}

void Controller::update()
{
  for (size_t i=0; i<methods_.size(); ++i)
  {
    methods_[i].callback();
    delay(1000);
  }
}

void Controller::setLedOnCallback()
{
  digitalWrite(constants::led_pin,HIGH);
}

void Controller::setLedOffCallback()
{
  digitalWrite(constants::led_pin,LOW);
}

Controller controller;
