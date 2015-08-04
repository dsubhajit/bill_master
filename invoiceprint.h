#ifndef INVOICEPRINT_H
#define INVOICEPRINT_H

#include <QWidget>

namespace Ui {
class InvoicePrint;
}

class InvoicePrint : public QWidget
{
    Q_OBJECT

public:
    explicit InvoicePrint(QWidget *parent = 0);
    ~InvoicePrint();

private:
    Ui::InvoicePrint *ui;
};

#endif // INVOICEPRINT_H
