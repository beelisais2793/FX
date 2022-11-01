
list(APPEND CMAKE_MODULE_PATH
${CMAKE_CURRENT_LIST_DIR}
)

include("../BuildExamples.cmake")


find_library(OPENVR_LIB openvr_api
  "${CMAKE_CURRENT_LIST_DIR}/../../../openvr-1.12.5/lib/win64"
  "${CMAKE_CURRENT_LIST_DIR}/../../../../openvr-1.12.5/lib/win64"
  "${CMAKE_CURRENT_LIST_DIR}/../../../openvr-1.6.10b/lib/win64"
  "${CMAKE_CURRENT_LIST_DIR}/../../../../openvr-1.6.10b/lib/win64"
  "${CMAKE_CURRENT_LIST_DIR}/../../../openvr/lib/win64"
  "${CMAKE_CURRENT_LIST_DIR}/../../../../openvr/lib/win64"
  )


message("OpenVR root: ${OPENVR_ROOT_DIR}")

set(THIS_EXTENSION_SRC
  "${CMAKE_CURRENT_LIST_DIR}/src/al_OpenVRWrapper.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/al_OpenVRDomain.cpp"
)

set(THIS_EXTENSION_HEADERS
  "${CMAKE_CURRENT_LIST_DIR}/al_OpenVRWrapper.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/al_OpenVRDomain.hpp"
)
set(THIS_EXTENSION_LIBRARY_NAME al_openvr)

add_library(${THIS_EXTENSION_LIBRARY_NAME} ${THIS_EXTENSION_SRC} ${THIS_EXTENSION_HEADERS})

target_include_directories(${THIS_EXTENSION_LIBRARY_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../.." )
target_link_libraries(${THIS_EXTENSION_LIBRARY_NAME} PUBLIC al)
set_target_properties(${THIS_EXTENSION_LIBRARY_NAME} PROPERTIES
  CXX_STANDARD 14
)

if (NOT OPENVR_LIB)
  message("OpenVR not found. Not building al_openvr extension.")
else()
  get_filename_component(OPENVR_ROOT ${OPENVR_LIB} DIRECTORY)
  set(OPENVR_ROOT "${OPENVR_ROOT}/../../")
  get_filename_component(OPENVR_ROOT ${OPENVR_ROOT} REALPATH)

  set_target_properties(${THIS_EXTENSION_LIBRARY_NAME} PROPERTIES OPENVR_ROOT ${OPENVR_ROOT})

  message("Using OpenVR at: ${OPENVR_ROOT}")
  message("Using OpenVR lib: ${OPENVR_LIB}")
  set(OPENVR_INCLUDE_DIR "${OPENVR_ROOT}/headers")

  target_link_libraries(${THIS_EXTENSION_LIBRARY_NAME} PUBLIC  ${OPENVR_LIB})
  target_compile_definitions(${THIS_EXTENSION_LIBRARY_NAME} PUBLIC AL_EXT_OPENVR)

  target_include_directories(${THIS_EXTENSION_LIBRARY_NAME} PUBLIC "${OPENVR_ROOT}/headers")


  if(BUILD_EXAMPLES)
    BuildExamples("${CMAKE_CURRENT_LIST_DIR}/examples" ${THIS_EXTENSION_LIBRARY_NAME} al_assets3d)
  endif(BUILD_EXAMPLES)

#  # unit tests
#  add_executable(soundfileBufferedRecordTests ${CMAKE_CURRENT_LIST_DIR}/unitTests/utSoundfileBufferedRecord.cpp)
#  target_link_libraries(soundfileBufferedRecordTests al ${SPATIALAUDIO_LINK_LIBRARIES} )
#  target_include_directories(soundfileBufferedRecordTests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/catch")
#  set_target_properties(soundfileBufferedRecordTests PROPERTIES
#    CXX_STANDARD 14
#    CXX_STANDARD_REQUIRED ON
#    )
#  add_test(NAME soundfileBufferedRecordTests
#    COMMAND $<TARGET_FILE:soundfileBufferedRecordTests> ${TEST_ARGS})

#  # Connections to outside
#  set(CURRENT_EXTENSION_LIBRARIES ${THIS_EXTENSION_LIBRARY_NAME} ${THIS_EXTENSION_LIBRARIES})
#  set(CURRENT_EXTENSION_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR} ${ASSIMP_INCLUDE_DIR})

#  set(CURRENT_EXTENSION_DEFINITIONS -DAL_EXT_ASSETS3D)
#  get_filename_component(ASSIMP_ROOT_DIR "${ASSIMP_LIBRARY}" DIRECTORY)
#  set(CURRENT_EXTENSION_DLLS "${ASSIMP_ROOT_DIR}/../../bin/x64/assimp-vc140-mt.dll")

endif()
