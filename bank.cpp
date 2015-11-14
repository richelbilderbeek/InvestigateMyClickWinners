#include "bank.h"

#include <iostream>
#include <sstream>

#include "balance.h"
#include "helper.h"
#include "money.h"

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
    balance sender("sender");
    const money amount_euros(100.0);
    balance receiver("receiver");
    assert(sender.get_value() == money(0.0));
    assert(receiver.get_value() == money(0.0));
    assert(the_bank.get_transfers().empty());
    the_bank.transfer(
      sender,
      amount_euros,
      receiver,
      today
    );
    assert(sender.get_value()   == -amount_euros);
    assert(receiver.get_value() ==  amount_euros);
    assert(!the_bank.get_transfers().empty());
  }
}
#endif

void ribi::imcw::bank::transfer(
  balance& sender,
  const money& value_in_euros,
  balance& receiver,
  const date& day
)
{
  #ifndef NDEBUG
  assert(sender != receiver);
  const money sum_before = sender.get_value() + receiver.get_value();
  #endif
  sender.set_value(    sender.get_value() - value_in_euros);
  receiver.set_value(receiver.get_value() + value_in_euros);

  std::stringstream msg;
  msg << day << ": " << sender.get_description() << " sent "
    << value_in_euros
    << " to " << receiver.get_description()
  ;
  m_transfers.push_back(msg.str());

  #ifndef NDEBUG
  const money sum_after = sender.get_value() + receiver.get_value();
  assert(sum_before == sum_after);
  #endif
}


void ribi::imcw::bank::transfer(
  balance& sender,
  const money& value_in_euros,
  const double proportion_to_a,
  balance& receiver_a,
  balance& receiver_b,
  const date& day
)
{
  #ifndef NDEBUG
  const auto sum_before
    = sender.get_value()
    + receiver_a.get_value()
    + receiver_b.get_value()
  ;
  #endif
  const double proportion_to_b{
    1.0 - proportion_to_a
  };
  const money value_in_euros_to_a{
    proportion_to_a * value_in_euros
  };
  const money value_in_euros_to_b{
    proportion_to_b * value_in_euros
  };
  sender.set_value(
    sender.get_value() - value_in_euros
  );
  receiver_a.set_value(
    receiver_a.get_value() + value_in_euros_to_a
  );
  receiver_b.set_value(
    receiver_b.get_value() + value_in_euros_to_b
  );

  {
    std::stringstream msg;
    msg << day << ": " << sender.get_description() << " sent "
      << value_in_euros_to_a
      << " to " << receiver_a.get_description()
    ;
    m_transfers.push_back(msg.str());
  }
  {
    std::stringstream msg;
    msg << day << ": " << sender.get_description() << " sent "
      << value_in_euros_to_b
      << " to " << receiver_b.get_description()
    ;
    m_transfers.push_back(msg.str());
  }

  #ifndef NDEBUG
  const auto sum_after
    = sender.get_value()
    + receiver_a.get_value()
    + receiver_b.get_value()
  ;
  assert(sum_before == sum_after);
  #endif
}

std::ostream& ribi::imcw::operator<<(std::ostream& os, const bank b) noexcept
{
  std::copy(
    std::begin(b.m_transfers),
    std::end(b.m_transfers),
    std::ostream_iterator<std::string>(os,"\n")
  );
  return os;
}

