#ifndef CREATEINVOICES_H
#define CREATEINVOICES_H

#include <QDialog>
#include <QTableWidgetItem>
namespace Ui {
class CreateInvoices;
}

class CreateInvoices : public QDialog
{
    Q_OBJECT

public:
    explicit CreateInvoices(QWidget *parent = 0);
    QString genInvoiceNumber();
    int m_booking_id;
    bool checkIfBookingAdded(int log_id);
    int genInvoiceId();
    void addBookingData(int booking_id);
    QString getInvoiceNumberFromBookingId(int booking_id);


    double getCgstTaxAmount();
    double getSgstTaxAmount();

    ~CreateInvoices();

private slots:


    //void on_add_booking_clicked();







    bool isInvoicesCreated(int booking_id);



    void on_saveBtn_clicked();

    void on_cancelInvBtn_clicked();

private:
    Ui::CreateInvoices *ui;
};

#endif // CREATEINVOICES_H
