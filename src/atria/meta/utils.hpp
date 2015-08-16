// Copyright: 2014, 2015, Ableton AG, Berlin. All rights reserved.
/*!
 * @file
 */

#pragma once

namespace atria {
namespace meta {

/*!
 * This is a type that pretends to be convertible to anything.  This
 * can be used as a placeholder for any parameter type in `result_of`
 * metacalls.
 *
 * @note Conversion is not defined. Only use in non-evaluated contexes.
 */
struct bottom
{
  template <typename T> operator T();
};

/*!
 * Type to enable making a type convertible from `void`.  The problem
 * is that `void` can not be used as a parameter, so it is imposible
 * to define a convertion between `void` and some types, in the
 * degenerate cases where it is desirable.
 *
 * @see could_not_find_common_type
 */
struct from_void {};

/*!
 * Does nothing.
 */
constexpr struct noop_t
{
  template <typename ...Args>
  void operator() (Args&&...) const {}
} noop {};

/*!
 * Use to allow a `void` expression by chaining it in a comma operator
 * thing.  A use-case is when expanding calls to variadic arguments
 * that might be void, as in:
 *
 * @code{.cpp}
 * template <typename ...Args>
 * void do_foo(Args& args)
 * {
 *   using namespace meta;
 *   noop((args.foo(), can_be_void)...);
 * }
 * @endcode
 *
 * Another example is when writing concepts checking expressions that
 * can be void, as in:
 *
 * @code{.cpp}
 * template <typename Arg1, typename Arg2=Arg1>
 * struct swapable : concept<swapable<Arg1, Arg2>>
 * {
 *   template <typename T, typename U>
 *   auto requires_(T&& x, T&& y) -> decltype(
 *     expressions(
 *       (swap(x, y), can_be_void),
 *       (swap(y, x), can_be_void)));
 * };
 * @endcode
 */
constexpr int can_be_void = 42;

/*!
 * Utility for defining generic functions with a deduced return type,
 * that are composed of a single expression.
 */
#define ABL_DECLTYPE_RETURN(body_expr)  \
  decltype(body_expr) { return (body_expr); }


#if ABL_CXX14
/*!
 * Macro forward universal-reference arguments.  This is only defined
 * in C++14, since outside of generic lambdas the macro-less syntax
 * should be preferred.
 */
#define ABL_FORWARD(x) std::forward<decltype(x)>(x)
#endif // ABL_CXX14

} // namespace meta
} // namespace atria
