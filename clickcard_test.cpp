#include "clickcard.h"

#include <boost/test/unit_test.hpp>

using namespace ribi::imcw;

BOOST_AUTO_TEST_CASE(imcw_clickcard)
{
  using date = ribi::imcw::click_card::date;
  //calculate_start_date
  {
    BOOST_CHECK(calculate_start_date(date(2015,6,15)) == date(2015,7,1));
    BOOST_CHECK(calculate_start_date(date(2015,6,1)) == date(2015,7,1));
    BOOST_CHECK(calculate_start_date(date(2015,1,15)) == date(2015,2,1));
    BOOST_CHECK(calculate_start_date(date(2015,12,15)) == date(2016,1,1));
  }
  //A ClickCard is valid the next month
  {
    const boost::gregorian::date d(2015,6,15);
    click_card c(d);
    BOOST_CHECK(!c.is_valid(date(2015,6, 1)));
    BOOST_CHECK(!c.is_valid(date(2015,6,15)));
    BOOST_CHECK(!c.is_valid(date(2015,6,30)));
    BOOST_CHECK( c.is_valid(date(2015,7, 1)));
    BOOST_CHECK( c.is_valid(date(2015,7,31)));
    BOOST_CHECK( c.is_valid(date(2016,6, 1)));
    BOOST_CHECK( c.is_valid(date(2016,6,30)));
  }
}
