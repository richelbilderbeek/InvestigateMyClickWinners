#ifndef RIBI_IMCW_BALANCE_H
#define RIBI_IMCW_BALANCE_H

#include <iosfwd>

namespace ribi {
namespace imcw {

///The amount of money in euros
///Positive values denote profit
///Negative values denote loss
class balance
{
public:
  balance(const double value_in_euros = 0.0);

  double get_value_euros() const noexcept { return m_euros;  }

private:

  //The amount of money in euros
  double m_euros;

  #ifndef NDEBUG
  static void test() noexcept;
  #endif

  friend bool operator==(const balance& a, const balance& b) noexcept;
};

std::ostream& operator<<(std::ostream& os, const balance& b) noexcept;

//Equal if less than half a eurocent different
bool operator==(const balance& a, const balance& b) noexcept;

//Unequal if at least half a eurocent different
bool operator!=(const balance& a, const balance& b) noexcept;

bool operator<(const balance& a, const balance& b) noexcept;
bool operator>(const balance& a, const balance& b) noexcept;
bool operator>=(const balance& a, const balance& b) noexcept;

//balance operator*(const balance& b, const double multiplier);

} //~namespace imcw
} //~namespace ribi

#endif // RIBI_IMCW_BALANCE_H
