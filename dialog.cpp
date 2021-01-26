#include "dialog.h"
#include "ui_dialog.h"
#include "sys/statfs.h"
#include <QProcess>
#include <QtDebug>
#include <QTimer>
#include <QGraphicsDropShadowEffect>
#include <QMenu>
#include <QStandardPaths>
#include <QFile>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    setWindowFlags (Qt::FramelessWindowHint);
    setWindowOpacity(0.8);
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);

    QString confFilename = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation)[0]+"/nemon.conf";
    QFile confFile(confFilename);
    if(confFile.exists()){
        if (confFile.open(QIODevice::ReadOnly | QIODevice::Text)){
            QString tmp=confFile.readAll();
            QStringList tmplist = tmp.split(' ');
            if(tmplist.size()==2) this->setGeometry(tmplist[0].toInt(),tmplist[1].toInt(),this->geometry().width(),this->geometry().height());
            confFile.close();
        }
    }

    ui->pushButton->setFlat(true);

    m_systemTray = new QSystemTrayIcon(this);
    m_systemTray->setIcon(QIcon(":/res/huorong.png"));
    m_systemTray->setToolTip("Nemon [Huorong Theme]");
    m_systemTray->show();

    traymenu = new QMenu();

    QAction *pinfo = new QAction(traymenu);
    pinfo->setText("Nemon by [Bilibili: Rullix]");
    pinfo->setEnabled(false);
    traymenu->addAction(pinfo);

    quitaction = new QAction(traymenu);
    quitaction->setText("Exit");
    traymenu->addAction(quitaction);
    connect(quitaction,&QAction::triggered,this,&QApplication::exit);
    m_systemTray->setContextMenu(traymenu);


    m_timer_interval__ = 1000;
    lastin=0;
    lastout=0;

    windowmovetimer = new QTimer(this);
    connect(windowmovetimer,&QTimer::timeout,this,&Dialog::movewindow);

    monitor_timer__ = new QTimer(this);
    connect(monitor_timer__, &QTimer::timeout, this, [&](){

        QProcess process;
        process.start("cat /proc/net/dev");
        process.waitForFinished();
        process.readLine();
        process.readLine();
        long long in=0,out=0;
        while(!process.atEnd())
        {
            QString str = process.readLine();

            str.replace("\n","");
            str.replace(QRegExp("( ){1,}")," ");
            QStringList lst = str.split(" ");
            //qDebug()<<lst;
            int flag=0;
            for(int i=0;i<lst.size();i++){
                if(lst[i]=="lo:"){
                    flag=1;
                    break;
                }
            }
            if(flag==1) continue;
            in+=lst[lst.size()-16].toLong();
            out+=lst[lst.size()-8].toLong();

        }
        /*
        QProcess process;
        process.start("ip -s link");
        process.waitForFinished();
        long long in=0,out=0;
        while(!process.atEnd())
        {
            QString str = process.readLine();
            if(str[3]=='l'&&str[4]=='o'){
                for(int i=0;i<5;i++) process.readLine();
            }else{
                process.readLine();
                process.readLine();
                str=process.readLine();
                str.replace(QRegExp("( ){1,}")," ");
                QStringList strlist= str.split(' ');
                in+=strlist[1].toLong();
                process.readLine();
                str=process.readLine();
                str.replace(QRegExp("( ){1,}")," ");
                strlist= str.split(' ');
                out+=strlist[1].toLong();
            }
        }
        */

        double inspeed=(in-lastin)/1024.0;
        double outspeed=(out-lastout)/1024.0;
        if(lastin!=0){
            if(outspeed>=1000) ui->label->setText(QString().asprintf("↑ %.1f MB/s",outspeed/1024.0));
            else ui->label->setText(QString().asprintf("↑ %.1f KB/s",outspeed));

            if(inspeed>=1000) ui->label_2->setText(QString().asprintf("↓ %.1f MB/s",inspeed/1024.0));
            else ui->label_2->setText(QString().asprintf("↓ %.1f KB/s",inspeed));
        }
        lastin=in;
        lastout=out;
        //qDebug()<<"inspd = "<<inspeed<<" & outspd = "<<outspeed;

    });
    monitor_timer__->start(m_timer_interval__);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::trayactived(QSystemTrayIcon::ActivationReason reason){
    if(reason==QSystemTrayIcon::Context){
        traymenu->show();
    }
}

void Dialog::movewindow(){
    this->setGeometry(QCursor::pos().x()-dx,QCursor::pos().y()-dy,this->geometry().width(),this->geometry().height());
}

void Dialog::on_pushButton_pressed()
{
    ui->pushButton->setText("MOVING");
    dx=QCursor::pos().x()-this->geometry().x();
    dy=QCursor::pos().y()-this->geometry().y();
    windowmovetimer->start(1);
}

void Dialog::on_pushButton_released()
{
    ui->pushButton->setText("");
    windowmovetimer->stop();

    QString confFilename = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation)[0]+"/nemon.conf";
    QFile confFile(confFilename);

    if (confFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        QString tmp = QString::asprintf("%d %d",this->geometry().x(),this->geometry().y());
        confFile.write(tmp.toUtf8(),tmp.toUtf8().length());
        confFile.close();
    }

}
