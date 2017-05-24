#include "log.h"
#include "ui_log.h"
#include <QDebug>

Log*    log = NULL;

Log::Log(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Log)
{
    ui->setupUi(this);
    this->setWindowTitle("Log");
    log = this;
}

Log::~Log()
{
    delete ui;
}

void Log::addLog(const QString& l)
{
    qDebug() << l;
    log->ui->l->appendPlainText(l);
}

void    Log::activated(QSystemTrayIcon::ActivationReason r)
{
    if (r == QSystemTrayIcon::Trigger || r == QSystemTrayIcon::MiddleClick)
    {
        if (this->isVisible())
            this->hide();
        else
            this->show();
    }
}

void    Log::closeEvent(QCloseEvent *event)
{
    this->hide();
    event->ignore();
}
