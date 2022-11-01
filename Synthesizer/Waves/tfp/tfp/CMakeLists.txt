###############################################################################
# compiler flags
###############################################################################

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    add_definitions (-W -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-unused-local-typedefs -Wno-long-long -Wno-variadic-macros -Wno-missing-field-initializers -Wno-write-strings)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_definitions (-W -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-unused-local-typedefs -Wno-long-long -Wno-variadic-macros -Wno-missing-field-initializers -Wno-write-strings)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
endif ()

add_definitions (-D_USE_MATH_DEFINES)

###############################################################################
# config
###############################################################################

# Required by export.hpp.in
set (LIBRARY_NAME "TFP")
set (BUILD_TYPE "SHARED")

add_definitions (-DTFP_BUILDING)

configure_file (${TEMPLATE_EXPORT_H} "include/tfp/export.hpp")
configure_file ("include/tfp/config.hpp.in" "include/tfp/config.hpp")

###############################################################################
# source files and library definition
###############################################################################

set (tfp_HEADERS_MOC
        "${CMAKE_CURRENT_LIST_DIR}/include/tfp/plot/DeltaPlotPicker.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/include/tfp/plot/Panner.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/include/tfp/plot/RectangleZoomer.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/include/tfp/plot/Zoomer.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/include/tfp/views/AdjustmentWidget.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/include/tfp/views/FloatAdjustmentWidget.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/include/tfp/views/InfiniSliderWidget.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/include/tfp/views/MainWindow.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/include/tfp/views/RealtimePlot.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/include/tfp/views/SettingsView.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/include/tfp/views/ToolContainer.hpp")

set (tfp_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/src/tfp.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/math/Expression.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/math/ExpressionParser.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/math/ExpressionManipulator.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/math/ExpressionManipulator_RationalFunction.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/math/ExpressionOptimiser.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/math/ExpressionOptimiser_Constants.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/math/ExpressionOptimiser_Simplifications.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/math/ExpressionOptimiser_UselessOperations.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/math/Math.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/math/VariableTable.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/models/Config.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/models/System.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/models/Translation.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/plot/AutoScaler.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/plot/ColourPalette.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/plot/DeltaPlotPicker.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/plot/MousePlotPickerMachine.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/plot/Panner.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/plot/RectangleZoomer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/plot/UnitTracker.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/plot/Zoomer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/plugin/Plugin.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/plugin/PluginManager.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/plugin/Tool.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/util/Logger.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/util/Metric.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/util/RefCounted.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/util/String.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/util/Util.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/views/AdjustmentWidget.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/views/DataTree.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/views/FloatAdjustmentWidget.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/views/InfiniSliderWidget.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/views/MainWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/views/RealtimePlot.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/views/RealtimePlot3D.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/views/SettingsView.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/views/ToolContainer.cpp")
set (tfp_FORMS
        "${CMAKE_CURRENT_LIST_DIR}/forms/MainWindow.ui"
        "${CMAKE_CURRENT_LIST_DIR}/forms/SettingsView.ui")
set (tfp_RESOURCES
        "${CMAKE_CURRENT_LIST_DIR}/res/res.qrc")

set (tfp_TRANSLATION_FILES
        "")

set (tfp_test_SOURCES "")
if (TFP_BUILD_TESTS)
    set (tfp_test_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/tests/run_tests_api.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/tests/test_Expression.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/tests/test_ExpressionManipulator_RationalFunction.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/tests/test_ExpressionOptimiser_Constants.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/tests/test_ExpressionOptimiser_Simplifications.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/tests/test_ExpressionOptimiser_UselessOperations.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/tests/test_Expression_parser.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/tests/test_VariableTable.cpp")
    add_executable (test_all
        "${CMAKE_CURRENT_LIST_DIR}/tests/run_tests_all.cpp")
    add_executable (test_tfp
        "${CMAKE_CURRENT_LIST_DIR}/tests/run_tests_tfp.cpp")
    target_link_libraries (test_all tfp)
    target_link_libraries (test_tfp tfp)
endif ()

find_package (Qt5
    COMPONENTS Core Widgets Gui OpenGL REQUIRED)

qt5_wrap_cpp (tfp_SOURCES_MOC ${tfp_HEADERS_MOC})
qt5_wrap_ui  (tfp_FORMS_MOC ${tfp_FORMS})
qt5_add_resources (tfp_RESOURCES_GEN ${tfp_RESOURCES})
#qt5_add_translation (tfp_QM_FILES ${tfp_TRANSLATION_FILES})

include_directories ("include")

add_library (tfp ${BUILD_TYPE}
    ${tfp_SOURCES}
    ${tfp_test_SOURCES}
    ${tfp_SOURCES_MOC}
    ${tfp_FORMS_MOC}
    ${tfp_RESOURCES_GEN}
    ${QM_FILES}
)

# generates the correct project files for visual studio, setting thee correct
# debug working directory and other stuff
create_vcproj_userfile (tfp)

###############################################################################
# dependencies
###############################################################################

# must be in this order due to linking
include_directories (${KISS_FFT_INCLUDE_DIRS})
include_directories (${QWT_INCLUDE_DIRS})
include_directories (${QWTPLOT3D_INCLUDE_DIRS})
include_directories (${ADS_INCLUDE_DIRS})
include_directories (${EIGEN_INCLUDE_DIRS})
include_directories (${PUGIXML_INCLUDE_DIRS})

target_link_libraries (tfp Qt5::Core)
target_link_libraries (tfp Qt5::Widgets)
target_link_libraries (tfp Qt5::Gui)

target_link_libraries (tfp ${QWT_LIBRARIES})
target_link_libraries (tfp ${QWTPLOT3D_LIBRARIES})
target_link_libraries (tfp ${ADS_LIBRARIES})
target_link_libraries (tfp ${KISS_FFT_LIBRARIES})
target_link_libraries (tfp ${PUGIXML_LIBRARIES})

if (TFP_BUILD_TESTS)
    target_link_libraries (tfp gmock)
endif ()

###############################################################################
# Compile translation files
###############################################################################

if (PLATFORM MATCHES "WINDOWS")
    set (LRELEASE_COMMAND ${Qt5Core_DIR}/../../../bin/lrelease.exe)
else ()
    set (LRELEASE_COMMAND lrelease)
endif ()

#add_custom_target (tfp_translations_compile
#    COMMAND ${LRELEASE_COMMAND} ${CMAKE_CURRENT_LIST_DIR}/translations/tfp_de.ts
#    COMMENT "Compiling translations"
#    VERBATIM
#)
#add_custom_target (tfp_translations_move
#    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations
#    COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_CURRENT_LIST_DIR}/translations/tfp_de.qm ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations/tfp_de.qm
#    VERBATIM
#)
#add_dependencies (tfp_translations_move tfp_translations_compile)
#add_dependencies (tfp tfp_translations_move)

###############################################################################
# DLLs need to be copied on Windows
###############################################################################

if (PLATFORM MATCHES "WINDOWS")
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        add_custom_target (Qt5CopyBinaries
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/platforms
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../plugins/platforms/qwindowsd.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/platforms
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Cored.dll         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Guid.dll          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5OpenGLd.dll       ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5PrintSupportd.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Svgd.dll          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Widgetsd.dll      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/libGLESv2d.dll       ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/libEGLd.dll          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}

            COMMENT "Copying Qt binaries from '${Qt5Core_DIR}/../../bin/' to '${CMAKE_RUNTIME_OUTPUT_DIRECTORY}'"
            VERBATIM
        )
        add_dependencies (tfp Qt5CopyBinaries)
    endif ()
    if (CMAKE_BUILD_TYPE MATCHES "Release")
        add_custom_target (Qt5CopyBinaries
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/platforms
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../plugins/platforms/qwindows.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/platforms
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Core.dll          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Gui.dll           ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5OpenGL.dll        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5PrintSupport.dll  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Svg.dll           ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Widgets.dll       ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/libGLESv2.dll        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/libEGL.dll           ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}

            COMMENT "Copying Qt binaries from '${Qt5Core_DIR}/../../bin/' to '${CMAKE_RUNTIME_OUTPUT_DIRECTORY}'"
            VERBATIM
        )
        add_dependencies (tfp Qt5CopyBinaries)
    endif ()
endif ()
