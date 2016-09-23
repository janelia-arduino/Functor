#include "Arduino.h"
#include "Streaming.h"
#include "Callback.h"

const unsigned int BAUDRATE = 9600;

//do5Times() is a function that takes a functor and invokes it 5 times

void do5Times(const CBFunctor1<int> & doIt)
{
  for(int i=0; i<5; ++i)
  {
    doIt(i);
  }
}

//Here are some standalone functions

void fred(int i)
{
  Serial << "fred: " << i << endl;
}

int ethel(long l)
{
  Serial << "ethel: " << l << endl;
  return l;
}

//Here is a class with a virtual function, and a derived class

class B
{
public:
  virtual void ricky(int i)
  {
    Serial << "B::ricky: " << i << endl;
  }
};

class D : public B
{
public:
  void ricky(int i)
  {
    Serial << "D::ricky: " << i << endl;
  }
};

void setup()
{
  Serial.begin(BAUDRATE);
  delay(1000);

  //create a typedef of the functor type to simplify dummy argument
  typedef CBFunctor1<int> * FtorType;

  CBFunctor1<int> ftor; //a functor variable
  //make a functor from ptr-to-function
  ftor = makeFunctor((FtorType)0,fred);
  do5Times(ftor);
  //note ethel is not an exact match - ok, is compatible
  ftor = makeFunctor((FtorType)0,ethel);
  do5Times(ftor);

  //create a D object to be a callback target
  D myD;
  //make functor from object and ptr-to-member-func
  ftor = makeFunctor((FtorType)0,myD,&B::ricky);
  do5Times(ftor);
}

void loop()
{
}
