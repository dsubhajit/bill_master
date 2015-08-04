#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#include <QWidget>

namespace Ui {
class Accounts;
}

class Accounts : public QWidget
{
    Q_OBJECT

public:
    explicit Accounts(QWidget *parent = 0);
    ~Accounts();
    void createIncomeTable();
    void createExpendatureTable();

private slots:
    void on_addExpBtn_clicked();

    void on_removeExpBtn_clicked();
    void on_totalIncome_textChanged(const QString &arg1);

    void on_totalExpenditure_textChanged(const QString &arg1);

    void on_expFilter_clicked();

    void on_printButton_clicked();

private:
    Ui::Accounts *ui;
};

#endif // ACCOUNTS_H
