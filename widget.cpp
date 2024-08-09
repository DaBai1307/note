#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QDebug>
#include <iostream>
#include <QMessageBox>
#include <QShortcut>
#include <QWheelEvent>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setLayout(ui->verticalLayout);
    ui->widget2->setLayout(ui->horizontalLayout2);

    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(oncurrentIndexChanged(int)));//编码信号
    connect(ui->textEdit,SIGNAL(cursorPositionChanged()),this,SLOT(oncursorPositionChanged()));//行列信号


    //快捷键
    QShortcut* shortcutOpen = new QShortcut(QKeySequence(tr("Ctrl+O", "File|Open")),this);
    QShortcut* shortcutSave = new QShortcut(QKeySequence(tr("Ctrl+S", "File|Save")),this);
    connect(shortcutOpen,&QShortcut::activated,[=](){//lambda表达式用作连接，activated是QShortcut自带的信号
        on_openpushButton_clicked();
    });
    connect(shortcutSave,&QShortcut::activated,[=](){
        on_savepushButton_clicked();
    });

    //字体放大缩小 ( 快捷键实现 )
    QShortcut* shortcutZoomIn = new QShortcut(QKeySequence(tr("Ctrl+Shift+=", "File|Save")),this);
    QShortcut* shortcutZoomOut = new QShortcut(QKeySequence(tr("Ctrl+Shift+-", "File|Save")),this);
    connect(shortcutZoomIn,&QShortcut::activated,[=](){
        zoomIn();
    });
    connect(shortcutZoomOut,&QShortcut::activated,[=](){
        zoomOut();
    });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::zoomIn()
{
    //获得TextEdit的当前字体信息
    QFont font = ui->textEdit->font();
    //获得当前字体的大小
    int fontSize = font.pointSize();
    if(fontSize == -1) return;
    //改变大小，并设置字体
    int newFontSize = fontSize + 1;
    font.setPointSize(newFontSize);
    ui->textEdit->setFont(font);
}

void Widget::zoomOut()
{
    //获得TextEdit的当前字体信息
    QFont font = ui->textEdit->font();
    //获得当前字体的大小
    int fontSize = font.pointSize();
    if(fontSize == -1) return;
    //改变大小，并设置字体
    int newFontSize = fontSize - 1;
    font.setPointSize(newFontSize);
    ui->textEdit->setFont(font);
}

void Widget::on_openpushButton_clicked()
{
    //打开文本
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),
                                                    "E:/qt/qtproject/note_final/",
                                                    tr("Text (*.txt)"));

    //选完文件就可以作相应的清空，清空上一个文件
    ui->textEdit->clear();

    file.setFileName(fileName);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)){
        qDebug() << "file open error";
    }
    this->setWindowTitle(fileName+"-记事本");
    //读取
    QTextStream on(&file);
    //on.setCodec("UTF-8");
    //设置编码
    QString str = ui->comboBox->currentText();
    const char* c_str = str.toStdString().c_str();//将原来的Qstring转换为char*类型
    on.setCodec(c_str);//这个括号里需要传char*类型
    while(!on.atEnd()){
        QString context = on.readLine();
        ui->textEdit->append(context);
    }
}

void Widget::on_savepushButton_clicked()
{
    if(!file.isOpen()){
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                    "E:/qt/qtproject/note_final/test5.txt",
                                    tr("Text (*.txt *.doc)"));
        file.setFileName(fileName);
        if (!file.open(QIODevice::Append|QIODevice::Text)){//这里用Append或者WriteOnly都可以
            qDebug() << "file open error";
        }
        this->setWindowTitle(fileName+"-记事本");
    }
    QTextStream out(&file);
    QString str = ui->comboBox->currentText();
    const char* c_str = str.toStdString().c_str();//将原来的Qstring转换为char*类型
    out.setCodec(c_str);//这个括号里需要传char*类型
    //out.setCodec("UTF-8");
    file.resize(0);//在写入前清空原文件，因为写入操作会将当前文本的全部内容写入，不清空会产生多余的内容
    QString context = ui->textEdit->toPlainText();
    out << context;
}

void Widget::on_closepushButton_3_clicked()
{
    int ret = QMessageBox::warning(this, tr("记事本"),
                                 tr("是否保存当前文档?"),
                                 QMessageBox::Save | QMessageBox::Discard
                                 | QMessageBox::Cancel,
                                 QMessageBox::Save);
    switch (ret) {
        case QMessageBox::Save:
            on_savepushButton_clicked();
            break;
        case QMessageBox::Discard:
            ui->textEdit->clear();
            break;
        case QMessageBox::Cancel:
            // Cancel was clicked
            break;
        default:
            // should never be reached
            break;
      }
}

void Widget::oncurrentIndexChanged(int index){//这个函数被调用会将当前文档乱码问题换个编码解决
    ui->textEdit->clear();
    if(file.isOpen()){
        QTextStream on(&file);
        on.setCodec(ui->comboBox->currentText().toStdString().c_str());//这里和上面一样转类型，只是写成一句话
        file.seek(0);//之前读文本光标已经走到最后了，需要将光标重置到开始的位置
        while(!on.atEnd()){//重新读
            QString context = on.readLine();
            ui->textEdit->append(context);
        }
    }
}

void Widget::oncursorPositionChanged()//设置高亮
{
    QTextCursor cursor = ui->textEdit->textCursor();//列
    //qDebug() << cursor.blockNumber() + 1 << "," <<cursor.columnNumber() + 1;//分别输出行、列号
    QString blockNum = QString::number(cursor.blockNumber() + 1);//行
    QString columnNum = QString::number(cursor.columnNumber() + 1);//列
    const QString labelMes = "第"+blockNum+"行,第"+columnNum+"列    ";
    ui->label->setText(labelMes);
    //设置当前行的高亮
    QList<QTextEdit::ExtraSelection> extraSelection;
    QTextEdit::ExtraSelection ext;
    //1、知道当前行
    ext.cursor = cursor;
    QBrush qBrush(Qt::lightGray);
    //2、颜色
    ext.format.setBackground(qBrush);
    ext.format.setProperty(QTextFormat::FullWidthSelection,true);//整行显示
    //3、设置
    extraSelection.append(ext);
    ui->textEdit->setExtraSelections(extraSelection);
}
