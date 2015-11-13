#include "buy_winners_strategy.h"

std::function<bool(const boost::gregorian::date&)>
ribi::imcw::always_buy() noexcept
{
  std::function<bool(const boost::gregorian::date&)> f
    = [](const boost::gregorian::date&) { return true; }
  ;
  return f;
}

std::function<bool(const boost::gregorian::date&)>
ribi::imcw::never_buy() noexcept
{
  std::function<bool(const boost::gregorian::date&)> f
   = [](const boost::gregorian::date&) { return false; }
  ;
  return f;
}

std::function<bool(const boost::gregorian::date&)>
ribi::imcw::buy_until(
  const boost::gregorian::date& until
) noexcept
{
  std::function<bool(const boost::gregorian::date&)> f
   = [until](const boost::gregorian::date& the_date) {
    return the_date < until;
  };
  return f;
}

