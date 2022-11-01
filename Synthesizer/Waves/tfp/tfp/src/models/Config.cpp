#include "tfp/models/Config.hpp"
#include "tfp/util/Util.hpp"
#include "tfp/util/Logger.hpp"
#include "pugixml.hpp"

namespace tfp {
Config g_config;
}

using namespace tfp;

// ----------------------------------------------------------------------------
void Config::load()
{
    using namespace pugi;

    QString settingsFile = Util::getConfigDir().filePath("settings.xml");

    g_log.logInfo("Loading settings from %s", settingsFile.toUtf8().data());
    xml_document doc;
    xml_parse_result result = doc.load_file(settingsFile.toUtf8().data());

    // Print as XML to the log
    g_log.beginDropdown(("XML settings \"" + settingsFile + "\"").toUtf8().data());
        FILE* fp = g_log.getFileStream();
        fprintf(fp, "<xmp>"); // using CDATA causes it not to display
        xml_writer_file writer(fp);
        doc.save(writer);
        fprintf(fp, "</xmp>");
    g_log.endDropdown();

    /*
     * Act as if loading worked. Rely on Pugi returning default values for
     * missing nodes for most of the settings if loading the file didn't
     * succeed.
     */
    xml_node root = doc.root().child("tfp");

    // Language settings
    currentLocale = root.child("language").attribute("locale").as_string("en_GB");

    // Expression solver settings
    {
        xml_node n = root.child("expression_solver");
        expressionSolver.allowVariableExponents = n.child("variable_exponents").attribute("enabled").as_bool(true);
        expressionSolver.analyticalExponents = n.child("analytical_exponents").attribute("enabled").as_bool(false);
        expressionSolver.exponentialExpansion = n.child("exponential_expansion").attribute("enabled").as_bool(true);
        expressionSolver.expansionCount = n.child("exponential_expansion").attribute("count").as_uint(5);
    }

    // Plot settings
    {
        xml_node n = root.child("plot");
        xml_node dp = n.child("data_points");
        plot.dataPointMode = dp.attribute("mode").as_uint(0);
        plot.fixedDataPoints = dp.attribute("fixed").as_uint(500);
        plot.maxDataPoints = dp.attribute("max").as_uint(5000);
        plot.minDataPoints = dp.attribute("min").as_uint(100);
    }
}

// ----------------------------------------------------------------------------
void Config::save()
{
    using namespace pugi;

    xml_document doc;
    xml_node root = doc.root().append_child("tfp");

    // Language settings
    root.append_child("language").append_attribute("locale").set_value(currentLocale.toUtf8().data());

    // Expression solver settings
    {
        xml_node n = root.append_child("expression_solver");
        n.append_child("variable_exponents").append_attribute("enabled").set_value(expressionSolver.allowVariableExponents);
        n.append_child("analytical_exponents").append_attribute("enabled").set_value(expressionSolver.analyticalExponents);
        xml_node exponentialExpansion =  n.append_child("exponential_expansion");
        exponentialExpansion.append_attribute("enabled").set_value(expressionSolver.exponentialExpansion);
        exponentialExpansion.append_attribute("count").set_value(expressionSolver.expansionCount);
    }

    // Plot settings
    {
        xml_node n = root.append_child("plot");
        xml_node dp = n.append_child("data_points");
        dp.append_attribute("mode").set_value(plot.dataPointMode);
        dp.append_attribute("fixed").set_value(plot.fixedDataPoints);
        dp.append_attribute("max").set_value(plot.maxDataPoints);
        dp.append_attribute("min").set_value(plot.minDataPoints);
    }

    // Save XML file
    QString settingsFile = Util::getConfigDir().filePath("settings.xml");
    g_log.logInfo("Saving settings to %s", settingsFile.toUtf8().data());
    doc.save_file(settingsFile.toUtf8().data());

    // Print as XML to the log
    g_log.beginDropdown(("XML settings \"" + settingsFile + "\"").toUtf8().data());
        FILE* fp = g_log.getFileStream();
        fprintf(fp, "<xmp>"); // using CDATA causes it not to display
        xml_writer_file writer(fp);
        doc.save(writer);
        fprintf(fp, "</xmp>");
    g_log.endDropdown();
}
