#include "logpage.h"
#include <QVBoxLayout>
#include <QDateTime>

LogPage::LogPage(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void LogPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    logArea = new QTextEdit;
    logArea->setReadOnly(true);
    
    // 设置 CLI 风格样式
    logArea->setStyleSheet(
        "background-color: #000000;"
        "color: #FFFFFF;"
        "font-family: 'Consolas', monospace;"
        "font-size: 14px;"
        "border: none;"
    );

    mainLayout->addWidget(logArea);

    // 100% 还原 Datalog.png 的初始输出内容
    appendLog("SYSTEM BOOTING... DONE.");
    appendLog("TCP SERVER STARTED ON 127.0.0.1:8080");
    appendLog("WAITTING FOR FIELD SIGNALS...");
}


void LogPage::appendLog(const QString &msg)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    logArea->append(QString("[%1] %2").arg(timestamp, msg));
}
