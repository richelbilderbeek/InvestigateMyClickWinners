#include "winner_package_name.h"

#include <sstream>
#include <stdexcept>

ribi::imcw::winner_package_name ribi::imcw::to_winner_package_name(
  const std::string& s
)
{
  if (s == "Starter") { return ribi::imcw::winner_package_name::starter; }
  if (s == "Basic") { return ribi::imcw::winner_package_name::basic; }
  if (s == "Junior") { return ribi::imcw::winner_package_name::junior; }
  if (s == "Senior") { return ribi::imcw::winner_package_name::senior; }
  if (s == "Master") { return ribi::imcw::winner_package_name::master; }
  if (s == "Executive") { return ribi::imcw::winner_package_name::executive; }
  std::stringstream msg;
  msg << __func__ << ": cannot convert '" << s << "' to winner_package_name";
  throw std::logic_error(msg.str());
}
