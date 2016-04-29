project(twuner_qt5 C CXX)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebKit REQUIRED)
find_package(Qt5WebKitWidgets REQUIRED)
find_package(Qt5Sql REQUIRED)

configure_file(./config.h.in config.h)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -fvisibility=hidden ${CMAKE_CXX_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed ${CMAKE_SHARED_LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--as-needed ${CMAKE_MODULE_LINKER_FLAGS}")

set(TWUNER_QT5_INCLUDE_DIRS
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5WebKit_INCLUDE_DIRS}
  ${Qt5WebKitWidgets_INCLUDE_DIRS}
  ${Qt5NetWork_INCLUDE_DIRS}
  )
set(TWUNER_QT5_LINKS
  ${Qt5Core_LIBRARIES}
  ${Qt5Gui_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  ${Qt5WebKit_LIBRARIES}
  ${Qt5WebKitWidgets_LIBRARIES}
  ${Qt5NetWork_LIBRARIES}
  )

add_definitions("-fPIC -DHAVE_QT5")

function(twuner_set_qt5_target tgt)
  get_target_property(TWUNER_INCLUDES "${tgt}" INCLUDE_DIRECTORIES)
  if(TWUNER_INCLUDES)
    set(TWUNER_QT5_INCLUDE_DIRS ${TWUNER_QT5_INCLUDE_DIRS}
      ${TWUNER_INCLUDES})
  endif()
  set_target_properties("${tgt}"
    PROPERTIES LINKER_LANGUAGE CXX
    INCLUDE_DIRECTORIES "${TWUNER_QT5_INCLUDE_DIRS}")
  target_link_libraries("${tgt}" ${TWUNER_QT5_LINKS})
endfunction()

set(Twuner_QT_SRCS
  formpost.cpp
  twunerrequest.cpp
  main.cpp
  mainwindow.cpp
  twunerwebpage.cpp
  qttraybackend.cpp
  )
set(Twuner_QT_MOC_SRCS
  mainwindow.h
  formpost.h
  twunerrequest.h
  twunerwebpage.h
  qttraybackend.h
  )
qt5_wrap_cpp(Twuner_QT_MOCS ${Twuner_QT_MOC_SRCS})

add_executable(twuner-qt5 ${Twuner_QT_SRCS} ${Twuner_QT_MOCS})
twuner_set_qt5_target(twuner-qt5)
install(TARGETS twuner-qt5 DESTINATION bin)
set(TWUNER_QT_BIN twuner-qt5)
configure_file("${PROJECT_SOURCE_DIR}/../misc/twuner-qt5.desktop.in.in"
  twuner-qt5.desktop.in @ONLY)
intltool_merge_translation(
  "${CMAKE_CURRENT_BINARY_DIR}/twuner-qt5.desktop.in"
  "${CMAKE_CURRENT_BINARY_DIR}/twuner-qt5.desktop")
add_custom_target(desktopfile-qt5 ALL
  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/twuner-qt5.desktop")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/twuner-qt5.desktop"
  DESTINATION share/applications/)
