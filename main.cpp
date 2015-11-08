#include "bank.h"
#include "calendar.h"
#include "company.h"
#include "money.h"
#include "person.h"
#include "winner_package.h"
#include "simulation.h"
#include "simulation_parameters.h"

int main()
{
  using ribi::imcw::bank;
  using ribi::imcw::calendar;
  using ribi::imcw::company;
  using ribi::imcw::person;
  using ribi::imcw::simulation;
  using ribi::imcw::simulation_parameters;

  calendar the_calendar(boost::gregorian::day_clock::local_day());
  bank the_bank;
  company mcw;

  const simulation_parameters parameters(
    person("Mister X"),
    {},
    boost::gregorian::day_clock::local_day(),
    boost::gregorian::day_clock::local_day()
      + boost::gregorian::years(10)
  );
  simulation s(
    the_bank,
    the_calendar,
    mcw,
    parameters
  );
  s.run();
}
