QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += release

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    s21_field_window.cpp \
    s21_next_field_window.cpp \
#    ../../brick_game/snake/s21_backend_snake.cpp

HEADERS += \
    mainwindow.h \
    s21_field_window.h \
    s21_next_field_window.h \
#    ../../brick_game_snake/s21_backend_snake.h

contains(DEFINES, SNAKE) {
    LIBS += ../../s21_snake.a
} else:contains(DEFINES, TETRIS) {
    LIBS += ../../s21_tetris.a
} else:contains(DEFINES, RACE) {
    LIBS += -L$$PWD/../../ -l:s21_race.so
    QMAKE_LFLAGS += -Wl,-rpath,$$PWD/../../
    LIBS += -lpthread
}

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../../s21_snake.a \
    ../../s21_tetris.a \
    ../../test_tetris
