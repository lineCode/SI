#include <catch.hpp>

#include <SI/detail/unit.h>
#include <ratio>
#include <type_traits>

using namespace SI::detail;

TEMPLATE_TEST_CASE(
    "given two values with exponent 1 WHEN multiplied THEN exponent is 2",
    "[unit_t][operator*]", int64_t, long double) {
  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v1{0};

  constexpr auto result = v1 * v1;

  static_assert(decltype(result)::exponent::value == 2,
                "Exponent value is two");
}

TEMPLATE_TEST_CASE(
    "given two units with different non-negative values AND same ratio "
    "AND same unit WHEN "
    "multiplied THEN resulting value is correct",
    "[unit_t][operator*]", int64_t, long double) {

  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v1{20};
  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v2{30};

  constexpr auto result = v1 * v2;

  static_assert(result == unit_t<'X', 2, TestType, std::ratio<1>>{600},
                "Raw value matches");
}

TEMPLATE_TEST_CASE(
    "given two units with different non-negative values AND different ratio "
    "AND same unit WHEN "
    "multiplied THEN resulting value is correct AND resulting ratio is ratio "
    "multiplied",
    "[unit_t][operator*]", int64_t, long double) {

  constexpr unit_t<'X', 1, TestType, std::deci> v1{2};
  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v2{30};

  constexpr auto result = v1 * v2;

  static_assert(result == unit_t<'X', 2, TestType, std::centi>{600},
                "value matches");
  static_assert(
      std::is_same<decltype(result),
                   const unit_t<'X', 2, TestType, std::centi>>::value);
}

TEMPLATE_TEST_CASE("given two units with different values AND different ratio "
                   "AND same unit WHEN "
                   "multiplied THEN resulting value is correct",
                   "[unit_t][operator*]", int64_t, long double) {

  constexpr unit_t<'X', 1, TestType, std::deci> v1{2};
  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v2{-30};

  constexpr auto result = v1 * v2;

  static_assert(result == unit_t<'X', 2, TestType, std::centi>{-600},
                "value matches");
  static_assert(
      std::ratio_equal<typename decltype(result)::ratio, std::centi>::value,
      "result is greatest common denominator squared");
}

/* This test is not templatized because of the == comparison of the raw values,
 Which does not work with floating points*/
TEST_CASE(
    "given two units with different values AND ratio of rhs is small AND type "
    "is integer WHEN "
    "multiplied THEN resulting ratio is ratios multiplied and value is correct",
    "[unit_t][operator*]") {

  constexpr unit_t<'X', 1, int64_t, std::ratio<1>> v1{2};
  constexpr unit_t<'X', 1, int64_t, std::milli> v2{30};

  constexpr auto result = v1 * v2;

  static_assert(result == unit_t<'X', 2, int64_t, std::micro>{60000},
                "value matches");
  static_assert(result.raw_value() == 60000);
  static_assert(
      std::ratio_equal<typename decltype(result)::ratio, std::micro>::value,
      "ratio is multiplied");
}

/* This test is not templatized because of the epsEqual comparison of the raw
 * values*/
TEST_CASE(
    "given two units with different values AND ratio of rhs is small AND type "
    "is floating point WHEN "
    "multiplied THEN resulting type is of left hand side and value is a "
    "fraction but does not match epsilon",
    "[unit_t][operator*]") {

  constexpr unit_t<'X', 1, long double, std::ratio<1>> v1{2};
  constexpr unit_t<'X', 1, long double, std::milli> v2{20};

  constexpr auto result = v1 * v2;

  constexpr auto expected =
      v1 * unit_cast<unit_t<'X', 1, long double, std::ratio<1>>>(v2);

  static_assert(epsEqual(result.raw_value(), 40000.0L));
  static_assert(result == expected, "value matches");
  static_assert(std::ratio_equal<typename decltype(result)::ratio,
                                 typename std::micro>::value,
                "ratio is gcd squared");
}

TEMPLATE_TEST_CASE("GIVEN two units with different exponents WHEN divided THEN "
                   "resulting exopnent is left exponent - right exponent",
                   "[unit_t][operator/]", int64_t, long double) {
  constexpr unit_t<'X', 3, TestType, std::ratio<1>> v1{1};
  constexpr unit_t<'X', 2, TestType, std::ratio<1>> v2{1};

  constexpr auto result = v1 / v2;

  static_assert(decltype(result)::exponent::value == 1,
                "Exponent is subtracted");
}

TEMPLATE_TEST_CASE(
    "GIVEN two units with exponent 1 WHEN divided result in a raw integer",
    "[unit_t][operator/]", int64_t, long double) {
  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v1{1};
  constexpr auto result = v1 / v1;

  static_assert(std::is_same<typename std::remove_const<decltype(result)>::type,
                             typename decltype(v1)::internal_type>::value,
                "raw internal type is returned");
}

TEST_CASE("GIVEN two units with the same ratio exponent 1 AND internal type is "
          "integral type  WHEN divided result "
          "is lhs.value / "
          "rhs.value",
          "[unit_t][operator/]") {
  constexpr unit_t<'X', 1, int64_t, std::ratio<1>> v1{1000};
  constexpr unit_t<'X', 1, int64_t, std::ratio<1>> v2{10};
  constexpr auto result = v1 / v2;

  static_assert(std::is_same<std::remove_const<decltype(result)>::type,
                             decltype(v1)::internal_type>::value,
                "raw internal type is returned");
  static_assert(result == 100, "division by 10");
}

TEST_CASE("GIVEN two units with the same ratio exponent 1 AND internal type is "
          "floating point WHEN divided result "
          "is lhs.value / "
          "rhs.value",
          "[unit_t][operator/]") {
  constexpr unit_t<'X', 1, long double, std::ratio<1>> v1{1000};
  constexpr unit_t<'X', 1, long double, std::ratio<1>> v2{10};
  constexpr auto result = v1 / v2;

  static_assert(std::is_same<std::remove_const<decltype(result)>::type,
                             decltype(v1)::internal_type>::value,
                "raw internal type is returned");
  static_assert(epsEqual(result, 100.0L), "division by 10");
}

TEMPLATE_TEST_CASE(
    "GIVEN a unit and a scalar WHEN scalar is divided by unit THEN unit "
    "exponent is negative",
    "[unit_t][operator/]", int64_t, long double) {
  constexpr TestType v1{1};
  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v2{1};
  constexpr auto result = v1 / v2;

  static_assert(decltype(result)::exponent::value == -1,
                "Exponent is negative");
}

TEST_CASE("GIVEN a unit with ratio<1> and a scalar AND internal type is "
          "integral WHEN scalar is divided by "
          "unit THEN resulting  value is scalar / unit.value",
          "[unit_t][operator/]") {
  constexpr int64_t v1{1000};
  constexpr unit_t<'X', 1, int64_t, std::ratio<1>> v2{2};
  constexpr auto result = v1 / v2;

  static_assert(result.raw_value() == 500, "1000 / 2 = 500");
}

TEST_CASE("GIVEN a unit with ratio<1> and a scalar AND internal type is "
          "floating point WHEN scalar is divided by "
          "unit THEN resulting  value is scalar / unit.value",
          "[unit_t][operator/]") {
  constexpr long double v1 = 1000.0L;
  constexpr unit_t<'X', 1, long double, std::ratio<1>> v2{2};
  constexpr auto result = v1 / v2;

  static_assert(epsEqual(result.raw_value(), 500.0L), "1000 / 2 = 500");
}

TEST_CASE("GIVEN a unit with ratio<1, 1000> and a scalar AND interal type is "
          "integral WHEN scalar is dived "
          "by unit THEN resulting value is adjusted by ratio",
          "[unit_t][operator/]") {
  constexpr int64_t v1{1000};
  constexpr unit_t<'X', 1, int64_t, std::deca> v2{2};

  constexpr auto result = v1 / v2;
  constexpr unit_t<'X', -1, int64_t, std::deca> expected{5};
  static_assert(v2.raw_value() == 2, "Is 2");
  static_assert(std::ratio_equal<std::deca, decltype(result)::ratio>::value,
                "Is of deca type");
  static_assert(result.raw_value() == 5, "1000 / 20 = 50");
  static_assert(result == expected, "1000 / 20 = 50");
}

TEST_CASE("GIVEN a unit with ratio<1, 1000> and a scalar AND interal type is "
          "floating point WHEN scalar is dived "
          "by unit THEN resulting value is adjusted by ratio",
          "[unit_t][operator/]") {
  constexpr long double v1{1000};
  constexpr unit_t<'X', 1, long double, std::deca> v2{2};

  constexpr auto result = v1 / v2;
  constexpr unit_t<'X', -1, long double, std::deca> expected{5};
  static_assert(v2.raw_value() == 2, "Is 2");
  static_assert(std::ratio_equal<std::deca, decltype(result)::ratio>::value,
                "Is of deca type");
  static_assert(epsEqual(result.raw_value(), 5.0L), "1000 / 20 = 50");
  static_assert(result == expected, "1000 / 20 = 50");
}

TEMPLATE_TEST_CASE(
    "GIVEN a unit WHEN added to itself THEN resulting value value * 2",
    "[unit_t][operator+]", int64_t, long double) {
  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v1{1};
  constexpr auto result = v1 + v1;

  static_assert(result.raw_value() == 2, "Result is raw_value * 2");
}

TEMPLATE_TEST_CASE(
    "GIVEN two units with different ratios WHEN added together THEN result is "
    "lhs + rhs with ratio considered AND result is of ratio of lhs",
    "[unit_t][operator+]", int64_t, long double) {
  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v1{1};
  constexpr unit_t<'X', 1, TestType, std::kilo> v2{1};

  constexpr auto result = v1 + v2;
  static_assert(std::ratio_equal<typename decltype(result)::ratio,
                                 typename decltype(v1)::ratio>::value == true,
                "ratio is of same ratio as left hand side");
  static_assert(result.raw_value() == 1001, "Result is correct");
}

TEMPLATE_TEST_CASE("GIVEN two units with value difference "
                   "smaller than type::epsilon WHEN compared for equality "
                   "THEN result is true",
                   "[unit_t][operator==]", int64_t, long double) {
  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v1{0};
  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v2{
      std::numeric_limits<TestType>::epsilon() / static_cast<TestType>(2)};

  static_assert(v1 == v2, "Result is the same");
  static_assert(!(v1 != v2), "Result is the same");
}

TEST_CASE("GIVEN two units with floating point types with value difference of "
          "epsilon WHEN compared for equality"
          "THEN result is false",
          "[unit_t][operator==]") {
  constexpr unit_t<'X', 1, long double, std::ratio<1>> v1{0};
  constexpr unit_t<'X', 1, long double, std::ratio<1>> v2{
      std::numeric_limits<long double>::epsilon()};

  static_assert(v1 != v2, "Result is the not the same");
  static_assert(!(v1 == v2), "Result is the not the same");
}

TEMPLATE_TEST_CASE(
    "GIVEN two units same absolute value AND different ratios WHEN "
    "compared for equality THEN result in true",
    "[unit_t][operator==]", int64_t, long double) {
  constexpr unit_t<'X', 1, TestType, std::milli> v1{1000};
  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v2{1};

  static_assert(v1 == v2, "values are equal");
  static_assert(!(v2 != v1), "values are equal");
}

TEMPLATE_TEST_CASE(
    "GIVEN two units with same absolute value AND same ratio WHEN "
    "compared with less than operator THEN result is false",
    "[unit_t][operator<]", int64_t, long double) {

  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v1{1};

  static_assert(!(v1 < v1), "Less than comparison is false");
}

TEMPLATE_TEST_CASE(
    "GIVEN two units v1 and v2 AND v1 is smaller than v2 AND same ratio WHEN "
    "compared with less than operator THEN result is true",
    "[unit_t][operator<]", int64_t, long double) {

  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v1{1};
  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v2{0};

  static_assert(v1 < v2, "Less than comparison is true");
}

TEMPLATE_TEST_CASE("GIVEN two units v1 and v2 AND v1 is smaller than v2 AND "
                   "different ratio WHEN "
                   "compared with less than operator THEN result is true",
                   "[unit_t][unit_cast]", int64_t, long double) {

  constexpr unit_t<'X', 1, TestType, std::kilo> v1{10};
  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v2{1000};

  static_assert(v1 < v2, "Less than comparison is true");
}

TEMPLATE_TEST_CASE(
    "GIVEN two units with same absolute value AND same ratio WHEN "
    "compared with greater than operator THEN result is false",
    "[unit_t][operator>]", int64_t, long double) {

  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v1{1};

  static_assert(!(v1 > v1), "Less than comparison is false");
}

TEMPLATE_TEST_CASE(
    "GIVEN two units with same absolute value AND differnt ratio WHEN "
    "compared with greater than operator THEN result is false",
    "[unit_t][operator>]", int64_t, long double) {

  constexpr unit_t<'X', 1, TestType, std::kilo> v1{1};
  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v2{1000};

  static_assert(!(v1 > v2), "Less than comparison is false");
}

TEMPLATE_TEST_CASE(
    "GIVEN two units v1 and v2 AND v1 is smaller than v2 AND same ratio WHEN "
    "compared with greater than operator THEN result is true",
    "[unit_t][operator>]", int64_t, long double) {

  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v1{1};
  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v2{0};

  static_assert(v2 > v1, "Less than comparison is true");
}

TEMPLATE_TEST_CASE("GIVEN two units v1 and v2 AND v1 is smaller than v2 AND "
                   "different ratio WHEN "
                   "compared with greater than operator THEN result is true",
                   "[unit_t][operator>]", int64_t, long double) {

  constexpr unit_t<'X', 1, TestType, std::kilo> v1{10};
  constexpr unit_t<'X', 1, TestType, std::ratio<1>> v2{1000};

  static_assert(v2 > v1, "Less than comparison is true");
}