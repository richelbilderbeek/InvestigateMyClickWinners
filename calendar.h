#ifndef RIBI_IMCW_CALENDAR_H
#define RIBI_IMCW_CALENDAR_H

#include <boost/date_time/gregorian/gregorian.hpp>

namespace ribi {
namespace imcw {

///The scheduled events
class calendar
{
public:

  explicit calendar(
    const boost::gregorian::date& today = boost::gregorian::day_clock::local_day()
  );

  const boost::gregorian::date& get_today() const noexcept { return m_today; }

  void go_to_next_day() noexcept { m_today += boost::gregorian::days(1); }

  ///Will today profit be transferred from 'undistributed' to the customers
  bool distribute_profit_today() const noexcept;

  //Move profit from webshop to 'undistributed' balance
  bool transfer_profit_webshop_today() const noexcept;

  //Move profit from website to 'undistributed' balance
  bool transfer_profit_website_today() const noexcept;

  private:

  boost::gregorian::date m_today;

  static const boost::gregorian::date sm_distibute_profit_webshop_day;
};

} //~namespace imcw
} //~namespace ribi

#endif // RIBI_IMCW_CALENDAR_H
