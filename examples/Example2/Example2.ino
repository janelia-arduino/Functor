#include "Arduino.h"
#include "Streaming.h"
#include "Callback.h"

const unsigned int BAUDRATE = 9600;

//Button is a component that provides a functor-based
//callback mechanism, so you can wire it up to whatever you wish

class Button
{
public:
  //ctor takes a functor and stores it away in a member

  Button(const CBFunctor0 & uponClickDoThis) :
    notify(uponClickDoThis)
  {}
  void click()
  {
    //invoke the functor, thus calling back client
    notify();
  }
private:
  //note this is a data member with a verb for a name - matches its
  //function-like usage
  CBFunctor0 notify;
};

class CDPlayer
{
public:
  void play()
  {
    Serial << "Playing" << endl;
  }
  void stop()
  {
    Serial << "Stopped" << endl;
  }
};

void wow()
{
  Serial  << "Wow!" << endl;
}

void setup()
{
  Serial.begin(BAUDRATE);
  delay(1000);

  // makeFunctor from object and ptr-to-member-function
  CDPlayer myCD;
  Button playButton(makeFunctor((CBFunctor0 *)0,myCD,&CDPlayer::play));
  Button stopButton(makeFunctor((CBFunctor0 *)0,myCD,&CDPlayer::stop));
  playButton.click(); //calls myCD.play()
  stopButton.click();  //calls myCD.stop()

  // makeFunctor from pointer-to-function
  Button wowButton(makeFunctor((CBFunctor0 *)0,&wow));
  wowButton.click(); //calls wow()
}

void loop()
{
}
