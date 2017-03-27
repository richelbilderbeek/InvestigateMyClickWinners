#include "calendar.h"

const boost::gregorian::date ribi::imcw::calendar::sm_distibute_profit_webshop_day
  = boost::gregorian::date(
    boost::gregorian::greg_year(2015),
    boost::gregorian::greg_month(2),
    boost::gregorian::greg_day(28)
  );

ribi::imcw::calendar::calendar(
  const boost::gregorian::date& today
) : m_today{today}
{

}

bool ribi::imcw::calendar::distribute_profit_today() const noexcept
{
  return m_today == m_today.end_of_month();
}

bool ribi::imcw::calendar::transfer_profit_webshop_today() const noexcept
{
  return m_today.day() == sm_distibute_profit_webshop_day.day()
    && m_today.month() == sm_distibute_profit_webshop_day.month()
  ;
}

bool ribi::imcw::calendar::transfer_profit_website_today() const noexcept
{
  return distribute_profit_today();
}
