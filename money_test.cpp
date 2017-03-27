#include "money.h"

#include <boost/test/unit_test.hpp>

using namespace ribi::imcw;

BOOST_AUTO_TEST_CASE(imcw_money)
{
  {
    const money a(3.14);
    const money b(a);
    BOOST_CHECK(a == b);
  }
  {
    const money a(3.14);
    const money b(2.71);
    BOOST_CHECK(a != b);
  }
  {
    const money a(3.14);
    money b(2.71);
    BOOST_CHECK(a != b);
    b = a;
    BOOST_CHECK(a == b);
  }
  {
    BOOST_CHECK(money(0.0) == money(0.0000));
    BOOST_CHECK(money(0.0) == money(0.0049));
    BOOST_CHECK(money(0.0) != money(0.0051));
  }
  {
    BOOST_CHECK(money(1.0) + money(2.0) == money(3.0));
    BOOST_CHECK(money(2.0) + money(1.0) == money(3.0));
  }
  {
    BOOST_CHECK(money(2.0) - money(1.0) == money(1.0));
    BOOST_CHECK(money(3.0) - money(1.0) == money(2.0));
  }
}
