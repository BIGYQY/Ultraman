#include "alarm_controller.h"
#include <QDebug>

AlarmController::AlarmController(QObject *parent) : QObject(parent) {
    // serial = new QSerialPort(this); // 屏蔽
}

bool AlarmController::open(const QString &portName) {
    qDebug() << "Alarm SerialPort Simulated Open for [ " << portName << " ] (Success)";
    return true;
}

void AlarmController::triggerAlarm() {
    qDebug() << ">>> [SIMULATED] Hardware Alert Triggered: NG Detected!";
}

void AlarmController::resetAlarm() {
    qDebug() << ">>> [SIMULATED] Hardware Alert Reset.";
}
