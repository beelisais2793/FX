#include "laguerre.h"

#include "Random.h"

#include <memory>

#define BOOST_TEST_MODULE laguerre
#include <boost/test/included/unit_test.hpp>

using namespace Storage_B;

namespace
{
  Random<double> dRan(-1.0, 1.0);
  Random<unsigned int> iRan(2, 10);

  auto tol = boost::test_tools::tolerance(0.00049);
}

// L0
BOOST_AUTO_TEST_CASE(L0)
{
  auto val1 = Laguerre::L0<double>(dRan());
  auto val2 = Laguerre::Ln<double>(0, dRan());

  BOOST_TEST(val1 == val2);
  BOOST_TEST(val1 == 1.0);
}

// L1, x = 1
BOOST_AUTO_TEST_CASE(L1)
{
  auto val1 = Laguerre::L1<float>(1.0);
  auto val2 = Laguerre::Ln<float>(1, 1.0);

  BOOST_TEST(val1 == val2);
  BOOST_TEST(val1 ==  0.0);
}

// Ln(x) when x = 0
BOOST_AUTO_TEST_CASE(LnXequalsZero)
{
  auto n = iRan();

  auto val = Laguerre::Ln<double>(n, 0.0);

  BOOST_TEST(val == 1.0);
}

BOOST_AUTO_TEST_CASE(Ln05)
{
  BOOST_TEST(Laguerre::Ln<float>(1, -0.05f) == 1.05, tol);
  BOOST_TEST(Laguerre::Ln<float>(2, -0.05f) == 1.10125, tol);
  BOOST_TEST(Laguerre::Ln<float>(3, -0.05f) == 1.1538, tol);
  BOOST_TEST(Laguerre::Ln<float>(4, -0.05f) == 1.2076, tol);
  BOOST_TEST(Laguerre::Ln<float>(5, -0.05f) == 1.2627, tol);

  BOOST_TEST(Laguerre::Ln<float>(1, 0.05) == 0.95, tol);
  BOOST_TEST(Laguerre::Ln<float>(2, 0.05f) == 0.90125, tol);
  BOOST_TEST(Laguerre::Ln<float>(3, 0.05f) == 0.8537, tol);
  BOOST_TEST(Laguerre::Ln<float>(4, 0.05f) == 0.8074, tol);
  BOOST_TEST(Laguerre::Ln<float>(5, 0.05f) == 0.7623, tol);
}

BOOST_AUTO_TEST_CASE(Ln50)
{
  BOOST_TEST(Laguerre::Ln<float>(1, -0.5) == 1.5);
  BOOST_TEST(Laguerre::Ln<float>(2, -0.5) == 2.125);
  BOOST_TEST(Laguerre::Ln<float>(3, -0.5) == 2.8958, tol);
  BOOST_TEST(Laguerre::Ln<float>(4, -0.5) == 3.8359, tol);
  BOOST_TEST(Laguerre::Ln<float>(5, -0.5) == 4.9716, tol);

  BOOST_TEST(Laguerre::Ln<float>(1, 0.5) == 0.5);
  BOOST_TEST(Laguerre::Ln<float>(2, 0.5) == 0.125);
  BOOST_TEST(Laguerre::Ln<float>(3, 0.5) == -0.1458, tol);
  BOOST_TEST(Laguerre::Ln<float>(4, 0.5) == -0.3307, tol);
  BOOST_TEST(Laguerre::Ln<float>(5, 0.5) == -0.4456, tol);
}

BOOST_AUTO_TEST_CASE(Ln95)
{
  BOOST_TEST(Laguerre::Ln<float>(1, -0.95) == 1.95, tol);
  BOOST_TEST(Laguerre::Ln<float>(2, -0.95) == 3.35125, tol);
  BOOST_TEST(Laguerre::Ln<float>(3, -0.95) == 5.3466, tol);
  BOOST_TEST(Laguerre::Ln<float>(4, -0.95) == 8.1130, tol);
  BOOST_TEST(Laguerre::Ln<float>(5, -0.95) == 11.8678, tol);

  BOOST_TEST(Laguerre::Ln<float>(1, 0.95) == 0.05, tol);
  BOOST_TEST(Laguerre::Ln<float>(2, 0.95) == -0.44875, tol);
  BOOST_TEST(Laguerre::Ln<float>(3, 0.95) == -0.6391, tol);
  BOOST_TEST(Laguerre::Ln<float>(4, 0.95) == -0.6301, tol);
  BOOST_TEST(Laguerre::Ln<float>(5, 0.95) == -0.5032, tol);
}
