#ifndef QTINVESTIGATEMYCLICKWINNERSMAINDIALOG_H
#define QTINVESTIGATEMYCLICKWINNERSMAINDIALOG_H

#include <QDialog>

#include <qwt_legend.h>
#include <qwt_plot_curve.h>
#include "person.h"
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

private slots:
  void on_button_run_clicked();
  void update_max_inspect_customer_index();
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
