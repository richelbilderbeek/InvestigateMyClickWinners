#ifndef RIBI_IMCW_WINNER_PACKAGE_NAME_H
#define RIBI_IMCW_WINNER_PACKAGE_NAME_H

#include <string>

namespace ribi {
namespace imcw {

enum class winner_package_name {
  starter,
  basic,
  junior,
  senior,
  master,
  executive
};

winner_package_name to_winner_package_name(const std::string& s);

} //~namespace imcw
} //~namespace ribi

#endif // RIBI_IMCW_WINNER_PACKAGE_NAME_H
