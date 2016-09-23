#Callback

Copyright 1994 Rich Hickey
Permission to use, copy, modify, distribute and sell this software
for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation.  Rich Hickey makes no
representations about the suitability of this software for any
purpose.  It is provided "as is" without express or implied warranty.


06/12/94 Rich Hickey
3rd major revision
Now functors are concrete classes, and should be held by value
Virtual function mechanism removed
Generic makeFunctor() mechanism added for building functors
from both stand-alone functions and object/ptr-to-mem-func pairs

To use:

  If you wish to build a component that provides/needs a callback,
  simply specify and hold a CBFunctor of the type corresponding to the
  args you wish to pass and the return value you need. There are 10
  Functors from which to choose:

  CBFunctor0
  CBFunctor1<P1>
  CBFunctor2<P1,P2>
  CBFunctor3<P1,P2,P3>
  CBFunctor4<P1,P2,P3,P4>
  CBFunctor0wRet<RT>
  CBFunctor1wRet<P1,RT>
  CBFunctor2wRet<P1,P2,RT>
  CBFunctor3wRet<P1,P2,P3,RT>
  CBFunctor4wRet<P1,P2,P3,P4,RT>

  These are parameterized by their args and return value if any. Each has
  a default ctor and an operator() with the corresponding signature.

  They can be treated and used just like ptr-to-functions.

  If you want to be a client of a component that uses callbacks, you
  create a CBFunctor by calling makeFunctor().

  There are three flavors of makeFunctor - you can create a functor from:

  a ptr-to-stand-alone function
  an object and a pointer-to-member function.
  a pointer-to-member function (which will be called on first arg of functor)

  The current iteration of this library requires you pass makeFunctor()
  a dummy first argument of type ptr-to-the-Functor-type-you-want-to-create.
  Simply cast 0 to provide this argument:

  makeFunctor((target-functor*)0,ptr-to-function)
  makeFunctor((target-functor*)0,reference-to-object,ptr-to-member-function)
  makeFunctor((target-functor*)0,ptr-to-member-function)

  Future versions will drop this requirement once member templates are
  available.

  The functor system is 100% type safe. It is also type flexible. You can
  build a functor out of a function that is 'type compatible' with the
  target functor - it need not have an exactly matching signature. By
  type compatible I mean a function with the same number of arguments, of
  types reachable from the functor's argument types by implicit conversion.
  The return type of the function must be implicitly convertible to the
  return type of the functor. A functor with no return can be built from a
  function with a return - the return value is simply ignored.
  (See ethel example below)

  All the correct virtual function behavior is preserved. (see ricky
  example below).

  If you somehow try to create something in violation
  of the type system you will get a compile-time or template-instantiation-
  time error.

  The CBFunctor base class and translator
  classes are artifacts of this implementation. You should not write
  code that relies upon their existence. Nor should you rely on the return
  value of makeFunctor being anything in particular.

  All that is guaranteed is that the Functor classes have a default ctor,
  an operator() with the requested argument types and return type, an
  operator that will allow it to be evaluated in a conditional (with
  'true' meaning the functor is set, 'false' meaning it is not), and that
  Functors can be constructed from the result of makeFunctor(), given
  you've passed something compatible to makeFunctor().

  /////////////////////// BEGIN Example 1 //////////////////////////
  #include <iostream.h>
  #include "callback.hpp"

  //do5Times() is a function that takes a functor and invokes it 5 times

  void do5Times(const CBFunctor1<int> &doIt)
  {
  for(int i=0;i<5;i++)
  doIt(i);
  }

  //Here are some standalone functions

  void fred(int i){cout << "fred: " << i<<endl;}
  int ethel(long l){cout << "ethel: " << l<<endl;return l;}

  //Here is a class with a virtual function, and a derived class

  class B{
  public:
  virtual void ricky(int i)
  {cout << "B::ricky: " << i<<endl;}
  };

  class D:public B{
  public:
  void ricky(int i)
  {cout << "D::ricky: " << i<<endl;}
  };

  void main()
  {
  //create a typedef of the functor type to simplify dummy argument
  typedef CBFunctor1<int> *FtorType;

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
  /////////////////////// END of example 1 //////////////////////////

  /////////////////////// BEGIN Example 2 //////////////////////////
  #include <iostream.h>
  #include "callback.hpp"

  //Button is a component that provides a functor-based
  //callback mechanism, so you can wire it up to whatever you wish

  class Button{
  public:
  //ctor takes a functor and stores it away in a member

  Button(const CBFunctor0 &uponClickDoThis):notify(uponClickDoThis)
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

  class CDPlayer{
  public:
  void play()
  {cout << "Playing"<<endl;}
  void stop()
  {cout << "Stopped"<<endl;}
  };

  void main()
  {
  CDPlayer myCD;
  Button playButton(makeFunctor((CBFunctor0*)0,myCD,&CDPlayer::play));
  Button stopButton(makeFunctor((CBFunctor0*)0,myCD,&CDPlayer::stop));
  playButton.click(); //calls myCD.play()
  stopButton.click();  //calls myCD.stop()
  }
  /////////////////////// END of example 2 //////////////////////////


