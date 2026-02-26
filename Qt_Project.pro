QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 移除 UI 文件依赖，采用纯代码编写
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    homepage.cpp \
    settingpage.cpp \
    logpage.cpp \
    database_manager.cpp \
    scanner_server.cpp \
    alarm_controller.cpp

HEADERS += \
    mainwindow.h \
    homepage.h \
    settingpage.h \
    logpage.h \
    row_delegate.h \
    database_manager.h \
    scanner_server.h \
    alarm_controller.h


# 暂时注释掉，我们将使用纯代码绘制
# FORMS += \
#     mainwindow.ui

# 资源文件（用于存放 QSS 等）
RESOURCES += \
    resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

