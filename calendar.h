#ifndef RIBI_IMCW_CALENDAR_H
#define RIBI_IMCW_CALENDAR_H

#include <boost/date_time/gregorian/gregorian.hpp>

namespace ribi {
namespace imcw {

///The scheduled events
class calendar
{
public:

  calendar(
    const boost::gregorian::date& today = boost::gregorian::day_clock::local_day()
  );

  const boost::gregorian::date& get_today() const noexcept { return m_today; }

  void go_to_next_day() noexcept { m_today += boost::gregorian::days(1); }

  //When a customer buys a WinnerPackage,
  //the first profit will be given at the end of the month after

  //Money from the Winners is distributed monthly, at the end of the month
  bool distibute_profit_winners_today() const noexcept;

  //The variable sources of income, e.g. the webshop
  //are distributed in february/march
  bool distibute_profit_webshop_today() const noexcept;

  private:

  boost::gregorian::date m_today;

  static const boost::gregorian::date sm_distibute_profit_webshop_day;

  #ifndef NDEBUG
  static void test() noexcept;
  #endif
};

} //~namespace imcw
} //~namespace ribi

#endif // RIBI_IMCW_CALENDAR_H
