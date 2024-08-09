#include "mytextedit.h"
#include <QWheelEvent>
#include <QDebug>

//继承QTextEdit的构造函数，方便提升原有ui的TextEdit组件之后，原来所作的操作都还能用
MyTextEdit::MyTextEdit(QWidget *parent) : QTextEdit(parent)
{

}

void MyTextEdit::wheelEvent(QWheelEvent *e)
{
    if(ctrlPressed == 1){
        //qDebug() << e->angleDelta().y();
        if(e->angleDelta().y() > 0){
            zoomIn();
        }else if(e->angleDelta().y() < 0){
            zoomOut();
        }
        e->accept();
    }else{
        QTextEdit::wheelEvent(e);//在ctrl未被按下时，交给它原来的逻辑处理，即上下滑动
    }
}

void MyTextEdit::keyPressEvent(QKeyEvent *e)
{

    if(e->key() == Qt::Key_Control){
        //qDebug() << "ctrl press";
        ctrlPressed = 1;
    }
    QTextEdit::keyPressEvent(e);
}

void MyTextEdit::keyReleaseEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Control){
        //qDebug() << "ctrl release";
        ctrlPressed = 0;
    }
    QTextEdit::keyPressEvent(e);
}
