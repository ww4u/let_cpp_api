TEMPLATE = app

TARGET = "cjson"

INCLUDEPATH += "C:/Program Files (x86)/IVI Foundation/VISA/WinNT/Include"
INCLUDEPATH += "./"
win32:LIBS += -L"C:/Program Files (x86)/IVI Foundation/VISA/WinNT/lib/msc"
win32:LIBS += -lvisa32

SOURCES = ..\cjson.c \
		  ..\cjsonobject.cpp \
#                  demo.cpp \
    ..\let_api.cpp \
    let_test.cpp
		  

HEADERS += \
    ..\let_api.h
