#ifndef TRANSFER_STRATEGY_H
#define TRANSFER_STRATEGY_H

#include <functional>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace ribi {
namespace imcw {

struct person;

std::function<bool(const person& p, const boost::gregorian::date&)>
  after_expiration_of_click_card() noexcept
;

//Lambda expression to determine to tranfer money from
//BankWallet to personal bank account
std::function<bool(const person& p, const boost::gregorian::date&)>
  before_expiration_of_click_card() noexcept
;


} //~namespace imcw
} //~namespace ribi

#endif // TRANSFER_STRATEGY_H
