#include "calendar.h"
#include "company.h"
#include "person.h"
#include "winner_package.h"
#include "simulation.h"
#include "simulation_parameters.h"

int main()
{
  calendar(boost::gregorian::day_clock::local_day());

  const simulation_parameters parameters(
    person(),
    {},
    boost::gregorian::day_clock::local_day(),
    boost::gregorian::day_clock::local_day() + boost::gregorian::years(1)
  );
  simulation s(parameters);
  s.run();
}
