#ifndef TFDIALOG_H
#define TFDIALOG_H

#include "jshelper.h"
#include "transferfunction.h"
#include "intervaltf.h"

#include <QDialog>
//#include <QWebEngineView>
#include <memory>
#include <QString>
//#include <QWebChannel>

namespace Ui {
class TfDialog;
}

class TfDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TfDialog(QWidget *parent = 0);
    ~TfDialog();

    std::shared_ptr<IntervalTF> tf() const;
    void setTf(const std::shared_ptr<IntervalTF> &tf);
    
    static QString PATH_POLY_HTML;

signals:
    void tfDialogueClosed(bool);    

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_lineEdit_zeroP_editingFinished();

    void on_lineEdit_poleP_editingFinished();

private:
    Ui::TfDialog *ui;
    std::shared_ptr<IntervalTF> m_tf;
    std::shared_ptr<IntervalTF> m_tf_original;
    //QWebEngineView *polyView;
    //QWebChannel *webchannel;
    void clearUI();
    //JsHelper *jshelper;
};

#endif // TFDIALOG_H
