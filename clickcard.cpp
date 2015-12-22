#include "clickcard.h"

#include <cassert>
#include <iostream>
#include "calendar.h"

ribi::imcw::click_card::click_card(const date& purchase_date) noexcept
  : m_date_start{calculate_start_date(purchase_date)},
    m_date_end{calculate_start_date(purchase_date) + boost::gregorian::years(1)}
{
  #ifndef NDEBUG
  test();
  #endif
}

boost::gregorian::date ribi::imcw::calculate_start_date(boost::gregorian::date d) noexcept
{
  while (1)
  {
    d += boost::gregorian::days(1);
    if (d.day() == 1) break;

  }
  return d;
}

bool ribi::imcw::click_card::is_valid(const date& the_date) const noexcept
{
  return the_date >= m_date_start
    && the_date < m_date_end
  ;
}


#ifndef NDEBUG
void ribi::imcw::click_card::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
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
#endif

std::ostream& ribi::imcw::operator<<(std::ostream& os, const click_card& c) noexcept
{
  os << "ClickCard ("
    << c.m_date_start << "-" << c.m_date_end
    << ")"
  ;
  return os;
}
