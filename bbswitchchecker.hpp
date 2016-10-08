#ifndef BBSWITCHCHECKER_H
#define BBSWITCHCHECKER_H

#include <QObject>
#include <QFile>
#include <QRegExp>
#include <QTimer>

class bbswitchChecker : public QObject
{
    Q_OBJECT
public:
    explicit bbswitchChecker(QObject *parent = 0);

signals:
    void    newState(bool);
    void    unknowState(bool bbinfo, bool shouldbe, QString pciReport);

public slots:
    void    check();

private:
    QFile   f;
    QRegExp rexp;
    QTimer  t;
};

#endif // BBSWITCHCHECKER_H
