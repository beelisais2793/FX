include (TFPPlugin)
add_plugin (chebyshev-filter
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/ChebyshevFilter.hpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/ChebyshevFilter.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Plugin.cpp")
