#include "transfer_strategy.h"

#include <cassert>

#include "person.h"

std::function<bool(const ribi::imcw::person& p, const boost::gregorian::date&)>
  ribi::imcw::after_expiration_of_click_card() noexcept
{
  std::function<bool(const person& p,const boost::gregorian::date&)> f
    = [](const person& p,const boost::gregorian::date& d)
    {
      //Tranfer if today the person has a ClickCard
      //and it is expired the next day
      return !p.has_valid_click_card(d) && p.get_winners().empty();
    }
  ;
  return f;

}

std::function<bool(const ribi::imcw::person& p, const boost::gregorian::date&)>
  ribi::imcw::before_expiration_of_click_card() noexcept
{
  std::function<bool(const person& p,const boost::gregorian::date&)> f
    = [](const person& p,const boost::gregorian::date& d)
    {
      //Tranfer if today the person has a ClickCard
      //and it is expired the next day
      return p.has_valid_click_card(d)
        && !p.has_valid_click_card(d + boost::gregorian::days(1));
    }
  ;
  return f;
}



