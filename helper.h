#ifndef HELPER_H
#define HELPER_H


struct helper
{
  helper();

  bool is_about_equal(const double a, const double b) const noexcept;

  private:

  #ifndef NDEBUG
  static void test() noexcept;
  #endif
};

#endif // HELPER_H
