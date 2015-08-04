#ifndef HOTELINFO_H
#define HOTELINFO_H

#include <QWidget>

namespace Ui {
class HotelInfo;
}

class HotelInfo : public QWidget
{
    Q_OBJECT

public:
    explicit HotelInfo(QWidget *parent = 0);
    void addHotelInfoInForm();
    ~HotelInfo();

private slots:
    void on_cancelBtn_clicked();

    void on_saveInfoBtn_clicked();

private:
    Ui::HotelInfo *ui;
};

#endif // HOTELINFO_H
