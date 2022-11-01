#include "tfp/util/Logger.hpp"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

namespace tfp {

tfp::Logger g_log;

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
static std::string currentDateTime()
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[23];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "[%Y-%m-%d.%X] ", &tstruct);

    return buf;
}

static const char* head =
"<html>\n"
"    <head>\n"
"        <style type=\"text/css\">\n"
"\n"
"            body{\n"
"            font-family: Trebuchet MS, Lucida Sans Unicode, Arial, sans-serif;    /* Font to use */\n"
"            margin:0px;\n"
"\n"
"            }\n"
"\n"
"            .dhtmlgoodies_question{    /* Styling question */\n"
"            /* Start layout CSS */\n"
"            color:#FFF;\n"
"            font-size:0.9em;\n"
"            background-color:#317082;\n"
"            width:1230px;\n"
"            margin-bottom:2px;\n"
"            margin-top:2px;\n"
"            padding-left:2px;\n"
"            height:20px;\n"
"\n"
"            /* End layout CSS */\n"
"\n"
"            overflow:hidden;\n"
"            cursor:pointer;\n"
"            }\n"
"            .dhtmlgoodies_answer{    /* Parent box of slide down content */\n"
"            /* Start layout CSS */\n"
"            border:1px solid #317082;\n"
"            background-color:#E2EBED;\n"
"            width:1200px;\n"
"            background-image:url('data/rd.png');\n"
"            background-repeat:repeat;\n"
"            background-position:top right;\n"
"\n"
"            /* End layout CSS */\n"
"\n"
"            visibility:hidden;\n"
"            height:0px;\n"
"            overflow:hidden;\n"
"            position:relative;\n"
"\n"
"            }\n"
"            .dhtmlgoodies_answer_content{    /* Content that is slided down */\n"
"            padding:1px;\n"
"            font-size:0.9em;\n"
"            position:relative;\n"
"            }\n"
"\n"
"        </style>\n"
"\n"
"        <script type=\"text/javascript\">\n"
"            /************************************************************************************************************\n"
"            Show hide content with slide effect\n"
"            Copyright (C) August 2010  DTHMLGoodies.com, Alf Magne Kalleland\n"
"\n"
"            This library is free software; you can redistribute it and/or\n"
"            modify it under the terms of the GNU Lesser General Public\n"
"            License as published by the Free Software Foundation; either\n"
"            version 2.1 of the License, or (at your option) any later version.\n"
"\n"
"            This library is distributed in the hope that it will be useful,\n"
"            but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"            MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
"            Lesser General Public License for more details.\n"
"\n"
"            You should have received a copy of the GNU Lesser General Public\n"
"            License along with this library; if not, write to the Free Software\n"
"            Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA\n"
"\n"
"            Dhtmlgoodies.com., hereby disclaims all copyright interest in this script\n"
"            written by Alf Magne Kalleland.\n"
"\n"
"            Alf Magne Kalleland, 2010\n"
"            Owner of DHTMLgoodies.com\n"
"\n"
"            ************************************************************************************************************/\n"
"\n"
"            var dhtmlgoodies_slideSpeed = 600;    // Higher value = faster\n"
"            var dhtmlgoodies_timer = 10;    // Lower value = faster\n"
"\n"
"            var objectIdToSlideDown = false;\n"
"            var dhtmlgoodies_activeId = false;\n"
"            var dhtmlgoodies_slideInProgress = false;\n"
"            var dhtmlgoodies_slideInProgress = false;\n"
"            var dhtmlgoodies_expandMultiple = true; // true if you want to be able to have multiple items expanded at the same time.\n"
"\n"
"            function showHideContent(e,inputId)\n"
"            {\n"
"            if(dhtmlgoodies_slideInProgress)return;\n"
"            dhtmlgoodies_slideInProgress = true;\n"
"            if(!inputId)inputId = this.id;\n"
"            inputId = inputId + '';\n"
"            var numericId = inputId.replace(/[^0-9]/g,'');\n"
"            var answerDiv = document.getElementById('dhtmlgoodies_a' + numericId);\n"
"\n"
"            objectIdToSlideDown = false;\n"
"\n"
"            if(!answerDiv.style.display || answerDiv.style.display=='none'){\n"
"                if(dhtmlgoodies_activeId &&  dhtmlgoodies_activeId!=numericId && !dhtmlgoodies_expandMultiple){\n"
"                    objectIdToSlideDown = numericId;\n"
"                    slideContent(dhtmlgoodies_activeId,(dhtmlgoodies_slideSpeed*-1));\n"
"                }else{\n"
"\n"
"                    answerDiv.style.display='block';\n"
"                    answerDiv.style.visibility = 'visible';\n"
"\n"
"                    slideContent(numericId,dhtmlgoodies_slideSpeed);\n"
"                }\n"
"            }else{\n"
"                slideContent(numericId,(dhtmlgoodies_slideSpeed*-1));\n"
"                dhtmlgoodies_activeId = false;\n"
"            }\n"
"            }\n"
"\n"
"            function slideContent(inputId,direction)\n"
"            {\n"
"\n"
"            var obj =document.getElementById('dhtmlgoodies_a' + inputId);\n"
"            var contentObj = document.getElementById('dhtmlgoodies_ac' + inputId);\n"
"            height = obj.clientHeight;\n"
"            if(height==0)height = obj.offsetHeight;\n"
"            height = height + direction;\n"
"            rerunFunction = true;\n"
"            if(height>contentObj.offsetHeight){\n"
"                height = contentObj.offsetHeight;\n"
"                rerunFunction = false;\n"
"            }\n"
"            if(height<=1){\n"
"                height = 1;\n"
"                rerunFunction = false;\n"
"            }\n"
"\n"
"            obj.style.height = height + 'px';\n"
"            var topPos = height - contentObj.offsetHeight;\n"
"            if(topPos>0)topPos=0;\n"
"            contentObj.style.top = topPos + 'px';\n"
"            if(rerunFunction){\n"
"                setTimeout('slideContent(' + inputId + ',' + direction + ')',dhtmlgoodies_timer);\n"
"            }else{\n"
"                if(height<=1){\n"
"                    obj.style.display='none';\n"
"                    if(objectIdToSlideDown && objectIdToSlideDown!=inputId){\n"
"                        document.getElementById('dhtmlgoodies_a' + objectIdToSlideDown).style.display='block';\n"
"                        document.getElementById('dhtmlgoodies_a' + objectIdToSlideDown).style.visibility='visible';\n"
"                        slideContent(objectIdToSlideDown,dhtmlgoodies_slideSpeed);\n"
"                    }else{\n"
"                        dhtmlgoodies_slideInProgress = false;\n"
"                    }\n"
"                }else{\n"
"                    dhtmlgoodies_activeId = inputId;\n"
"                    dhtmlgoodies_slideInProgress = false;\n"
"                }\n"
"            }\n"
"            }\n"
"\n"
"            function initShowHideDivs()\n"
"            {\n"
"            var divs = document.getElementsByTagName('DIV');\n"
"            var divCounter = 1;\n"
"            for(var no=0;no<divs.length;no++){\n"
"                if(divs[no].className=='dhtmlgoodies_question'){\n"
"                    divs[no].onclick = showHideContent;\n"
"                    divs[no].id = 'dhtmlgoodies_q'+divCounter;\n"
"                    var answer = divs[no].nextSibling;\n"
"                    while(answer && answer.tagName!='DIV'){\n"
"                        answer = answer.nextSibling;\n"
"                    }\n"
"                    answer.id = 'dhtmlgoodies_a'+divCounter;\n"
"                    contentDiv = answer.getElementsByTagName('DIV')[0];\n"
"                    contentDiv.style.top = 0 - contentDiv.offsetHeight + 'px';\n"
"                    contentDiv.className='dhtmlgoodies_answer_content';\n"
"                    contentDiv.id = 'dhtmlgoodies_ac' + divCounter;\n"
"                    answer.style.display='none';\n"
"                    answer.style.height='1px';\n"
"                    divCounter++;\n"
"                }\n"
"            }\n"
"            }\n"
"            window.onload = initShowHideDivs;\n"
"        </script>\n"
"    </head>\n"
"    <body>\n";

#ifdef COMMON_PLATFORM_WINDOWS
#    define LOG_NAME "\\log.html"
#else
#    define LOG_NAME "/log.html"
#endif

// ----------------------------------------------------------------------------
Logger::Logger() :
    fp_(NULL)
{
}

// ----------------------------------------------------------------------------
Logger::Logger(const char* fileName)
{
    open(fileName);
}

// ----------------------------------------------------------------------------
Logger::~Logger()
{
    if(!fp_)
        return;

    fprintf(fp_, "</pre></body></html>");
    fclose(fp_);
}

// ----------------------------------------------------------------------------
bool Logger::openDefaultLog()
{
    g_log.open(LOG_NAME);

    return (g_log.getFileStream() != NULL);
}

// ----------------------------------------------------------------------------
bool Logger::openDefaultLog(int argc, char** argv)
{
    std::string path = argv[0];

    for (int pos = path.length(); pos != 0; --pos)
    {
        if (path[pos] == '/' || path[pos] == '\\')
        {
            path.erase(pos);
            break;
        }
    }

    path.append(LOG_NAME);
    g_log.open(path.c_str());

    return (g_log.getFileStream() != NULL);
}

// ----------------------------------------------------------------------------
void Logger::open(const char* fileName)
{
    if(!(fp_ = fopen(fileName, "w")))
        {
        fprintf(stderr, "Failed to open log \"%s\"", fileName);
        return;
    }

    fileName_ = fileName;

    // write HTML header
    fprintf(fp_, "%s", head);

    // make sure to open <pre> tag, this is where everything gets logged
    fprintf(fp_, "<pre>");
/*
    beginDropdown("Build system info");
    logInfo("Commit hash: %s", BuildInfo::getCommitHash());
    logInfo("Build time stamp: %s", BuildInfo::getBuildTimeStamp());
    logInfo("Build system: %s", BuildInfo::getBuildSystem());
    endDropdown();*/
}

#define FORWARD_TO_FPRINTF(msg)     \
        va_list va;                 \
        va_start(va, fmt);          \
        vfprintf(target, msg, va);  \
        va_end(va);                 \
        fflush(target);             \
        va_start(va, fmt);          \
        vprintf(msg, va);           \
        va_end(va);                 \
        fflush(stdout)

#define DISPATCH_AND_FORWARD_TO_PRINTF(msg, member_func)      \
        va_list va;                                           \
        va_start(va, fmt);                                    \
        unsigned len = vsnprintf(NULL, 0, msg, va);           \
        va_end(va);                                           \
        char* str = (char*)malloc((len + 1) * sizeof(char));  \
        va_start(va, fmt);                                    \
        vsprintf(str, msg, va);                               \
        va_end(va);                                           \
        fprintf(target, "%s", str);                           \
        fflush(target);                                       \
        dispatcher.dispatch(member_func, str);                \
        free(str);                                            \

// ----------------------------------------------------------------------------
void Logger::logInfo(const char* fmt, ...)
{
    FILE* target = (fp_ ? fp_ : stdout);
    FORWARD_TO_FPRINTF(toHTML("353535", "INFO", fmt).c_str());
}

// ----------------------------------------------------------------------------
void Logger::logNotice(const char* fmt, ...)
{
    FILE* target = (fp_ ? fp_ : stdout);
    FORWARD_TO_FPRINTF(toHTML("007050", "NOTICE", fmt).c_str());
}

// ----------------------------------------------------------------------------
void Logger::logWarning(const char* fmt, ...)
{
    FILE* target = (fp_ ? fp_ : stderr);
    FORWARD_TO_FPRINTF(toHTML("ff7000", "WARNING", fmt).c_str());
}

// ----------------------------------------------------------------------------
void Logger::logError(const char* fmt, ...)
{
    FILE* target = (fp_ ? fp_ : stderr);
    std::string htmlFmt = toHTML("ff0020", "ERROR", fmt);
    DISPATCH_AND_FORWARD_TO_PRINTF(htmlFmt.c_str(), &LogListener::onError);
}

// ----------------------------------------------------------------------------
void Logger::logFatal(const char* fmt, ...)
{
    FILE* target = (fp_ ? fp_ : stderr);
    std::string htmlFmt = toHTML("ff0020", "FATAL", fmt);
    DISPATCH_AND_FORWARD_TO_PRINTF(htmlFmt.c_str(), &LogListener::onFatal);
}

// ----------------------------------------------------------------------------
void Logger::beginDropdown(const char* title)
{
    /*
     * Hold a mutex until the dropdown is closed again. Prevents other threads
     * from logging stuff into the dropdown.
     */
    //mutex_.acquire();

    FILE* target = (fp_ ? fp_ : stdout);
    std::string str("<font color=#000000>");
    str += title; str += "</font>";
    fprintf(target, "%s", "</pre><div class=\"dhtmlgoodies_question\">");
    fprintf(target, "%s", str.c_str());
    fprintf(target, "%s", "</div>");
    fprintf(target, "%s", "<div class=\"dhtmlgoodies_answer\"><div><ul><pre>");
}

// ----------------------------------------------------------------------------
void Logger::endDropdown()
{
    FILE* target = (fp_ ? fp_ : stdout);
    fprintf(target, "</pre></ul></div></div><pre>");
    fflush(target);

    //mutex_.release();
}

// ----------------------------------------------------------------------------
FILE* Logger::getFileStream() const
{
    return (fp_ ? fp_ : stdout);
}

// ----------------------------------------------------------------------------
const char* Logger::getFileName() const
{
    return fileName_.c_str();
}

// ----------------------------------------------------------------------------
std::string Logger::toHTML(const char* colour, const char* tag, const char* message)
{
    using std::string;
    string str = currentDateTime();
    str += "<font color=#"; str += colour;
        str += ">["; str += tag; str += "] "; str += message;
    str += "</font>\n";
    return str;
}

} // namespace tfp
