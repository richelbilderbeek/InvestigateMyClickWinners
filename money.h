#ifndef MONEY_H
#define MONEY_H

//Embodies an amount of money in euros
class money
{
public:
  money(const double value_in_euros = 0.0);


private:

  double m_euros;

  #ifndef NDEBUG
  static void test() noexcept;
  #endif

  friend bool operator==(const money& a, const money& b) noexcept;
};

//Equal if less than half a eurocent different
bool operator==(const money& a, const money& b) noexcept;

//Unequal if at least half a eurocent different
bool operator!=(const money& a, const money& b) noexcept;

#endif // MONEY_H
