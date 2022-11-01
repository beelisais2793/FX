include (TFPPlugin)
add_plugin (butterworth-filter
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/ButterworthFilter.hpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/ButterworthFilter.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Plugin.cpp")
