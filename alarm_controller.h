#ifndef ALARM_CONTROLLER_H
#define ALARM_CONTROLLER_H

#include <QObject>

/*
 * AlarmController: 物理报警总线 (环境兼容版)
 * 如果环境没有 SerialPort 模块，我们在这里先用空指针和虚拟逻辑占位。
 */
class AlarmController : public QObject
{
    Q_OBJECT
public:
    explicit AlarmController(QObject *parent = nullptr);
    bool open(const QString &portName);
    
    void triggerAlarm(); // 开启报警 (暂为虚拟打印)
    void resetAlarm();   // 关闭报警 (暂为虚拟打印)

private:
   // QSerialPort *serial; // 暂时屏蔽底层硬件对象
};

#endif // ALARM_CONTROLLER_H
