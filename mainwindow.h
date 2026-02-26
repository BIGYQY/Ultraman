#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPoint>

#include "homepage.h"
#include "settingpage.h"
#include "logpage.h"

// 引入后端核心
#include "database_manager.h"
#include "scanner_server.h"
#include "alarm_controller.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupUI();         
    void applyStyle();      

    QWidget *sideBar;
    QWidget *mainArea;
    QWidget *topBar;
    QStackedWidget *stack;

    QPushButton *sideBtnHome;
    QPushButton *sideBtnSetting;
    QPushButton *sideBtnLog;
    QPushButton *btnClose;
    QLabel *lblTitle; // 关键：声明为类成员，解决 Lambda 捕获问题

    HomePage *pageHome;
    SettingPage *pageSetting;
    LogPage *pageLog;

    // --- 状态栏标签 ---
    class QDateEdit *dateEditStatus; // 升级为日历控件
    QLabel *lblStatusYield;
    QLabel *lblStatusFilter;

    // --- 后端核心引擎 ---
    ScannerServer *scannerServer;
    AlarmController *alarmController;

    QPoint dragPosition;
};

#endif // MAINWINDOW_H
