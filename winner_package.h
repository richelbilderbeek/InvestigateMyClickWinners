#ifndef RIBI_IMCW_WINNER_PACKAGE_H
#define RIBI_IMCW_WINNER_PACKAGE_H

#include "winner_package_name.h"

namespace ribi {
namespace imcw {

struct winner_package
{
  explicit winner_package(const winner_package_name name);

  int get_n_winners() const noexcept { return m_n_winners; }

  private:

  const int m_n_winners;

  #ifndef NDEBUG
  static void test() noexcept;
  #endif
};

int calculate_n_winners(const winner_package_name name);

} //~namespace imcw
} //~namespace ribi

#endif // RIBI_IMCW_WINNER_PACKAGE_H
