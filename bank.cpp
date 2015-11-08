#include "bank.h"

#include <iostream>
#include <sstream>

#include "helper.h"

ribi::imcw::bank::bank()
  : m_transfers{}
{
  #ifndef NDEBUG
  test();
  #endif
}


#ifndef NDEBUG
void ribi::imcw::bank::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
  //const bool verbose{false};
  const boost::gregorian::date today = boost::gregorian::day_clock::local_day();

  {
    bank b;
  }
  {
    bank the_bank;
    balance sender;
    const double amount_euros = 100.0;
    balance receiver;
    assert(sender.get_value_euros() == 0.0);
    assert(receiver.get_value_euros() == 0.0);
    assert(the_bank.get_transfers().empty());
    the_bank.transfer(
      sender,
      amount_euros,
      receiver,
      today
    );
    assert(sender.get_value_euros()   == -amount_euros);
    assert(receiver.get_value_euros() ==  amount_euros);
    assert(!the_bank.get_transfers().empty());
  }
}
#endif

void ribi::imcw::bank::transfer(
  balance& sender,
  const double value_in_euros,
  balance& receiver,
  const date& day
)
{
  const auto sum_before = sender.get_value_euros() + receiver.get_value_euros();
  sender   = balance(sender.get_value_euros() - value_in_euros);
  receiver = balance(receiver.get_value_euros() + value_in_euros);

  std::stringstream msg;
  msg << day << ": " << sender << " sent "
    << value_in_euros
    << " to " << receiver
  ;
  m_transfers.push_back(msg.str());

  const auto sum_after = sender.get_value_euros() + receiver.get_value_euros();

  assert(helper().is_about_equal(sum_before,sum_after));
  //assert(sum_before == sum_after);
}


void ribi::imcw::bank::transfer(
  balance& sender,
  const double value_in_euros,
  const double proportion_to_a,
  balance& receiver_a,
  balance& receiver_b,
  const date& day
)
{
  const auto sum_before
    = sender.get_value_euros()
    + receiver_a.get_value_euros()
    + receiver_b.get_value_euros()
  ;
  const double proportion_to_b{
    1.0 - proportion_to_a
  };
  const double value_in_euros_to_a{
    proportion_to_a * value_in_euros
  };
  const double value_in_euros_to_b{
    proportion_to_b * value_in_euros
  };
  sender   = balance(sender.get_value_euros() - value_in_euros);
  receiver_a = balance(
    receiver_a.get_value_euros() + value_in_euros_to_a
  );
  receiver_b = balance(
    receiver_b.get_value_euros() + value_in_euros_to_b
  );

  {
    std::stringstream msg;
    msg << day << ": " << sender << " sent "
      << value_in_euros_to_a
      << " to " << receiver_a
    ;
    m_transfers.push_back(msg.str());
  }
  {
    std::stringstream msg;
    msg << day << ": " << sender << " sent "
      << value_in_euros_to_b
      << " to " << receiver_b
    ;
    m_transfers.push_back(msg.str());
  }

  const auto sum_after
    = sender.get_value_euros()
    + receiver_a.get_value_euros()
    + receiver_b.get_value_euros()
  ;
  assert(sum_before == sum_after);
}

/*
void ribi::imcw::bank::transfer_profit_to_customer(
  balance& sender,
  const double value_in_euros,
  person& person,
  const boost::gregorian::date& day
)
{
  const auto prev_money = m_bank_wallet_euros + m_shop_wallet_euros;
  person
  m_bank_wallet_euros += (proportion_of_profit_to_bank_wallet * money_euros);
  m_shop_wallet_euros += (proportion_of_profit_to_shop_wallet * money_euros);

  const auto new_money = m_bank_wallet_euros + m_shop_wallet_euros;
  assert(prev_money + money_euros == new_money);
}
*/

std::ostream& ribi::imcw::operator<<(std::ostream& os, const bank b) noexcept
{
  std::copy(
    std::begin(b.m_transfers),
    std::end(b.m_transfers),
    std::ostream_iterator<std::string>(os,"\n")
  );
  return os;
}

