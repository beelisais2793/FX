set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core)
find_package(CLN)
find_package(GiNaC)

set(libsynthesizer_SOURCES
    chromosome.cpp
    crossover.cpp
    estimator.cpp
    geneticsynthesizer.cpp
    mutation.cpp
    opampfilterchromosome.cpp
    otrafilterchromosome.cpp
    cciifilterchromosome.cpp
    cfoafilterchromosome.cpp
    population.cpp
    selection.cpp
    utilities.cpp
)

set(libsynthesizer_HEADERS
  ${CMAKE_SOURCE_DIR}/lib/dbg.h
  chromosome.h
  crossover.h
  estimator.h
  geneticsynthesizer.h
  mutation.h
  opampfilterchromosome.h
  otrafilterchromosome.h
  cciifilterchromosome.h
  cfoafilterchromosome.h
  population.h
  selection.h
  utilities.h
)

add_library(${SYNTHESIZERLIB} STATIC ${libsynthesizer_SOURCES} ${libsynthesizer_HEADERS})

target_link_libraries(${SYNTHESIZERLIB}
  Qt5::Core
  ${CLN_LIB}
  ${GINAC_LIB})

target_include_directories(${SYNTHESIZERLIB} PRIVATE
  ${CMAKE_SOURCE_DIR}/lib/Synthesizer
  ${CMAKE_SOURCE_DIR}/lib/Analyzer
  ${CMAKE_SOURCE_DIR}/lib)
