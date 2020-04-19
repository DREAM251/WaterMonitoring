#include "qfmain.h"
#include "systemwindow.h"
#include "profile.h"
#include "ui_qfmain.h"
#include "ui_setui.h"
#include "ui_maintaince.h"
#include "ui_measuremode.h"
#include "ui_lightvoltage.h"
#include <QDebug>
#include <QMessageBox>
#include <QToolButton>

QFMain::QFMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFMain),
    setui(new Ui::SetUI),
    maintaince(new Ui::Maintaince),
    measuremode(new Ui::MeasureMode),
    lightVoltage(new Ui::LightVoltage),
    signalMapper(new QSignalMapper(this)),
    timer(new QTimer(this)),
    element(new ElementInterface(ET_NH3N, this))
{
    ui->setupUi(this);

    initCalibration();
    initMaintaince();
    initSettings();
    initQuery();

    nameMeasureMethod <<  tr("单次测量") <<  tr("受控测量") <<  tr("周期测量") << tr("连续测量");
    nameRange <<  tr("0-10mg/L") <<  tr("0-50mg/L") <<  tr("0-200mg/L");
    nameSamplePipe <<  tr("水样") <<  tr("标样") <<  tr("零样");
    nameOnlineOffline <<  tr("在线测量") <<  tr("离线测量");

    DatabaseProfile profile;
    if (profile.beginSection("measure")) {
        int x1 = profile.value("MeasureMethod", 0).toInt();
        if (x1 < 0 || x1 >= nameMeasureMethod.count())
            x1 = 0;
        int x2 = profile.value("Range", 0).toInt();
        if (x2 < 0 || x2 >= nameRange.count())
            x2 = 0;
        int x3 = profile.value("SamplePipe", 0).toInt();
        if (x3 < 0 || x3 >= nameSamplePipe.count())
            x3 = 0;
        int x4 = profile.value("OnlineOffline", 0).toInt();
        if (x4 < 0 || x4 >= nameOnlineOffline.count())
            x4 = 0;

        ui->MeasureMethod->setProperty("value", x1);
        ui->MeasureMethod->setText(nameMeasureMethod[x1]);

        ui->Range->setProperty("value", x2);
        ui->Range->setText(nameRange[x2]);

        ui->SamplePipe->setProperty("value", x3);
        ui->SamplePipe->setText(nameSamplePipe[x3]);

        ui->OnlineOffline->setProperty("value", x4);
        ui->OnlineOffline->setText(nameOnlineOffline[x4]);
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

    connect(ui->OnlineOffline, SIGNAL(clicked()), this, SLOT(OnlineOffline()));
    connect(ui->MeasureMethod, SIGNAL(clicked()), this, SLOT(MeasureMethod()));
    connect(ui->Range, SIGNAL(clicked()), this, SLOT(Range()));
    connect(ui->SamplePipe, SIGNAL(clicked()), this, SLOT(SamplePipe()));
}

QFMain::~QFMain()
{
    delete ui;
}

// set ui
void QFMain::initSettings()
{
    QWidget *w = new QWidget();
    setui->setupUi(w);
    setui->tabWidget->addTab(new SystemWindow, tr("系统设置"));

    QFrame *f = new QFrame;
    measuremode->setupUi(f);
    setui->tabWidget->insertTab(0, f, tr("计划任务"));

    setui->tabWidget->setCurrentIndex(0);
    ui->contentStackedWidget->addWidget(w);
    ui->contentStackedWidget->setCurrentIndex(0);
    connect(setui->Cancel, SIGNAL(clicked()), this, SLOT(loadSettings()));
    connect(setui->Save, SIGNAL(clicked()), this, SLOT(saveSettings()));

    loadSettings();
}

void QFMain::initCalibration()
{
    // calibraiton
    QTabWidget *tabwidget = new QTabWidget();
    usercalib = new CalibFrameUser;
    usercalib->addPipeName(tr("零样"));
    usercalib->addPipeName(tr("标样"));
    usercalib->setRange(0, "0-10mg/L");
    usercalib->setRange(1, "0-50mg/L");
    usercalib->setRange(2, "0-200mg/L");
    usercalib->setSampleLow(0, 10);
    usercalib->setSampleHigh(5, 5);
    usercalib->loadParams();
    usercalib->renewUI();
    factorycalib = new CalibFrameFactory;
    factorycalib->addPipeName(tr("零样"));
    factorycalib->addPipeName(tr("标样"));
    factorycalib->setRange(0, "0-10mg/L");
    factorycalib->setRange(1, "0-50mg/L");
    factorycalib->setRange(2, "0-200mg/L");
    factorycalib->setSampleLow(0, 10);
    factorycalib->setSampleHigh(5, 5);
    factorycalib->loadParams();
    factorycalib->renewUI();
    tabwidget->addTab(usercalib, tr("用户标定"));
    tabwidget->addTab(factorycalib, tr("出厂标定"));
    ui->contentStackedWidget->addWidget(tabwidget);
}


// maintaince
void QFMain::initMaintaince()
{
    QWidget *w = new QWidget;
    maintaince->setupUi(w);

    modbusframe = new ModbusModule();
    maintaince->tabWidget->addTab(modbusframe, tr("数字通信"));

    QWidget *w1 = new QWidget;
    lightVoltage->setupUi(w1);
    maintaince->tabWidget->addTab(w1, tr("光源调节"));

    struct ColumnInfo aa[] = {
    {QObject::tr("编号"),4,"1000"},
    {QObject::tr("执行时间"),4,"0005"},
    {QObject::tr("蠕动泵"),1,"0"},
    {QObject::tr("蠕动泵转速"),2,"20"},
    {QObject::tr("泵2"),1,"0"},
    {QObject::tr("十通阀1"),1,"0"},
    {QObject::tr("十通阀2"),1,"0"},
    {QObject::tr("阀1"),1,"0"},
    {QObject::tr("阀2"),1,"0"},
    {QObject::tr("阀3"),1,"0"},
    {QObject::tr("阀4"),1,"0"},
    {QObject::tr("阀5"),1,"0"},
    {QObject::tr("阀6"),1,"0"},
    {QObject::tr("阀7"),1,"0"},
    {QObject::tr("阀8"),1,"0"},
    {QObject::tr("水泵"),1,"0"},
    {QObject::tr("模拟量"),4,"0000"},
    {QObject::tr("外控1"),1,"0"},
    {QObject::tr("外控2"),1,"0"},
    {QObject::tr("外控3"),1,"0"},
    {QObject::tr("风扇"),1,"0"},
    {QObject::tr("液位"),1,"0"},
    {QObject::tr("加热温度"),4,"0000"},
    {QObject::tr("保留"),4,"0000"},
    {QObject::tr("分隔符"),1,":"},
    {QObject::tr("时间关联"),2,"00", ColumnInfo::CDT_Combox,
                QObject::tr("无,时间1,时间2")},
    {QObject::tr("额外时间"),4,"0000"},
    {QObject::tr("温度关联"),2,"00", ColumnInfo::CDT_Combox,
                QObject::tr("无,温度1,温度2")},
    {QObject::tr("循环"),2,"00", ColumnInfo::CDT_Combox,
                QObject::tr("无,循环1开始,循环1结束,循环2开始,循环2结束,循环3开始,循环3结束,循环4开始,循环4结束")},
    {QObject::tr("流程判定"),1,"0", ColumnInfo::CDT_Combox,
                QObject::tr("无,液位到达判定,液位检测,温度到达判断,温度检测,降温到达判定,降温检测")},
    {QObject::tr("信号采集"),1,"0", ColumnInfo::CDT_Combox,
                QObject::tr("无,空白值采集,显示值采集")},
    {QObject::tr("注释代码"),2,"00", ColumnInfo::CDT_Combox,
                QObject::tr("无,进样,排空")}};
    QList<ColumnInfo> ci ;
    int lines = sizeof(aa)/sizeof(struct ColumnInfo);
    for(int i=0;i<lines;i++){
        ci << aa[i];
    }
    CommondFileInfo bb[] = {{tr("测量"), "measure.txt"},
                            {tr("停止"), "stop.txt"},
                            {tr("异常处理"), "error.txt"},
                            {tr("调试"), "test.txt"},
                            {tr("清洗"), "wash.txt"},
                            {tr("排空"), "drain.txt"},
                            {tr("初始化"), "poweron.txt"}};
    QList<CommondFileInfo> cfi;
    lines = sizeof(bb)/sizeof(struct CommondFileInfo);
    for(int i=0;i<lines;i++){
        cfi << bb[i];
    }
    editor = new InstructionEditor(ci, cfi);
    maintaince->tabWidget->addTab(editor, tr("命令编辑"));

    maintaince->tabWidget->setCurrentIndex(0);
    ui->contentStackedWidget->addWidget(w);

    connect(maintaince->SampleMeasure, SIGNAL(clicked()), this, SLOT(SampleMeasure()));
    connect(maintaince->ZeroMeasure, SIGNAL(clicked()), this, SLOT(ZeroMeasure()));
    connect(maintaince->StandardMeasure, SIGNAL(clicked()), this, SLOT(StandardMeasure()));
    connect(maintaince->QCMeasure, SIGNAL(clicked()), this, SLOT(QCMeasure()));

    connect(maintaince->Drain, SIGNAL(clicked()), this, SLOT(Drain()));
    connect(maintaince->Stop, SIGNAL(clicked()), this, SLOT(Stop()));
    connect(maintaince->Clean, SIGNAL(clicked()), this, SLOT(Clean()));
    connect(maintaince->OneStepExec, SIGNAL(clicked()), this, SLOT(OneStepExec()));
    connect(maintaince->FuncExec, SIGNAL(clicked()), this, SLOT(FuncExec()));
}

// query ui
void QFMain::initQuery()
{
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
        queryData =  new QueryData(column1);
        for(int i=0;i<column1;i++){
            queryData->setColumnWidth(i,width1[i]);
            queryData->setHeaderName(i,name1[i]);
        }
        queryData->setLabel(label);
        queryData->setSqlString(table1,items1);
        queryData->UpdateModel();
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
        queryCalib =  new QueryData(column1);
        for(int i=0;i<column1;i++){
            queryCalib->setColumnWidth(i,width1[i]);
            queryCalib->setHeaderName(i,name1[i]);
        }
        queryCalib->setLabel(label);
        queryCalib->setSqlString(table1,items1);
        queryCalib->UpdateModel();
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
        queryError =  new QueryData(column1);
        for(int i=0;i<column1;i++){
            queryError->setColumnWidth(i,width1[i]);
            queryError->setHeaderName(i,name1[i]);
        }
        queryError->setLabel(label);
        queryError->setSqlString(table1,items1);
        queryError->UpdateModel();
        queryError->initFirstPageQuery();
        tabwidget->addTab(queryError, tr("报警记录"));
    }

    {
        int column1 = 2;
        QString label = tr("日志记录查询");
        QString table1 = "Log";
        QString items1 = "A1,A2";
        QString name1[] = {
            tr("时间"),
            tr("信息")
        };
        int width1[] = {120,550};
        queryLog =  new QueryData(column1);
        for(int i=0;i<column1;i++){
            queryLog->setColumnWidth(i,width1[i]);
            queryLog->setHeaderName(i,name1[i]);
        }
        queryLog->setLabel(label);
        queryLog->setSqlString(table1,items1);
        queryLog->UpdateModel();
        queryLog->initFirstPageQuery();
        tabwidget->addTab(queryLog, tr("日志记录"));
    }
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
    case 0: break;
    case 1: break;
    case 2: usercalib->loadParams();
        usercalib->renewUI();
        factorycalib->loadParams();
        factorycalib->renewUI();
        break;
    case 3: break;
    case 4: loadSettings();break;
    case 5: break;
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

//用户登陆权限管理
void QFMain::login(int level)
{
    qDebug() << "login:" << level;
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

    if (profile.beginSection("measuremode"))
    {
        const int c = 5;
        QCheckBox *enables[c] = {measuremode->AdjustEnable1,measuremode->AdjustEnable2,measuremode->AdjustEnable3,measuremode->AdjustEnable4,measuremode->AdjustEnable5};
        QComboBox *tasks[c] = {measuremode->AdjustTask1,measuremode->AdjustTask2,measuremode->AdjustTask3,measuremode->AdjustTask4,measuremode->AdjustTask5};
        QSpinBox *periods[c] = {measuremode->AdjustPeriod1,measuremode->AdjustPeriod2,measuremode->AdjustPeriod3,measuremode->AdjustPeriod4,measuremode->AdjustPeriod5};
        QTimeEdit *startTimes[c] = {measuremode->AdjustStartTime1,measuremode->AdjustStartTime2,measuremode->AdjustStartTime3,measuremode->AdjustStartTime4,measuremode->AdjustStartTime5};

        for (int i = 0; i < c; i++)
        {
            enables[i]->setChecked(profile.value(QString("enable%1").arg(i), false).toBool());
            tasks[i]->setCurrentIndex(profile.value(QString("task%1").arg(i), 0).toBool());
            periods[i]->setValue(profile.value(QString("period%1").arg(i), 1).toBool());
            startTimes[i]->setTime(profile.value(QString("startTime%1").arg(i), QTime::currentTime()).toTime());
        }

        measuremode->MeasurePeriod->setValue(profile.value("MeasurePeriod", 60).toInt());
        measuremode->MeasureStartTime->setTime(profile.value("MeasureStartTime", QTime::currentTime()).toTime());
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

    if (profile.beginSection("measuremode"))
    {
        const int c = 5;
        QCheckBox *enables[c] = {measuremode->AdjustEnable1,measuremode->AdjustEnable2,measuremode->AdjustEnable3,measuremode->AdjustEnable4,measuremode->AdjustEnable5};
        QComboBox *tasks[c] = {measuremode->AdjustTask1,measuremode->AdjustTask2,measuremode->AdjustTask3,measuremode->AdjustTask4,measuremode->AdjustTask5};
        QSpinBox *periods[c] = {measuremode->AdjustPeriod1,measuremode->AdjustPeriod2,measuremode->AdjustPeriod3,measuremode->AdjustPeriod4,measuremode->AdjustPeriod5};
        QTimeEdit *startTimes[c] = {measuremode->AdjustStartTime1,measuremode->AdjustStartTime2,measuremode->AdjustStartTime3,measuremode->AdjustStartTime4,measuremode->AdjustStartTime5};

        for (int i = 0; i < c; i++)
        {
            profile.setValue(QString("enable%1").arg(i), enables[i]->isChecked());
            profile.setValue(QString("task%1").arg(i), tasks[i]->currentIndex());
            profile.setValue(QString("period%1").arg(i), periods[i]->value());
            profile.setValue(QString("startTime%1").arg(i), startTimes[i]->time());
        }

        profile.setValue("MeasurePeriod", measuremode->MeasurePeriod->value());
        profile.setValue("MeasureStartTime", measuremode->MeasureStartTime->time());
    }
    QMessageBox::information(NULL, tr("提示"), tr("保存成功"));
}

void QFMain::OnlineOffline()
{
    QPushButton *button = ui->OnlineOffline;

    int value = button->property("value").toInt();
    if (++value >= nameOnlineOffline.count())
        value = 0;
    button->setProperty("value", value);
    button->setText(nameOnlineOffline[value]);

    DatabaseProfile profile;
    if (profile.beginSection("measure"))
        profile.setValue("OnlineOffline", value);
}

void QFMain::MeasureMethod()
{
    QPushButton *button = ui->MeasureMethod;

    int value = button->property("value").toInt();
    if (++value >= nameMeasureMethod.count())
        value = 0;
    button->setProperty("value", value);
    button->setText(nameMeasureMethod[value]);

    DatabaseProfile profile;
    if (profile.beginSection("measure"))
        profile.setValue("MeasureMethod", value);
}

void QFMain::Range()
{
    QPushButton *button = ui->Range;
    int value = button->property("value").toInt();
    if (++value >= nameRange.count())
        value = 0;
    button->setProperty("value", value);
    button->setText(nameRange[value]);

    DatabaseProfile profile;
    if (profile.beginSection("measure"))
        profile.setValue("Range", value);
}

void QFMain::SamplePipe()
{
    QPushButton *button = ui->SamplePipe;
    int value = button->property("value").toInt();
    if (++value >= nameSamplePipe.count())
        value = 0;
    button->setProperty("value", value);
    button->setText(nameSamplePipe[value]);

    DatabaseProfile profile;
    if (profile.beginSection("measure"))
        profile.setValue("SamplePipe", value);
}

void QFMain::SampleMeasure()
{
    int ret = element->startTask(TT_Measure);

    if (ret != 0)
        QMessageBox::warning(this, tr("警告"), tr("%1，执行失败").arg(element->translateStartCode(ret)));
}

void QFMain::ZeroMeasure()
{
    int ret = element->startTask(TT_ZeroCheck);

    if (ret != 0)
        QMessageBox::warning(this, tr("警告"), tr("%1，执行失败").arg(element->translateStartCode(ret)));
}

void QFMain::StandardMeasure()
{
    int ret = element->startTask(TT_SampleCheck);

    if (ret != 0)
        QMessageBox::warning(this, tr("警告"), tr("%1，执行失败").arg(element->translateStartCode(ret)));
}

void QFMain::QCMeasure()
{
    int ret = element->startTask(TT_SpikedCheck);

    if (ret != 0)
        QMessageBox::warning(this, tr("警告"), tr("%1，执行失败").arg(element->translateStartCode(ret)));
}

void QFMain::Drain()
{
    int ret = element->startTask(TT_Drain);

    if (ret != 0)
        QMessageBox::warning(this, tr("警告"), tr("%1，执行失败").arg(element->translateStartCode(ret)));
}

void QFMain::Stop()
{
    if (element->getCurrentTask() <= TT_ErrorProc &&
            QMessageBox::question(this, tr("提示"), tr("当前正在执行其他任务，是否确定停止？"),
                                     QMessageBox::Yes|QMessageBox::No)
                    == QMessageBox::No)
        return;

    element->stopTasks();
}

void QFMain::Clean()
{
    int ret = element->startTask(TT_Clean);

    if (ret != 0)
        QMessageBox::warning(this, tr("警告"), tr("%1，执行失败").arg(element->translateStartCode(ret)));
}

void QFMain::OneStepExec()
{
    DatabaseProfile profile;
    if (profile.beginSection("pumpTest"))
    {
        QRadioButton *rb1[] = {maintaince->EC1Close, maintaince->TV1_1, maintaince->TV1_2, maintaince->TV1_3, maintaince->TV1_4,
                               maintaince->TV1_5, maintaince->TV1_6, maintaince->TV1_7, maintaince->TV1_8, maintaince->TV1_9, maintaince->TV1_10};
        QRadioButton *rb2[] = {maintaince->EC2Close, maintaince->TV2_1, maintaince->TV2_2, maintaince->TV2_3, maintaince->TV2_4,
                               maintaince->TV2_5, maintaince->TV2_6, maintaince->TV2_7, maintaince->TV2_8, maintaince->TV2_9, maintaince->TV2_10};
        QCheckBox *cb[] = {maintaince->valve1, maintaince->valve2, maintaince->valve3, maintaince->valve4, maintaince->valve5, maintaince->valve6,
                           maintaince->valve7, maintaince->valve8, maintaince->valve9, maintaince->valve10, maintaince->valve11, maintaince->valve12};

        int tv1 = 0, tv2 = 0;
        for (int i = 0; i < 11; i++) {
            if (rb1[i]->isChecked())
                tv1 = i;
            if (rb2[i]->isChecked())
                tv2 = i;
        }

        profile.setValue("TV1", tv1);
        profile.setValue("TV2", tv2);

        for (int i = 0; i < 12; i++)
            profile.setValue(QString("valve%1").arg(i), cb[i]->isChecked()?1:0);
        profile.setValue("WorkTime", maintaince->WorkTime->value());
        profile.setValue("Temp", maintaince->Temp->value());
        profile.setValue("Speed", maintaince->Speed->value());
        profile.setValue("PumpRotate", maintaince->PumpRotate->currentIndex());
    }

    int ret = element->startTask(TT_Debug);

    if (ret != 0)
        QMessageBox::warning(this, tr("警告"), tr("%1，执行失败").arg(element->translateStartCode(ret)));
}

void QFMain::FuncExec()
{
    int ret = element->startTask(TT_Func);

    if (ret != 0)
        QMessageBox::warning(this, tr("警告"), tr("%1，执行失败").arg(element->translateStartCode(ret)));
}
