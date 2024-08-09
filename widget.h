#ifndef WIDGET_H
#define WIDGET_H

#include <QFile>
#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    QFile file;
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void zoomIn();
    void zoomOut();
private slots:
    void on_openpushButton_clicked();

    void on_savepushButton_clicked();

    void on_closepushButton_3_clicked();

    void oncurrentIndexChanged(int index);

    void oncursorPositionChanged();

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
