#ifndef BBSWITCHER_H
#define BBSWITCHER_H

#include <QObject>
#include <QFile>

class bbswitcher : public QObject
{
    Q_OBJECT
public:
    explicit bbswitcher(QObject *parent = 0);

signals:

public slots:
    void poweron();
    void poweroff();

private:
    void    open();
    QFile   f;
};

#endif // BBSWITCHER_H
