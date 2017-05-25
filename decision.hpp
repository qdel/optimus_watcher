#ifndef DECISION_H
#define DECISION_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QTimer>

class decision : public QObject
{
    Q_OBJECT
public:
    explicit decision(QObject *parent = 0);
    void    setSysTray(QSystemTrayIcon*);
signals:
    void poweron();
    void poweroff();
    void ask(bool,bool,QString);
    void bnetClean();

public slots:
  void newState(bool st);
  void unknowState(bool bbinfo, bool shouldbe, QString pciReport);
  void showNotif(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon icon, int millisecondsTimeoutHint);
  void glxgears();
  void getprocesses();
  void analyze(int);
  void kill();
  void purge();

private:
  void  _tryCorrect(bool,bool,QString);
  QSystemTrayIcon*   _ic;
  QMenu m;
  QAction*   autoHandle;
  QAction*   cleanBnet;
  QAction*   processes;
  QAction*   glx;
  QAction*   exit;
  QAction*   purgeaction;
  QTimer     poll;
  QIcon on;
  QIcon off;
  QIcon unknow;
  QMap<unsigned int, QString>   _processes;
  QMap<QAction*, unsigned int>  _processesActions;

};

#endif // DECISION_H
