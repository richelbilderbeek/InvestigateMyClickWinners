#include "clickcard.h"

#include <cassert>
#include <iostream>
#include "calendar.h"

ribi::imcw::click_card::click_card(const date& acquisition_date) noexcept
  : m_date_start{acquisition_date},
    m_date_end{acquisition_date + boost::gregorian::years(1)}
{
  #ifndef NDEBUG
  test();
  #endif
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
  const boost::gregorian::date today = boost::gregorian::day_clock::local_day();
  click_card c(today);
  assert(c.is_valid(today));
  assert(c.is_valid(today + boost::gregorian::months(11)));
  assert(!c.is_valid(today + boost::gregorian::years(1)));
  assert(!c.is_valid(today + boost::gregorian::months(12)));
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
