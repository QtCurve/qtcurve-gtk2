project(QtCurve-Gtk2)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}"
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(QTCURVE_VERSION_MAJOR "1")
set(QTCURVE_VERSION_MINOR "8")
set(QTCURVE_VERSION_PATCH "16")
set(QTCURVE_VERSION "${QTCURVE_VERSION_MAJOR}.${QTCURVE_VERSION_MINOR}")
set(QTCURVE_VERSION_FULL "${QTCURVE_VERSION}.${QTCURVE_VERSION_PATCH}")

function(def_compat_option new_opt old_opt)
  if("${ARGC}" GREATER 2 AND "${ARGV2}")
    set(reverse True)
  else()
    set(reverse False)
  endif()
  if(DEFINED "${old_opt}" AND NOT DEFINED "${new_opt}")
    if(reverse)
      if("${${old_opt}}")
        set("${new_opt}" Off PARENT_SCOPE)
      else()
        set("${new_opt}" On PARENT_SCOPE)
      endif()
    else()
      set("${new_opt}" "${${old_opt}}" PARENT_SCOPE)
    endif()
  endif()
endfunction()

option(ENABLE_GTK2 "Building Gtk2 theme." On)
def_compat_option(QTC_GTK2_MODIFY_MOZILLA QTC_MODIFY_MOZILLA)
option(QTC_GTK2_MODIFY_MOZILLA "Modify Mozilla Gtk2 with js and css." Off)
def_compat_option(QTC_GTK2_OLD_MOZILLA QTC_OLD_MOZILLA)
option(QTC_GTK2_OLD_MOZILLA
  "Defaulting to treating Firefox(Gtk2) as Firefox < 3.0." Off)
def_compat_option(QTC_GTK2_USE_CAIRO_FOR_ARROWS QTC_USE_CAIRO_FOR_ARROWS)
option(QTC_GTK2_USE_CAIRO_FOR_ARROWS "Using ciaro to draw arrows." Off)
option(QTC_GTK2_ENABLE_PARENTLESS_DIALOG_FIX_SUPPORT
  "Qt5 support for the 'fix parentless dialogs'." Off)

# Only try to figure out what kde prefix is if not specified at command line.
if(NOT DEFINED QTC_KDE4_PREFIX)
  find_package(KDE4)
  find_program(KDE4_KDECONFIG_EXECUTABLE NAMES kde4-config
    PATHS "${CMAKE_INSTALL_PREFIX}/bin" ${_KDEDIRS} /opt/kde4/bin
    NO_DEFAULT_PATH)
  if(NOT KDE4_KDECONFIG_EXECUTABLE)
    find_program(KDE4_KDECONFIG_EXECUTABLE NAMES kde4-config)
  endif()
  if(KDE4_KDECONFIG_EXECUTABLE)
    execute_process(COMMAND "${KDE4_KDECONFIG_EXECUTABLE}" --prefix
      OUTPUT_VARIABLE QTC_KDE4_PREFIX)
    string(STRIP "${QTC_KDE4_PREFIX}" QTC_KDE4_PREFIX)
    execute_process(COMMAND "${KDE4_KDECONFIG_EXECUTABLE}" --expandvars
      --install icon
      OUTPUT_VARIABLE QTC_KDE4_ICONS_PREFIX)
    string(STRIP "${QTC_KDE4_ICONS_PREFIX}" QTC_KDE4_ICONS_PREFIX)
  else()
    set(QTC_KDE4_PREFIX "${CMAKE_INSTALL_PREFIX}")
  endif()
endif()

if(NOT DEFINED QTC_KDE4_ICONS_PREFIX)
  set(QTC_KDE4_ICONS_PREFIX "${QTC_KDE4_PREFIX}/share/icons")
endif()

configure_file(config.h.in config.h @ONLY)

include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_BINARY_DIR}")
set(CMAKE_C_FLAGS "-Wall -Wextra ${CMAKE_C_FLAGS} -std=gnu99")
set(CMAKE_CXX_FLAGS "-Wall -Wextra ${CMAKE_CXX_FLAGS} -std=gnu++0x")
set(CMAKE_SHARED_LINKER_FLAGS
  "-Wl,--as-needed -Wl,--no-undefined ${CMAKE_SHARED_LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS
  "-Wl,--as-needed -Wl,--no-undefined ${CMAKE_MODULE_LINKER_FLAGS}")
add_definitions("-D_GNU_SOURCE")

add_subdirectory(gtk2)

if(QTC_GTK2_OLD_MOZILLA)
  message("**************************************************************\n"
    "Information: Defaulting to treating Firefox as Firefox < 3.0\n"
    "**************************************************************\n")
else()
  message("**************************************************************\n"
    "Information: Defaulting to treating Firefox as Firefox >= 3.0\n"
    "**************************************************************\n")
endif()
if(QTC_GTK2_MODIFY_MOZILLA)
  message("**************************************************************\n"
    "**************************************************************\n"
    "Warning: You have enabled the option for QtCurve to modify the\n"
    "         configuration files of Mozilla applications (Firefox, \n"
    "         Thunderbird, Seamonkey). Note: This sometimes causes\n"
    "         issues with newer versions of these applications. Should\n"
    "         such issues occur, you should recompile QtCurve without\n"
    "         this option - and you will have to remove BY HAND the \n"
    "         changes that QtCurve has made. This will entail removing\n"
    "         lines from userChrom.css that indicate 'Added by QtCurve'\n"
    "         userChrome.css is usually in a folder such as:\n"
    "             ~/.mozilla/firefox/xxxxxxx.default/chrome/userChrome.css\n"
    "             ~/.thunderbird/xxxxxxx.default/chrome/userChrome.css\n"
    "         Where xxxxxxx is a series of random characters.\n"
    "**************************************************************\n"
    "**************************************************************\n")
endif()

if(QTC_GTK2_USE_CAIRO_FOR_ARROWS)
  message("**************************************************************\n"
    "Warning: Using cairo to draw arrows - this has been reported to \n"
    "         fail with intel Xorg driver > 2.8\n"
    "**************************************************************\n")
else()
  message("**************************************************************\n"
    "Information: Using Gdk to draw arrows.\n"
    "**************************************************************\n")
endif()

if(QTC_GTK2_ENABLE_PARENTLESS_DIALOG_FIX_SUPPORT)
  message("**************************************************************\n"
    "Warning: You have enabled support for the 'fix parentless dialogs'\n"
    "         option. This is known to BREAK some applications. Please\n"
    "         DO NOT report errors to application authors when you have\n"
    "         this enabled. Please reconsider DISABLING this option.\n"
    "**************************************************************\n")
endif()
