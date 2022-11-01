#include "tfp/plugin/Plugin.hpp"
#include "tfp/plugin/Tool.hpp"
#include "tfp/listeners/PluginListener.hpp"
#include <QLibrary>
#include <QHBoxLayout>
#include <iostream>

namespace tfp {

struct ToolDecorator : public Tool, public PluginListener
{
    ToolDecorator(ToolFactory* factory, Plugin* plugin) :
        factory_(factory),
        plugin_(plugin)
    {
        plugin_->dispatcher.addListener(this);

        wrapped_ = factory_->create();
        QHBoxLayout* layout = new QHBoxLayout;
        layout->addWidget(wrapped_);
        setLayout(layout);
    }

    ~ToolDecorator()
    {
        if (plugin_ != NULL)
            plugin_->dispatcher.removeListener(this);
        if (wrapped_ != NULL)
            factory_->destroy(wrapped_);
    }

    virtual void onPluginUnloading() OVERRIDE
    {
        if (wrapped_ != NULL)
            factory_->destroy(wrapped_);
        wrapped_ = NULL;
    }

    virtual void setSystem(System* system) OVERRIDE
    {
        if (wrapped_ != NULL)
            wrapped_->setSystem(system);
    }

    virtual void onSetSystem() OVERRIDE {}
    virtual void onSystemStructureChanged() OVERRIDE {}
    virtual void onSystemParametersChanged() OVERRIDE {}
    virtual void autoScale() OVERRIDE {}
    virtual void replot() OVERRIDE {}

private:
    Tool* wrapped_;
    ToolFactory* factory_;
    WeakReference<Plugin> plugin_;
};

// ----------------------------------------------------------------------------
Plugin::Plugin(QString name) :
    start(NULL),
    stop(NULL),
    library_(new QLibrary),
    name_(name)
{
}

// ----------------------------------------------------------------------------
Plugin::~Plugin()
{
}

// ----------------------------------------------------------------------------
const QString& Plugin::name() const
{
    return name_;
}

// ----------------------------------------------------------------------------
bool Plugin::registerTool(ToolFactory* factory,
                          Category category,
                          SubCategory subCategory,
                          const QString& name,
                          const QString& author,
                          const QString& blurb,
                          const QString& contactInfo)
{
    if (toolFactories_.find(name) != toolFactories_.end())
        return false;
    factory->category = category;
    factory->subCategory = subCategory;
    factory->name = name;
    factory->author = author;
    factory->blurb = blurb;
    factory->contactInfo = contactInfo;
    toolFactories_.insert(name, factory);
    return true;
}

// ----------------------------------------------------------------------------
QVector<ToolFactory*> Plugin::getToolsList() const
{
    QVector<ToolFactory*> list;
    for (ToolFactories::const_iterator it = toolFactories_.begin(); it != toolFactories_.end(); ++it)
        list.push_back(it.value());
    return list;
}

// ----------------------------------------------------------------------------
Tool* Plugin::createTool(const QString& name)
{
    ToolFactories::iterator it = toolFactories_.find(name);
    if (it == toolFactories_.end())
        return NULL;
    return new ToolDecorator(it.value(), this);
}

// ----------------------------------------------------------------------------
int Plugin::runTests(int* argc, char** argv)
{
    if (run_tests == NULL)
        return 0;
    return run_tests(argc, argv);
}

}
