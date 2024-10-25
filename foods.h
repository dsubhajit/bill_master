#ifndef FOODS_H
#define FOODS_H

#include <QWidget>

namespace Ui {
class Foods;
}

class Foods : public QWidget
{
    Q_OBJECT

public:
    explicit Foods(QWidget *parent = 0);
    void createFoodsTable();
    void createBookingTable();
    void createFoodOrderTable(int bookingId);
    ~Foods();

private slots:

    void on_bookingTable_cellDoubleClicked(int row, int column);

    void on_addOrder_clicked();

    void on_addFood_clicked();

    void on_deleteFoods_clicked();

    void on_pushButton_clicked();

    void on_searchBtn_clicked();

    void on_reload_clicked();

    void on_removeOrder_clicked();

    void on_finalizeFoods_clicked();

    void on_editFoodDetails_clicked();

private:
    Ui::Foods *ui;
};

#endif // FOODS_H
