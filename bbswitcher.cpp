#include "bbswitcher.hpp"
#include <QProcess>
#include <QDebug>
#include "log.h"

bbswitcher::bbswitcher(QObject *parent) : QObject(parent)
{
    f.setFileName("/proc/acpi/bbswitch");
}

void    bbswitcher::open()
{
    f.open(QFile::WriteOnly);
    if (!f.isOpen())
    {
        QProcess::execute("sudo chmod a+w /proc/acpi/bbswitch");
        Log::addLog("ow:INFO:According myself rights to bbswitch");
        if (!f.open(QFile::WriteOnly))
            Log::addLog("ow:ERROR:COULD NOT OPEN BBSWITCH!");
    }
}

void    bbswitcher::poweron()
{
  if (QFile::exists("/tmp/lockopti_watcher") == false)
  {
    this->open();
    Log::addLog("ow:INFO:Powering on card");
    if (this->f.write("ON\n") == -1)
        Log::addLog("ow:ERROR:COULD NOT WRITE ON");
    this->f.close();
  }
  else
    Log::addLog("ow:INFO:Nothing, locked...");
}

void    bbswitcher::poweroff()
{
    if (QFile::exists("/tmp/lockopti_watcher") == false)
    {
      this->open();
      Log::addLog("ow:INFO:Powering off card");
      if (this->f.write("OFF\n") == -1)
          Log::addLog("ow:ERROR:COULD NOT WRITE OFF");
      this->f.close();
    }
    else
      Log::addLog("ow:INFO:Nothing, locked...");
}
