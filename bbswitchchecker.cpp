#include "bbswitchchecker.hpp"
#include <QByteArray>
#include <QStringList>
#include "log.h"
#include <QProcess>

bbswitchChecker::bbswitchChecker(QObject *parent) : QObject(parent)
{
    rexp = QRegExp(".*(\\[[ 0-9.]*\\]) (.*)");
    connect(&f, SIGNAL(readyRead()), this, SLOT(check()));
    connect(&t, SIGNAL(timeout()), this, SLOT(check()));
    connect(&tRestart, SIGNAL(timeout()), this, SLOT(check()));
    t.start(1000);
    tRestart.setInterval(1000 * 3600);
    tRestart.start();
    f.setFileName("/var/log/messages");
    QTimer::singleShot(0, this, SIGNAL(check()));
}

void    bbswitchChecker::check()
{
  bool shouldBe = false;
  bool foundBbinfo = false;
  bool unknow = false;
  QString mode;

  if (this->sender() == &f)
      Log::addLog(QString() + "ow: trigger by file");
  else if (this->sender() == &tRestart)
      Log::addLog(QString() + "ow: trigger by tempoRestart");
  if (f.isOpen())
  {

    QByteArray b;
    b = f.readLine();
    if (b.size() == 0 && this->sender() == &tRestart)
    {
        Log::addLog(QString() + "ow: file is open, but no log since 1m, try re-open...");
        f.close();
        QTimer::singleShot(0, this, SIGNAL(check()));
    }
    else if (b.size() != 0)
        tRestart.start();
    while (b.length())
      {
        int pos = rexp.indexIn(b.trimmed());
        if (pos > -1)
          {
            QByteArray line = rexp.cap(2).toUtf8();

            Log::addLog(QString() + "lm: " + line);
            if (line == "bbswitch: disabling discrete graphics")
            {
               shouldBe = false;
               foundBbinfo = true;
               unknow = false;
            }
            if (line == "bbswitch: Succesfully loaded. Discrete card 0000:01:00.0 is on" ||
                line == "bbswitch: Unloaded. Discrete card 0000:01:00.0 is on")
            {
                shouldBe = true;
                foundBbinfo = true;
                unknow = false;
            }
            else if (line == "bbswitch: Succesfully loaded. Discrete card 0000:01:00.0 is off" ||
                     line == "bbswitch: Unloaded. Discrete card 0000:01:00.0 is off")
            {
                shouldBe = false;
                foundBbinfo = true;
                unknow = false;
            }
            else if (line == "bbswitch: device 0000:01:00.0 is in use by driver 'nvidia', refusing OFF")
            {
                shouldBe = true;
                foundBbinfo = true;
                unknow = false;
            }
            else if (line == "bbswitch: enabling discrete graphics")
              {
                shouldBe = true;
                foundBbinfo = true;
                unknow = false;
              }
            if (line == "pci 0000:01:00.0: Refused to change power state, currently in D0")
              {
                unknow = true;
                mode = "D0";
              }
            else if (line == "pci 0000:01:00.0: Refused to change power state, currently in D3Cold")
              {
                unknow = true;
                mode = "D3Cold";
              }
            else if (line == "OUT OF SLEEP, NVIDIA CARD SHOULD BE OFF")
            {
                unknow = true;
                shouldBe = false;
                mode = "Wake Off";
            }
            else if (line == "OUT OF SLEEP, NVIDIA CARD SHOULD BE ON")
            {
                unknow = true;
                shouldBe = false;
                mode = "Wake On";
            }
            else if (line == "nouveau 0000:01:00.0: DRM: resuming object tree..." ||
                     line == "nouveau 0000:01:00.0: DRM: resuming fence...")
            {
                shouldBe = true;
                foundBbinfo = true;
                unknow = false;
            }
            else if (line == "nouveau 0000:01:00.0: DRM: suspending object tree..." ||
                     line == "nouveau 0000:01:00.0: DRM: suspending fence...")
            {
                shouldBe = false;
                foundBbinfo = true;
                unknow = false;
            }
            else if (line.contains("bbswitch_proc_write"))
            {
                unknow = true;
                foundBbinfo = false;
                shouldBe = false;
                mode = "D0";
                Log::addLog("ow:INFO:reloading bbswitch module");
                QProcess::execute("sudo modprobe -r bbswitch");
                QProcess::execute("sudo modprobe bbswitch");
            }
          }
        b = f.readLine();
      }
    if (unknow)
      emit unknowState(foundBbinfo, shouldBe, mode);
    else if (foundBbinfo)
      emit newState(shouldBe);
  }
  else
  {
     Log::addLog("ow:INFO:re-opening syslog file!");
     f.open(QFile::ReadOnly);
     if (!f.isOpen())
        // Warning
         Log::addLog("ow:ERROR:optimus_watcher can't open syslog!");
     else
     {
            unsigned int pos;

            pos = f.size() - 2000;
            if (pos > 0)
                f.seek(pos);
            emit unknowState(false, false, "??");
            QTimer::singleShot(0, this, SIGNAL(check()));
     }
  }
}
