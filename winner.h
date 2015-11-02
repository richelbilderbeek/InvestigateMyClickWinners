#ifndef WINNER_H
#define WINNER_H


struct winner
{
  winner();

  ///Get value of the Winner in euros
  void add_value_euros(const double euros);

  bool is_full() const noexcept { return m_value_euros >= max_value_euros; }

  ///Get value of the Winner in euros
  double get_value_euros() const noexcept { return m_value_euros; }

  ///A winner costs 40 euros (exempt from VAT) to buy
  constexpr static const double price_vat_exempt_euros = 40.0;

  ///The maximum value a Winner can have
  constexpr static const double max_value_euros = 50.0;

  private:

  ///The value of the Winner in euros
  double m_value_euros;


  #ifndef NDEBUG
  static void test() noexcept;
  #endif
};

#endif // WINNER_H
