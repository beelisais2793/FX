#include "controlsystem.h"
#include "outputdialog.h"
#include "rootlocusgraph.h"
#include "ui_outputdialog.h"

OutputDialog::OutputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OutputDialog)
{
    ui->setupUi(this);

    plotDialog = new PlotDialog();


    tfdiag = new TfDialog();

    //outdialog = new OutputDialog()

    connect(tfdiag,&TfDialog::tfDialogueClosed,this,&OutputDialog::on_tfdialog_closed);

    m_cs = std::make_shared<ControlSystem>();

}

QString OutputDialog::PATH_POLY_HTML="screen1.html";

OutputDialog::~OutputDialog()
{
    delete ui;
    //webchannel->deregisterObject(jshelper);
}

std::shared_ptr<ControlSystem> OutputDialog::cs() const
{
    return m_cs;
}

void OutputDialog::setCs(const std::shared_ptr<ControlSystem> &cs)
{
    m_cs = cs;    
    ui->tableView_roots->setModel(m_cs->getRootTModel());
    //m_cs->simplifyCS();
    //m_cs->updateRootTable();
    displayEquation();
}

void OutputDialog::displayEquation(){
    //auto ts = m_cs->getCsTF();
    QString eq = m_cs->getCsTF()->getTfEquation();
    //qDebug()<<"Equation to display at output: "<< eq ;
    //if (!this->isHidden())
    //jshelper->displayEquation(equation);
    ui->CS_textEdit->setText(eq);
}




void OutputDialog::on_pushButton_rootlocus_clicked()
{
    //display root locus plot of given transfer function
    std::shared_ptr<IntervalTF> tf = m_cs->getCsTF();
    m_cgraph = std::make_shared<RootLocusGraph>(tf);
    plotDialog->setCustomGraph(m_cgraph.get());
    plotDialog->show();
}



void OutputDialog::on_pushButton_EditTF_clicked()
{
    //call transfer function editor
    if (tfdiag != nullptr){
        tfdiag->setTf(m_cs->getCsTF());
        tfdiag->setWindowTitle("Общая передаточная функция");
        tfdiag->show();
    }
}

void OutputDialog::on_tfdialog_closed(bool status)
{
    if (status){
        //update cs
        m_cs->setCsTF(tfdiag->tf());
        this->setCs(m_cs);
        //qDebug()<<"Display Equation signal to be called";
        //emit displayEquation(m_cs->getCsTF()->getTfEquation());
        //jshelper->displayEquation(equation);
        displayEquation();
    }
}





