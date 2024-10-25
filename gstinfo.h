#ifndef GSTINFO_H
#define GSTINFO_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QDebug>

namespace Ui {
class GstInfo;
}

class GstInfo : public QWidget
{
    Q_OBJECT

public:
    explicit GstInfo(QWidget *parent = 0);

    void createYearlyTree();

    void populateData(int aYear);

    ~GstInfo();

private slots:
    void on_treeMonths_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_printGstReport_clicked();

private:
    Ui::GstInfo *ui;
};

#endif // GSTINFO_H
