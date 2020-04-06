#include "qfmain.h"
#include "ui_qfmain.h"

QFMain::QFMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFMain),
    signalMapper(new QSignalMapper(this)),
    timer(new QTimer(this))
{
    ui->setupUi(this);
    addChemicalModule("MNH3N");

    QToolButton *btns[] = {ui->statusButton, ui->measureButton, ui->calibrationButton,
                          ui->maintenanceButton, ui->settingsButton,ui->queryButton, ui->loginButton};
    for (int i = 0; i < sizeof(btns)/sizeof(QToolButton *); ++i)
    {
        connect(btns[i], SIGNAL(clicked()), signalMapper, SLOT(map()));
        signalMapper->setMapping(btns[i], i);
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(menuClicked(int)));
    menuClicked(0);

    timer->start(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateStatus()));
    updateStatus();

}

QFMain::~QFMain()
{
    delete ui;
}

void QFMain::addChemicalModule(const QString &name)
{
    element = new MNH3NElement(name, "/dev/ttyAMA2");
    ui->lyContent->addWidget(element);
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
        btns[i]->setIcon(QIcon(i==p ? icon[i] : iconw[i]));
    }

    switch (p)
    {
    case 0:
        element->slot_ConfParam();
        break;
    case 1:
        element->slot_pBMC();
        break;
    case 2:
        element->slot_pBCalibration();
        break;
    case 3:
        element->slot_pBSystemMaintenance();
        break;
    case 4:
        element->slot_pBMachineSet();
        break;
    case 5:
        element->slot_pBMachineDataQuery();
        break;
    case 6:
        element->slot_pBUserManagement();
        break;
    }
}

void QFMain::updateStatus()
{
    QString s = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->datetime->setText(s);
}
