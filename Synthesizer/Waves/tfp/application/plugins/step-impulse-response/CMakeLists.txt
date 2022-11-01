include (TFPPlugin)
add_plugin (step-impulse-responses
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/TimeDomainResponsePlot.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/ImpulsePlot.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/StepPlot.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Plugin.cpp")
