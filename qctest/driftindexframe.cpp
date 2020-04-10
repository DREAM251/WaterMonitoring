#include "driftindexframe.h"
#include "ui_driftindexframe.h"
#include "src/element.h"
#include "src/Trace.h"
#include "src/Common.h"
#include "src/mainwindow.h"

DriftIndexFrame::DriftIndexFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::DriftIndexFrame)
{
    ui->setupUi(this);

    TraceDebugPrint;


    setLableReferIndex1Visible(false);

}

DriftIndexFrame::~DriftIndexFrame()
{
    delete ui;
}

void DriftIndexFrame::initgo(MainWindow *ft)
{
    father = ft;
    ui->cbPipe->clear();
    int i = 0;
    while( !ELEMENT->GetQcOfflinePipeName(i).isEmpty() )
    {
        ui->cbPipe->addItem(ELEMENT->GetQcOfflinePipeName(i));
        i++;
        if(i > 8)break;
    }

    ui->cbRange->clear();
    ui->cbRange->addItems(ELEMENT->getRanges());
}

double DriftIndexFrame::getConc()
{
    return indexConc;
}

int DriftIndexFrame::getRange()
{
    return indexRange;
}

QString DriftIndexFrame::getStrRange()
{
    return ui->cbRange->itemText(indexRange);
}

int DriftIndexFrame::getPipe()
{
    return indexPipe;
}

int DriftIndexFrame::getUiTimes()
{
    return ui->spTimes->value();
}

int DriftIndexFrame::getTimes()
{
    return indexTimes;
}

int DriftIndexFrame::getUiIgnoreTimes()
{
    return ui->spIgnoreTimes->value();
}

int DriftIndexFrame::getIgnoreTimes()
{
    return indexIgnoreTimes;
}

int DriftIndexFrame::getSelExec()
{
    return indexSelExec;
}

double DriftIndexFrame::getIndex()
{
    return indexIndex;
}

double DriftIndexFrame::getReferIndex()
{
    return indexReferIndex;
}

double DriftIndexFrame::getReferIndex1()
{
    return indexReferIndex1;
}
//////////////////////////////////////////////////////////////
bool DriftIndexFrame::setSelExec(int setValue)
{
    indexSelExec = setValue;
    ui->cbSelExec->setCurrentIndex(indexSelExec);
    return true;
}

bool DriftIndexFrame::setIndex(double setValue)
{
    indexIndex = setValue;
    ui->dbIndex->setValue(indexIndex);
    return true;
}

bool DriftIndexFrame::setReferIndex(double setValue)
{
    indexReferIndex = setValue;
    ui->dbReferIndex->setValue(indexReferIndex);
    return true;
}

bool DriftIndexFrame::setReferIndex1(double setValue)
{
    indexReferIndex1 = setValue;
    ui->dbReferIndex1->setValue(indexReferIndex1);
    return true;
}
///////////////////////////////////////////////////////////////////
bool DriftIndexFrame::setLabelIndexText(QString text)
{
    ui->lbIndex->setText(text);
    return true;
}

bool DriftIndexFrame::setLabelReferIndexText(QString text)
{
    ui->lbReferIndex->setText(text);
    return true;
}

bool DriftIndexFrame::setLabelReferIndex1Text(QString text)
{
    ui->lbReferIndex1->setText(text);
    return true;
}
bool DriftIndexFrame::renewUi()
{
    ui->dbConc->setValue( indexConc );
    ui->cbRange->setCurrentIndex( indexRange );
    ui->cbPipe->setCurrentIndex( indexPipe);
    ui->spTimes->setValue( indexTimes );
    ui->spIgnoreTimes->setValue( indexIgnoreTimes );
    ui->dbIndex->setValue( indexIndex );
    ui->dbReferIndex->setValue( indexReferIndex );
    ui->dbReferIndex1->setValue( indexReferIndex1 );
    ui->cbSelExec->setCurrentIndex( indexSelExec );
    return true;
}

bool DriftIndexFrame::saveUi()
{
    indexConc       = ui->dbConc->value();
    indexRange      = ui->cbRange->currentIndex();
    indexPipe       = ui->cbPipe->currentIndex();
    indexTimes      = ui->spTimes->value();
    indexIgnoreTimes= ui->spIgnoreTimes->value();
    indexIndex      = ui->dbIndex->value();
    indexReferIndex = ui->dbReferIndex->value();
    indexReferIndex1= ui->dbReferIndex1->value();
    indexSelExec    = ui->cbSelExec->currentIndex();
    return true;
}

bool DriftIndexFrame::setLableConcVisible(bool flags)
{
    ui->lbConc->setVisible(flags);
    ui->dbConc->setVisible(flags);
    return true;
}

bool DriftIndexFrame::setLableRangeVisible(bool flags)
{
    ui->lbRange->setVisible(flags);
    ui->cbRange->setVisible(flags);
    return true;
}

bool DriftIndexFrame::setLableReferIndex1Visible(bool flags)
{
    ui->lbReferIndex1->setVisible(flags);
    ui->dbReferIndex1->setVisible(flags);
    return true;
}

