#ifndef QTINVESTIGATEMYCLICKWINNERSMAINDIALOG_H
#define QTINVESTIGATEMYCLICKWINNERSMAINDIALOG_H

#include <QDialog>

#include <qwt_legend.h>
#include <qwt_plot_curve.h>

namespace Ui {
  class QtInvestigateMyClickWinnersMainDialog;
}

class QtInvestigateMyClickWinnersMainDialog : public QDialog
{
  Q_OBJECT

public:
  explicit QtInvestigateMyClickWinnersMainDialog(QWidget *parent = 0);
  ~QtInvestigateMyClickWinnersMainDialog();

private slots:
  void on_button_run_clicked();

private:
  Ui::QtInvestigateMyClickWinnersMainDialog *ui;

  QwtPlotCurve m_curve_balance;
  QwtPlotCurve m_curve_bank_wallet;
  QwtPlotCurve m_curve_shop_wallet;
  QwtPlotCurve m_curve_winners;
};

#endif // QTINVESTIGATEMYCLICKWINNERSMAINDIALOG_H
