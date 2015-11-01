#ifndef WINNER_H
#define WINNER_H


struct winner
{
  ///A winner costs 40 euros (exempt from VAT) to buy
  constexpr static const double cost_vat_exempt_euros = 40.0;

  #ifndef NDEBUG
  static void test() noexcept;
  #endif
};

#endif // WINNER_H
