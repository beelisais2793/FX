#include "tfp/views/SettingsView.hpp"
#include "tfp/ui_SettingsView.h"
#include "tfp/models/Config.hpp"
#include "tfp/models/Translation.hpp"
#include <QDir>

using namespace tfp;

static const char* g_categoryTitles[] = {
    "Language Options",
    "Algorithm Settings",
    "Configure Expression Solver",
    "Plot Settings"
};

// ----------------------------------------------------------------------------
SettingsView::SettingsView(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SettingsView)
{
    ui->setupUi(this);

    // Relies on the QStackedWidget pages being in the same order as the items
    // in the tree.
    QTreeWidgetItemIterator it(ui->treeWidget_categories);
    for (int i = 0; *it; ++it, ++i)
    {
        ui->treeWidget_categories->expandItem(*it);
        categoryIndexMap_[*it] = i;
    }

    // Need to intercept enter events so we can update the description whenever
    // the user hovers over an item in the settings menu.
    ui->checkBox_allowVariableeExponents->installEventFilter(this);
    ui->checkBox_analyticalExponents->installEventFilter(this);
    ui->checkBox_exponentialExpansion->installEventFilter(this);
    ui->container_exponentialExpansionSettings->installEventFilter(this);
    ui->radioButton_fixedDataPoints->installEventFilter(this);
    ui->radioButton_optimalDataPoints->installEventFilter(this);

    connect(ui->pushButton_ok, SIGNAL(released()), this, SLOT(onPushButtonOKReleased()));
    connect(ui->pushButton_apply, SIGNAL(released()), this, SLOT(onPushButtonApplyReleased()));
    connect(ui->pushButton_cancel, SIGNAL(released()), this, SLOT(onPushButtonCancelReleased()));
    connect(ui->treeWidget_categories, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onTreeItemClicked(QTreeWidgetItem*,int)));

    loadConfigIntoUI(&g_config);
    setCategoryIndex(0);
}

// ----------------------------------------------------------------------------
SettingsView::~SettingsView()
{
    delete ui;
}

// ----------------------------------------------------------------------------
void SettingsView::saveUISettingsToConfig(Config* config)
{
    config->expressionSolver.allowVariableExponents = ui->checkBox_allowVariableeExponents->isChecked();
    config->expressionSolver.analyticalExponents = ui->checkBox_analyticalExponents->isChecked();
    config->expressionSolver.exponentialExpansion = ui->checkBox_exponentialExpansion->isChecked();
    config->expressionSolver.expansionCount = ui->spinBox_expansionCount->value();

    config->plot.dataPointMode = ui->radioButton_optimalDataPoints->isChecked() ? 0 : 1;
    config->plot.fixedDataPoints = ui->spinBox_fixedDataPoints->value();
    config->plot.minDataPoints = ui->spinBox_minDataPoints->value();
    config->plot.maxDataPoints = ui->spinBox_maxDataPoints->value();
}

// ----------------------------------------------------------------------------
void SettingsView::loadConfigIntoUI(Config* config)
{
    /*
     * These are things that aren't stored in the config and can change
     * dynamically.
     */
    QStringList availableLocales = scanForLocales();

    // This is always available because it's the native language this app was written in
    availableLocales.append("en_GB");

    // Add locales into combo box
    availableLocales.sort();
    for (int i = 0; i != availableLocales.size(); ++i)
    {
        ui->comboBox_defaultLocale->addItem(availableLocales[i]);
        if (availableLocales[i] == config->currentLocale)
            ui->comboBox_defaultLocale->setCurrentIndex(i);
    }

    ui->checkBox_allowVariableeExponents->setChecked(config->expressionSolver.allowVariableExponents);
    ui->checkBox_analyticalExponents->setChecked(config->expressionSolver.analyticalExponents);
    ui->checkBox_exponentialExpansion->setChecked(config->expressionSolver.exponentialExpansion);
    ui->spinBox_expansionCount->setValue(config->expressionSolver.expansionCount);

    if (config->plot.dataPointMode == 0)
        ui->radioButton_optimalDataPoints->setChecked(true);
    else
        ui->radioButton_fixedDataPoints->setChecked(true);
    ui->spinBox_fixedDataPoints->setValue(config->plot.fixedDataPoints);
    ui->spinBox_minDataPoints->setValue(config->plot.minDataPoints);
    ui->spinBox_maxDataPoints->setValue(config->plot.maxDataPoints);
}

// ----------------------------------------------------------------------------
void SettingsView::setCategoryIndex(int index)
{
    ui->stackedWidget_categories->setCurrentIndex(index);
    ui->label_title->setText(g_categoryTitles[index]);
}

// ----------------------------------------------------------------------------
bool SettingsView::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::Enter)
    {
        if (obj == ui->checkBox_allowVariableeExponents)
            ui->label_description->setText(qtTrId(ID_DESCRIPTION_ALLOWVARIABLEEXPONENTS));
        else if (obj == ui->checkBox_analyticalExponents)
            ui->label_description->setText(qtTrId(ID_DESCRIPTION_ANALYTICALEXPONENTS));
        else if(obj == ui->checkBox_exponentialExpansion)
            ui->label_description->setText(qtTrId(ID_DESCRIPTION_EXPONENTIALEXPANSION));
        else if(obj == ui->container_exponentialExpansionSettings)
            ui->label_description->setText(qtTrId(ID_DESCRIPTION_EXPANSIONCOUNT));
        else if(obj == ui->radioButton_fixedDataPoints)
            ui->label_description->setText(qtTrId(ID_DESCRIPTION_FIXEDDATAPOINTSMODE));
        else if(obj == ui->radioButton_optimalDataPoints)
            ui->label_description->setText(qtTrId(ID_DESCRIPTION_OPTIMALDATAPOINTSMODE));
        else
            ui->label_description->setText(ID_DESCRIPTION_NONE);
    }

    return QWidget::eventFilter(obj, event);
}

// ----------------------------------------------------------------------------
void SettingsView::onPushButtonOKReleased()
{
    saveUISettingsToConfig(&g_config);
    g_config.save();
    close();
}

// ----------------------------------------------------------------------------
void SettingsView::onPushButtonApplyReleased()
{
    saveUISettingsToConfig(&g_config);
    g_config.save();
}

// ----------------------------------------------------------------------------
void SettingsView::onPushButtonCancelReleased()
{
    close();
}

// ----------------------------------------------------------------------------
void SettingsView::onTreeItemClicked(QTreeWidgetItem* item, int column)
{
    setCategoryIndex(categoryIndexMap_[item]);
}

// ----------------------------------------------------------------------------
QStringList SettingsView::scanForLocales() const
{
    QString langPath = QApplication::applicationDirPath();
    langPath.append("/translations");
    QStringList translationList = QDir(langPath).entryList(QStringList("tfp_*.qm"));

    for(QStringList::iterator it = translationList.begin(); it != translationList.end(); ++it)
    {
        it->truncate(it->lastIndexOf('.'));   // e.g. "tfp_en_GB"
        it->remove(0, it->indexOf('_') + 1);  // e.g. "en_GB"
    }

    return translationList;
}

// ----------------------------------------------------------------------------
QString SettingsView::getSystemLocale() const
{
    QString defaultLocale = QLocale::system().name();  // e.g. "en_GB"
    return defaultLocale;
}

// ----------------------------------------------------------------------------
QString SettingsView::askUserForDefaultLocale()
{
    QStringList locales = scanForLocales();
    QString systemLocale = getSystemLocale();

    QComboBox* comboBox_languages = new QComboBox;
    comboBox_languages->addItems(locales);
    for(QStringList::const_iterator it = locales.begin(); it != locales.end(); ++it)
        if(*it == systemLocale)
            comboBox_languages->setCurrentIndex(it - locales.begin());

    QLabel* label = new QLabel("Select Default Language");
    QPushButton* pushButton_ok = new QPushButton("OK");

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(label, 0, 0);
    layout->addWidget(comboBox_languages, 0, 1);
    layout->addWidget(pushButton_ok, 1, 0, 2, 1);

    QWidget* widget = new QWidget(NULL, Qt::Popup | Qt::Dialog);
    widget->setLayout(layout);
    widget->setWindowModality(Qt::WindowModal);
    widget->show();

    QString result = comboBox_languages->currentText();
    delete widget;

    return result;
}
