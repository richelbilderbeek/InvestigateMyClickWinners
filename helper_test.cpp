#include "helper.h"

void ribi::imcw::helper::test() noexcept
{
  const helper h;

  //is_about_equal
  {
    assert( h.is_about_equal(0.0,0.0000));
    assert( h.is_about_equal(0.0,0.0049));
    assert(!h.is_about_equal(0.0,0.0051));
  }
}
