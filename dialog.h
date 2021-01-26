#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();
    long long lastin,lastout;
    double m_timer_interval__;
    QTimer *monitor_timer__;
    QTimer *windowmovetimer;
    QSystemTrayIcon *m_systemTray;
    void movewindow();
    int dx,dy;
    void trayactived(QSystemTrayIcon::ActivationReason reason);
    QMenu *traymenu;
    QAction *quitaction;

private slots:
    void on_pushButton_pressed();

    void on_pushButton_released();

private:
    Ui::Dialog *ui;
};
#endif // DIALOG_H
