include (TFPPlugin)
add_plugin (elliptic-filter
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/EllipticFilter.hpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/EllipticFilter.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Plugin.cpp")
