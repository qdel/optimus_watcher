#ifndef BATTLENETCHECKER_HPP
#define BATTLENETCHECKER_HPP

#include <QObject>
#include <QTimer>
#include <QProcess>
#include <QSystemTrayIcon>

class BattlenetChecker : public QObject
{
    Q_OBJECT
public:
    explicit BattlenetChecker(QObject *parent = 0);

private:
    QTimer   checkBnet;
    QProcess _proc;
    bool        _norestart;
private slots:
    void    check();
    void    finished(int exitCode, QProcess::ExitStatus exitStatus);
    void    bnetClean();
    void    newState(bool);
signals:
    void    msg(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon icon, int millisecondsTimeoutHint);
public slots:
};

#endif // BATTLENETCHECKER_HPP
