#include "clickcard.h"

#include <cassert>
#include <iostream>
#include "calendar.h"

ribi::imcw::click_card::click_card(const date& purchase_date) noexcept
  : m_date_purchase{purchase_date},
    m_date_start{calculate_start_date(purchase_date)},
    m_date_end{calculate_start_date(purchase_date) + boost::gregorian::years(1)}
{

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

std::ostream& ribi::imcw::operator<<(std::ostream& os, const click_card& c) noexcept
{
  os << "ClickCard ("
    << c.m_date_start << "-" << c.m_date_end
    << ")"
  ;
  return os;
}
