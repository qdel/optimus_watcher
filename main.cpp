#include <QApplication>
#include "bbswitchchecker.hpp"
#include <QDebug>
#include <QSystemTrayIcon>
#include "decision.hpp"
#include "bbswitcher.hpp"
#include "runGuard.hpp"

RunGuard* guard = NULL;

bool    takeGuard()
{
#ifndef NOGUARD
  guard = new RunGuard("optimus_watcher");
  if ( !guard->tryToRun() )
    return false;
#endif
  return true;
}

void    releaseGuard()
{
  delete guard;
}

int run(int argc, char *argv[])
{
    QApplication        app(argc, argv);
    bbswitchChecker     b;
    decision*           d;
    QSystemTrayIcon*    ic;
    bbswitcher          bsw;

    ic = new QSystemTrayIcon(&app);
    d = new decision(&app);
    d->setSysTray(ic);
    QObject::connect(&b, SIGNAL(newState(bool)),
            d, SLOT(newState(bool)));
    QObject::connect(&b, SIGNAL(unknowState(bool,bool,QString)),
            d, SLOT(unknowState(bool,bool,QString)));
    QObject::connect(d, SIGNAL(poweron()),
                     &bsw, SLOT(poweron()));
    QObject::connect(d, SIGNAL(poweroff()),
                     &bsw, SLOT(poweroff()));
    int code = app.exec();
    releaseGuard();
    return code;
}

int main(int argc, char *argv[])
{
    if (takeGuard())
        return run(argc, argv);
    return 1;
}
