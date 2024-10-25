#ifndef RESTAURANT_H
#define RESTAURANT_H

#include <QWidget>

namespace Ui {
class Restaurant;
}

class Restaurant : public QWidget
{
    Q_OBJECT

public:
    explicit Restaurant(QWidget *parent = 0);
    ~Restaurant();

private:
    Ui::Restaurant *ui;
};

#endif // RESTAURANT_H
