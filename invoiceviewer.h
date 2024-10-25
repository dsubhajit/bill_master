#ifndef INVOICEVIEWER_H
#define INVOICEVIEWER_H

#include <QDialog>

namespace Ui {
class InvoiceViewer;
}

class InvoiceViewer : public QDialog
{
    Q_OBJECT

public:
    explicit InvoiceViewer(QWidget *parent = 0);

    void getInvoices(int aInvId);
    QString getLodgingInvoice(int aInvId);
    QString getFoodingInvoice(int aInvId);

    ~InvoiceViewer();

private:
    Ui::InvoiceViewer *ui;
int mInvoiceId;

};

#endif // INVOICEVIEWER_H
