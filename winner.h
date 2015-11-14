#ifndef RIBI_IMCW_WINNER_H
#define RIBI_IMCW_WINNER_H

#include <iosfwd>
#include <vector>
#include "balance.h"
#include "money.h"

namespace ribi {
namespace imcw {

struct winner
{
  explicit winner(const std::string& owner_name);

  ///Get value of the Winner in euros
  //void add_value_euros(const double euros);

  bool is_full() const noexcept { return get_value() >= money(max_value_euros); }

  ///Get value of the Winner in euros
  const balance& get_balance() const noexcept { return m_balance; }
        balance& get_balance()       noexcept { return m_balance; }

  ///Get value of the Winner in euros
  const money& get_value() const noexcept { return m_balance.get_value(); }

  ///A winner costs 40 euros (exempt from VAT) to buy
  constexpr static const double price_vat_exempt_euros = 40.0;

  ///The maximum value a Winner can have
  constexpr static const double max_value_euros = 50.0;

  private:

  ///The value of the Winner in euros
  balance m_balance;

  ///Unique ID
  int m_id;

  static int sm_ids;

  #ifndef NDEBUG
  static void test() noexcept;
  #endif

  friend std::ostream& operator<<(std::ostream& os, const winner& w) noexcept;
};

money get_sum_value(const std::vector<winner>& winners) noexcept;

std::ostream& operator<<(std::ostream& os, const winner& w) noexcept;

} //~namespace imcw
} //~namespace ribi

#endif // RIBI_IMCW_WINNER_H
