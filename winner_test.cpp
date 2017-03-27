#include "winner.h"

#include <boost/test/unit_test.hpp>

using namespace ribi::imcw;

BOOST_AUTO_TEST_CASE(imcw_winner)
{
  {
    winner w("Test winner");
    BOOST_CHECK(w.get_value() == money(0.0));
  }
  {
    winner w("Not yet full winner");
    w.get_balance().set_value(money(49.99));
    BOOST_CHECK(!w.is_full());
  }
  {
    winner w("Full winner");
    w.get_balance().set_value(money(50.0));
    BOOST_CHECK(w.is_full());
  }
  {
    winner w("Fuller winner");
    w.get_balance().set_value(money(100.0));
    BOOST_CHECK(w.is_full());
  }
}
