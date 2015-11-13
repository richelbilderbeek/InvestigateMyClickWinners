#ifndef BUY_WINNERS_STRATEGY
#define BUY_WINNERS_STRATEGY

#include <functional>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace ribi {
namespace imcw {

//Lambda expression to determine to auto-buy Winners
//depending on the date

std::function<bool(const boost::gregorian::date&)> always_buy() noexcept;
std::function<bool(const boost::gregorian::date&)> never_buy() noexcept;
std::function<bool(const boost::gregorian::date&)> buy_until(const boost::gregorian::date& until) noexcept;

} //~namespace imcw
} //~namespace ribi


#endif // BUY_WINNERS_STRATEGY
