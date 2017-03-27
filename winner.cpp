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

std::ostream& ribi::imcw::operator<<(std::ostream& os, const winner& w) noexcept
{
  os << "ID: " << w.m_id << ": " << w.get_value();
  return os;
}
