// Copyright: 2014, Ableton AG, Berlin. All rights reserved.

#pragma once

#include <ableton/base/meta/CommonType.hpp>

#include <boost/variant/static_visitor.hpp>

#include <type_traits>
#include <cassert>
#include <utility>

namespace ableton {
namespace base {
namespace variant {

namespace detail {

//!
// General visitor based on a set of function objects.
//
template <typename ...Fns>
struct VisitorImpl : public Fns...
{
  VisitorImpl(Fns&& ...fns)
    : Fns(std::forward<Fns>(fns))... { }
};

} // namespace detail

//!
// General visitor based on a set of function objects.
//
template <typename ReturnType, typename ...Fns>
struct Visitor : public boost::static_visitor<ReturnType>
{
  Visitor(Fns&& ...fns)
    : mImpl(std::forward<Fns>(fns)...)
  {}

  template<typename T>
  ReturnType operator() (T&& x)
  {
    return mImpl(std::forward<T>(x));
  }

private:
  detail::VisitorImpl<Fns...> mImpl;
};

namespace detail
{

//!
// This is a type that pretends to be convertible to anything. This
// can be used as a placeholder for any parameter type in `result_of`
// metacalls.
//
struct Bottom
{
  template <typename T> operator T&();
  template <typename T> operator const T&();
};

} // namespace detail

//!
// Wraps a functor such that it has a fixed return value. Can be
// use to disambiguate cases of a variant visitor.
//
template <typename Fn, typename ReturnType=void>
struct Otherwise
{
  using result_type = ReturnType;

  Otherwise(Fn&& fn)
    : mImpl(std::forward<Fn>(fn))
  {};

  template <typename ...Args>
  result_type operator() (Args&& ...args)
  {
    return mImpl(std::forward<Args>(args)...);
  }

private:
  Fn mImpl;
};

namespace detail {

template<typename ReturnT>
struct Default
{
  template <typename ...Args>
  ReturnT operator() (Args&& ...args)
  {
    return ReturnT();
  }
};

} // namespace detail

template <typename ReturnType=void, typename Fn=detail::Default<ReturnType>>
Otherwise<Fn, ReturnType>
otherwise(Fn&& fn = Fn())
{
  return { std::forward<Fn>(fn) };
}

//!
// Wraps a functor such that it has a fixed argument list.  Can be
// use to disambiguate cases of a variant visitor.
//
template <typename Fn, typename ...Args>
struct When
{
  using result_type = typename std::result_of<Fn(Args...)>::type;

  When(Fn&& fn)
    : mImpl(std::forward<Fn>(fn))
  {};

  result_type operator() (Args&& ...args)
  {
    return mImpl(std::forward<Args>(args)...);
  }

private:
  Fn mImpl;
};

//!
// Factory for when functors.  Returns a wrapper for functor F with
// signature fixed to `Args`.  Copy arguments are converted to
// `const&`, which eases use with `std::bind` and `std::mem_fn`.
//
template <typename ...Args, typename Fn>
When<Fn,
     typename boost::mpl::if_<std::is_reference<Args>,
                              Args, const Args&>::type...>
when(Fn&& fn)
{
  return { std::forward<Fn>(fn) };
}


//!
// Returns a boost variant static_visitor that uses a set of given
// functions to implement it. The return type is inferred from the
// first function passed.
//
template <typename... Fns>
Visitor<
  typename meta::CommonType<
      typename std::result_of<Fns(detail::Bottom)>::type...
    >::type,
  Fns...>
visitor(Fns&& ...fns)
{
  return { std::forward<Fns>(fns)... };
}

} // namespace variant
} // namespace base
} // namespace ableton
