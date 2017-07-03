#include "decision.hpp"
#include <QDebug>
#include <QApplication>
#include <QUrl>
#include "log.h"
#include <QProcess>

QProcess pglx;
QProcess pprocs;

decision::decision(QObject *parent) : QObject(parent)
{
    QStringList l;

    l << "glxgears" << "-info";
    pglx.setProgram("/usr/bin/primusrun");
    pglx.setArguments(l);
    pprocs.setProgram("/home/rholk/sbin/nvprocess");
    pglx.setProcessChannelMode(QProcess::MergedChannels);
    pglx.setStandardOutputFile("/tmp/glxgears", QProcess::Append);
    on = QIcon(":/nvidia-icon.png");
    off = QIcon(":/nvidia-icon-gray.png");
    unknow = QIcon(":/nvidia-icon-orange.png");
    this->autoHandle = m.addAction("auto correct");
    this->processes = m.insertSeparator(this->autoHandle);
    this->purgeaction = m.addAction("--Purge ALL--", this, SLOT(purge()));
    m.insertAction(this->processes, this->purgeaction);
    this->processes = m.insertSeparator(this->autoHandle);
    this->autoHandle->setCheckable(true);
    this->autoHandle->setChecked(true);
    this->cleanBnet = m.addAction("Clean Bnet", this, SIGNAL(bnetClean()));
    this->cleanBnet->setCheckable(false);
    this->glx = m.addAction("glxgears", this, SLOT(glxgears()));
    this->glx->setCheckable(false);
    this->exit = m.addAction("exit",QApplication::instance(), SLOT(quit()));
    this->poll.setInterval(2000);
    connect(&(this->poll), SIGNAL(timeout()),
            this, SLOT(getprocesses()));
    connect(&pprocs, SIGNAL(finished(int)),
            this, SLOT(analyze(int)));
}

void    decision::purge()
{
    QMap<QAction*, unsigned int>::iterator it = this->_processesActions.begin();
    while (it != this->_processesActions.end())
    {
        QString cmd;

        cmd = QString() + "sudo kill -9 " + QString::number(it.value());
        this->showNotif("Optimus", "Killing " + this->_processes[it.value()] + "\n" + cmd, QSystemTrayIcon::Information, 5000);
        QProcess::startDetached(cmd);
        it++;
    }
}

void    decision::kill()
{
    QAction*    act = dynamic_cast<QAction*>(this->sender());

    if (this->_processesActions.contains(act))
    {
        QString cmd;

        cmd = QString() + "sudo kill -9 " + QString::number(this->_processesActions[act]);
        this->showNotif("Optimus", "Killing " + this->_processes[this->_processesActions[act]] + "\n" + cmd, QSystemTrayIcon::Information, 5000);
        QProcess::startDetached(cmd);
    }
}

void    decision::analyze(int)
{
    QMap<unsigned int, QString> procs;

    while (true)
    {
        QByteArray sl = pprocs.readLine().trimmed();
        if (sl.size() == 0)
            break ;
        QList<QByteArray> slist = sl.split(';');
        if (slist.size() >= 2)
            procs[QString(slist[0]).toUInt()] = slist[1];
    }
    qDebug() << procs;
    for (QMap<unsigned int, QString>::iterator it = procs.begin(); it != procs.end(); it++)
    {
        if (this->_processes.contains(it.key()) == false)
        {
            // adding
            Log::addLog(QString() + "ow:INFO:Adding new process:" + QString::number(it.key()) + " " + it.value());

            this->_processes[it.key()] = it.value();
            QAction * act = m.addAction(it.value(), this, SLOT(kill()));
            m.insertAction(this->purgeaction, act);
            this->_processesActions[act] = it.key();
        }
    }
    QMap<QAction*, unsigned int>::iterator it = this->_processesActions.begin();
    while (it != this->_processesActions.end())
    {
        if (procs.contains(it.value()) == false)
        {
            Log::addLog(QString() + "ow:INFO:removing old process: " + QString::number(it.value()));
            delete it.key();
            qDebug() << "deleted";
            _processes.remove(it.value());
            qDebug() << "_processesActions";

            _processesActions.remove(it.key());
            qDebug() << " new it";

            it = this->_processesActions.begin();
        }
        else
            it++;
    }
}

void    decision::getprocesses()
{
    if (pprocs.state() != QProcess::Running)
        pprocs.start();
}

void    decision::glxgears()
{
    if (pglx.state() == QProcess::Running)
        pglx.kill();
    else
        pglx.start();
}

void decision::newState(bool st)
{
  if (st)
  {
      this->poll.start();
      this->_ic->setIcon(on);
      system("sudo touch /tmp/nvidiaunsafe");
  }
  else
  {
      this->poll.stop();
      this->_ic->setIcon(off);
      system("sudo rm /tmp/nvidiaunsafe");
  }
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
        QTimer::singleShot(2000, this, SIGNAL(poweron()));
        QTimer::singleShot(2300, this, SIGNAL(poweroff()));
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
    system("sudo touch /tmp/nvidiaunsafe");
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
