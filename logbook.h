#ifndef LOGBOOK_H
#define LOGBOOK_H

#include <QWidget>

namespace Ui {
class LogBook;
}

class LogBook : public QWidget
{
    Q_OBJECT

public:
    explicit LogBook(QWidget *parent = 0);
    void populateLogBookTable();
    ~LogBook();

private slots:
    void on_logbook_cellDoubleClicked(int row, int column);



    void on_createInvoice_clicked();

private:
    Ui::LogBook *ui;
};

#endif // LOGBOOK_H
