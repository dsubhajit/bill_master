#ifndef STATS_H
#define STATS_H

#include <QWidget>

namespace Ui {
class Stats;
}

class Stats : public QWidget
{
    Q_OBJECT

public:
    explicit Stats(QWidget *parent = 0);
    void createGraph();
    void createInvoiceTable();
    ~Stats();

private slots:
    void createBookingTable();
    void on_filterBtn_clicked();

    void on_printBookingList_clicked();

    void on_printCustomerList_clicked();

    void on_printInvoiceList_clicked();

private:
    Ui::Stats *ui;
};

#endif // STATS_H
