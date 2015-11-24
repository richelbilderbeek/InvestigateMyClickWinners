#ifndef QTINVESTIGATEMYCLICKWINNERAXISDATEDRAWER_H
#define QTINVESTIGATEMYCLICKWINNERAXISDATEDRAWER_H

#include <qwt_scale_draw.h>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace ribi {
namespace imcw {

struct QtAxisDateDrawer: public QwtScaleDraw
{
  using date = boost::gregorian::date;
  QtAxisDateDrawer(const date& start_date);
  QwtText label(const double x) const override;

  private:
  date m_start_date;
};

} //~namespace imcw
} //~namespace ribi


#endif // QTINVESTIGATEMYCLICKWINNERAXISDATEDRAWER_H
