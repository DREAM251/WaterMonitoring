#include "qfmain.h"
#include "ui_qfmain.h"
#include "ui_setui.h"
#include <QDebug>
#include <QToolButton>

QFMain::QFMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFMain),
    setui(new Ui::SetUI),
    signalMapper(new QSignalMapper(this)),
    timer(new QTimer(this)),
    element(new ElementInterface(ET_NH3N, this)),
    userDB(NULL)
{
    ui->setupUi(this);

    // calibraiton
    calibframe = new CalibFrameUser;
    ui->contentStackedWidget->addWidget(calibframe);

    // set ui
    QWidget *setw = new QWidget();
    setui->setupUi(setw);
    setui->tabWidget->setCurrentIndex(0);
    ui->contentStackedWidget->addWidget(setw);
    ui->contentStackedWidget->setCurrentIndex(0);
    connect(setui->Save, SIGNAL(clicked()), this, SLOT(saveSettings()));

    // query ui
    QTabWidget *tabwidget = new QTabWidget();
    ui->contentStackedWidget->addWidget(tabwidget);
    {
        int column1 = 9;
        QString label = tr("测量数据查询");
        QString table1 = "Data";
        QString items1 = "A1,A2,A3,A4,A5,B1,B2,A6,A8,A7";
        QString name1[] = {
            tr("时间"),
            tr("浓度(mg/L)"),
            tr("吸光度"),
            tr("空白值C1"),
            tr("显色值C1"),
            tr("空白值C2"),
            tr("显色值C2"),
            tr("温度"),
            tr("操作类型"),
            tr("湿度(%)")
        };
        //   int width1[] = {120,100,70,65,65,65,68,120};
        int width1[] = {130,100,68,85,85,85,85,55,110,55};
        queryData =  new QueryData(11, column1);
        for(int i=0;i<column1;i++){
            queryData->setColumnWidth(i,width1[i]);
            queryData->setHeaderName(i,name1[i]);
        }
        queryData->setLabel(label);
        queryData->setSqlString(table1,items1);
        queryData->UpdateModel();
        queryData->setSQLDatabase(userDB);
        queryData->initFirstPageQuery();
        tabwidget->addTab(queryData, tr("数据查询"));
    }

    {
        int column1 = 9;
        QString label = tr("标定数据查询");
        QString table1 = "Calibration";
        QString items1 = "A1,A2,A3,A4,A5,B1,B2,A6,A8,A7";
        QString name1[] = {
            tr("时间"),
            tr("浓度(mg/L)"),
            tr("吸光度"),
            tr("空白值C1"),
            tr("显色值C1"),
            tr("空白值C2"),
            tr("显色值C2"),
            tr("温度"),
            tr("操作类型"),
            tr("湿度(%)")
        };
        //   int width1[] = {120,100,70,65,65,65,68,120};
        int width1[] = {130,100,68,85,85,85,85,55,110,55};
        queryCalib =  new QueryData(11, column1);
        for(int i=0;i<column1;i++){
            queryCalib->setColumnWidth(i,width1[i]);
            queryCalib->setHeaderName(i,name1[i]);
        }
        queryCalib->setLabel(label);
        queryCalib->setSqlString(table1,items1);
        queryCalib->UpdateModel();
        queryCalib->setSQLDatabase(userDB);
        queryCalib->initFirstPageQuery();
        tabwidget->addTab(queryCalib, tr("标定数据"));
    }

    {
        int column1 = 3;
        QString label = tr("报警记录查询");
        QString table1 = "Error";
        QString items1 = "A1,A2,A3";
        QString name1[] = {
            tr("时间"),
            tr("级别"),
            tr("信息")
        };
        int width1[] = {120,100,550};
        queryError =  new QueryData(11, column1);
        for(int i=0;i<column1;i++){
            queryError->setColumnWidth(i,width1[i]);
            queryError->setHeaderName(i,name1[i]);
        }
        queryError->setLabel(label);
        queryError->setSqlString(table1,items1);
        queryError->UpdateModel();
        queryError->setSQLDatabase(userDB);
        queryError->initFirstPageQuery();
        tabwidget->addTab(queryError, tr("报警记录"));
    }

    {
        int column1 = 3;
        QString label = tr("日志记录查询");
        QString table1 = "Log";
        QString items1 = "A1,A2,A3";
        QString name1[] = {
            tr("时间"),
            tr("类别"),
            tr("信息")
        };
        int width1[] = {120,100,550};
        queryLog =  new QueryData(11, column1);
        for(int i=0;i<column1;i++){
            queryLog->setColumnWidth(i,width1[i]);
            queryLog->setHeaderName(i,name1[i]);
        }
        queryLog->setLabel(label);
        queryLog->setSqlString(table1,items1);
        queryLog->UpdateModel();
        queryLog->setSQLDatabase(userDB);
        queryLog->initFirstPageQuery();
        tabwidget->addTab(queryLog, tr("日志记录"));
    }


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

    switch (p)
    {
    case 2: loadSettings();break;
    case 3: break;
    default:
        break;
    }
}

void QFMain::updateStatus()
{
    QString s = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->datetime->setText(s);

    Receiver re = element->getReceiver();
    ui->lightVoltage->setText(QString("%1").arg(re.lightVoltage1()));
}

void QFMain::login(int level)
{
}

void QFMain::loadSettings()
{
    DatabaseProfile profile;
    if (profile.beginSection("settings"))
    {
        setui->Loop0->setValue(profile.value("Loop0", 1).toInt());
        setui->Loop1->setValue(profile.value("Loop1", 1).toInt());
        setui->Loop2->setValue(profile.value("Loop2", 1).toInt());
        setui->Loop3->setValue(profile.value("Loop3", 1).toInt());
        setui->Time0->setValue(profile.value("Time0", 3).toInt());
        setui->Time1->setValue(profile.value("Time1", 3).toInt());
        setui->Time2->setValue(profile.value("Time2", 3).toInt());
        setui->Time3->setValue(profile.value("Time3", 3).toInt());
        setui->Time4->setValue(profile.value("Time4", 3).toInt());
        setui->Temp0->setValue(profile.value("Temp0", 0).toInt());
        setui->Temp1->setValue(profile.value("Temp1", 0).toInt());

        setui->AlarmLine->setValue(profile.value("AlarmLine", 999).toDouble());
        setui->RangeSwitch->setCurrentIndex(profile.value("RangeSwitch", 0).toInt());
        setui->MeasureType->setCurrentIndex(profile.value("MeasureType", 0).toInt());
        setui->RotateSpeed->setValue(profile.value("RotateSpeed", 50).toInt());

        setui->UserK->setValue(profile.value("UserK", 1).toDouble());
        setui->UserB->setValue(profile.value("UserB", 0).toDouble());
        setui->TurbidityOffset->setValue(profile.value("TurbidityOffset", 0).toDouble());
        setui->TempOffset->setValue(profile.value("TempOffset", 0).toInt());
        setui->DeviceTempOffset->setValue(profile.value("DeviceTempOffset", 0).toInt());
        setui->BlankErrorThreshold->setValue(profile.value("BlankErrorThreshold", 0).toInt());

        setui->SmoothOffset->setValue(profile.value("SmoothOffset", 0).toInt());
    }
}

void QFMain::saveSettings()
{
    DatabaseProfile profile;
    if (profile.beginSection("settings"))
    {
        profile.setValue("Loop0",setui->Loop0->value());
        profile.setValue("Loop1",setui->Loop1->value());
        profile.setValue("Loop2",setui->Loop2->value());
        profile.setValue("Loop3",setui->Loop3->value());
        profile.setValue("Time0",setui->Time0->value());
        profile.setValue("Time1",setui->Time1->value());
        profile.setValue("Time2",setui->Time2->value());
        profile.setValue("Time3",setui->Time3->value());
        profile.setValue("Time4",setui->Time4->value());
        profile.setValue("Temp0",setui->Temp0->value());
        profile.setValue("Temp1",setui->Temp1->value());

        profile.setValue("AlarmLine",setui->AlarmLine->value());
        profile.setValue("RangeSwitch",setui->RangeSwitch->currentIndex());
        profile.setValue("MeasureType",setui->MeasureType->currentIndex());
        profile.setValue("RotateSpeed",setui->RotateSpeed->value());

        profile.setValue("UserK",setui->UserK->value());
        profile.setValue("UserB",setui->UserB->value());
        profile.setValue("TurbidityOffset",setui->TurbidityOffset->value());
        profile.setValue("TempOffset",setui->TempOffset->value());
        profile.setValue("DeviceTempOffset",setui->DeviceTempOffset->value());
        profile.setValue("BlankErrorThreshold",setui->BlankErrorThreshold->value());

        profile.setValue("SmoothOffset",setui->SmoothOffset->value());
    }
}
