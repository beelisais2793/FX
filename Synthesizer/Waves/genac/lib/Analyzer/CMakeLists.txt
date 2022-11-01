set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core)
find_package(CLN)
find_package(GiNaC)

set(libanalyzer_SOURCES
  Primitive/capacitor.cpp
  Primitive/cccs.cpp
  Primitive/ccvs.cpp
  Primitive/currentsource.cpp
  Primitive/inductor.cpp
  Primitive/resistor.cpp
  Primitive/vccs.cpp
  Primitive/vcvs.cpp
  Primitive/voltagesource.cpp
  Primitive/y.cpp
  abstractcircuit.cpp
  analyzer.cpp
  circuit.cpp
  component.cpp
  mapper.cpp
  netlist.cpp
  pretty.cpp
  primitive.cpp
  x.cpp
)

set(libanalyzer_HEADERS
  ${CMAKE_SOURCE_DIR}/lib/dbg.h
  Primitive/capacitor.h
  Primitive/cccs.h
  Primitive/ccvs.h
  Primitive/currentsource.h
  Primitive/inductor.h
  Primitive/resistor.h
  Primitive/vccs.h
  Primitive/vcvs.h
  Primitive/voltagesource.h
  Primitive/y.h
  abstractcircuit.h
  analyzer.h
  circuit.h
  component.h
  mapable.h
  mapper.h
  netlist.h
  pretty.h
  primitive.h
  subcircuit.h
  syms.h
  x.h
)

add_library(${ANALYZERLIB} STATIC ${libanalyzer_SOURCES} ${libanalyzer_HEADERS})

target_link_libraries(${ANALYZERLIB}
  Qt5::Core
  ${CLN_LIB}
  ${GINAC_LIB})

target_include_directories(${ANALYZERLIB} PRIVATE
  ${CMAKE_SOURCE_DIR}/lib/Analyzer
  ${CMAKE_SOURCE_DIR}/lib)

