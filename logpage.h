#ifndef LOGPAGE_H
#define LOGPAGE_H

#include <QWidget>
#include <QTextEdit>

/*
 * LogPage 类说明：
 * 系统的“黑匣子”。CLI 终端风格，
 * 实时滚动显示所有扫码、通讯及内部报警日志。
 */
class LogPage : public QWidget
{
    Q_OBJECT
public:
    explicit LogPage(QWidget *parent = nullptr);

    // 暴露一个添加日志的方法
    void appendLog(const QString &msg);

private:
    void setupUI();
    QTextEdit *logArea;
};

#endif // LOGPAGE_H
