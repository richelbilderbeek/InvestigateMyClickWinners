#ifndef QTINVESTIGATEMYCLICKWINNERSMAINDIALOG_H
#define QTINVESTIGATEMYCLICKWINNERSMAINDIALOG_H

#include <QDialog>

#include <qwt_legend.h>
#include <qwt_plot_curve.h>
#include "person.h"
#include "simulation_parameters.h"
#include "winner_package.h"

namespace Ui {
  class QtInvestigateMyClickWinnersMainDialog;
}

namespace ribi {
namespace imcw {

class QtMainDialog : public QDialog
{
  Q_OBJECT

public:
  explicit QtMainDialog(QWidget *parent = 0);
  ~QtMainDialog();

  winner_package_name get_winner_package_name() const noexcept;

  boost::gregorian::date get_ending_date() const noexcept;

  ///Will throw if the parameters are invalid
  simulation_parameters get_parameters() const;

  boost::gregorian::date get_starting_date() const noexcept;


private slots:
  void on_button_run_clicked();
  void update_max_inspect_customer_index();
  void on_calendar_start_clicked(const QDate &date);
  void on_calendar_end_clicked(const QDate &date);

private:
  Ui::QtInvestigateMyClickWinnersMainDialog *ui;

  QwtPlotCurve m_curve_company_compensation_plan;
  QwtPlotCurve m_curve_company_holding;
  QwtPlotCurve m_curve_company_reserves;
  QwtPlotCurve m_curve_company_undistributed;

  QwtPlotCurve m_curve_focal_person_balance;
  QwtPlotCurve m_curve_focal_person_bank_wallet;
  QwtPlotCurve m_curve_focal_person_shop_wallet;
  QwtPlotCurve m_curve_focal_person_winners;

  QwtPlotCurve m_curve_other_person_balance;
  QwtPlotCurve m_curve_other_person_bank_wallet;
  QwtPlotCurve m_curve_other_person_shop_wallet;
  QwtPlotCurve m_curve_other_person_winners;

  std::vector<person> create_other_customers() const noexcept;

};

} //~namespace imcw
} //~namespace ribi

#endif // QTINVESTIGATEMYCLICKWINNERSMAINDIALOG_H
