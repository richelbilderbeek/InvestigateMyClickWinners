#include "winner.h"

void ribi_imcw_winner_test() noexcept
{
  {
    winner w("Test winner");
    assert(w.get_value() == money(0.0));
  }
  {
    winner w("Not yet full winner");
    w.get_balance().set_value(money(49.99));
    assert(!w.is_full());
  }
  {
    winner w("Full winner");
    w.get_balance().set_value(money(50.0));
    assert(w.is_full());
  }
  {
    winner w("Fuller winner");
    w.get_balance().set_value(money(100.0));
    assert(w.is_full());
  }
}
