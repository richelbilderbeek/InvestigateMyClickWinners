#include "calendar.h"

void ribi_imcw_calendar_test() noexcept
{
  const bool verbose{false};
  //Distribute Winners money at the end of the month
  {
    boost::gregorian::date today = boost::gregorian::day_clock::local_day();
    calendar c(today);
    for (int i=0; i!=12; ++i)
    {
      boost::gregorian::date next_month = today + boost::gregorian::months(1);
      bool has_distributed_winners{false};
      for ( ; c.get_today() != next_month; c.go_to_next_day() ) {
        if (c.distribute_profit_today()) {
          if (verbose) {
            std::cout << "Distribute Winners profit at " << c.get_today() << std::endl;
          }
          has_distributed_winners = true;
        }
      }
      assert(has_distributed_winners);
      today = next_month;
    }
  }
  //Distribute profit webshop at Februari/March
  {
    const boost::gregorian::date today = boost::gregorian::day_clock::local_day();
    calendar c(today);
    const boost::gregorian::date next_year = today + boost::gregorian::years(1);
    int n_profit_of_webshop_distributed = 0;
    for ( ; c.get_today() != next_year; c.go_to_next_day() ) {
      if (c.transfer_profit_webshop_today()) {
        if (verbose) {
          std::cout << "Distribute webshop profit at " << c.get_today() << std::endl;
        }
        ++n_profit_of_webshop_distributed;
      }
    }
    assert(n_profit_of_webshop_distributed == 1);
  }
}

