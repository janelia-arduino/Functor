/* Functor.h
 * This is Rich Hickey's callback library...
 * http://www.tutok.sk/fastgl/callback.html
 *
 * I've just renamed it to Functor.h
 */
//**************** Functor.h **********************
// Copyright 1994 Rich Hickey
/* Permission to use, copy, modify, distribute and sell this software
 * for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Rich Hickey makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

// 06/12/94 Rich Hickey
// 3rd major revision
// Now functors are concrete classes, and should be held by value
// Virtual function mechanism removed
// Generic makeFunctor() mechanism added for building functors
// from both stand-alone functions and object/ptr-to-mem-func pairs

#ifndef FUNCTOR_H
#define FUNCTOR_H

/*
  To use:

  If you wish to build a component that provides/needs a callback, simply
  specify and hold a Functor of the type corresponding to the args
  you wish to pass and the return value you need. There are 10 Functors
  from which to choose:

  Functor0
  Functor1<P1>
  Functor2<P1,P2>
  Functor3<P1,P2,P3>
  Functor4<P1,P2,P3,P4>
  Functor0wRet<RT>
  Functor1wRet<P1,RT>
  Functor2wRet<P1,P2,RT>
  Functor3wRet<P1,P2,P3,RT>
  Functor4wRet<P1,P2,P3,P4,RT>

  These are parameterized by their args and return value if any. Each has
  a default ctor and an operator() with the corresponding signature.

  They can be treated and used just like ptr-to-functions.

  If you want to be a client of a component that uses callbacks, you
  create a Functor by calling makeFunctor().

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

  The Functor base class and translator
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
  #include "Functor.h"

  //do5Times() is a function that takes a functor and invokes it 5 times

  void do5Times(const Functor1<int> &doIt)
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
  typedef Functor1<int> *FtorType;

  Functor1<int> ftor; //a functor variable
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
  #include "Functor.h"

  //Button is a component that provides a functor-based
  //callback mechanism, so you can wire it up to whatever you wish

  class Button{
  public:
  //ctor takes a functor and stores it away in a member

  Button(const Functor0 &uponClickDoThis):notify(uponClickDoThis)
  {}
  void click()
  {
  //invoke the functor, thus calling back client
  notify();
  }
  private:
  //note this is a data member with a verb for a name - matches its
  //function-like usage
  Functor0 notify;
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
  Button playButton(makeFunctor((Functor0*)0,myCD,&CDPlayer::play));
  Button stopButton(makeFunctor((Functor0*)0,myCD,&CDPlayer::stop));
  playButton.click(); //calls myCD.play()
  stopButton.click();  //calls myCD.stop()
  }
  /////////////////////// END of example 2 //////////////////////////

  */

//******************************************************************
///////////////////////////////////////////////////////////////////*
//WARNING - no need to read past this point, lest confusion ensue. *
//Only the curious need explore further - but remember         *
//about that cat!                                  *
///////////////////////////////////////////////////////////////////*
//******************************************************************

//////////////////////////////
// COMPILER BUG WORKAROUNDS:
// As of version 4.02 Borland has a code generation bug
// returning the result of a call via a ptr-to-function in a template

#ifdef __BORLANDC__
#define BC4_RET_BUG(x) RT(x)
#else
#define BC4_RET_BUG(x) x
#endif

//////////////////////////////

#include <string.h> //for memcpy
#include <stddef.h> //for size_t

//typeless representation of a function and optional object

class FunctorBase{
public:
  typedef void (FunctorBase::*_MemFunc)();
  typedef void (*_Func)();
  FunctorBase():func(0),callee(0){}
  FunctorBase(const void *c,const void *f,size_t sz)
  {
    if(c) //must be callee/memfunc
    {
      callee = (void *)c;
      memcpy(memFunc,f,sz);
    }
    else  //must be ptr-to-func
    {
      func = f;
    }
  }
  //for evaluation in conditionals
  operator int()const{return func||callee;}

  class DummyInit{
  };
  ////////////////////////////////////////////////////////////////
  // Note: this code depends on all ptr-to-mem-funcs being same size
  // If that is not the case then make memFunc as large as largest
  ////////////////////////////////////////////////////////////////

  union{
    const void *func;
    char memFunc[sizeof(_MemFunc)];
  };
  void *callee;
};

/************************* no arg - no return *******************/
class Functor0:protected FunctorBase{
public:
  Functor0(DummyInit * = 0){}
  void operator()()const
  {
    thunk(*this);
  }
  using FunctorBase::operator int;
protected:
  typedef void (*Thunk)(const FunctorBase &);
  Functor0(Thunk t,const void *c,const void *f,size_t sz):
    FunctorBase(c,f,sz),thunk(t){}
private:
  Thunk thunk;
};

template <class Callee, class MemFunc>
class CBMemberTranslator0:public Functor0{
public:
  CBMemberTranslator0(Callee &c,const MemFunc &m):
    Functor0(thunk,&c,&m,sizeof(MemFunc)){}
  static void thunk(const FunctorBase &ftor)
  {
    Callee *callee = (Callee *)ftor.callee;
    MemFunc &memFunc(*(MemFunc*)(void *)(ftor.memFunc));
    (callee->*memFunc)();
  }
};

template <class Func>
class CBFunctionTranslator0:public Functor0{
public:
  CBFunctionTranslator0(Func f):Functor0(thunk,0,(const void *)f,0){}
  static void thunk(const FunctorBase &ftor)
  {
    (Func(ftor.func))();
  }
};

template <class Callee,class TRT,class CallType>
inline CBMemberTranslator0<Callee,TRT (CallType::*)()>
makeFunctor(Functor0 *,Callee &c,TRT (CallType::* const &f)())
{
  typedef TRT (CallType::*MemFunc)();
  return CBMemberTranslator0<Callee,MemFunc>(c,f);
}

template <class Callee,class TRT,class CallType>
inline CBMemberTranslator0<const Callee,TRT (CallType::*)()const>
makeFunctor(Functor0 *,const Callee &c,TRT (CallType::* const &f)()const)
{
  typedef TRT (CallType::*MemFunc)()const;
  return CBMemberTranslator0<const Callee,MemFunc>(c,f);
}

template <class TRT>
inline CBFunctionTranslator0<TRT (*)()>
makeFunctor(Functor0 *,TRT (*f)())
{
  return CBFunctionTranslator0<TRT (*)()>(f);
}

/************************* no arg - with return *******************/
template <class RT>
class Functor0wRet:protected FunctorBase{
public:
  Functor0wRet(DummyInit * = 0){}
  RT operator()()const
  {
    return BC4_RET_BUG(thunk(*this));
  }
  using FunctorBase::operator int;
protected:
  typedef RT (*Thunk)(const FunctorBase &);
  Functor0wRet(Thunk t,const void *c,const void *f,size_t sz):
    FunctorBase(c,f,sz),thunk(t){}
private:
  Thunk thunk;
};

template <class RT,class Callee, class MemFunc>
class CBMemberTranslator0wRet:public Functor0wRet<RT>{
public:
  CBMemberTranslator0wRet(Callee &c,const MemFunc &m):
    Functor0wRet<RT>(thunk,&c,&m,sizeof(MemFunc)){}
  static RT thunk(const FunctorBase &ftor)
  {
    Callee *callee = (Callee *)ftor.callee;
    MemFunc &memFunc(*(MemFunc*)(void *)(ftor.memFunc));
    return BC4_RET_BUG((callee->*memFunc)());
  }
};

template <class RT,class Func>
class CBFunctionTranslator0wRet:public Functor0wRet<RT>{
public:
  CBFunctionTranslator0wRet(Func f):Functor0wRet<RT>(thunk,0,(const void *)f,0){}
  static RT thunk(const FunctorBase &ftor)
  {
    return (Func(ftor.func))();
  }
};

template <class RT,class Callee,class TRT,class CallType>
inline CBMemberTranslator0wRet<RT,Callee,TRT (CallType::*)()>
makeFunctor(Functor0wRet<RT>*,Callee &c,TRT (CallType::* const &f)())
{
  typedef TRT (CallType::*MemFunc)();
  return CBMemberTranslator0wRet<RT,Callee,MemFunc>(c,f);
}

template <class RT,class Callee,class TRT,class CallType>
inline CBMemberTranslator0wRet<RT,const Callee,TRT (CallType::*)()const>
makeFunctor(Functor0wRet<RT>*,const Callee &c,TRT (CallType::* const &f)()const)
{
  typedef TRT (CallType::*MemFunc)()const;
  return CBMemberTranslator0wRet<RT,const Callee,MemFunc>(c,f);
}

template <class RT,class TRT>
inline CBFunctionTranslator0wRet<RT,TRT (*)()>
makeFunctor(Functor0wRet<RT>*,TRT (*f)())
{
  return CBFunctionTranslator0wRet<RT,TRT (*)()>(f);
}

/************************* one arg - no return *******************/
template <class P1>
class Functor1:protected FunctorBase{
public:
  Functor1(DummyInit * = 0){}
  void operator()(P1 p1)const
  {
    thunk(*this,p1);
  }
  using FunctorBase::operator int;
  //for STL
  typedef P1 argument_type;
  typedef void result_type;
protected:
  typedef void (*Thunk)(const FunctorBase &,P1);
  Functor1(Thunk t,const void *c,const void *f,size_t sz):
    FunctorBase(c,f,sz),thunk(t){}
private:
  Thunk thunk;
};

template <class P1,class Callee, class MemFunc>
class CBMemberTranslator1:public Functor1<P1>{
public:
  CBMemberTranslator1(Callee &c,const MemFunc &m):
    Functor1<P1>(thunk,&c,&m,sizeof(MemFunc)){}
  static void thunk(const FunctorBase &ftor,P1 p1)
  {
    Callee *callee = (Callee *)ftor.callee;
    MemFunc &memFunc(*(MemFunc*)(void *)(ftor.memFunc));
    (callee->*memFunc)(p1);
  }
};

template <class P1,class Func>
class CBFunctionTranslator1:public Functor1<P1>{
public:
  CBFunctionTranslator1(Func f):Functor1<P1>(thunk,0,(const void *)f,0){}
  static void thunk(const FunctorBase &ftor,P1 p1)
  {
    (Func(ftor.func))(p1);
  }
};

template <class P1,class Callee,class TRT,class CallType,class TP1>
inline CBMemberTranslator1<P1,Callee,TRT (CallType::*)(TP1)>
makeFunctor(Functor1<P1>*,Callee &c,TRT (CallType::* const &f)(TP1))
{
  typedef TRT (CallType::*MemFunc)(TP1);
  return CBMemberTranslator1<P1,Callee,MemFunc>(c,f);
}

template <class P1,class Callee,class TRT,class CallType,class TP1>
inline CBMemberTranslator1<P1,const Callee,TRT (CallType::*)(TP1)const>
makeFunctor(Functor1<P1>*,const Callee &c,TRT (CallType::* const &f)(TP1)const)
{
  typedef TRT (CallType::*MemFunc)(TP1)const;
  return CBMemberTranslator1<P1,const Callee,MemFunc>(c,f);
}

template <class P1,class TRT,class TP1>
inline CBFunctionTranslator1<P1,TRT (*)(TP1)>
makeFunctor(Functor1<P1>*,TRT (*f)(TP1))
{
  return CBFunctionTranslator1<P1,TRT (*)(TP1)>(f);
}

template <class P1,class MemFunc>
class CBMemberOf1stArgTranslator1:public Functor1<P1>{
public:
  CBMemberOf1stArgTranslator1(const MemFunc &m):
    Functor1<P1>(thunk,(void *)1,&m,sizeof(MemFunc)){}
  static void thunk(const FunctorBase &ftor,P1 p1)
  {
    MemFunc &memFunc(*(MemFunc*)(void *)(ftor.memFunc));
    (p1.*memFunc)();
  }
};

template <class P1,class TRT,class CallType>
inline CBMemberOf1stArgTranslator1<P1,TRT (CallType::*)()>
makeFunctor(Functor1<P1>*,TRT (CallType::* const &f)())
{
  typedef TRT (CallType::*MemFunc)();
  return CBMemberOf1stArgTranslator1<P1,MemFunc>(f);
}

template <class P1,class TRT,class CallType>
inline CBMemberOf1stArgTranslator1<P1,TRT (CallType::*)()const>
makeFunctor(Functor1<P1>*,TRT (CallType::* const &f)()const)
{
  typedef TRT (CallType::*MemFunc)()const;
  return CBMemberOf1stArgTranslator1<P1,MemFunc>(f);
}

/************************* one arg - with return *******************/
template <class P1,class RT>
class Functor1wRet:protected FunctorBase{
public:
  Functor1wRet(DummyInit * = 0){}
  RT operator()(P1 p1)const
  {
    return BC4_RET_BUG(thunk(*this,p1));
  }
  using FunctorBase::operator int;
  //for STL
  typedef P1 argument_type;
  typedef RT result_type;
protected:
  typedef RT (*Thunk)(const FunctorBase &,P1);
  Functor1wRet(Thunk t,const void *c,const void *f,size_t sz):
    FunctorBase(c,f,sz),thunk(t){}
private:
  Thunk thunk;
};

template <class P1,class RT,class Callee, class MemFunc>
class CBMemberTranslator1wRet:public Functor1wRet<P1,RT>{
public:
  CBMemberTranslator1wRet(Callee &c,const MemFunc &m):
    Functor1wRet<P1,RT>(thunk,&c,&m,sizeof(MemFunc)){}
  static RT thunk(const FunctorBase &ftor,P1 p1)
  {
    Callee *callee = (Callee *)ftor.callee;
    MemFunc &memFunc(*(MemFunc*)(void *)(ftor.memFunc));
    return BC4_RET_BUG((callee->*memFunc)(p1));
  }
};

template <class P1,class RT,class Func>
class CBFunctionTranslator1wRet:public Functor1wRet<P1,RT>{
public:
  //  CBFunctionTranslator1wRet(Func f):Functor1wRet<P1,RT>(thunk,0,(const void *)f,0){}
  //  EBR
  CBFunctionTranslator1wRet(Func f):Functor1wRet<P1,RT>(thunk,0,(void*)f,0){}
  static RT thunk(const FunctorBase &ftor,P1 p1)
  {
    return (Func(ftor.func))(p1);
  }
};

template <class P1,class RT,
          class Callee,class TRT,class CallType,class TP1>
inline CBMemberTranslator1wRet<P1,RT,Callee,TRT (CallType::*)(TP1)>
makeFunctor(Functor1wRet<P1,RT>*,Callee &c,TRT (CallType::* const &f)(TP1))
{
  typedef TRT (CallType::*MemFunc)(TP1);
  return CBMemberTranslator1wRet<P1,RT,Callee,MemFunc>(c,f);
}

template <class P1,class RT,
          class Callee,class TRT,class CallType,class TP1>
inline CBMemberTranslator1wRet<P1,RT,
                               const Callee,TRT (CallType::*)(TP1)const>
makeFunctor(Functor1wRet<P1,RT>*,
            const Callee &c,TRT (CallType::* const &f)(TP1)const)
{
  typedef TRT (CallType::*MemFunc)(TP1)const;
  return CBMemberTranslator1wRet<P1,RT,const Callee,MemFunc>(c,f);
}

template <class P1,class RT,class TRT,class TP1>
inline CBFunctionTranslator1wRet<P1,RT,TRT (*)(TP1)>
makeFunctor(Functor1wRet<P1,RT>*,TRT (*f)(TP1))
{
  return CBFunctionTranslator1wRet<P1,RT,TRT (*)(TP1)>(f);
}

template <class P1,class RT,class MemFunc>
class CBMemberOf1stArgTranslator1wRet:public Functor1wRet<P1,RT>{
public:
  CBMemberOf1stArgTranslator1wRet(const MemFunc &m):
    Functor1wRet<P1,RT>(thunk,(void *)1,&m,sizeof(MemFunc)){}
  static RT thunk(const FunctorBase &ftor,P1 p1)
  {
    MemFunc &memFunc(*(MemFunc*)(void *)(ftor.memFunc));
    return BC4_RET_BUG((p1.*memFunc)());
  }
};

template <class P1,class RT,class TRT,class CallType>
inline CBMemberOf1stArgTranslator1wRet<P1,RT,TRT (CallType::*)()>
makeFunctor(Functor1wRet<P1,RT>*,TRT (CallType::* const &f)())
{
  typedef TRT (CallType::*MemFunc)();
  return CBMemberOf1stArgTranslator1wRet<P1,RT,MemFunc>(f);
}

template <class P1,class RT,class TRT,class CallType>
inline CBMemberOf1stArgTranslator1wRet<P1,RT,TRT (CallType::*)()const>
makeFunctor(Functor1wRet<P1,RT>*,TRT (CallType::* const &f)()const)
{
  typedef TRT (CallType::*MemFunc)()const;
  return CBMemberOf1stArgTranslator1wRet<P1,RT,MemFunc>(f);
}


/************************* two args - no return *******************/
template <class P1,class P2>
class Functor2:protected FunctorBase{
public:
  Functor2(DummyInit * = 0){}
  void operator()(P1 p1,P2 p2)const
  {
    thunk(*this,p1,p2);
  }
  using FunctorBase::operator int;
  //for STL
  typedef P1 first_argument_type;
  typedef P2 second_argument_type;
  typedef void result_type;
protected:
  typedef void (*Thunk)(const FunctorBase &,P1,P2);
  Functor2(Thunk t,const void *c,const void *f,size_t sz):
    FunctorBase(c,f,sz),thunk(t){}
private:
  Thunk thunk;
};

template <class P1,class P2,class Callee, class MemFunc>
class CBMemberTranslator2:public Functor2<P1,P2>{
public:
  CBMemberTranslator2(Callee &c,const MemFunc &m):
    Functor2<P1,P2>(thunk,&c,&m,sizeof(MemFunc)){}
  static void thunk(const FunctorBase &ftor,P1 p1,P2 p2)
  {
    Callee *callee = (Callee *)ftor.callee;
    MemFunc &memFunc(*(MemFunc*)(void *)(ftor.memFunc));
    (callee->*memFunc)(p1,p2);
  }
};

template <class P1,class P2,class Func>
class CBFunctionTranslator2:public Functor2<P1,P2>{
public:
  CBFunctionTranslator2(Func f):Functor2<P1,P2>(thunk,0,(const void *)f,0){}
  static void thunk(const FunctorBase &ftor,P1 p1,P2 p2)
  {
    (Func(ftor.func))(p1,p2);
  }
};

template <class P1,class P2,class Callee,
          class TRT,class CallType,class TP1,class TP2>
inline CBMemberTranslator2<P1,P2,Callee,TRT (CallType::*)(TP1,TP2)>
makeFunctor(Functor2<P1,P2>*,Callee &c,TRT (CallType::* const &f)(TP1,TP2))
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2);
  return CBMemberTranslator2<P1,P2,Callee,MemFunc>(c,f);
}

template <class P1,class P2,class Callee,
          class TRT,class CallType,class TP1,class TP2>
inline CBMemberTranslator2<P1,P2,const Callee,
                           TRT (CallType::*)(TP1,TP2)const>
makeFunctor(Functor2<P1,P2>*,const Callee &c,
            TRT (CallType::* const &f)(TP1,TP2)const)
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2)const;
  return CBMemberTranslator2<P1,P2,const Callee,MemFunc>(c,f);
}

template <class P1,class P2,class TRT,class TP1,class TP2>
inline CBFunctionTranslator2<P1,P2,TRT (*)(TP1,TP2)>
makeFunctor(Functor2<P1,P2>*,TRT (*f)(TP1,TP2))
{
  return CBFunctionTranslator2<P1,P2,TRT (*)(TP1,TP2)>(f);
}

template <class P1,class P2,class MemFunc>
class CBMemberOf1stArgTranslator2:public Functor2<P1,P2>{
public:
  CBMemberOf1stArgTranslator2(const MemFunc &m):
    Functor2<P1,P2>(thunk,(void *)1,&m,sizeof(MemFunc)){}
  static void thunk(const FunctorBase &ftor,P1 p1,P2 p2)
  {
    MemFunc &memFunc(*(MemFunc*)(void *)(ftor.memFunc));
    (p1.*memFunc)(p2);
  }
};

template <class P1,class P2,class TRT,class CallType,class TP1>
inline CBMemberOf1stArgTranslator2<P1,P2,TRT (CallType::*)(TP1)>
makeFunctor(Functor2<P1,P2>*,TRT (CallType::* const &f)(TP1))
{
  typedef TRT (CallType::*MemFunc)(TP1);
  return CBMemberOf1stArgTranslator2<P1,P2,MemFunc>(f);
}

template <class P1,class P2,class TRT,class CallType,class TP1>
inline CBMemberOf1stArgTranslator2<P1,P2,TRT (CallType::*)(TP1)const>
makeFunctor(Functor2<P1,P2>*,TRT (CallType::* const &f)(TP1)const)
{
  typedef TRT (CallType::*MemFunc)(TP1)const;
  return CBMemberOf1stArgTranslator2<P1,P2,MemFunc>(f);
}


/************************* two args - with return *******************/
template <class P1,class P2,class RT>
class Functor2wRet:protected FunctorBase{
public:
  Functor2wRet(DummyInit * = 0){}
  RT operator()(P1 p1,P2 p2)const
  {
    return BC4_RET_BUG(thunk(*this,p1,p2));
  }
  using FunctorBase::operator int;
  //for STL
  typedef P1 first_argument_type;
  typedef P2 second_argument_type;
  typedef RT result_type;
protected:
  typedef RT (*Thunk)(const FunctorBase &,P1,P2);
  Functor2wRet(Thunk t,const void *c,const void *f,size_t sz):
    FunctorBase(c,f,sz),thunk(t){}
private:
  Thunk thunk;
};

template <class P1,class P2,class RT,class Callee, class MemFunc>
class CBMemberTranslator2wRet:public Functor2wRet<P1,P2,RT>{
public:
  CBMemberTranslator2wRet(Callee &c,const MemFunc &m):
    Functor2wRet<P1,P2,RT>(thunk,&c,&m,sizeof(MemFunc)){}
  static RT thunk(const FunctorBase &ftor,P1 p1,P2 p2)
  {
    Callee *callee = (Callee *)ftor.callee;
    MemFunc &memFunc(*(MemFunc*)(void *)(ftor.memFunc));
    return BC4_RET_BUG((callee->*memFunc)(p1,p2));
  }
};

template <class P1,class P2,class RT,class Func>
class CBFunctionTranslator2wRet:public Functor2wRet<P1,P2,RT>{
public:
  //  CBFunctionTranslator2wRet(Func f):Functor2wRet<P1,P2,RT>(thunk,0,(const void *)f,0){}
  //  EBR
  CBFunctionTranslator2wRet(Func f):Functor2wRet<P1,P2,RT>(thunk,0,(void*)f,0){}
  static RT thunk(const FunctorBase &ftor,P1 p1,P2 p2)
  {
    return (Func(ftor.func))(p1,p2);
  }
};

template <class P1,class P2,class RT,class Callee,
          class TRT,class CallType,class TP1,class TP2>
inline CBMemberTranslator2wRet<P1,P2,RT,Callee,
                               TRT (CallType::*)(TP1,TP2)>
makeFunctor(Functor2wRet<P1,P2,RT>*,Callee &c,TRT (CallType::* const &f)(TP1,TP2))
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2);
  return CBMemberTranslator2wRet<P1,P2,RT,Callee,MemFunc>(c,f);
}

template <class P1,class P2,class RT,class Callee,
          class TRT,class CallType,class TP1,class TP2>
inline CBMemberTranslator2wRet<P1,P2,RT,const Callee,
                               TRT (CallType::*)(TP1,TP2)const>
makeFunctor(Functor2wRet<P1,P2,RT>*,const Callee &c,
            TRT (CallType::* const &f)(TP1,TP2)const)
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2)const;
  return CBMemberTranslator2wRet<P1,P2,RT,const Callee,MemFunc>(c,f);
}

template <class P1,class P2,class RT,class TRT,class TP1,class TP2>
inline CBFunctionTranslator2wRet<P1,P2,RT,TRT (*)(TP1,TP2)>
makeFunctor(Functor2wRet<P1,P2,RT>*,TRT (*f)(TP1,TP2))
{
  return CBFunctionTranslator2wRet<P1,P2,RT,TRT (*)(TP1,TP2)>(f);
}

template <class P1,class P2,class RT,class MemFunc>
class CBMemberOf1stArgTranslator2wRet:public Functor2wRet<P1,P2,RT>{
public:
  CBMemberOf1stArgTranslator2wRet(const MemFunc &m):
    Functor2wRet<P1,P2,RT>(thunk,(void *)1,&m,sizeof(MemFunc)){}
  static RT thunk(const FunctorBase &ftor,P1 p1,P2 p2)
  {
    MemFunc &memFunc(*(MemFunc*)(void *)(ftor.memFunc));
    return BC4_RET_BUG((p1.*memFunc)(p2));
  }
};

template <class P1,class P2,class RT,class TRT,class CallType,class TP1>
inline CBMemberOf1stArgTranslator2wRet<P1,P2,RT,TRT (CallType::*)(TP1)>
makeFunctor(Functor2wRet<P1,P2,RT>*,TRT (CallType::* const &f)(TP1))
{
  typedef TRT (CallType::*MemFunc)(TP1);
  return CBMemberOf1stArgTranslator2wRet<P1,P2,RT,MemFunc>(f);
}

template <class P1,class P2,class RT,class TRT,class CallType,class TP1>
inline CBMemberOf1stArgTranslator2wRet<P1,P2,RT,TRT (CallType::*)(TP1)const>
makeFunctor(Functor2wRet<P1,P2,RT>*,TRT (CallType::* const &f)(TP1)const)
{
  typedef TRT (CallType::*MemFunc)(TP1)const;
  return CBMemberOf1stArgTranslator2wRet<P1,P2,RT,MemFunc>(f);
}


/************************* three args - no return *******************/
template <class P1,class P2,class P3>
class Functor3:protected FunctorBase{
public:
  Functor3(DummyInit * = 0){}
  void operator()(P1 p1,P2 p2,P3 p3)const
  {
    thunk(*this,p1,p2,p3);
  }
  using FunctorBase::operator int;
protected:
  typedef void (*Thunk)(const FunctorBase &,P1,P2,P3);
  Functor3(Thunk t,const void *c,const void *f,size_t sz):
    FunctorBase(c,f,sz),thunk(t){}
private:
  Thunk thunk;
};

template <class P1,class P2,class P3,class Callee, class MemFunc>
class CBMemberTranslator3:public Functor3<P1,P2,P3>{
public:
  CBMemberTranslator3(Callee &c,const MemFunc &m):
    Functor3<P1,P2,P3>(thunk,&c,&m,sizeof(MemFunc)){}
  static void thunk(const FunctorBase &ftor,P1 p1,P2 p2,P3 p3)
  {
    Callee *callee = (Callee *)ftor.callee;
    MemFunc &memFunc(*(MemFunc*)(void *)(ftor.memFunc));
    (callee->*memFunc)(p1,p2,p3);
  }
};

template <class P1,class P2,class P3,class Func>
class CBFunctionTranslator3:public Functor3<P1,P2,P3>{
public:
  CBFunctionTranslator3(Func f):Functor3<P1,P2,P3>(thunk,0,(const void *)f,0){}
  static void thunk(const FunctorBase &ftor,P1 p1,P2 p2,P3 p3)
  {
    (Func(ftor.func))(p1,p2,p3);
  }
};

template <class P1,class P2,class P3,class Callee,
          class TRT,class CallType,class TP1,class TP2,class TP3>
inline CBMemberTranslator3<P1,P2,P3,Callee,
                           TRT (CallType::*)(TP1,TP2,TP3)>
makeFunctor(Functor3<P1,P2,P3>*,Callee &c,
            TRT (CallType::* const &f)(TP1,TP2,TP3))
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2,TP3);
  return CBMemberTranslator3<P1,P2,P3,Callee,MemFunc>(c,f);
}

template <class P1,class P2,class P3,class Callee,
          class TRT,class CallType,class TP1,class TP2,class TP3>
inline CBMemberTranslator3<P1,P2,P3,const Callee,
                           TRT (CallType::*)(TP1,TP2,TP3)const>
makeFunctor(Functor3<P1,P2,P3>*,const Callee &c,
            TRT (CallType::* const &f)(TP1,TP2,TP3)const)
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2,TP3)const;
  return CBMemberTranslator3<P1,P2,P3,const Callee,MemFunc>(c,f);
}

template <class P1,class P2,class P3,
          class TRT,class TP1,class TP2,class TP3>
inline CBFunctionTranslator3<P1,P2,P3,TRT (*)(TP1,TP2,TP3)>
makeFunctor(Functor3<P1,P2,P3>*,TRT (*f)(TP1,TP2,TP3))
{
  return CBFunctionTranslator3<P1,P2,P3,TRT (*)(TP1,TP2,TP3)>(f);
}

template <class P1,class P2,class P3,class MemFunc>
class CBMemberOf1stArgTranslator3:public Functor3<P1,P2,P3>{
public:
  CBMemberOf1stArgTranslator3(const MemFunc &m):
    Functor3<P1,P2,P3>(thunk,(void *)1,&m,sizeof(MemFunc)){}
  static void thunk(const FunctorBase &ftor,P1 p1,P2 p2,P3 p3)
  {
    MemFunc &memFunc(*(MemFunc*)(void *)(ftor.memFunc));
    (p1.*memFunc)(p2,p3);
  }
};

template <class P1,class P2,class P3,class TRT,class CallType,
          class TP1,class TP2>
inline CBMemberOf1stArgTranslator3<P1,P2,P3,TRT (CallType::*)(TP1,TP2)>
makeFunctor(Functor3<P1,P2,P3>*,TRT (CallType::* const &f)(TP1,TP2))
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2);
  return CBMemberOf1stArgTranslator3<P1,P2,P3,MemFunc>(f);
}

template <class P1,class P2,class P3,class TRT,class CallType,
          class TP1,class TP2>
inline CBMemberOf1stArgTranslator3<P1,P2,P3,TRT (CallType::*)(TP1,TP2)const>
makeFunctor(Functor3<P1,P2,P3>*,TRT (CallType::* const &f)(TP1,TP2)const)
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2)const;
  return CBMemberOf1stArgTranslator3<P1,P2,P3,MemFunc>(f);
}


/************************* three args - with return *******************/
template <class P1,class P2,class P3,class RT>
class Functor3wRet:protected FunctorBase{
public:
  Functor3wRet(DummyInit * = 0){}
  RT operator()(P1 p1,P2 p2,P3 p3)const
  {
    return BC4_RET_BUG(thunk(*this,p1,p2,p3));
  }
  using FunctorBase::operator int;
protected:
  typedef RT (*Thunk)(const FunctorBase &,P1,P2,P3);
  Functor3wRet(Thunk t,const void *c,const void *f,size_t sz):
    FunctorBase(c,f,sz),thunk(t){}
private:
  Thunk thunk;
};

template <class P1,class P2,class P3,
          class RT,class Callee, class MemFunc>
class CBMemberTranslator3wRet:public Functor3wRet<P1,P2,P3,RT>{
public:
  CBMemberTranslator3wRet(Callee &c,const MemFunc &m):
    Functor3wRet<P1,P2,P3,RT>(thunk,&c,&m,sizeof(MemFunc)){}
  static RT thunk(const FunctorBase &ftor,P1 p1,P2 p2,P3 p3)
  {
    Callee *callee = (Callee *)ftor.callee;
    MemFunc &memFunc(*(MemFunc*)(void *)(ftor.memFunc));
    return BC4_RET_BUG((callee->*memFunc)(p1,p2,p3));
  }
};

template <class P1,class P2,class P3,class RT,class Func>
class CBFunctionTranslator3wRet:public Functor3wRet<P1,P2,P3,RT>{
public:
  CBFunctionTranslator3wRet(Func f):
    Functor3wRet<P1,P2,P3,RT>(thunk,0,(const void *)f,0){}
  static RT thunk(const FunctorBase &ftor,P1 p1,P2 p2,P3 p3)
  {
    return (Func(ftor.func))(p1,p2,p3);
  }
};

template <class P1,class P2,class P3,class RT,class Callee,
          class TRT,class CallType,class TP1,class TP2,class TP3>
inline CBMemberTranslator3wRet<P1,P2,P3,RT,Callee,
                               TRT (CallType::*)(TP1,TP2,TP3)>
makeFunctor(Functor3wRet<P1,P2,P3,RT>*,Callee &c,
            TRT (CallType::* const &f)(TP1,TP2,TP3))
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2,TP3);
  return CBMemberTranslator3wRet<P1,P2,P3,RT,Callee,MemFunc>(c,f);
}

template <class P1,class P2,class P3,class RT,class Callee,
          class TRT,class CallType,class TP1,class TP2,class TP3>
inline CBMemberTranslator3wRet<P1,P2,P3,RT,const Callee,
                               TRT (CallType::*)(TP1,TP2,TP3)const>
makeFunctor(Functor3wRet<P1,P2,P3,RT>*,const Callee &c,
            TRT (CallType::* const &f)(TP1,TP2,TP3)const)
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2,TP3)const;
  return CBMemberTranslator3wRet<P1,P2,P3,RT,const Callee,MemFunc>(c,f);
}

template <class P1,class P2,class P3,class RT,
          class TRT,class TP1,class TP2,class TP3>
inline CBFunctionTranslator3wRet<P1,P2,P3,RT,TRT (*)(TP1,TP2,TP3)>
makeFunctor(Functor3wRet<P1,P2,P3,RT>*,TRT (*f)(TP1,TP2,TP3))
{
  return CBFunctionTranslator3wRet<P1,P2,P3,RT,TRT (*)(TP1,TP2,TP3)>(f);
}

template <class P1,class P2,class P3,class RT,class MemFunc>
class CBMemberOf1stArgTranslator3wRet:public Functor3wRet<P1,P2,P3,RT>{
public:
  CBMemberOf1stArgTranslator3wRet(const MemFunc &m):
    Functor3wRet<P1,P2,P3,RT>(thunk,(void *)1,&m,sizeof(MemFunc)){}
  static RT thunk(const FunctorBase &ftor,P1 p1,P2 p2,P3 p3)
  {
    MemFunc &memFunc(*(MemFunc*)(void *)(ftor.memFunc));
    return BC4_RET_BUG((p1.*memFunc)(p2,p3));
  }
};

template <class P1,class P2,class P3,class RT,class TRT,class CallType,
          class TP1,class TP2>
inline CBMemberOf1stArgTranslator3wRet<P1,P2,P3,RT,TRT (CallType::*)(TP1,TP2)>
makeFunctor(Functor3wRet<P1,P2,P3,RT>*,TRT (CallType::* const &f)(TP1,TP2))
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2);
  return CBMemberOf1stArgTranslator3wRet<P1,P2,P3,RT,MemFunc>(f);
}

template <class P1,class P2,class P3,class RT,class TRT,class CallType,
          class TP1,class TP2>
inline CBMemberOf1stArgTranslator3wRet<P1,P2,P3,RT,
                                       TRT (CallType::*)(TP1,TP2)const>
makeFunctor(Functor3wRet<P1,P2,P3,RT>*,
            TRT (CallType::* const &f)(TP1,TP2)const)
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2)const;
  return CBMemberOf1stArgTranslator3wRet<P1,P2,P3,RT,MemFunc>(f);
}


/************************* four args - no return *******************/
template <class P1,class P2,class P3,class P4>
class Functor4:protected FunctorBase{
public:
  Functor4(DummyInit * = 0){}
  void operator()(P1 p1,P2 p2,P3 p3,P4 p4)const
  {
    thunk(*this,p1,p2,p3,p4);
  }
  using FunctorBase::operator int;
protected:
  typedef void (*Thunk)(const FunctorBase &,P1,P2,P3,P4);
  Functor4(Thunk t,const void *c,const void *f,size_t sz):
    FunctorBase(c,f,sz),thunk(t){}
private:
  Thunk thunk;
};

template <class P1,class P2,class P3,class P4,
          class Callee, class MemFunc>
class CBMemberTranslator4:public Functor4<P1,P2,P3,P4>{
public:
  CBMemberTranslator4(Callee &c,const MemFunc &m):
    Functor4<P1,P2,P3,P4>(thunk,&c,&m,sizeof(MemFunc)){}
  static void thunk(const FunctorBase &ftor,P1 p1,P2 p2,P3 p3,P4 p4)
  {
    Callee *callee = (Callee *)ftor.callee;
    MemFunc &memFunc(*(MemFunc*)(void *)(ftor.memFunc));
    (callee->*memFunc)(p1,p2,p3,p4);
  }
};

template <class P1,class P2,class P3,class P4,class Func>
class CBFunctionTranslator4:public Functor4<P1,P2,P3,P4>{
public:
  //        CBFunctionTranslator4(Func f):Functor4<P1,P2,P3,P4>(thunk,0,(const void *)f,0){}
  //EBR
  CBFunctionTranslator4(Func f):Functor4<P1,P2,P3,P4>(thunk,0,(void*)f,0){}
  static void thunk(const FunctorBase &ftor,P1 p1,P2 p2,P3 p3,P4 p4)
  {
    (Func(ftor.func))(p1,p2,p3,p4);
  }
};

template <class P1,class P2,class P3,class P4,class Callee,
          class TRT,class CallType,class TP1,class TP2,class TP3,class TP4>
inline CBMemberTranslator4<P1,P2,P3,P4,Callee,
                           TRT (CallType::*)(TP1,TP2,TP3,TP4)>
makeFunctor(Functor4<P1,P2,P3,P4>*,Callee &c,
            TRT (CallType::* const &f)(TP1,TP2,TP3,TP4))
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2,TP3,TP4);
  return CBMemberTranslator4<P1,P2,P3,P4,Callee,MemFunc>(c,f);
}

template <class P1,class P2,class P3,class P4,class Callee,
          class TRT,class CallType,class TP1,class TP2,class TP3,class TP4>
inline CBMemberTranslator4<P1,P2,P3,P4,const Callee,
                           TRT (CallType::*)(TP1,TP2,TP3,TP4)const>
makeFunctor(Functor4<P1,P2,P3,P4>*,const Callee &c,
            TRT (CallType::* const &f)(TP1,TP2,TP3,TP4)const)
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2,TP3,TP4)const;
  return CBMemberTranslator4<P1,P2,P3,P4,const Callee,MemFunc>(c,f);
}

template <class P1,class P2,class P3,class P4,
          class TRT,class TP1,class TP2,class TP3,class TP4>
inline CBFunctionTranslator4<P1,P2,P3,P4,TRT (*)(TP1,TP2,TP3,TP4)>
makeFunctor(Functor4<P1,P2,P3,P4>*,TRT (*f)(TP1,TP2,TP3,TP4))
{
  return CBFunctionTranslator4<P1,P2,P3,P4,TRT (*)(TP1,TP2,TP3,TP4)>(f);
}

template <class P1,class P2,class P3,class P4,class MemFunc>
class CBMemberOf1stArgTranslator4:public Functor4<P1,P2,P3,P4>{
public:
  CBMemberOf1stArgTranslator4(const MemFunc &m):
    Functor4<P1,P2,P3,P4>(thunk,(void *)1,&m,sizeof(MemFunc)){}
  static void thunk(const FunctorBase &ftor,P1 p1,P2 p2,P3 p3,P4 p4)
  {
    MemFunc &memFunc(*(MemFunc*)(void *)(ftor.memFunc));
    (p1.*memFunc)(p2,p3,p4);
  }
};

template <class P1,class P2,class P3,class P4,class TRT,class CallType,
          class TP1,class TP2,class TP3>
inline CBMemberOf1stArgTranslator4<P1,P2,P3,P4,TRT (CallType::*)(TP1,TP2,TP3)>
makeFunctor(Functor4<P1,P2,P3,P4>*,TRT (CallType::* const &f)(TP1,TP2,TP3))
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2,TP3);
  return CBMemberOf1stArgTranslator4<P1,P2,P3,P4,MemFunc>(f);
}

template <class P1,class P2,class P3,class P4,class TRT,class CallType,
          class TP1,class TP2,class TP3>
inline CBMemberOf1stArgTranslator4<P1,P2,P3,P4,
                                   TRT (CallType::*)(TP1,TP2,TP3)const>
makeFunctor(Functor4<P1,P2,P3,P4>*,
            TRT (CallType::* const &f)(TP1,TP2,TP3)const)
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2,TP3)const;
  return CBMemberOf1stArgTranslator4<P1,P2,P3,P4,MemFunc>(f);
}


/************************* four args - with return *******************/
template <class P1,class P2,class P3,class P4,class RT>
class Functor4wRet:protected FunctorBase{
public:
  Functor4wRet(DummyInit * = 0){}
  RT operator()(P1 p1,P2 p2,P3 p3,P4 p4)const
  {
    return BC4_RET_BUG(thunk(*this,p1,p2,p3,p4));
  }
  using FunctorBase::operator int;
protected:
  typedef RT (*Thunk)(const FunctorBase &,P1,P2,P3,P4);
  Functor4wRet(Thunk t,const void *c,const void *f,size_t sz):
    FunctorBase(c,f,sz),thunk(t){}
private:
  Thunk thunk;
};

template <class P1,class P2,class P3,class P4,class RT,
          class Callee, class MemFunc>
class CBMemberTranslator4wRet:public Functor4wRet<P1,P2,P3,P4,RT>{
public:
  CBMemberTranslator4wRet(Callee &c,const MemFunc &m):
    Functor4wRet<P1,P2,P3,P4,RT>(thunk,&c,&m,sizeof(MemFunc)){}
  static RT thunk(const FunctorBase &ftor,P1 p1,P2 p2,P3 p3,P4 p4)
  {
    Callee *callee = (Callee *)ftor.callee;
    MemFunc &memFunc(*(MemFunc*)(void *)(ftor.memFunc));
    return BC4_RET_BUG((callee->*memFunc)(p1,p2,p3,p4));
  }
};

template <class P1,class P2,class P3,class P4,class RT,class Func>
class CBFunctionTranslator4wRet:public Functor4wRet<P1,P2,P3,P4,RT>{
public:
  CBFunctionTranslator4wRet(Func f):
    Functor4wRet<P1,P2,P3,P4,RT>(thunk,0,(const void *)f,0){}
  static RT thunk(const FunctorBase &ftor,P1 p1,P2 p2,P3 p3,P4 p4)
  {
    return (Func(ftor.func))(p1,p2,p3,p4);
  }
};

template <class P1,class P2,class P3,class P4,class RT,class Callee,
          class TRT,class CallType,class TP1,class TP2,class TP3,class TP4>
inline CBMemberTranslator4wRet<P1,P2,P3,P4,RT,Callee,
                               TRT (CallType::*)(TP1,TP2,TP3,TP4)>
makeFunctor(Functor4wRet<P1,P2,P3,P4,RT>*,Callee &c,
            TRT (CallType::* const &f)(TP1,TP2,TP3,TP4))
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2,TP3,TP4);
  return CBMemberTranslator4wRet<P1,P2,P3,P4,RT,Callee,MemFunc>(c,f);
}

template <class P1,class P2,class P3,class P4,class RT,class Callee,
          class TRT,class CallType,class TP1,class TP2,class TP3,class TP4>
inline CBMemberTranslator4wRet<P1,P2,P3,P4,RT,const Callee,
                               TRT (CallType::*)(TP1,TP2,TP3,TP4)const>
makeFunctor(Functor4wRet<P1,P2,P3,P4,RT>*,const Callee &c,
            TRT (CallType::* const &f)(TP1,TP2,TP3,TP4)const)
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2,TP3,TP4)const;
  return CBMemberTranslator4wRet<P1,P2,P3,P4,RT,const Callee,MemFunc>(c,f);
}

template <class P1,class P2,class P3,class P4,class RT,
          class TRT,class TP1,class TP2,class TP3,class TP4>
inline CBFunctionTranslator4wRet<P1,P2,P3,P4,RT,TRT (*)(TP1,TP2,TP3,TP4)>
makeFunctor(Functor4wRet<P1,P2,P3,P4,RT>*,TRT (*f)(TP1,TP2,TP3,TP4))
{
  return CBFunctionTranslator4wRet
    <P1,P2,P3,P4,RT,TRT (*)(TP1,TP2,TP3,TP4)>(f);
}


template <class P1,class P2,class P3,class P4,class RT,class MemFunc>
class CBMemberOf1stArgTranslator4wRet:public Functor4wRet<P1,P2,P3,P4,RT>{
public:
  CBMemberOf1stArgTranslator4wRet(const MemFunc &m):
    Functor4wRet<P1,P2,P3,P4,RT>(thunk,(void *)1,&m,sizeof(MemFunc)){}
  static RT thunk(const FunctorBase &ftor,P1 p1,P2 p2,P3 p3,P4 p4)
  {
    MemFunc &memFunc(*(MemFunc*)(void *)(ftor.memFunc));
    return BC4_RET_BUG((p1.*memFunc)(p2,p3,p4));
  }
};

template <class P1,class P2,class P3,class P4,class RT,class TRT,
          class CallType,class TP1,class TP2,class TP3>
inline CBMemberOf1stArgTranslator4wRet<P1,P2,P3,P4,RT,
                                       TRT (CallType::*)(TP1,TP2,TP3)>
makeFunctor(Functor4wRet<P1,P2,P3,P4,RT>*,
            TRT (CallType::* const &f)(TP1,TP2,TP3))
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2,TP3);
  return CBMemberOf1stArgTranslator4wRet<P1,P2,P3,P4,RT,MemFunc>(f);
}

template <class P1,class P2,class P3,class P4,class RT,class TRT,
          class CallType,class TP1,class TP2,class TP3>
inline CBMemberOf1stArgTranslator4wRet<P1,P2,P3,P4,RT,
                                       TRT (CallType::*)(TP1,TP2,TP3)const>
makeFunctor(Functor4wRet<P1,P2,P3,P4,RT>*,
            TRT (CallType::* const &f)(TP1,TP2,TP3)const)
{
  typedef TRT (CallType::*MemFunc)(TP1,TP2,TP3)const;
  return CBMemberOf1stArgTranslator4wRet<P1,P2,P3,P4,RT,MemFunc>(f);
}


#endif //FUNCTOR_H
