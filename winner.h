#ifndef RIBI_IMCW_WINNER_H
#define RIBI_IMCW_WINNER_H

#include <iosfwd>
#include "balance.h"

namespace ribi {
namespace imcw {

struct winner
{
  winner();

  ///Get value of the Winner in euros
  //void add_value_euros(const double euros);

  bool is_full() const noexcept { return m_value_euros >= balance(max_value_euros); }

  ///Get value of the Winner in euros
  const balance& get_value() const noexcept { return m_value_euros; }
        balance& get_value()       noexcept { return m_value_euros; }

  ///A winner costs 40 euros (exempt from VAT) to buy
  constexpr static const double price_vat_exempt_euros = 40.0;

  ///The maximum value a Winner can have
  constexpr static const double max_value_euros = 50.0;

  private:

  ///The value of the Winner in euros
  balance m_value_euros;

  #ifndef NDEBUG
  static void test() noexcept;
  #endif
};

std::ostream& operator<<(std::ostream& os, const winner& w) noexcept;

} //~namespace imcw
} //~namespace ribi

#endif // RIBI_IMCW_WINNER_H
