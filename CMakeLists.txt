cmake_minimum_required(VERSION 2.7)
cmake_policy(SET CMP0002 OLD)
project(twuner NONE)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

find_package(Gettext REQUIRED)

option(WITH_GIR "Use GIR as GTK Version" Off)

option(WITH_GTK3 "Enable GTK3 Version" On)

option(WITH_QT5 "Enable Qt5 Version" On)

find_program(INTLTOOL_MERGE intltool-merge)

if(NOT INTLTOOL_MERGE)
  message(FATAL_ERROR "intltool-merge required for i18n generation")
endif()

set(INTLTOOL_PO_DIR "${PROJECT_SOURCE_DIR}/po")

function(intltool_merge_translation infile outfile)
  add_custom_command(
    OUTPUT ${outfile}
    COMMAND LC_ALL=C ${INTLTOOL_MERGE} -d -u "${INTLTOOL_PO_DIR}"
    ${infile} ${outfile}
    DEPENDS ${infile})
endfunction()

# uninstall target
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

function(twuner_convert_flags __flags_var __defs_var)
  get_directory_property(_old_flags COMPILE_FLAGS)
  get_directory_property(_old_defs COMPILE_DEFINITIONS)
  set_directory_properties(PROPERTIES
    COMPILE_FLAGS ""
    COMPILE_DEFINITIONS "")
  add_definitions(${ARGN})
  get_directory_property(_new_flags COMPILE_FLAGS)
  get_directory_property(_new_defs COMPILE_DEFINITIONS)
  set_directory_properties(PROPERTIES
    COMPILE_FLAGS "${_old_flags}"
    COMPILE_DEFINITIONS "${_old_defs}")
endfunction()
if(NOT DEFINED LOCALEDIR)
  set(LOCALEDIR ${CMAKE_INSTALL_PREFIX}/share/locale)
endif()

##################### GTK Version ######################
if(WITH_GTK3)
  find_package(PythonLibrary REQUIRED)
  if(NOT PYTHONLIBRARY_FOUND)
    message(FATAL_ERROR "GTK Wrapper for Twuner need python")
  endif()
  include(PythonMacros)
  # for backword compatibility, if GTK3 isn't manually set we will
  # still use the old binary name instead of twuner-gtk{2,3}
  if(WITH_GTK3)
    set(TWUNER_GTK_OLD_NAME Off)
  else()
    set(TWUNER_GTK_OLD_NAME On)
  endif()

  function(twuner_gtk_build suffix TWUNER_GTK_NAME dir)
    if(TWUNER_GTK_OLD_NAME)
      set(TWUNER_BIN_NAME "twuner")
      set(TWUNER_PY_NAME "twuner")
    else()
      set(TWUNER_BIN_NAME "twuner-${suffix}")
      set(TWUNER_PY_NAME "twuner_${suffix}")
    endif()
    set(twuner_bin "${CMAKE_CURRENT_BINARY_DIR}/scripts/${TWUNER_BIN_NAME}")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/scripts/twuner.in"
      "${twuner_bin}" @ONLY)
    install(PROGRAMS "${twuner_bin}" DESTINATION bin)
    add_subdirectory("${dir}")
    configure_file("misc/twuner-${suffix}.desktop.in.in"
      "misc/${TWUNER_BIN_NAME}.desktop.in")
    intltool_merge_translation(
      "${CMAKE_CURRENT_BINARY_DIR}/misc/${TWUNER_BIN_NAME}.desktop.in"
      "${CMAKE_CURRENT_BINARY_DIR}/misc/${TWUNER_BIN_NAME}.desktop")
    add_custom_target("desktopfile-${TWUNER_BIN_NAME}" ALL
      DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/misc/${TWUNER_BIN_NAME}.desktop")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/misc/${TWUNER_BIN_NAME}.desktop"
      DESTINATION share/applications/)
  endfunction()

  if(WITH_GIR)
    set(WITH_GTK3 On)
  endif()
  if(WITH_GTK3)
    twuner_gtk_build(gtk3 Gtk3 twuner-gir)
  endif()
endif()

##################### Qt Version ######################

if(WITH_QT5)
  add_subdirectory(qt5)
endif()

add_subdirectory(po)
add_subdirectory(misc)
