include (TFPPlugin)
add_plugin (std-low-order-filters
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/StandardLowOrderFilter.hpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Plugin.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/StandardLowOrderFilter.cpp")
