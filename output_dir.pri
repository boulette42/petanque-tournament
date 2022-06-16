#output_dir.pri

isEmpty(WORK_BASE_DIR) {
  WORK_BASE_DIR = $$PWD
}

isEmpty(TARGET_NAME) {
  TARGET_NAME = $$TARGET
}

UIC_DR = uic
CONFIG(debug, debug|release) {
  DR_DIR = debug
} else {
  DR_DIR = release
}
OBJ_SUB_DIR = obj
EXE_SUB_DIR = exe
win32 {
  contains(QT_ARCH, i386) {
    OBJ_SUB_DIR = obj32
    EXE_SUB_DIR = exe32
  }
} 
MOC_DIR = $$WORK_BASE_DIR/$$OBJ_SUB_DIR/$$UIC_DR/$$TARGET_NAME
UI_DIR = $$MOC_DIR
RCC_DIR = $$MOC_DIR
PRECOMPILED_DIR = $$MOC_DIR
QMAKE_INCDIR_VULKAN =

OBJECTS_DIR = $$WORK_BASE_DIR/$$OBJ_SUB_DIR/$$DR_DIR/$$TARGET_NAME
DESTDIR = $$WORK_BASE_DIR/$$EXE_SUB_DIR/$$DR_DIR
