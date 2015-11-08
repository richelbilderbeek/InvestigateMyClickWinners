#ifndef RIBI_IMCW_BANK_H
#define RIBI_IMCW_BANK_H

#include <iosfwd>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace ribi {
namespace imcw {

struct balance;
struct money;

///The only class that can tranfer money
struct bank
{
public:
  using date = boost::gregorian::date;
  bank();

  const std::vector<std::string>& get_transfers() const noexcept { return m_transfers; }

  void transfer(
    balance& sender,
    const money& value_in_euros,
    balance& receiver,
    const date& day
  );

  void transfer(
    balance& sender,
    const money& value_in_euros,
    const double proportion_to_a,
    balance& receiver_a,
    balance& receiver_b,
    const date& day
  );

private:

  std::vector<std::string> m_transfers;
  #ifndef NDEBUG
  static void test() noexcept;
  #endif

  friend std::ostream& operator<<(std::ostream& os, const bank b) noexcept;
};

//Shows all account
std::ostream& operator<<(std::ostream& os, const bank b) noexcept;

} //~namespace imcw
} //~namespace ribi

#endif // RIBI_IMCW_BANK_H
