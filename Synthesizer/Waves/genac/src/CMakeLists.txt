set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 5.5.0 COMPONENTS Core)
find_package(CLN)
find_package(GiNaC)

set(genac_SOURCES
  main.cpp
  command.cpp
)

set(genac_HEADERS
  command.h
)

include(ConfigureHeaderFile)
ConfigureHeaderFile("config.h")

add_executable(${GENAC} ${genac_SOURCES} ${genac_HEADERS})
add_dependencies(${GENAC} ${ANALYZERLIB})

target_link_libraries(${GENAC}
  Qt5::Core
  ${CMAKE_BINARY_DIR}/lib/Analyzer/lib${ANALYZERLIB}.a
  ${CMAKE_BINARY_DIR}/lib/Synthesizer/lib${SYNTHESIZERLIB}.a
  ${CLN_LIB}
  ${GINAC_LIB})

target_include_directories(${GENAC} PRIVATE
  ${CMAKE_SOURCE_DIR}/lib)

install (TARGETS ${GENAC} DESTINATION /usr/bin)
