#include "qtinvestigatemyclickwinneraxisdatedrawer.h"

#include <sstream>

ribi::imcw::QtAxisDateDrawer::QtAxisDateDrawer(
  const boost::gregorian::date& start_date
) : m_start_date{start_date}
{

}

QwtText ribi::imcw::QtAxisDateDrawer::label(const double x) const
{
  const auto then = m_start_date + boost::gregorian::days(x);
  std::stringstream s;
  s << then.day() << "-" << then.month() << "-" << then.year();
  //s << then;
  QwtText t(s.str().c_str());
  return t;
}

