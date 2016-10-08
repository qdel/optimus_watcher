#include "bbswitcher.hpp"
#include <QProcess>
#include <QDebug>

bbswitcher::bbswitcher(QObject *parent) : QObject(parent)
{
    f.setFileName("/proc/acpi/bbswitch");
}

void    bbswitcher::open()
{
    if (!f.isOpen())
    {
        QProcess::execute("sudo chmod a+w /proc/acpi/bbswitch");
        qDebug() << "According myself rights to bbswitch";
        f.open(QFile::WriteOnly);
    }
}

void    bbswitcher::poweron()
{
  if (QFile::exists("/tmp/lockopti_watcher") == false)
  {
    this->open();
    qDebug() << "Powering on card";
    this->f.write("ON\n");
    this->f.close();
  }
}

void    bbswitcher::poweroff()
{
    if (QFile::exists("/tmp/lockopti_watcher") == false)
    {
      this->open();
      qDebug() << "Powering off card";
      this->f.write("OFF\n");
      this->f.close();
    }
}
