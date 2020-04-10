#include "concindexframe.h"
#include "ui_concindexframe.h"
#include "src/element.h"
#include "src/Trace.h"
#include "src/Common.h"
#include "src/mainwindow.h"

ConcIndexFrame::ConcIndexFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ConcIndexFrame)
{
    ui->setupUi(this);

    TraceDebugPrint;

}

ConcIndexFrame::~ConcIndexFrame()
{
    delete ui;
}

void ConcIndexFrame::initgo(MainWindow *ft)
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




ConcIndexFrame& ConcIndexFrame::operator=(const ConcIndexFrame &src)
{
    qDebug("concIndexFrame class operator==");
    this->indexConc = src.indexConc;
    this->indexRange = src.indexRange;
    this->indexPipe = src.indexPipe;
    this->indexTimes = src.indexTimes;
    this->indexSelExec = src.indexSelExec;
    this->indexIndicating = src.indexIndicating;
    this->indexRepeat = src.indexRepeat;
    this->indexStable = src.indexStable;
    return *this;
}
//////////////////////////////////////////////////////////////
double ConcIndexFrame::getConc()
{
    return indexConc;
}

int ConcIndexFrame::getRange()
{
    return indexRange;
}

int ConcIndexFrame::getPipe()
{
    return indexPipe;
}

int ConcIndexFrame::getTimes()
{
    return indexTimes;
}

int ConcIndexFrame::getSelExec()
{
    return indexSelExec;
}

double ConcIndexFrame::getIndicating()
{
    return indexIndicating;
}

double ConcIndexFrame::getRepeat()
{
    return indexRepeat;
}

double ConcIndexFrame::getStable()
{
    return indexStable;
}
///////////////////////////////////////////////////////////////////
bool ConcIndexFrame::setSelExec(int setValue)
{
    indexSelExec = setValue;
    ui->cbSelExec->setCurrentIndex( indexSelExec );
    return true;
}

bool ConcIndexFrame::setIndicating(double setValue)
{
    indexIndicating = setValue;
    ui->dbIndicating->setValue( indexIndicating );
    return true;
}

bool ConcIndexFrame::setRepeat(double setValue)
{
    indexRepeat = setValue;
    ui->dbRepeat->setValue( indexRepeat );
    return true;
}

bool ConcIndexFrame::setStable(double setValue)
{
    indexStable = setValue;
    ui->dbStable->setValue( indexStable );
    return true;
}
//////////////////////////////////////////////////////////////////////
bool ConcIndexFrame::renewUi()
{
    this->ui->dbConc->setValue( indexConc );
    this->ui->cbRange->setCurrentIndex( indexRange );
    this->ui->cbPipe->setCurrentIndex( indexPipe);
    this->ui->spTimes->setValue( indexTimes );
    this->ui->cbSelExec->setCurrentIndex( indexSelExec );
    this->ui->dbIndicating->setValue( indexIndicating );
    this->ui->dbRepeat->setValue( indexRepeat );
    this->ui->dbStable->setValue( indexStable );
    return true;
}

bool ConcIndexFrame::saveUi()
{
    indexConc       = ui->dbConc->value();
    indexRange      = ui->cbRange->currentIndex();
    indexPipe       = ui->cbPipe->currentIndex();
    indexTimes      = ui->spTimes->value();
    indexSelExec    = ui->cbSelExec->currentIndex();
    indexIndicating = ui->dbIndicating->value();
    indexRepeat     = ui->dbRepeat->value();
    indexStable     = ui->dbStable->value();
    return true;
}
