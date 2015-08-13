// Copyright: 2014, 2015, Ableton AG, Berlin. All rights reserved.

#include <atria/xform/any_state.hpp>
#include <atria/xform/concepts.hpp>
#include <atria/xform/transduce.hpp>
#include <atria/xform/transducer/filter.hpp>
#include <atria/xform/transducer/map.hpp>
#include <atria/xform/transducer/take.hpp>

#include <atria/prelude/identity.hpp>
#include <atria/estd/functional.hpp>
#include <atria/testing/gtest.hpp>
#include <vector>

namespace atria {
namespace xform {

TEST(transduce, identity)
{
  auto v = std::vector<int> { 1, 2, 3, 6 };
  EXPECT_EQ(transduce(identity, std::plus<int>{}, 1, v), 13);
}

TEST(transduce, variadic)
{
  auto v1 = std::vector<int> { 1, 2, 3, 6 };
  auto v2 = std::vector<int> { 1, 2, 1, 2 };

  EXPECT_EQ(transduce(map(std::multiplies<int>{}),
                      std::plus<int>{},
                      1,
                      v1, v2),
            21);
}

TEST(transduce, variadic_different_lengths)
{
  auto v1 = std::vector<int> { 1, 2, 3, 6 };
  auto v2 = std::vector<int> { 1, 2, 1 };

  EXPECT_EQ(transduce(map(std::multiplies<int>{}),
                      std::plus<int>{},
                      1,
                      v1, v2),
            9);
}

TEST(transduce, early_termination_does_not_leak)
{
  auto v1 = { 1, 2, 3, 4 };

  EXPECT_EQ(transduce(take(2),
                      std::plus<int>{},
                      0,
                      v1),
            3);
}

} // namespace xform
} // namespace atria
