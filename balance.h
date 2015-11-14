#ifndef RIBI_IMCW_BALANCE_H
#define RIBI_IMCW_BALANCE_H

#include <iosfwd>
#include <string>
#include "money.h"


namespace ribi {
namespace imcw {

///The amount of money in euros
///Positive values denote profit
///Negative values denote loss
class balance
{
public:
  explicit balance(
    const std::string& description,
    const money& initial_value_in_euros = money(0.0)
  );

  const std::string& get_description() const noexcept { return m_description; }

  const money& get_value() const noexcept { return m_value;  }

  void set_value(const money& value) noexcept { m_value = value; }

private:

  //For example, who owns this money
  std::string m_description;

  //The amount of money in euros
  money m_value;

  #ifndef NDEBUG
  static void test() noexcept;
  #endif

  friend bool operator==(const balance& a, const balance& b) noexcept;
};

std::ostream& operator<<(std::ostream& os, const balance& b) noexcept;

bool operator==(const balance& a, const balance& b) noexcept;
bool operator!=(const balance& a, const balance& b) noexcept;


} //~namespace imcw
} //~namespace ribi

#endif // RIBI_IMCW_BALANCE_H
