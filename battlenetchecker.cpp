#include "battlenetchecker.hpp"
#include <QProcess>
#include <QDebug>

BattlenetChecker::BattlenetChecker(QObject *parent) : QObject(parent)
{
    this->checkBnet.setSingleShot(true);
    connect(&(this->checkBnet), SIGNAL(timeout()),
            this, SLOT(check()));
    _proc.setProgram("/home/rholk/sbin/checkbnet");
    connect(&this->_proc, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(finished(int,QProcess::ExitStatus)));
}

void    BattlenetChecker::bnetClean()
{
    emit msg("Optimus", "Force killing Bnet", QSystemTrayIcon::Warning, 5000);
    QProcess::startDetached("/home/rholk/sbin/checkbnet", QStringList("forceclean"));
}

void    BattlenetChecker::check()
{
    _proc.start();
}

void    BattlenetChecker::newState(bool st)
{
    if (st)
    {
        this->checkBnet.start(0);
        this->_norestart = false;
    }
    else
    {
        this->checkBnet.stop();
        this->_norestart = true;
    }
}

void    BattlenetChecker::finished(int , QProcess::ExitStatus )
{
    QByteArray data = this->_proc.readAll().trimmed();

    if (!this->_norestart)
        this->checkBnet.start(5000);
    if (data == "KILLING")
    {
        emit msg("Optimus", "Killed Agent.exe", QSystemTrayIcon::Information, 5000);
        return ;
    }
    if (data == "NO RUN")
        return ;
    if (data == "NO AGENT")
        return ;
    if (data == "RUNNING")
        return ;
}
