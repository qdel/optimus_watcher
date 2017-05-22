#include "decision.hpp"
#include <QDebug>
#include <QApplication>
#include <QUrl>
#include "log.h"
#include <QProcess>

QProcess pglx;

decision::decision(QObject *parent) : QObject(parent)
{
    QStringList l;

    l << "glxgears" << "-info";
    pglx.setProgram("/usr/bin/primusrun");
    pglx.setArguments(l);
    pglx.setProcessChannelMode(QProcess::MergedChannels);
    pglx.setStandardOutputFile("/tmp/glxgears", QProcess::Append);
    on = QIcon(":/nvidia-icon.png");
    off = QIcon(":/nvidia-icon-gray.png");
    unknow = QIcon(":/nvidia-icon-orange.png");
    this->autoHandle = m.addAction("auto correct");
    this->autoHandle->setCheckable(true);
    this->autoHandle->setChecked(true);
    this->cleanBnet = m.addAction("Clean Bnet", this, SIGNAL(bnetClean()));
    this->cleanBnet->setCheckable(false);
    this->glx = m.addAction("glxgears", this, SLOT(glxgears()));
    this->glx->setCheckable(true);
    this->exit = m.addAction("exit",QApplication::instance(), SLOT(quit()));
}

void    decision::glxgears()
{
    qDebug() << "glx" << this->glx->isChecked();
    if (this->glx->isChecked())
        pglx.start();
    else
        pglx.kill();
}

void decision::newState(bool st)
{
  if (st)
      this->_ic->setIcon(on);
  else
      this->_ic->setIcon(off);
  this->_ic->show();
}

void decision::_tryCorrect(bool bbinfo, bool shouldbe, QString pciReport)
{
    if (pciReport == "D0" && bbinfo && shouldbe == false && this->autoHandle->isChecked())
    {
        Log::addLog("ow:INFO:on then off because of D0 glitch");
        emit poweron();
        emit poweroff();
    }
    else if (pciReport == "Wake Off" && this->autoHandle->isChecked())
    {
        Log::addLog("ow:INFO:on then off because of wakeup off");
        emit poweron();
        emit poweroff();
    }
    else if (pciReport == "Wake On" && this->autoHandle->isChecked())
    {
        Log::addLog("ow:INFO:on then off because of wakeup on");
        emit poweron();
        emit poweroff();
    }
    else if (pciReport == "??" && shouldbe == false && this->autoHandle->isChecked())
    {
        Log::addLog("ow:INFO:Try on then off");
        emit poweron();
        emit poweroff();
    }
    else
    {
        emit ask(bbinfo, shouldbe, pciReport);
    }

}

void decision::unknowState(bool bbinfo, bool shouldbe, QString pciReport)
{
    this->_ic->setIcon(unknow);
    this->_ic->show();
    this->_tryCorrect(bbinfo, shouldbe, pciReport);

}

void decision::setSysTray(QSystemTrayIcon* ic)
{
    this->_ic = ic;
    this->_ic->setContextMenu(&m);
    this->_ic->setIcon(unknow);
    this->_ic->setVisible(true);
    this->_ic->show();
}

void    decision::showNotif(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon icon, int millisecondsTimeoutHint)
{
    this->_ic->showMessage(title, message, icon, millisecondsTimeoutHint);
}
