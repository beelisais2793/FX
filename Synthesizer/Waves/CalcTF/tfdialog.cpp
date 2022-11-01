#include "tfdialog.h"
#include "ui_tfdialog.h"
#include <QDir>

TfDialog::TfDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TfDialog)
{
    ui->setupUi(this);

    m_tf = std::make_shared<IntervalTF>();
}

TfDialog::~TfDialog()
{
    //webchannel->deregisterObject(jshelper);
    delete ui;    
}

std::shared_ptr<IntervalTF> TfDialog::tf() const
{
    return m_tf;
}

void TfDialog::setTf(const std::shared_ptr<IntervalTF> &tf)
{
    m_tf = tf;
    m_tf_original = std::make_shared<IntervalTF>();

    m_tf_original->setD(m_tf->getD());
    m_tf_original->setN(m_tf->getN());
    //m_tf_original->setTF(tf->getZeroVectorStr());
    ui->lineEdit_poleP->setText(m_tf->getD());
    ui->lineEdit_zeroP->setText(m_tf->getN());
    //jshelper->displayEquation(m_tf->getTfEquation());
}

void TfDialog::on_buttonBox_accepted()
{
    emit tfDialogueClosed(true);
}

void TfDialog::on_buttonBox_rejected()
{
    m_tf = m_tf_original;
    emit tfDialogueClosed(false);
}


void TfDialog::on_lineEdit_zeroP_editingFinished()
{
    QString zerosStr = ui->lineEdit_zeroP->text();
    zerosStr = zerosStr.simplified();
    if (zerosStr == "" ) return ;    
    m_tf->setN(zerosStr);

     ui->TF_textEdit->setText(m_tf->getTfEquation());
}

void TfDialog::on_lineEdit_poleP_editingFinished()
{
    QString polesStr = ui->lineEdit_poleP->text();
    polesStr = polesStr.simplified();
    if (polesStr == "") return;
    QString err = "";
     m_tf->setD(polesStr);

    ui->TF_textEdit->setText(m_tf->getTfEquation());
}

void TfDialog::clearUI(){
    ui->lineEdit_poleP->setText("");
    ui->lineEdit_zeroP->setText("");
    //ui->label_error->setText("");
}

QString TfDialog::PATH_POLY_HTML="screen.html";
