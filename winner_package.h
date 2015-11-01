#ifndef WINNER_PACKAGE_H
#define WINNER_PACKAGE_H

#include "winner_package_name.h"

struct winner_package
{
  winner_package(const winner_package_name name);

  int get_n_winners() const noexcept { return m_n_winners; }

  private:

  const int m_n_winners;

  #ifndef NDEBUG
  static void test() noexcept;
  #endif
};

constexpr int calculate_n_winners(const winner_package_name name);

#endif // WINNER_PACKAGE_H
