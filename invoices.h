#ifndef INVOICES_H
#define INVOICES_H

#include <QWidget>

namespace Ui {
class Invoices;
}

class Invoices : public QWidget
{
    Q_OBJECT

public:
    explicit Invoices(QWidget *parent = 0);
    int findCustomerIdFormInvID(int inv_id);
    QString getFoodInvoice();
    QString getLodgingInvoice();
    ~Invoices();

private slots:
    void on_addInvoices_clicked();
    void createInvoiceTable();

    //void on_cancelInvoice_clicked();
    void on_deleteInvoice_clicked();

    void on_printInvoice_clicked();

    void on_printFoodInvoice_clicked();

    void on_paymentDoneBtn_clicked();

    void on_invoiceTable_cellDoubleClicked(int row, int column);

    void on_filterBtn_clicked();

    void on_pushButton_clicked();

private:
    Ui::Invoices *ui;
};

#endif // INVOICES_H
