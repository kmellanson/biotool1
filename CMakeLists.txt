cmake_minimum_required(VERSION 3.0.0)
project(biotool1)

if(APPLE)
  if(DEFINED ENV{QTDIR})
    set(CMAKE_PREFIX_PATH $ENV{QTDIR} ${CMAKE_PREFIX_PATH})
  else()
    message(FATAL_ERROR "ERROR: Environment variable QTDIR is not set. Please locate your Qt folder MY_QT5_DIR.")
  endif()
endif()

################################################################################
# Build
################################################################################
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SRCS src/*.cpp src/*.h)
include_directories(src)

qt5_wrap_ui(UIS_HDRS ui/mainwindow.ui ui/csvExportOptions.ui)

file(GLOB_RECURSE IMAGES ${CMAKE_CURRENT_SOURCE_DIR}/res/*.png)
set(IMAGES_QRC res/icons.qrc)
file(WRITE ${IMAGES_QRC} "<RCC>\n    <qresource prefix=\"/icons\">\n")
foreach(image_file ${IMAGES})
  file(RELATIVE_PATH image_file_relative_path ${CMAKE_CURRENT_SOURCE_DIR}/res ${image_file})
  get_filename_component(image_file_short_name ${image_file} NAME_WE)
  file(APPEND ${IMAGES_QRC} "        <file alias=\"${image_file_short_name}\">${image_file_relative_path}</file>\n")
endforeach()
file(APPEND ${IMAGES_QRC} "    </qresource>\n</RCC>")

qt5_add_resources(RCC ${IMAGES_QRC})

if(APPLE)
  set(EXECUTABLE_TYPE MACOSX_BUNDLE)
elseif(WIN32)
  set(EXECUTABLE_TYPE WIN32)
endif()

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")
add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SRCS} ${UIS_HDRS} ${IMAGES} ${RCC} ${IMAGES_QRC})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL Qt5::Xml ${OPENGL_LIBRARIES})
if(WIN32)
  set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX .exe)
endif()
################################################################################


################################################################################
# Install
################################################################################
if(APPLE)
  set(BIN_INSTALL_DIR ".")
  install(TARGETS ${PROJECT_NAME} DESTINATION ${BIN_INSTALL_DIR})
endif()
################################################################################


################################################################################
# Packaging
################################################################################
if(APPLE)
  set(PACKAGE_VERSION "1.0.0.0")

  set(CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME})
  set(CPACK_PACKAGE_EXECUTABLES ${PROJECT_NAME} ${PROJECT_NAME})
  set(CPACK_PACKAGE_VENDOR ${APPVENDOR})
  set(CPACK_PACKAGE_VERSION ${PACKAGE_VERSION})
  set(CPACK_PACKAGE_VERSION_MAJOR ${PACKAGE_VERSION_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${PACKAGE_VERSION_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH ${PACKAGE_VERSION_PATCH})
  set(CPACK_PACKAGE_VERSION_TWEAK ${PACKAGE_VERSION_TWEAK})

  set(CPACK_SOURCE_PACKAGE_FILE_NAME ${PACKAGE_NAME}-${PACKAGE_VERSION})

  set(MACOSX_BUNDLE_INFO_STRING           "${PROJECT_NAME} ${PACKAGE_VERSION}")
  set(MACOSX_BUNDLE_BUNDLE_VERSION        "${PROJECT_NAME} ${PACKAGE_VERSION}")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING   "${PROJECT_NAME} ${PACKAGE_VERSION}")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING  "${PACKAGE_VERSION}")
  set(MACOSX_BUNDLE_BUNDLE_NAME           "${PROJECT_NAME}")

  set(CPACK_GENERATOR "DragNDrop")
  set(CPACK_DMG_FORMAT "UDBZ")
  set(CPACK_DMG_VOLUME_NAME "${PROJECT_NAME}")
  set(CPACK_SYSTEM_NAME "OSX")
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PACKAGE_VERSION}")

  set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "${BIN_INSTALL_DIR}")
  include(InstallRequiredSystemLibraries)

  set(QT_DIR          ${QT_QTCORE_LIBRARY}/../..)
  set(FRAMEWORKS_DST  @executable_path/../Frameworks)

  include (cmake/DeployQt5.cmake)
  install_qt5_executable(${PROJECT_NAME}.app qcocoa)

  include(CPack)
endif()
###############################################################################
