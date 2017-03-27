#include "winner.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>

int ribi::imcw::winner::sm_ids = 0;

ribi::imcw::winner::winner(const std::string& owner_name)
  : m_balance{balance("Winner of " + owner_name,money(0.0))},
    m_id{sm_ids++}
{
  #ifndef NDEBUG
  test();
  #endif
}

ribi::imcw::money ribi::imcw::get_sum_value(const std::vector<winner>& winners) noexcept
{
  return std::accumulate(
    std::begin(winners),
    std::end(winners),
    money(0.0),
    [](const money& init, const winner& w) {
      return init + w.get_value();
    }
  );
}

#ifndef NDEBUG
void ribi::imcw::winner::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
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
#endif

std::ostream& ribi::imcw::operator<<(std::ostream& os, const winner& w) noexcept
{
  os << "ID: " << w.m_id << ": " << w.get_value();
  return os;
}
