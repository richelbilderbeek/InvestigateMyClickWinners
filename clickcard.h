#ifndef CLICKCARD_H
#define CLICKCARD_H

#include <chrono>

struct click_card
{
  ///The card costs 60 euros (inc VAT) to buy
  constexpr static const double cost_inc_vat_euros = 60.0;
  ///The card is valid for a year
  constexpr static const double validity_duration_years = 1.0;

  #ifndef NDEBUG
  static void test() noexcept;
  #endif
};

#endif // CLICKCARD_H
