#ifndef SIMULATION_PARAMETERS_H
#define SIMULATION_PARAMETERS_H

#include <vector>

#include "person.h"
#include <boost/date_time/gregorian/gregorian.hpp>

struct simulation_parameters
{
  explicit simulation_parameters(
    const person& focal_person,
    const std::vector<person>& others,
    const boost::gregorian::date& start,
    const boost::gregorian::date& end
  );

  const boost::gregorian::date& get_end() const noexcept { return m_end; }
  const person& get_focal_person() const noexcept { return m_focal_person; }
  const std::vector<person>&  get_others() const noexcept { return m_others; }
  const boost::gregorian::date& get_start() const noexcept { return m_start; }

  private:
  const boost::gregorian::date& m_end;
  const person m_focal_person;
  const std::vector<person> m_others;
  const boost::gregorian::date& m_start;

  private:
  #ifndef NDEBUG
  static void test() noexcept;
  #endif
};

#endif // SIMULATION_PARAMETERS_H
