#ifndef RIBI_IMCW_HELPER_H
#define RIBI_IMCW_HELPER_H

namespace ribi {
namespace imcw {

struct helper
{
  helper();

  bool is_about_equal(const double a, const double b) const noexcept;
};

} //~namespace imcw
} //~namespace ribi

#endif // RIBI_IMCW_HELPER_H
