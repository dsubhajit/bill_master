#ifndef ADDFOODS_H
#define ADDFOODS_H

#include <QDialog>
#include "foods.h"

namespace Ui {
class AddFoods;
}

class AddFoods : public QDialog
{
    Q_OBJECT
    Foods *m_food;
public:

    explicit AddFoods(QWidget *parent = 0);
    void addFoodParent(Foods *parent);
    void setEditMode(bool aEditMode);
    void setFoodId(int aFoodId);
    ~AddFoods();

private slots:
    void on_cancelAddFood_clicked();

    void on_saveBtn_clicked();

private:
    Ui::AddFoods *ui;
    bool mEditMode;
    int mFoodId;
};

#endif // ADDFOODS_H
