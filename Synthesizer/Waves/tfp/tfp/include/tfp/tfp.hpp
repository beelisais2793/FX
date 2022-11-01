#pragma once

#include "tfp/config.hpp"

C_HEADER_BEGIN

TFP_PUBLIC_API void tfp_init(int* argc, char** argv);
TFP_PUBLIC_API void tfp_deinit();

TFP_PUBLIC_API int tfp_run_tests(int* argc, char** argv);
TFP_PUBLIC_API int tfp_run_plugin_tests(int* argc, char** argv, const char* pluginName);
TFP_PUBLIC_API int tfp_run_all_tests(int* argc, char** argv);

C_HEADER_END
