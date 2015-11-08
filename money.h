#ifndef MONEY_H
#define MONEY_H

#include <iosfwd>

namespace ribi {
namespace imcw {

class money
{
public:
  explicit money(const double value_in_euros);

  double get_value_euros() const noexcept { return m_value_euros; }

private:
  double m_value_euros;

  #ifndef NDEBUG
  static void test() noexcept;
  #endif
};

std::ostream& operator<<(std::ostream& os, const money& m) noexcept;

//Equal if less than half a eurocent different
bool operator==(const money& a, const money& b) noexcept;

//Unequal if at least half a eurocent different
bool operator!=(const money& a, const money& b) noexcept;

bool operator<(const money& a, const money& b) noexcept;
bool operator>(const money& a, const money& b) noexcept;
bool operator>=(const money& a, const money& b) noexcept;

money operator+(const money& lhs, const money& rhs) noexcept;
money operator-(const money& lhs) noexcept;
money operator-(const money& lhs, const money& rhs) noexcept;

money operator*(const money& m, const double multiplier);
money operator*(const double multiplier, const money& m);

money operator/(const money& m, const double denominator);

} //~namespace imcw
} //~namespace ribi

#endif // MONEY_H
