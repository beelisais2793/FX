#pragma once

#include "tfp/config.hpp"
#include "tfp/models/System.hpp"
#include "tfp/util/Reference.hpp"
#include "tfp/util/ListenerDispatcher.hpp"
#include <typeinfo>
#include <QMap>
#include <QVector>

class QLibrary;

namespace tfp {

class DataTree;
class PluginManager;
class PluginListener;
class Tool;
class ToolFactory;
template <class T> class ToolFactoryImpl;

/*!
 * @brief Provides an interface for shared libraries to register Tools, and
 * allows the host application to instantiate those tools.
 *
 * Whenever a shared library is loaded, the host application instantiates a
 * Plugin object and passes it to the shared library's start_plugin() function.
 *
 * The shared library can then register "tools" to the plugin object using the
 * registerTool<T> method, where T is a plugin-specific class inheriting from
 * Tool.
 *
 * The host application can then call createTool("name") to instantiate these
 * tools provided by the plugin.
 *
 * @note Due to googletest weirdness, all unit tests are cleared before loading
 * and unloading each plugin, only to be populated again by the plugin specific
 * unit tests (if available). This is unfortunately necessary because unit
 * tests are stored globally and there is no way to keep unit tests that exist
 * in plugins apart from unit tests that exist in the main application.
 */
class TFP_PUBLIC_API Plugin : public RefCounted
{
public:
    /*!
     * @brief When registering a tool, its main purpose needs to be specified.
     * This tells the application how and where to instantiate each tool
     * provided by the plugin.
     *
     * + GENERATOR There can only ever be one generator tool active at a time.
     *   Generators are tools that output systems (such as a filter designer,
     *   a signal flow graph tool, etc.) Generators don't take any input
     *   systems.
     *
     * + VISUALISER These take a system as their input and visualise it in some
     *   way.
     *
     * @warning Only GENERATOR tools may modify the system. VISUALISER tools
     * must guarantee that they only perform read-only operations on the
     * system passed to them.
     */
    enum Category
    {
        GENERATOR,
        VISUALISER,

        CATEGORY_COUNT
    };

    enum SubCategory
    {
        LTI_SYSTEM_CONTINUOUS,
        LTI_SYSTEM_DISCRETE,

        SUB_CATEGORY_COUNT
    };

    /*!
     * @brief Constructs a new plugin with a name. The name is typically the
     * shared library's file name without extension and must be unique among
     * all plugins.
     */
    Plugin(QString name);
    ~Plugin();

    /*!
     * @return Returns the plugin's name.
     */
    const QString& name() const;

    /*!
     * @brief Used by dynamically loaded shared libraries to register their
     * tools with the main application.
     *
     * @param[in] category Specifies the main purpose of the tool. See
     * Plugin::Category for more information.
     * @param[in] subCategory Which subcategory this tool should belong to. See
     * Plugin::SubCategory for more information.
     * @param[in] name Specifies the name of the tool. The name should be
     * unique among all tools and is what appears in the UI's dropdown menus,
     * so it should be human readable (e.g. use "Filter Designer" instead of
     * "filter_designer").
     * @param[in] author The author of this tool/plugin. Shows up in the about
     * menu.
     * @param[in] blurb A short description of what this tool does. Shows up
     * in the about menu.
     * @param[in] contactInfo Your contact information (e-mail)
     */
    template <class T>
    bool registerTool(Category category,
                      SubCategory subCategory,
                      const QString& name,
                      const QString& author,
                      const QString& blurb,
                      const QString& contactInfo)
    {
        return registerTool(new ToolFactoryImpl<T>(), category, subCategory, name, author, blurb, contactInfo);
    }

    /*!
     * @brief Get a list of all available tools that can be instantiated.
     */
    QVector<ToolFactory*> getToolsList() const;

    /*!
     * @brief Instantiates the first tool it can find with the specified name.
     * @return If no tool with such a name exists, NULL is returned. Otherwise,
     * the tool object is returned.
     */
    Tool* createTool(const QString& name);

    /*!
     * @brief Executes all unit tests in the plugin.
     * @note Due to googletest weirdness, this can only ever be called once
     * after loading the shared library.
     */
    int runTests(int* argc, char** argv);

    /*!
     * @brief PluginListener implementations can listen to plugin events (such
     * as unloading events) here.
     */
    ListenerDispatcher<PluginListener> dispatcher;

    friend class PluginManager;

private:
    /// Implementation of register tool.
    bool registerTool(ToolFactory* factory,
                      Category category,
                      SubCategory subCategory,
                      const QString& name,
                      const QString& author,
                      const QString& blurb,
                      const QString& contactInfo);
    void unregisterTool(const QString& typeName);

private:
    typedef bool (*start_plugin_func)(Plugin*, DataTree*);
    typedef void (*stop_plugin_func)(Plugin*);
    typedef int (*run_tests_func)(int*,char**);
    typedef QMap< QString, Reference<ToolFactory> > ToolFactories;

    start_plugin_func start;
    stop_plugin_func  stop;
    run_tests_func    run_tests;
    QLibrary*         library_;
    QString           name_;
    ToolFactories     toolFactories_;
};

/*!
 * @brief Factory, capable of instantiating registered tools. All tool
 * meta-information is also available here.
 */
class ToolFactory : public RefCounted
{
public:
    virtual Tool* create() = 0;
    virtual void destroy(Tool* o) = 0;

    Plugin::Category category;
    Plugin::SubCategory subCategory;
    QString name;
    QString author;
    QString blurb;
    QString contactInfo;
    QString typeName;
};

template <class T>
class ToolFactoryImpl : public ToolFactory
{
public:
    ToolFactoryImpl() { typeName = typeid(T).name(); }
    virtual Tool* create() OVERRIDE { return new T; }
    virtual void destroy(Tool* o) OVERRIDE { delete o; }
};

} // namespace tfp

#if defined(PLUGIN_BUILDING)
extern "C" {
    Q_DECL_EXPORT bool start_plugin(tfp::Plugin* plugin, tfp::DataTree* dataTree);
    Q_DECL_EXPORT void stop_plugin(tfp::Plugin* plugin);
    Q_DECL_EXPORT int run_tests(int argc, char** argv);
}
#endif
