#include "bbswitchchecker.hpp"
#include <QByteArray>
#include <QStringList>
#include <QDebug>
#include <QDebug>

bbswitchChecker::bbswitchChecker(QObject *parent) : QObject(parent)
{
    rexp = QRegExp(".*(\\[[ 0-9.]*\\]) (.*)");
    connect(&f, SIGNAL(readyRead()), this, SLOT(check()));
    connect(&t, SIGNAL(timeout()), this, SLOT(check()));
    t.start(500);
    f.setFileName("/var/log/messages");
}

void    bbswitchChecker::check()
{
  bool shouldBe = false;
  bool foundBbinfo = false;
  bool unknow = false;
  QString mode;

  qDebug() << "begin of read";
  if (f.isOpen())
  {
    QByteArray b;
    b = f.readLine();
    while (b.length())
      {
        int pos = rexp.indexIn(b.trimmed());
        if (pos > -1)
          {
            QByteArray line = rexp.cap(2).toUtf8();

            qDebug() << "Read:" << line;
            if (line == "bbswitch: disabling discrete graphics")
            {
               shouldBe = false;
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
                mode = "Wake Off";
            }
            else if (line == "OUT OF SLEEP, NVIDIA CARD SHOULD BE ON")
            {
                unknow = true;
                mode = "Wake On";
            }
          }
        b = f.readLine();
        qDebug() <<"reading...";
      }
    if (unknow)
      emit unknowState(foundBbinfo, shouldBe, mode);
    else if (foundBbinfo)
      emit newState(shouldBe);
  }
  else
  {
     f.open(QFile::ReadOnly);
     if (!f.isOpen())
        // Warning
        qDebug() << "cant't open syslog!";
     else
     {
            unsigned int pos;

            pos = f.size() - 500;
            if (pos > 0)
                f.seek(pos);
            emit unknowState(false, false, "??");
            this->check();
     }
  }
  qDebug() << "End of read";
}
