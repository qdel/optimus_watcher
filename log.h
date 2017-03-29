#ifndef LOG_H
#define LOG_H

#include <QWidget>
#include <QString>
#include <QSystemTrayIcon>

namespace Ui {
class Log;
}

class Log : public QWidget
{
    Q_OBJECT

public:
    explicit Log(QWidget *parent = 0);
    ~Log();

    static void addLog(const QString& l);
    friend void addLog(const QString& l);

public slots:
    void    activated(QSystemTrayIcon::ActivationReason);
    void    closeEvent(QCloseEvent *event);

private:
    Ui::Log *ui;


};

#endif // LOG_H
