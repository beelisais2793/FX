#pragma once

#include "tfp/config.hpp"
#include <QWidget>
#include <QMap>

class QTreeWidgetItem;

namespace Ui {
    class SettingsView;
}

namespace tfp {

class Config;

class SettingsView : public QWidget
{
    Q_OBJECT

public:
    SettingsView(QWidget* parent=NULL);
    ~SettingsView();

    void saveUISettingsToConfig(Config* config);
    void loadConfigIntoUI(Config* config);
    void setCategoryIndex(int index);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void onPushButtonOKReleased();
    void onPushButtonApplyReleased();
    void onPushButtonCancelReleased();
    void onTreeItemClicked(QTreeWidgetItem* item, int column);

private:
    QStringList scanForLocales() const;
    QString getSystemLocale() const;
    QString askUserForDefaultLocale();

private:
    Ui::SettingsView* ui;
    QMap<QTreeWidgetItem*, int> categoryIndexMap_;
};

}
