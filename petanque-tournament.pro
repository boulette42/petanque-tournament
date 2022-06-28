#petu.pro

TEMPLATE = app
TARGET = petu
CONFIG += force_asserts c++17
QT += widgets

win32 {
  X86_X64 = $$(x86_x64_)
  contains(QT_ARCH, i386) {
    equals(X86_X64, "x64") {
      error("wrong Qt-architecture: expected x64")
#    } else {
#      message("Qt-architecture: x86")
    }
  } else {
    equals(X86_X64, "x86") {
      error("wrong Qt-architecture: expected x86")
#    } else {
#      message("Qt-architecture: x64")
    }
  }
}

include(output_dir.pri)

DEFINES += \
  QT_NO_CAST_TO_ASCII \
  QT_NO_CAST_FROM_ASCII \
  QT_STRICT_ITERATORS \
  QT_FORCE_ASSERTS

win32-msvc* {
  QMAKE_CXXFLAGS += /utf-8
  QMAKE_LFLAGS += /OPT:REF
  contains(QT_ARCH, i386) {
    QMAKE_LFLAGS += /MACHINE:X86
  } else {
    QMAKE_LFLAGS += /MACHINE:X64
  }

  #pdb in release-build
  QMAKE_CXXFLAGS_RELEASE += /Zi
  QMAKE_CXXFLAGS_RELEASE += /Fd$$OBJECTS_DIR\petu.vc.pdb
  QMAKE_LFLAGS_RELEASE += /DEBUG
}

#Input
FORMS += \
  ui/edit_match_dlg.ui \
  ui/edit_player_dlg.ui \
  ui/main_window.ui \
  ui/select_window_dlg.ui \
  ui/settings_dlg.ui \
  ui/show_result_dlg.ui

HEADERS += \
  src/config.h \
  src/edit_match_dlg.h \
  src/edit_player_dlg.h \
  src/external_window.h \
  src/json_common.h \
  src/main_window.h \
  src/match.h \
  src/player.h \
  src/player_model.h \
  src/player_result.h \
  src/player_result_model.h \
  src/result.h \
  src/round_calculator.h \
  src/round_model.h \
  src/select_window_dlg.h \
  src/settings.h \
  src/site.h \
  src/site_model.h \
  src/show_result_dlg.h \
  src/team.h \
  src/team_result_model.h \
  src/tournament.h \
  src/tournament_simulator.h \
  src/version.h

SOURCES += \
  src/edit_match_dlg.cpp \
  src/edit_player_dlg.cpp \
  src/external_window.cpp \
  src/json_common.cpp \
  src/main.cpp \
  src/main_window.cpp \
  src/match.cpp \
  src/player.cpp \
  src/player_model.cpp \
  src/player_result.cpp \
  src/player_result_model.cpp \
  src/result.cpp \
  src/round_calculator.cpp \
  src/round_model.cpp \
  src/site.cpp \
  src/site_model.cpp \
  src/select_window_dlg.cpp \
  src/settings.cpp \
  src/show_result_dlg.cpp \
  src/team.cpp \
  src/team_result_model.cpp \
  src/tournament.cpp \
  src/tournament_simulator.cpp

win32 {
  RC_FILE = src/petu.rc
}
