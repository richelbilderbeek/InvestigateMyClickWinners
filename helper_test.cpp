#include "helper.h"

#include <boost/test/unit_test.hpp>

using namespace ribi::imcw;

BOOST_AUTO_TEST_CASE(imcw_helper)
{
  const helper h;

  //is_about_equal
  {
    BOOST_CHECK( h.is_about_equal(0.0,0.0000));
    BOOST_CHECK( h.is_about_equal(0.0,0.0049));
    BOOST_CHECK(!h.is_about_equal(0.0,0.0051));
  }
}
