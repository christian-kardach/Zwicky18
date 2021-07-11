QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ..\third-party\openpnp\include
INCLUDEPATH += ..\third-party\opencv\include
INCLUDEPATH += ..\third-party\opencv\bin\x64

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    previewscene.cpp \
    previewscenebase.cpp \
    previewscenewidget.cpp \
    usbcameras.cpp

HEADERS += \
    mainwindow.h \
    previewscene.h \
    previewscenebase.h \
    previewscenewidget.h \
    usbcameras.h

FORMS += \
    mainwindow.ui

LIBS += -L..\third-party\openpnp\libs\x64\ -lopenpnp-capture

LIBS += -L..\third-party\opencv\libs\ -lopencv_core453d
LIBS += -L..\third-party\opencv\libs\ -lopencv_highgui453d
LIBS += -L..\third-party\opencv\libs\ -lopencv_imgcodecs453d
LIBS += -L..\third-party\opencv\libs\ -lopencv_imgproc453d
LIBS += -L..\third-party\opencv\libs\ -lopencv_features2d453d
LIBS += -L..\third-party\opencv\libs\ -lopencv_calib3d453d
LIBS += -L..\third-party\opencv\libs\ -lopencv_video453d
LIBS += -L..\third-party\opencv\libs\ -lopencv_videoio453d

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# add run-path relative dylib search path
QMAKE_RPATHDIR += .
