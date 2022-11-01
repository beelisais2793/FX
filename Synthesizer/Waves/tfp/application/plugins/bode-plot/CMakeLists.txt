include (TFPPlugin)
add_plugin (bode-plot
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/BodePlot.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Plugin.cpp")
