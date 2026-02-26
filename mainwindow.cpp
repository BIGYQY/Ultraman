#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QWindow>
#include <QDateEdit>
#include <QCalendarWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    applyStyle();
    
    // 初始化加载：默认显示当天数据
    QDate today = QDate::currentDate();
    pageHome->loadHistoryData(today);
    lblTitle->setText(QString("监控看板 (%1)").arg(today.toString("yyyy-MM-dd")));
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(1200, 800); // 彻底还原老板要求的固定尺寸

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    setCentralWidget(centralWidget);
    
    QHBoxLayout *globalLayout = new QHBoxLayout(centralWidget);
    globalLayout->setContentsMargins(0, 0, 0, 0);
    globalLayout->setSpacing(0);

    // ================= [左侧导航栏 SideBar] =================
    int barDimension = 65; // 从 70 缩小到 65，更加精致
    sideBar = new QWidget();
    sideBar->setObjectName("sideBar");
    sideBar->setFixedWidth(barDimension);
    QVBoxLayout *sideLayout = new QVBoxLayout(sideBar);
    sideLayout->setContentsMargins(0, 0, 0, 0);
    sideLayout->setSpacing(0);

    auto createSideBtn = [&](const QString &objName) {
        QPushButton *btn = new QPushButton(); // 移除文字
        btn->setObjectName(objName);
        btn->setFixedSize(barDimension, barDimension);
        btn->setProperty("class", "sideBtn");
        btn->setCheckable(true);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setAutoExclusive(true); // 互斥
        return btn;
    };

    sideBtnHome    = createSideBtn("sideHome"); 
    sideBtnSetting = createSideBtn("sideSetting");
    sideBtnLog     = createSideBtn("sideLog");
    
    sideBtnHome->setChecked(true);

    sideLayout->addWidget(sideBtnHome);
    sideLayout->addWidget(sideBtnSetting);
    sideLayout->addWidget(sideBtnLog);
    sideLayout->addStretch();

    // ================= [右侧主控区 MainArea] =================
    mainArea = new QWidget();
    QVBoxLayout *areaLayout = new QVBoxLayout(mainArea);
    areaLayout->setContentsMargins(0, 0, 0, 0);
    areaLayout->setSpacing(0);

    // --- Dynamic Dashboard 顶栏 ---
    topBar = new QWidget();
    topBar->setObjectName("topBar");
    topBar->setFixedHeight(barDimension);
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(20, 0, 20, 0);

    // 核心更改：标题会随页面动态变化
    lblTitle = new QLabel("监控看板");
    lblTitle->setObjectName("dashboardTitle");

    // 状态组
    QWidget *statusGroup = new QWidget;
    QHBoxLayout *statusLayout = new QHBoxLayout(statusGroup);
    statusLayout->setSpacing(25);
    
    auto createStatusItem = [&](const QString &txt, const QString &iconPath, QLabel* &outLabel) {
        QWidget *item = new QWidget;
        QHBoxLayout *il = new QHBoxLayout(item);
        il->setContentsMargins(0,0,0,0);
        
        QLabel *ico = new QLabel();
        ico->setFixedSize(32, 32); // 从 24 放大到 32，解决“太小”问题
        ico->setPixmap(QPixmap(iconPath).scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ico->setProperty("class", "statusIcon");

        outLabel = new QLabel(txt);
        outLabel->setStyleSheet("color: white; font-size: 16px; margin-left:8px;");
        il->addWidget(ico);
        il->addWidget(outLabel);
        return item;
    };

    // 升级日历控件项
    QWidget *dateItem = new QWidget;
    QHBoxLayout *dl = new QHBoxLayout(dateItem);
    dl->setContentsMargins(0,0,0,0);
    QLabel *dateIco = new QLabel();
    dateIco->setFixedSize(32, 32);
    dateIco->setPixmap(QPixmap(":/assets/date_white.png").scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    dateEditStatus = new QDateEdit(QDate::currentDate());
    dateEditStatus->setCalendarPopup(true);
    dateEditStatus->setDisplayFormat("yyyy/MM/dd");
    dateEditStatus->setFixedWidth(120);
    dateEditStatus->setCursor(Qt::PointingHandCursor);
    dateEditStatus->setStyleSheet("QDateEdit { background: transparent; border: none; color: white; font-size: 16px; font-weight: bold; } "
                                 "QDateEdit::drop-down { border: none; }");
    dl->addWidget(dateIco);
    dl->addWidget(dateEditStatus);
    statusLayout->addWidget(dateItem);

    statusLayout->addWidget(createStatusItem("79%", ":/assets/yield_white.png", lblStatusYield));
    statusLayout->addWidget(createStatusItem("全部", ":/assets/filter_white.png", lblStatusFilter));

    btnClose = new QPushButton("✕");
    btnClose->setFixedSize(45, 45);
    btnClose->setCursor(Qt::PointingHandCursor);
    btnClose->setStyleSheet("QPushButton { border: none; color: white; font-size: 24px; font-weight: bold; } QPushButton:hover { color: #FF0000; }");

    topLayout->addWidget(lblTitle);
    topLayout->addStretch();
    topLayout->addWidget(statusGroup);
    topLayout->addStretch();
    topLayout->addWidget(btnClose);

    // --- 页面堆栈 ---
    stack = new QStackedWidget();
    pageHome = new HomePage();
    pageSetting = new SettingPage();
    pageLog = new LogPage();
    stack->addWidget(pageHome);
    stack->addWidget(pageSetting);
    stack->addWidget(pageLog);

    areaLayout->addWidget(topBar);
    areaLayout->addWidget(stack);

    globalLayout->addWidget(sideBar);
    globalLayout->addWidget(mainArea);

    // --- V7.0 日历联动逻辑 ---
    connect(dateEditStatus, &QDateEdit::dateChanged, this, [=](const QDate &date){
        pageHome->loadHistoryData(date);
        lblTitle->setText(QString("监控看板 (%1)").arg(date.toString("yyyy-MM-dd")));
    });

    // 动态更新标题与按钮状态
    auto updateNav = [this](int idx){
        stack->setCurrentIndex(idx);
        sideBtnHome->setChecked(idx == 0);
        sideBtnSetting->setChecked(idx == 1);
        sideBtnLog->setChecked(idx == 2);
        
        // 选中状态强制反色图标 (代码逻辑确保 100% 成功，解决白底白图问题)
        sideBtnHome->setIcon(QIcon(idx == 0 ? ":/assets/home_black.png" : ":/assets/home_white.png"));
        sideBtnSetting->setIcon(QIcon(idx == 1 ? ":/assets/setting_black.png" : ":/assets/setting_white.png"));
        sideBtnLog->setIcon(QIcon(idx == 2 ? ":/assets/log_black.png" : ":/assets/log_white.png"));

        // 动态切换标题 (纯净多语言版)
        bool isChn = (pageSetting->getLanguageIndex() == 0);
        if (idx == 0) lblTitle->setText(isChn ? "监控看板" : "Dashboard");
        else if (idx == 1) lblTitle->setText(isChn ? "参数设置" : "Settings");
        else if (idx == 2) lblTitle->setText(isChn ? "系统日志" : "Data Log");

        // 刷新状态栏翻译
        if (lblStatusFilter) lblStatusFilter->setText(isChn ? "全部" : "All");
    };

    connect(sideBtnHome, &QPushButton::clicked, [=](){ updateNav(0); });
    connect(sideBtnSetting, &QPushButton::clicked, [=](){ updateNav(1); });
    connect(sideBtnLog, &QPushButton::clicked, [=](){ updateNav(2); });
    connect(btnClose, &QPushButton::clicked, qApp, &QApplication::quit);

    // 语言切换连接
    connect(pageSetting, &SettingPage::languageChanged, [=](int langIdx){
        pageHome->retranslateUi(langIdx); // 联动表格汉化
        updateNav(stack->currentIndex()); // 刷新当前标题
    });

    // 端口热切换：当老板在设置页修改端口时，后端自动重连
    connect(pageSetting, &SettingPage::portChanged, [=](int newPort){
        scannerServer->stop();
        if (scannerServer->start(newPort)) {
            pageLog->appendLog(QString("--- Hot-Switched: Receiver restarted on Port %1 ---").arg(newPort));
        } else {
            pageLog->appendLog(QString("--- ERROR: Failed to switch to Port %1 ---").arg(newPort));
        }
    });

    // 初始状态触发
    updateNav(0);

    // ================= [后端引擎启动逻辑] =================
    // 1. 初始化数据库
    if (DatabaseManager::instance().init()) {
        pageLog->appendLog("System Database Initialized.");
    }

    // 2. 启动 TCP 扫码服务 (直接使用 SettingPage 用户设定的端口)
    int initialPort = pageSetting->getScannerPort();
    scannerServer = new ScannerServer(this);
    if (scannerServer->start(initialPort)) {
        pageLog->appendLog(QString("TCP Data Receiver started on port %1 (Computer IP: %2)").arg(initialPort).arg(pageSetting->getLocalIP()));
    }

    connect(scannerServer, &ScannerServer::clientConnected, [=](const QString &ip){
        pageLog->appendLog(QString("--- [DEVICE CONNECTED] IP: %1 ---").arg(ip));
    });

    connect(scannerServer, &ScannerServer::clientDisconnected, [=](const QString &ip){
        pageLog->appendLog(QString("--- [DEVICE DISCONNECTED] IP: %1 ---").arg(ip));
    });

    // 3. 串口报警准备
    alarmController = new AlarmController(this);
    // 暂时注释掉，老板确定好 COM 口后在 Settings 页面开启
    // alarmController->open("COM1"); 

    // 核心业务连接：收到扫码数据 -> 触发逻辑引擎
    connect(scannerServer, &ScannerServer::qrCodeReceived, this, [=](const QString &qr, const QString &ip){
        // 1. 获取扫码枪别名
        QString sourceName = pageSetting->getScannerAlias(ip);

        pageLog->appendLog(QString("Scanner[%1] Scanned: %2").arg(sourceName).arg(qr));
        
        // 2. 执行逻辑引擎
        ProductionRecord record;
        int threshold = pageSetting->getTimeout();

        if (!DatabaseManager::instance().recordPressStart(qr, threshold, record)) {
            // 第一次扫码（点胶）
            if (DatabaseManager::instance().recordGlueStart(qr, sourceName)) {
                // 获取最新插入的记录以获取 ID 用于表格定位
                QList<ProductionRecord> recent = DatabaseManager::instance().getRecentRecords(1);
                if (!recent.isEmpty()) {
                    pageHome->updateTableWithNewRecord(recent.first());
                    pageLog->appendLog("Action: Glue Start (Gray PENDING).");
                }
            }
        } else {
            // 第二次扫码（热压）
            pageLog->appendLog(QString("Action: Press End. Duration: %1s, Status: %2").arg(record.duration).arg(record.status));
            
            // 触发硬件报警 (如果是 NG)
            if (record.status == "NG") {
                alarmController->triggerAlarm();
            }
            
            // 刷新首页表格（自动更新对应的灰色行）
            pageHome->updateTableWithNewRecord(record);
        }
    });
}


void MainWindow::applyStyle()
{
    QFile file(":/style.qss");
    if (file.open(QFile::ReadOnly)) {
        this->setStyleSheet(QLatin1String(file.readAll()));
        file.close();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - dragPosition);
        event->accept();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) qApp->quit();
    else QMainWindow::keyPressEvent(event);
}
