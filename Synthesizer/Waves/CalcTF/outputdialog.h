#ifndef OUTPUTDIALOG_H
#define OUTPUTDIALOG_H

#include "jshelper.h"
#include "plotdialog.h"
#include "customgraph.h"
#include "tfdialog.h"

#include <QDialog>
//#include <QWebChannel>
//#include <QWebEngineView>
#include <controlsystem.h>
#include <memory>

namespace Ui {
class OutputDialog;
}

class OutputDialog : public QDialog
{
    Q_OBJECT

public:
    static QString PATH_POLY_HTML;
    explicit OutputDialog(QWidget *parent = 0);
    ~OutputDialog();

    std::shared_ptr<ControlSystem> cs() const;
    void setCs(const std::shared_ptr<ControlSystem> &cs);

    TfDialog *getTfdiag() const;
    void setTfdiag(TfDialog *value);

public slots:
    void displayEquation();
    void on_tfdialog_closed(bool status);    

private slots:

    void on_pushButton_rootlocus_clicked();
    void on_pushButton_EditTF_clicked();    

private:
    Ui::OutputDialog *ui;
//    QWebEngineView *polyView;
//    QWebChannel *webchannel;

    std::shared_ptr<ControlSystem> m_cs;
    //JsHelper *jshelper;
    PlotDialog *plotDialog;
    std::shared_ptr<CustomGraph> m_cgraph;

    TfDialog *tfdiag;
    OutputDialog *outdialog;    
};

#endif // OUTPUTDIALOG_H
