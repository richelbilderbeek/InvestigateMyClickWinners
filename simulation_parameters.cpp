#include "simulation_parameters.h"

#include <stdexcept>
#include <sstream>

simulation_parameters::simulation_parameters(
  const person& focal_person,
  const std::vector<person>& others,
  const boost::gregorian::date& start,
  const boost::gregorian::date& end
) : m_end{end},
    m_focal_person{focal_person},
    m_others{others},
    m_start{start}
{
  #ifndef NDEBUG
  test();
  #endif
  if (start >= end)
  {
    std::stringstream msg;
    msg << __func__ << ": start date (" << start
      << ") must be before end date (" << end
      << ")"
    ;
    throw std::logic_error(msg.str());
  }
}




#ifndef NDEBUG
void simulation_parameters::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }

  //Correct date range
  {
    boost::gregorian::date today = boost::gregorian::day_clock::local_day();
    boost::gregorian::date yesterday = today - boost::gregorian::days(1);
    boost::gregorian::date tomorrow = today + boost::gregorian::days(1);
    simulation_parameters(person(),{},today,tomorrow);
    try {
      simulation_parameters(person(),{},today,yesterday);
      assert(!"Should not get here");
    }
    catch (std::logic_error&) {
      //OK
    }
  }
}
#endif

