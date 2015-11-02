#include "simulation.h"

simulation::simulation(const simulation_parameters& parameters)
  : m_company{},
    m_parameters{parameters}
{

}

void simulation::run() noexcept
{
  company c;

  //Add all people to company
  c.add(m_parameters.get_focal_person());
  std::for_each(
    std::begin(m_parameters.get_others()),
    std::end(m_parameters.get_others()),
    [c](const auto& p) { c.add(p); }
  );

}
