#pragma once

#include "tfp/config.hpp"
#include "tfp/util/ListenerDispatcher.hpp"

#include <stdio.h>
#include <iostream>
#include <string>


namespace tfp {

enum LogSeverity {
    INFO,
    NOTICE,
    WARNING,
    ERROR,
    FATAL
};


class LogListener
{
protected:
    ~LogListener() {}

public:
    virtual void onError(const char* message) {}
    virtual void onFatal(const char* message) {}
};


/*!
 * @brief A simple HTML logger.
 *
 * Messages can be logged via the global variable *g_log*, which is an
 * instance of this class.
 *
 * All messages are converted to HTML so it looks nice in a browser.
 */
class TFP_PUBLIC_API Logger
{
public:
    Logger();
    Logger(const char* fileName);
    ~Logger();

    /*!
     * @brief Opens the log (if not already) in the default path, as specified
     * by the compile time constant DEFAULT_LOG_PATH.
     * @note The default log path can be relative, which may cause the log to
     * be opened in the folder where a measurement file is double-clicked.
     * Consider using openDefaultLog(argc, argv).
     * @return Returns true if the log was opened.
     */
    static bool openDefaultLog();

    /*!
     * @brief Opens the log in the same folder as the executable, regardless of
     * working directory. May fail if the program was installed in a write
     * protected location.
     */
    static bool openDefaultLog(int argc, char** argv);

    /*!
     * @brief Begins a dropdown box. All proceeding logged messages are written
     * to the dropdown box.
     *
     * Dropdowns are useful for keeping the overall log clean from clutter.
     * One might have to dump large amounts of debug data that is not
     * necessarily directly relevant, but still useful. Putting this kind of
     * data in a dropdown hides the data initially from the viewer.
     *
     * @param[in] title This is the title of the dropdown box. When clicking
     */
    void beginDropdown(const char* title);

    /*!
     * @brief Ends a dropdown box. All proceeding logged messages are written
     * normally again.
     */
    void endDropdown();

    /// Non-critical information. Uses the printf-style format string.
    void logInfo(const char* fmt, ...);
    /// More interesting information, but not critical. Uses the printf-style format string.
    void logNotice(const char* fmt, ...);
    /// Warning message. Uses the printf-style format string.
    void logWarning(const char* fmt, ...);
    /// An error has occurred. Uses the printf-style format string.
    void logError(const char* fmt, ...);
    /// A fatal error has occurred. Uses the printf-style format string.
    void logFatal(const char* fmt, ...);

    /// Returns the open file stream of the log file.
    FILE* getFileStream() const;
    /// Returns the name of the open log file. This includes its path.
    const char* getFileName() const;

    ListenerDispatcher<LogListener> dispatcher;

private:
    void open(const char* fileName);
    std::string toHTML(const char* colour, const char* tag, const char* message);

    FILE* fp_;
    std::string fileName_;
};

TFP_PUBLIC_API extern tfp::Logger g_log;

} // namespace tfp

