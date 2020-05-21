#include "qfmain.h"
#include "systemwindow.h"
#include "profile.h"
#include "ui_qfmain.h"
#include "ui_setui.h"
#include "ui_maintaince.h"
#include "ui_measuremode.h"
#include "ui_lightvoltage.h"
#include "common.h"
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

    nameMeasureMethod <<  tr("周期模式") <<  tr("定点模式") <<  tr("维护模式") ;
    nameRange <<  tr("0-10mg/L") <<  tr("0-50mg/L") <<  tr("0-200mg/L");
    nameSamplePipe <<  tr("水样") <<  tr("标样") <<  tr("零样");
    nameOnlineOffline <<  tr("在线测量") <<  tr("离线测量");

    DatabaseProfile profile;
    if (profile.beginSection("measuremode")) {
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
    connect(ui->Stop, SIGNAL(clicked()), this, SLOT(Stop()));
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
    connect(lightVoltage->Save, SIGNAL(clicked()), this, SLOT(SaveLigthVoltage()));    
    connect(lightVoltage->Renew, SIGNAL(clicked()), this, SLOT(Stop()));
    maintaince->tabWidget->addTab(w1, tr("光源调节"));

    struct ColumnInfo aa[] = {
    {QObject::tr("编号"),4,"1000"},
    {QObject::tr("执行时间"),4,"0005"},
    {QObject::tr("蠕动泵"),1,"0"},
    {QObject::tr("蠕动泵转速"),2,"20"},
    {QObject::tr("泵2"),1,"0"},
    {QObject::tr("十通阀1"),1,"0",ColumnInfo::CDT_Combox,
                QObject::tr("关,阀1,阀2,阀3,阀4,阀5,阀6,阀7,阀8,阀9,阀10-A")},
    {QObject::tr("十通阀2"),1,"0", ColumnInfo::CDT_Combox,
                QObject::tr("关,阀1,阀2,阀3,阀4,阀5,阀6,阀7,阀8,阀9,阀10-A")},
    {QObject::tr("阀1"),1,"0"},
    {QObject::tr("阀2"),1,"0"},
    {QObject::tr("阀3"),1,"0"},
    {QObject::tr("阀4"),1,"0"},
    {QObject::tr("阀5"),1,"0"},
    {QObject::tr("阀6"),1,"0"},
    {QObject::tr("阀7"),1,"0"},
    {QObject::tr("阀8"),1,"0"},
    {QObject::tr("水泵"),1,"0"},
    {QObject::tr("外控1"),1,"0"},
    {QObject::tr("外控2"),1,"0"},
    {QObject::tr("外控3"),1,"0"},
    {QObject::tr("模拟量"),4,"0000"},
    {QObject::tr("风扇"),1,"0"},
    {QObject::tr("设置液位"),1,"0"},
    {QObject::tr("加热温度"),4,"0000"},
    {QObject::tr("液位LED控制"),1,"0"},
    {QObject::tr("测量LED控制"),1,"0"},
    {QObject::tr("保留"),4,"0000"},
    {QObject::tr("分隔符"),1,":"},
    {QObject::tr("时间关联"),2,"00", ColumnInfo::CDT_Combox,
                QObject::tr("无,采样时长,沉沙时长,水样润洗时长,水样排空时长,加热时长,"
                            "比色池排空时长,预抽时长1,预抽时长2,预抽时长3")},
    {QObject::tr("额外时间"),4,"0000"},
    {QObject::tr("温度关联"),2,"00", ColumnInfo::CDT_Combox,
                QObject::tr("无,加热温度,降温温度")},
    {QObject::tr("循环"),2,"00", ColumnInfo::CDT_Combox,
                QObject::tr("无,流路清洗开始,流路清洗结束,水样润洗开始,水样润洗结束,"
                            "进水样开始,进水样结束,进清水开始,进清水结束")},
    {QObject::tr("流程判定"),1,"0", ColumnInfo::CDT_Combox,
                QObject::tr("无,液位1到达判定,液位2到达判定,液位3到达判定,加热判断,降温判定")},
    {QObject::tr("信号采集"),1,"0", ColumnInfo::CDT_Combox,
                QObject::tr("无,空白值采集,显示值采集")},
    {QObject::tr("注释代码"),2,"00", ColumnInfo::CDT_Combox,
                QObject::tr("无,降温,排空比色池,排空计量管,开采样,水样润洗,进***,消解,空白检测,比色检测,流路清洗,显色,静置,鼓泡,试剂替换,空闲")}};
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
                            {tr("初始化"), "poweron.txt"},
                            {tr("试剂替换"), "initialize.txt"}};
    QList<CommondFileInfo> cfi;
    lines = sizeof(bb)/sizeof(struct CommondFileInfo);
    for(int i=0;i<lines;i++){
        cfi << bb[i];
    }
    editor = new InstructionEditor(ci, cfi);
    maintaince->tabWidget->addTab(editor, tr("命令编辑"));

    maintaince->tabWidget->setCurrentIndex(0);
    ui->contentStackedWidget->addWidget(w);
    maintaince->valve11->hide();
    maintaince->valve12->hide();
    connect(maintaince->SampleMeasure, SIGNAL(clicked()), this, SLOT(SampleMeasure()));
    connect(maintaince->ZeroMeasure, SIGNAL(clicked()), this, SLOT(ZeroMeasure()));
    connect(maintaince->StandardMeasure, SIGNAL(clicked()), this, SLOT(StandardMeasure()));
    connect(maintaince->QCMeasure, SIGNAL(clicked()), this, SLOT(QCMeasure()));

    connect(maintaince->Drain, SIGNAL(clicked()), this, SLOT(Drain()));
    connect(maintaince->Stop, SIGNAL(clicked()), this, SLOT(Stop()));
    connect(maintaince->Clean, SIGNAL(clicked()), this, SLOT(Clean()));
    connect(maintaince->OneStepExec, SIGNAL(clicked()), this, SLOT(OneStepExec()));
    connect(maintaince->FuncExec, SIGNAL(clicked()), this, SLOT(FuncExec()));
    connect(maintaince->InitLoad, SIGNAL(clicked()), this, SLOT(InitLoad()));
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
        QString items1 = "A1,A2,A3,A4,A5,A6,A7,A8,A9,B1";
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
        QString items1 = "A1,A2,A3,A4,A5,A6,A7,A8,A9,B1";
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
        int column1 = 13;
        QString label = tr("核查数据");
        QString table1 = "QC";
        QString items1 = "A1,A2,A3,A4,A5,A6,A7,A8,A9,B1,B2,B3,B4";
        QString name1[] = {
            tr("时间"),
            tr("类别"),
            tr("浓度(mg/L)"),
            tr("吸光度"),
            tr("参比1"),
            tr("吸收1"),
            tr("参比2"),
            tr("吸收2"),
            tr("温度"),
            tr("数据标识"),
            tr("指标1"),
            tr("指标2"),
            tr("指标3")};
        //   int width1[] = {120,100,70,65,65,65,68,120};
        int width1[] = {130,100,68,85,85,85,85,55,110,55,85,55,110,55};
        queryQC =  new QueryData(column1);
        for(int i=0;i<column1;i++){
            queryQC->setColumnWidth(i,width1[i]);
            queryQC->setHeaderName(i,name1[i]);
        }
        queryQC->setLabel(label);
        queryQC->setSqlString(table1,items1);
        queryQC->UpdateModel();
        queryQC->initFirstPageQuery();
        tabwidget->addTab(queryQC, tr("核查数据"));
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
        int column1 = 3;
        QString label = tr("日志记录查询");
        QString table1 = "Log";
        QString items1 = "A1,A2,A3";
        QString name1[] = {
            tr("时间"),
            tr("类别"),
            tr("信息")
        };
        int width1[] = {120,120,550};
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
    case 3: loadMaintaince();break;
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

    switch (element->getTaskType())
    {
    case TT_Idle:ui->status->setText(tr("空闲"));break;
    case TT_Measure: ui->status->setText(tr("水样测试"));break;
    case TT_ZeroCalibration: ui->status->setText(tr("零点校准"));break;
    case TT_SampleCalibration: ui->status->setText(tr("标样校准"));break;
    case TT_ZeroCheck: ui->status->setText(tr("零点测试"));break;
    case TT_SampleCheck: ui->status->setText(tr("标样测试"));break;
    case TT_SpikedCheck: ui->status->setText(tr("质控样测试"));break;
    case TT_ErrorProc: ui->status->setText(tr("异常处理"));break;
    case TT_Stop: ui->status->setText(tr("停止"));break;
    case TT_Clean: ui->status->setText(tr("清洗"));break;
    case TT_Drain: ui->status->setText(tr("排空"));break;
    case TT_Initial: ui->status->setText(tr("初始排空"));break;
    case TT_Debug: ui->status->setText(tr("调试"));break;
    case TT_Initload: ui->status->setText(tr("试剂替换"));break;
    case TT_Func: ui->status->setText(tr("抽取试剂"));break;
    case TT_Config: ui->status->setText(tr("参数设置"));break;
    default:
        break;
    }

    ITask *task = element->getTask();
    if (task)
    {
        ui->progressBar->setRange(0, task->getLastProcessTime());
        ui->progressBar->setValue(task->getProcess());
    } else {
        ui->progressBar->setValue(0);
    }

    Receiver re = element->getReceiver();
    if (!re.data().isEmpty())
    {
//        ui->RealTimeResult->setText(QString("%1").arg(re.lightVoltage1()));
        ui->waterVoltage->setText(QString("%1").arg(re.lightVoltage1()));
        ui->waterVoltage1->setText(QString("%1").arg(re.lightVoltage2()));
        ui->waterVoltage2->setText(QString("%1").arg(re.lightVoltage3()));
        ui->waterLevel->setText(QString("%1").arg(re.pumpStatus()));
        ui->RefLightVoltage->setText(QString("%1").arg(re.refLightSignal()));
        ui->measureVoltage->setText(QString("%1").arg(re.measureSignal()));
        ui->setTemp->setText(QString("%1").arg(re.heatTemp()) + tr("℃"));
        ui->deviceTemp->setText(QString("%1").arg(re.mcu1Temp()) + tr("℃"));

        lightVoltage->WaterLevel->setText(QString("%1").arg(re.pumpStatus()));
        lightVoltage->WaterLevel1->setText(QString("%1").arg(re.lightVoltage1()));
        lightVoltage->WaterLevel2->setText(QString("%1").arg(re.lightVoltage2()));
        lightVoltage->WaterLevel3->setText(QString("%1").arg(re.lightVoltage3()));
        lightVoltage->RefWaterLevel1->setText(QString("%1").arg(re.refLightSignal()));
        lightVoltage->RefWaterLevel2->setText(QString("%1").arg(re.measureSignal()));

        ui->Recv->setText(re.data());
        ui->warning->setText(getLastErrorMsg());
    }

    const QString style1 = "image: url(:/LedGreen.ico);";
    const QString style2 = "image: url(:/LedRed.ico);";
    Sender sd = element->getSender();
    if (!sd.data().isEmpty())
    {
        ui->pump1->setText(QString("%1").arg(sd.peristalticPump()));
        ui->pump2->setText(QString("%1").arg(sd.pump2()));
        ui->TV1->setText(QString("%1").arg(sd.TCValve1()));
        ui->TV2->setText(QString("%1").arg(sd.TCValve2()));
        ui->led1->setStyleSheet(sd.valve1()?style1:style2);
        ui->led2->setStyleSheet(sd.valve2()?style1:style2);
        ui->led3->setStyleSheet(sd.valve3()?style1:style2);
        ui->led4->setStyleSheet(sd.valve4()?style1:style2);
        ui->led5->setStyleSheet(sd.valve5()?style1:style2);
        ui->led6->setStyleSheet(sd.valve6()?style1:style2);
        ui->led7->setStyleSheet(sd.valve7()?style1:style2);
        ui->led8->setStyleSheet(sd.valve8()?style1:style2);

        QString explainString = sd.translateExplainCode();
        if (!explainString.isEmpty())
            ui->CurrentTask->setText(tr("当前流程：") + explainString);
    }

    DatabaseProfile profile;
    if (profile.beginSection("measure")){
        float v = profile.value("conc", 0).toFloat();
        ui->measureResult->setText(QString::number(v, 'f', 2) + "mg/L");
    }
    if (profile.beginSection("settings")) {
        ui->temp->setText(profile.value("Temp0").toString() + tr("℃"));
    }


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
        setui->Time5->setValue(profile.value("Time5", 3).toInt());

        setui->Time6->setValue(profile.value("Time6", 3).toInt());
        setui->Time7->setValue(profile.value("Time7", 3).toInt());
        setui->Time8->setValue(profile.value("Time8", 3).toInt());
        setui->Temp0->setValue(profile.value("Temp0", 0).toInt());
        setui->Temp1->setValue(profile.value("Temp1", 0).toInt());

        setui->AlarmLineL->setValue(profile.value("AlarmLineL", 999).toDouble());
        setui->AlarmLineH->setValue(profile.value("AlarmLineH", 999).toDouble());
        setui->RangeSwitch->setCurrentIndex(profile.value("RangeSwitch", 0).toInt());
        setui->_4mA->setValue(profile.value("_4mA", 0).toInt());
        setui->_20mA->setValue(profile.value("_20mA", 50).toInt());

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
        QPushButton *enables[] = {measuremode->point_1,measuremode->point_2,measuremode->point_3,measuremode->point_4,
                                   measuremode->point_5,measuremode->point_6,measuremode->point_7,measuremode->point_8,
                                   measuremode->point_9,measuremode->point_10,measuremode->point_11,measuremode->point_12,
                                   measuremode->point_13,measuremode->point_14,measuremode->point_15,measuremode->point_16,
                                   measuremode->point_17,measuremode->point_18,measuremode->point_19,measuremode->point_20,
                                   measuremode->point_21,measuremode->point_22,measuremode->point_23,measuremode->point_24};

        for (int i = 0; i < 24; i++)
            enables[i]->setChecked(profile.value(QString("Point%1").arg(i), false).toBool());

        measuremode->PointMin->setValue(profile.value("PointMin", 0).toInt());
        measuremode->MeasurePeriod->setValue(profile.value("MeasurePeriod", 60).toInt());
        measuremode->MeasureStartTime->setTime(profile.value("MeasureStartTime", QTime::currentTime()).toTime());
    }
}

void QFMain::saveSettings()
{
    DatabaseProfile profile;
    if (profile.beginSection("settings"))
    {
        profile.setValue("Loop0",setui->Loop0->value(),setui->label_18->text());
        profile.setValue("Loop1",setui->Loop1->value(),setui->label_19->text());
        profile.setValue("Loop2",setui->Loop2->value(),setui->label_20->text());
        profile.setValue("Loop3",setui->Loop3->value(),setui->label_21->text());
        profile.setValue("Time0",setui->Time0->value(),setui->label_82->text());

        profile.setValue("Time1",setui->Time1->value(),setui->label_22->text());
        profile.setValue("Time2",setui->Time2->value(),setui->label_123->text());
        profile.setValue("Time3",setui->Time3->value(),setui->label_17->text());
        profile.setValue("Time4",setui->Time4->value(),setui->label_161->text());
        profile.setValue("Time5",setui->Time5->value(),setui->label_26->text());

        profile.setValue("Time6",setui->Time6->value(),setui->label_23->text());
        profile.setValue("Time7",setui->Time7->value(),setui->label_24->text());
        profile.setValue("Time8",setui->Time8->value(),setui->label_25->text());
        profile.setValue("Temp0",setui->Temp0->value(),setui->label_157->text());
        profile.setValue("Temp1",setui->Temp1->value(),setui->label_159->text());

        profile.setValue("AlarmLineL",setui->AlarmLineL->value(),setui->label_13->text());
        profile.setValue("AlarmLineH",setui->AlarmLineH->value(),setui->label_12->text());
        profile.setValue("RangeSwitch",setui->RangeSwitch->currentIndex(),setui->label_3->text());
        profile.setValue("_4mA",setui->_4mA->value(),setui->label_42->text());
        profile.setValue("_20mA",setui->_20mA->value(),setui->label_43->text());

        profile.setValue("UserK",setui->UserK->value(),setui->label_8->text());
        profile.setValue("UserB",setui->UserB->value(),setui->label_7->text());
        profile.setValue("TurbidityOffset",setui->TurbidityOffset->value(),setui->label_6->text());
        profile.setValue("TempOffset",setui->TempOffset->value(),setui->label->text());
        profile.setValue("DeviceTempOffset",setui->DeviceTempOffset->value(),setui->label_4->text());
        profile.setValue("BlankErrorThreshold",setui->BlankErrorThreshold->value(),setui->label_5->text());

        profile.setValue("SmoothOffset",setui->SmoothOffset->value());
    }

    if (profile.beginSection("measuremode"))
    {
        QPushButton *enables[] = {measuremode->point_1,measuremode->point_2,measuremode->point_3,measuremode->point_4,
                                   measuremode->point_5,measuremode->point_6,measuremode->point_7,measuremode->point_8,
                                   measuremode->point_9,measuremode->point_10,measuremode->point_11,measuremode->point_12,
                                   measuremode->point_13,measuremode->point_14,measuremode->point_15,measuremode->point_16,
                                   measuremode->point_17,measuremode->point_18,measuremode->point_19,measuremode->point_20,
                                    measuremode->point_21,measuremode->point_22,measuremode->point_23,measuremode->point_24};

        for (int i = 0; i < 24; i++)
            profile.setValue(QString("Point%1").arg(i), enables[i]->isChecked());

        profile.setValue("PointMin", measuremode->PointMin->value());
        profile.setValue("MeasurePeriod", measuremode->MeasurePeriod->value());
        profile.setValue("MeasureStartTime", measuremode->MeasureStartTime->time());
    }
    QMessageBox::information(NULL, tr("提示"), tr("保存成功"));
}

void QFMain::loadMaintaince()
{
    DatabaseProfile profile;
    if (profile.beginSection("lightVoltage"))
    {
        lightVoltage->Color1Current->setValue(profile.value("Color1Current").toInt());
        lightVoltage->Color2Current->setValue(profile.value("Color2Current").toInt());
        lightVoltage->Color1Gain->setCurrentIndex(profile.value("Color1Gain").toInt());
        lightVoltage->Color2Gain->setCurrentIndex(profile.value("Color2Gain").toInt());
        lightVoltage->CurrentHigh->setValue(profile.value("CurrentHigh").toInt());
        lightVoltage->CurrentLow->setValue(profile.value("CurrentLow").toInt());
        lightVoltage->ThresholdHigh->setValue(profile.value("ThresholdHigh").toInt());
        lightVoltage->ThresholdMid->setValue(profile.value("ThresholdMid").toInt());
        lightVoltage->ThresholdLow->setValue(profile.value("ThresholdLow").toInt());
    }
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
    if (profile.beginSection("measuremode"))
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
    if (profile.beginSection("measuremode"))
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
    if (profile.beginSection("measuremode"))
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
    if (profile.beginSection("measuremode"))
        profile.setValue("SamplePipe", value);
}

void QFMain::SampleMeasure()
{
    int ret = element->startTask(TT_Measure);

    if (ret != 0)
        QMessageBox::warning(this, tr("警告"), tr("%1，执行失败").arg(element->translateStartCode(ret)));
    else
        addLogger(tr("水样测试"), LoggerTypeOperations);
}

void QFMain::ZeroMeasure()
{
    int ret = element->startTask(TT_ZeroCheck);

    if (ret != 0)
        QMessageBox::warning(this, tr("警告"), tr("%1，执行失败").arg(element->translateStartCode(ret)));
    else
        addLogger(tr("零点核查"), LoggerTypeOperations);
}

void QFMain::StandardMeasure()
{
    int ret = element->startTask(TT_SampleCheck);

    if (ret != 0)
        QMessageBox::warning(this, tr("警告"), tr("%1，执行失败").arg(element->translateStartCode(ret)));
    else
        addLogger(tr("标样核查"), LoggerTypeOperations);
}

void QFMain::QCMeasure()
{
    int ret = element->startTask(TT_SpikedCheck);

    if (ret != 0)
        QMessageBox::warning(this, tr("警告"), tr("%1，执行失败").arg(element->translateStartCode(ret)));
    else
        addLogger(tr("水样加标"), LoggerTypeOperations);
}

void QFMain::Drain()
{
    int ret = element->startTask(TT_Drain);

    if (ret != 0)
        QMessageBox::warning(this, tr("警告"), tr("%1，执行失败").arg(element->translateStartCode(ret)));
    else
        addLogger(tr("手动排空"), LoggerTypeMaintiance);
}

void QFMain::Stop()
{
    if (element->getTaskType() <= TT_ErrorProc &&
            QMessageBox::question(this, tr("提示"), tr("当前正在执行其他任务，是否确定停止？"),
                                     QMessageBox::Yes|QMessageBox::No)
                    == QMessageBox::No)
        return;

    addLogger(tr("手动停止"), LoggerTypeMaintiance);

    clearLastErrorMsg();
    ui->warning->clear();
    element->stopTasks();
}

void QFMain::Clean()
{
    int ret = element->startTask(TT_Clean);

    if (ret != 0)
        QMessageBox::warning(this, tr("警告"), tr("%1，执行失败").arg(element->translateStartCode(ret)));
    else
        addLogger(tr("手动清洗"), LoggerTypeMaintiance);
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
        profile.setValue("PumpRotate1", maintaince->PumpRotate1->currentIndex());
        profile.setValue("PumpRotate2", maintaince->PumpRotate2->currentIndex());
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
    else
        addLogger(tr("独立进样"), LoggerTypeMaintiance);
}

void QFMain::InitLoad()
{
    int ret = element->startTask(TT_Initload);

    if (ret != 0)
        QMessageBox::warning(this, tr("警告"), tr("%1，执行失败").arg(element->translateStartCode(ret)));
    else
        addLogger(tr("初始进样"), LoggerTypeMaintiance);
}

void QFMain::SaveLigthVoltage()
{
    DatabaseProfile profile;
    if (profile.beginSection("lightVoltage"))
    {
        profile.setValue("Color1Current", lightVoltage->Color1Current->value());
        profile.setValue("Color2Current", lightVoltage->Color2Current->value());
        profile.setValue("Color1Gain", lightVoltage->Color1Gain->currentIndex());
        profile.setValue("Color2Gain", lightVoltage->Color2Gain->currentIndex());
        profile.setValue("CurrentHigh", lightVoltage->CurrentHigh->value());
        profile.setValue("CurrentLow", lightVoltage->CurrentLow->value());
        profile.setValue("ThresholdHigh", lightVoltage->ThresholdHigh->value());
        profile.setValue("ThresholdMid", lightVoltage->ThresholdMid->value());
        profile.setValue("ThresholdLow", lightVoltage->ThresholdLow->value());
    }

    int ret = element->startTask(TT_Config);

    if (ret != 0)
        QMessageBox::warning(this, tr("警告"), tr("%1，执行失败").arg(element->translateStartCode(ret)));
    else
        addLogger(tr("光源调节"), LoggerTypeMaintiance);
}
