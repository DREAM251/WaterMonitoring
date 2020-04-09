#include "qfmain.h"
#include "ui_qfmain.h"
#include <QDebug>
#include <QToolButton>

QFMain::QFMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFMain),
    signalMapper(new QSignalMapper(this)),
    timer(new QTimer(this)),
    element(new ElementInterface(ET_NH3N, this))
{
    ui->setupUi(this);

    QToolButton *btns[] = {ui->statusButton, ui->measureButton, ui->calibrationButton,
                          ui->maintenanceButton, ui->settingsButton,ui->queryButton/*, ui->loginButton*/};
    for (int i = 0; i < sizeof(btns)/sizeof(QToolButton *); ++i)
    {
        connect(btns[i], SIGNAL(clicked()), signalMapper, SLOT(map()));
        signalMapper->setMapping(btns[i], i);
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(menuClicked(int)));
    connect(ui->loginButton, SIGNAL(clicked()), this, SIGNAL(userTrigger()));
    menuClicked(0);

    timer->start(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateStatus()));
    updateStatus();
}

QFMain::~QFMain()
{
    delete ui;
}

void QFMain::menuClicked(int p)
{
    QToolButton *btns[] = {ui->statusButton, ui->measureButton, ui->calibrationButton,
                          ui->maintenanceButton, ui->settingsButton, ui->queryButton, ui->loginButton};
    QString icon[] = {"://home.png","://measure.png","://calibration.png",
                      "://maintain.png","://settings.png","://query.png","://user.png"};
    QString iconw[] = {"://homew.png","://measurew.png","://calibrationw.png",
                       "://maintainw.png","://settingsw.png","://queryw.png","://userw.png"};

    for (int i = 0; i < sizeof(btns)/sizeof(QToolButton *); ++i)
    {
        btns[i]->setChecked(i == p);
        btns[i]->setIcon(QIcon(i == p ? icon[i] : iconw[i]));
    }

    ui->contentStackedWidget->setCurrentIndex(p < 2 ? 0 : p - 1);
    if (p < 2)
        ui->stackedWidget->setCurrentIndex(p);
}

void QFMain::updateStatus()
{
    QString s = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->datetime->setText(s);
}

void QFMain::login(int level)
{
}
