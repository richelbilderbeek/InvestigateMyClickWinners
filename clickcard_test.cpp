#include "clickcard.h"

void ribi::imcw::click_card::test() noexcept
{
  //calculate_start_date
  {
    assert(calculate_start_date(date(2015,6,15)) == date(2015,7,1));
    assert(calculate_start_date(date(2015,6,1)) == date(2015,7,1));
    assert(calculate_start_date(date(2015,1,15)) == date(2015,2,1));
    assert(calculate_start_date(date(2015,12,15)) == date(2016,1,1));
  }
  //A ClickCard is valid the next month
  {
    const boost::gregorian::date d(2015,6,15);
    click_card c(d);
    assert(!c.is_valid(date(2015,6, 1)));
    assert(!c.is_valid(date(2015,6,15)));
    assert(!c.is_valid(date(2015,6,30)));
    assert( c.is_valid(date(2015,7, 1)));
    assert( c.is_valid(date(2015,7,31)));
    assert( c.is_valid(date(2016,6, 1)));
    assert( c.is_valid(date(2016,6,30)));
  }
}
