#ifndef RIBI_IMCW_CLICKCARD_H
#define RIBI_IMCW_CLICKCARD_H

#include <chrono>
#include "balance.h"

#include <boost/date_time/gregorian/gregorian.hpp>

namespace ribi {
namespace imcw {

struct click_card
{
  using date = boost::gregorian::date;

  ///The day a ClickCard is bought, it is not valid yet
  ///It will become valid the first day of the next month.
  ///From that moment on, the customer must do the clicking
  ///and will have his/her profit the end of that month
  explicit click_card(const date& purchase_date) noexcept;

  bool is_valid(const date& the_date) const noexcept;

  ///The card costs 60 euros (inc VAT) to buy
  constexpr static const double cost_inc_vat_euros = 60.0;
  ///The card is valid for a year
  constexpr static const double validity_duration_years = 1.0;

  private:
  ///Buying date
  boost::gregorian::date m_date_start;

  ///Expiration date
  boost::gregorian::date m_date_end;


  #ifndef NDEBUG
  static void test() noexcept;
  #endif

  friend std::ostream& operator<<(std::ostream& os, const click_card& c) noexcept;
};

///The start date is the first day of the next month after the purchase date
boost::gregorian::date calculate_start_date(boost::gregorian::date d) noexcept;

std::ostream& operator<<(std::ostream& os, const click_card& c) noexcept;

} //~namespace imcw
} //~namespace ribi

#endif // RIBI_IMCW_CLICKCARD_H
