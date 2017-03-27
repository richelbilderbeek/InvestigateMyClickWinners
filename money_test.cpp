#include "money.h"

void ribi::imcw::money::test() noexcept
{
  {
    const money a(3.14);
    const money b(a);
    assert(a == b);
  }
  {
    const money a(3.14);
    const money b(2.71);
    assert(a != b);
  }
  {
    const money a(3.14);
    money b(2.71);
    assert(a != b);
    b = a;
    assert(a == b);
  }
  {
    assert(money(0.0) == money(0.0000));
    assert(money(0.0) == money(0.0049));
    assert(money(0.0) != money(0.0051));
  }
  {
    assert(money(1.0) + money(2.0) == money(3.0));
    assert(money(2.0) + money(1.0) == money(3.0));
  }
  {
    assert(money(2.0) - money(1.0) == money(1.0));
    assert(money(3.0) - money(1.0) == money(2.0));
  }
}
